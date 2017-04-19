#include <cstring>
#include <iostream>

#include "fft.h"
#include "fftw3.h"

namespace freeze {

class FFT::Impl {
 public:
  float nfft;
  fftwf_plan forward_plan;
  fftwf_plan backward_plan;
  float* forward_in;
  fftwf_complex* forward_out;
  fftwf_complex* backward_in;
  float* backward_out;
  bool plan_initialized;
};
FFT::FFT() : impl_(std::make_shared<FFT::Impl>()) {
  impl_->plan_initialized = false;
}

FFT::~FFT() {
  if (!impl_->plan_initialized) {
    return;
  }
  fftwf_destroy_plan(impl_->forward_plan);
  fftwf_destroy_plan(impl_->backward_plan);
}

void FFT::Init(size_t nfft, const std::string& wisdom) {
  impl_->nfft = nfft;

  auto fftw_flags = FFTW_WISDOM_ONLY | FFTW_MEASURE;
  auto success = fftwf_import_wisdom_from_filename(wisdom.c_str());
  if (!success) {
    fftw_flags = FFTW_MEASURE;
    std::cout << "Couldn't import wisdom file: " << wisdom
              << ". Using estimate instead." << std::endl;
  }

  // forward plan
  impl_->forward_in = new float[nfft];
  impl_->forward_out = new fftwf_complex[nfft/2 + 1];
  impl_->forward_plan = fftwf_plan_dft_r2c_1d(impl_->nfft,
                                              impl_->forward_in,
                                              impl_->forward_out,
                                              fftw_flags);

  // backward plan
  impl_->backward_in = new fftwf_complex[nfft/2 + 1];
  impl_->backward_out = new float[nfft];
  impl_->backward_plan = fftwf_plan_dft_c2r_1d(impl_->nfft,
                                               impl_->backward_in,
                                               impl_->backward_out,
                                               fftw_flags);

  impl_->plan_initialized = true;
}

void FFT::Forward(float* in, std::complex<float>* out) {
  std::memcpy(impl_->forward_in, in,
              (impl_->nfft) * sizeof( float ) );
  fftwf_execute(impl_->forward_plan);
  std::memcpy(out, impl_->forward_out,
              (impl_->nfft/2 +1) * sizeof( fftwf_complex ) );

}

void FFT::Inverse(std::complex<float>* in, float* out) {
  std::memcpy(impl_->backward_in, in,
              (impl_->nfft/2 +1) * sizeof( fftwf_complex ) );

  fftwf_execute(impl_->backward_plan);
  for (size_t index = 0; index < impl_->nfft; index++) {
    out[index] = impl_->backward_out[index]/impl_->nfft;
  }
}

}  // namespace freeze
