#include "fft.h"

#include "fftw3.h"

namespace freeze {

class FFT::Impl {
 public:
  float nfft;
};
FFT::FFT() : impl_(std::make_shared<FFT::Impl>()) {}

void FFT::Init(size_t nfft) { impl_->nfft = nfft; }

void FFT::Forward(float* in, std::complex<float>* out) {
  auto plan = fftwf_plan_dft_r2c_1d(
      impl_->nfft, in, reinterpret_cast<fftwf_complex*>(out), FFTW_ESTIMATE);
  fftwf_execute(plan);
  fftwf_destroy_plan(plan);
}

void FFT::Inverse(std::complex<float>* in, float* out) {
  auto plan = fftwf_plan_dft_c2r_1d(
      impl_->nfft, reinterpret_cast<fftwf_complex*>(in), out, FFTW_ESTIMATE);
  fftwf_execute(plan);
  fftwf_destroy_plan(plan);
  for (size_t index = 0; index < impl_->nfft; index++) {
    out[index] /= impl_->nfft;
  }
}

}  // namespace freeze
