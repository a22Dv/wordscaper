/**
 * screenshot.hpp
 * Declaration for the Screenshot class.
 * Allows for taking screenshots of the primary display.
 */

#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "utilities/types.hpp"

namespace wsr {

struct Rgba {
  uint8_t r = {};
  uint8_t g = {};
  uint8_t b = {};
  uint8_t a = {};
};

class Screenshot {
  mutable std::mutex threadMutex_ = {};
  mutable std::atomic<bool> terminate_ = {};
  mutable std::thread thread_{};
  mutable std::atomic<bool> acknowledged_ = {};
  mutable std::condition_variable cycle_ = {};

  std::atomic<std::chrono::milliseconds> interval_ = {};
  std::atomic<uint32_t> threadStatus_ = {};
  std::exception_ptr threadException_ = {};
  std::vector<Rgba> buffer_ = {};
  int screenWidth_ = {};
  int screenHeight_ = {};

  void threadExec_() noexcept;

 public:
  Screenshot(std::chrono::milliseconds interval = std::chrono::milliseconds{100});
  Screenshot(const Screenshot &) = delete;
  Screenshot &operator=(const Screenshot &) = delete;
  Screenshot(Screenshot &&) = delete;
  Screenshot &operator=(Screenshot &&) = delete;
  ~Screenshot();

  std::chrono::milliseconds interval() const noexcept;
  int screenHeight() const noexcept;
  int screenWidth() const noexcept;
  void setInterval(std::chrono::milliseconds interval);

  void take(std::vector<Rgba>& output) const;
  std::vector<Rgba> take() const;

  void takeNew(std::vector<Rgba>& output) const;
  std::vector<Rgba> takeNew() const;
};

}  // namespace wsr
