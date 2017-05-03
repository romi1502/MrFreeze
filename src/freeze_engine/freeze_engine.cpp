#include "freeze_engine.h"

#include <cmath>
// On windows, M_PI isn't define if cmath is included without _USE_MATH_DEFINES.
// Defining it here if it isn't already is a more portable way of doing
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif  // M_PI

#include <Eigen/Core>
//#include <unsupported/Eigen/FFT>
#include "fft.h"

namespace freeze {

// Helpers
using Matrix = Eigen::MatrixXf;
using CplxMatrix = Eigen::MatrixXcf;
using Vector = Eigen::VectorXf;

const size_t kMaxBufferLen = 2048;

struct Freezer::Parameters {
  Matrix input;

  // params that has to be initialized
  Matrix sliding_buffer;
  Matrix output_buffer;
  CplxMatrix fourier_transform;
  Vector window;

  // params that can be initialized at runtime
  CplxMatrix previous_fourier_transform;
  Matrix dphi;
  Matrix freeze_ft_magnitude;
  Matrix total_dphi;

  size_t channel_number;
  size_t nfft;
  size_t hop_size;
  size_t index_sliding;

  bool is_on;
  bool first_on;
  bool just_on;

  FFT fft;
  //  Eigen::FFT<float> fft;
};

Vector MakeSqrtHanningWindow(size_t length) {
  Vector output = Vector::Zero(length);
  for (size_t index = 0; index < length; index++) {
    output[index] =
        sqrt(0.5 - 0.5 * cos(2 * M_PI * (static_cast<float>(index) / length)));
  }
  return output;
}

void ShiftBuffer(Matrix* buffer, int shift) {
  assert(shift < 0);
  // copy end of buffer at the beginning
  for (Matrix::Index col = 0; col < buffer->cols() - std::abs(shift); col++) {
    memcpy(buffer->data() + (col * buffer->rows()),
           buffer->data() + ((col + std::abs(shift)) * buffer->rows()),
           buffer->rows() * sizeof(float));
  }
  // set zeros at the end
  buffer
      ->block(0, buffer->cols() - std::abs(shift), buffer->rows(),
              std::abs(shift))
      .setZero();
}

Matrix Angle(const CplxMatrix& input) {
  Matrix output(input.rows(), input.cols());
  size_t count = input.cols() * input.rows();
  for (size_t index = 0; index < count; index++) {
    output(index) = std::arg(input(index));
  }
  return output;
}
Matrix Abs(const CplxMatrix& input) {
  Matrix output(input.rows(), input.cols());
  size_t count = input.cols() * input.rows();
  for (size_t index = 0; index < count; index++) {
    output(index) = std::abs(input(index));
  }
  return output;
}
CplxMatrix Exp(const CplxMatrix& input) {
  CplxMatrix output(input.rows(), input.cols());
  size_t count = input.cols() * input.rows();
  for (size_t index = 0; index < count; index++) {
    output(index) = std::exp(input(index));
  }
  return output;
}
CplxMatrix j(const Matrix& input) {
  CplxMatrix output(input.rows(), input.cols());
  size_t count = input.cols() * input.rows();
  for (size_t index = 0; index < count; index++) {
    output(index) = std::complex<float>(0, input(index));
  }
  return output;
}

void InplaceModulo(Matrix* matrix, float value) {
  size_t count = matrix->cols() * matrix->rows();
  for (size_t index = 0; index < count; index++) {
    (*matrix)(index) = std::fmod((*matrix)(index), value);
  }
}

// Class definitions
Freezer::Freezer() : params_(std::make_shared<Parameters>()) {}

void Freezer::Init(size_t channel_number, const std::string& wisdom,
                   size_t fft_size, float overlap_rate) {
  // initialize input values
  params_->input.resize(channel_number, 0);

  // Init parameters
  params_->sliding_buffer =
      Matrix::Zero(channel_number, (fft_size + kMaxBufferLen));
  params_->output_buffer =
      Matrix::Zero(channel_number, (fft_size + kMaxBufferLen));
  params_->fourier_transform =
      CplxMatrix::Zero(fft_size / 2 + 1, channel_number);
  params_->previous_fourier_transform =
      CplxMatrix::Zero(fft_size / 2 + 1, channel_number);
  params_->window = MakeSqrtHanningWindow(fft_size);

  params_->channel_number = channel_number;
  params_->nfft = fft_size;
  params_->hop_size = static_cast<size_t>(fft_size * (1.0 - overlap_rate));
  params_->index_sliding = fft_size - params_->hop_size;
  params_->is_on = false;
  params_->first_on = false;
  params_->just_on = false;
  params_->fft.Init(fft_size, wisdom);
}

void Freezer::Write(const std::vector<float>& data, std::error_code& err) {
  std::lock_guard<std::mutex> guard(mutex_);
  auto channel_number = params_->channel_number;

  // check if data buffer is valid
  if (data.size() % channel_number != 0) {
    err = std::make_error_code(std::errc::invalid_argument);
    return;
  }

  // get new input size
  auto current_frame_number = params_->input.cols();
  auto extra_frame_number = data.size() / channel_number;

  // resize
  params_->input.conservativeResize(channel_number,
                                    current_frame_number + extra_frame_number);

  // fill
  for (size_t col = current_frame_number; col < extra_frame_number; col++) {
    for (size_t row = 0; row < channel_number; row++) {
      params_->input(row, col) =
          data[(col - current_frame_number) * channel_number + row];
    }
  }
}

std::vector<float> Freezer::Read(std::error_code& err) {
  std::lock_guard<std::mutex> guard(mutex_);

  auto buffer_length = params_->input.cols();

  // slide output
  ShiftBuffer(&(params_->output_buffer), -buffer_length);

  params_->sliding_buffer.block(0, params_->index_sliding,
                                params_->channel_number, buffer_length) =
      params_->input;
  params_->index_sliding += buffer_length;

  // input is being processed
  params_->input.resize(params_->channel_number, 0);

  size_t out_buffer_offset = 0;  // for input buffer greater than nfft
  while (params_->index_sliding >= params_->nfft) {
    params_->previous_fourier_transform = params_->fourier_transform;

    // fft
    for (size_t channel = 0; channel < params_->channel_number; channel++) {
      auto analyzed_buffer =
          params_->sliding_buffer.block(channel, 0, 1, params_->nfft);
      Vector windowed_buffer =
          analyzed_buffer.transpose().array() * params_->window.array();

      //      auto fourier_at_channel = params_->fourier_transform.col(channel);
      //      fourier_at_channel = params_->fft.fwd(windowed_buffer,
      //      params_->nfft);
      auto ptr = params_->fourier_transform.data();
      auto first_idx = channel * params_->fourier_transform.rows();
      params_->fft.Forward(windowed_buffer.data(), ptr + first_idx);
    }

    // get freeze parameters
    if (params_->just_on) {
      params_->total_dphi = Angle(params_->fourier_transform);
      params_->dphi =
          params_->total_dphi - Angle(params_->previous_fourier_transform);
      params_->freeze_ft_magnitude = Abs(params_->fourier_transform);
      params_->just_on = false;
    }

    // update output if is_on
    if (params_->first_on) {//params_->is_on
      params_->total_dphi += params_->dphi;
      InplaceModulo(&(params_->total_dphi), 2 * M_PI);

      // modify output
      CplxMatrix modified_fft = params_->freeze_ft_magnitude.array() *
                                Exp(j(params_->total_dphi)).array();

      for (size_t channel = 0; channel < params_->channel_number; channel++) {
        auto ptr = modified_fft.data();
        auto first_idx = channel * modified_fft.rows();
        Vector inverse_fourier(params_->nfft);
        params_->fft.Inverse(ptr + first_idx, inverse_fourier.data());

        // get the right block
        auto output_block = params_->output_buffer.block(
            channel, out_buffer_offset, 1, params_->nfft);

        Matrix windowed_synthesis =
            inverse_fourier.real().array() * params_->window.array();
        output_block += windowed_synthesis.transpose();
      }

      out_buffer_offset += params_->hop_size;
    }

    ShiftBuffer(&(params_->sliding_buffer), -params_->hop_size);
    params_->index_sliding -= params_->hop_size;
  }

  auto output_data = params_->output_buffer.data();
  return std::vector<float>(
      output_data, output_data + (buffer_length * params_->channel_number));
}

void Freezer::Enable() {
  params_->first_on = true;
  if (!params_->is_on) {
    params_->just_on = true;
  }
  params_->is_on = true;
}

void Freezer::Disable() {
  params_->just_on = false;
  params_->is_on = false;
}

bool Freezer::IsEnabled() const { return params_->is_on; }

}  // namespace freeze
