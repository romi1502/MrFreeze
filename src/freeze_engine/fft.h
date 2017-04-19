#ifndef FREEZE_FREEZE_FFT_H_
#define FREEZE_FREEZE_FFT_H_

#include <complex>
#include <memory>

namespace freeze {
class FFT {
 public:
  FFT();
  ~FFT();
  void Init(size_t nfft, const std::string& wisdom);
  void Forward(float* input, std::complex<float>* output);
  void Inverse(std::complex<float>* input, float* output);

 private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}  // namespace freeze

#endif  // FREEZE_FREEZE_FFT_H_
