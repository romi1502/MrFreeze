#ifndef FREEZE_UTIL_QUEUE_H_
#define FREEZE_UTIL_QUEUE_H_

#include <cassert>

namespace freeze {
/**
 * A simple circular queue
 */
template <typename T>
class Queue {
 public:
  Queue(size_t content_size) :
    data_(content_size),
    front_index_(0),
    back_index_(0) {}

  void push(T value) {
    data_[back_index_] = value;
    back_index_++;
    if (back_index_ == data_.size()) {
      back_index_ = 0;
    }
  }

  T front() const {
    return data_[front_index_];
  }

  void pop() {
    assert(front_index_ + 1 < back_index_);
    front_index_++;
    if (front_index_ == data_.size()) {
      front_index_ = 0;
    }
  }

  bool empty() const {
    return (front_index_ == back_index_);
  }

  size_t size() const {
    auto size = front_index_ - back_index_;
    if (size < 0) {
      size += data_.size();
    }
    return size;
  }

 private:
  std::vector<T> data_;
  size_t front_index_;
  size_t back_index_;
};
}  // namespace freeze

#endif  // FREEZE_UTIL_QUEUE_H_
