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
};
FFT::FFT() : impl_(std::make_shared<FFT::Impl>()) {}

void FFT::Init(size_t nfft) {
    impl_->nfft = nfft;

    // forward plan
    impl_->forward_in = new float[nfft];
    impl_->forward_out = new fftwf_complex[nfft/2 + 1];
    impl_->forward_plan = fftwf_plan_dft_r2c_1d(impl_->nfft, impl_->forward_in, impl_->forward_out, FFTW_MEASURE);

    // backward plan
    impl_->backward_in = new fftwf_complex[nfft/2 + 1];
    impl_->backward_out = new float[nfft];
    impl_->backward_plan = fftwf_plan_dft_c2r_1d(impl_->nfft, impl_->backward_in, impl_->backward_out, FFTW_MEASURE);

  }
/*fftwf_destroy_plan(impl_->forward_plan);*/
/*fftwf_destroy_plan(impl_->backward_plan);*/

void FFT::Forward(float* in, std::complex<float>* out) {
  memcpy( impl_->forward_in, in, (impl_->nfft) * sizeof( float ) );
  fftwf_execute(impl_->forward_plan);
  memcpy( out, impl_->forward_out, (impl_->nfft/2 +1) * sizeof( fftwf_complex ) );

}

void FFT::Inverse(std::complex<float>* in, float* out) {
  memcpy( impl_->backward_in, in, (impl_->nfft/2 +1) * sizeof( fftwf_complex ) );

  fftwf_execute(impl_->backward_plan);
  for (size_t index = 0; index < impl_->nfft; index++) {
    out[index] = impl_->backward_out[index]/impl_->nfft;
  }
}

}  // namespace freeze
