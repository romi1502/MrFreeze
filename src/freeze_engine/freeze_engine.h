#ifndef FREEZE_FREEZE_FREEZE_H_
#define FREEZE_FREEZE_FREEZE_H_

#include <mutex>
#include <system_error>
#include <vector>
#include <memory>

namespace freeze {

class Freezer {
 public:
  Freezer();
  void Init(size_t channel_number, const std::string& wisdom,
            size_t fft_size = 2048, float overlap_rate = 0.5);

  void Write(const std::vector<float>& data, std::error_code& err);
  std::vector<float> Read(std::error_code& err);

  void Enable();
  void Disable();
  bool IsEnabled() const;

 private:
  std::mutex mutex_;

  struct Parameters;
  using ParametersPtr = std::shared_ptr<Parameters>;
  ParametersPtr params_;
};

}  // namespace freeze

#endif  // FREEZE_FREEZE_FREEZE_H_
