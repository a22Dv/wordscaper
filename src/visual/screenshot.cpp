/**
 * screenshot.cpp
 * Implementation for screenshot.hpp
 */

#ifdef _WIN64
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#error Requires a Windows (x64) compilation target.
#endif

#include <algorithm>
#include <chrono>
#include <climits>
#include <mutex>
#include <system_error>
#include <thread>

#include "utilities/profiling.hpp"
#include "utilities/types.hpp"
#include "visual/screenshot.hpp"

namespace chr = std::chrono;

namespace {

void winRequire(bool condition, const char *message) {
  if (!condition) [[unlikely]] {
    throw std::system_error(GetLastError(), std::system_category(), message);
  }
}

constexpr const char *smFailure = "Failed to get system metrics.";
constexpr const char *threadFailure = "Thread encountered an unexpected failure.";
constexpr const char *dcFailure = "Failed to get or create device context.";
constexpr const char *sObjFailure = "SelectObject() encountered an unexpected failure.";
constexpr const char *bitbltFailure = "Bit-Block Transfer encountered an unexpected failure.";
constexpr const char *dibFailure = "Bitmap to buffer copy encountered an unexpected failure.";

struct GdiData {
  HDC screenDc = {};
  HDC memoryDc = {};
  HBITMAP exBitmap = {};
  HBITMAP bitmap = {};
  BITMAPINFOHEADER bitmapInfo = {};

  GdiData(int sx, int sy) {
    screenDc = GetDC(nullptr);
    winRequire(screenDc, dcFailure);

    memoryDc = CreateCompatibleDC(screenDc);
    winRequire(memoryDc, dcFailure);

    bitmap = CreateCompatibleBitmap(screenDc, sx, sy);
    winRequire(bitmap, dcFailure);

    exBitmap = static_cast<HBITMAP>(SelectObject(memoryDc, bitmap));
    winRequire(exBitmap, sObjFailure);

    bitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.biBitCount = sizeof(wsr::Rgba) * CHAR_BIT;
    bitmapInfo.biCompression = BI_RGB;
    bitmapInfo.biHeight = -sy;
    bitmapInfo.biWidth = sx;
    bitmapInfo.biPlanes = 1;
  }
  GdiData(const GdiData &) = delete;
  GdiData &operator=(const GdiData &) = delete;
  GdiData(GdiData &&) = delete;
  GdiData &operator=(GdiData &&) = delete;
  ~GdiData() {
    SelectObject(memoryDc, exBitmap);
    DeleteObject(bitmap);
    DeleteDC(memoryDc);
    ReleaseDC(nullptr, screenDc);
  }
};

}  // namespace

namespace wsr {

void Screenshot::threadExec_() noexcept {
  /**
   * BUG:
   * Metrics are only queried once during construction.
   * Changing the screen resolution during the program's lifetime
   * will result in undefined behavior.
   */
  try {
    const int sx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int sy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    auto staging = std::vector<Rgba>(buffer_.size());
    GdiData gdiData(screenWidth_, screenHeight_);

    while (!terminate_.load()) {
      WSR_PROFILESCOPEN("ITERATION SS_THREADEXEC");

      BOOL winRt = {};
      {
        WSR_PROFILESCOPEN("BITBLT"); // ~30ms
        winRt = BitBlt(
            gdiData.memoryDc,  // Destination DC.
            0, 0,              // Offset on destination.
            screenWidth_,      // Bitmap width.
            screenHeight_,     // Bitmap height.
            gdiData.screenDc,  // Source DC.
            sx, sy,            // Offset on source.
            SRCCOPY            // Regular copy
        );
      }
      winRequire(winRt, bitbltFailure);

      {
        WSR_PROFILESCOPEN("DIBITS"); // ~7-9ms
        winRt = GetDIBits(
            gdiData.memoryDc,                                     // Source DC.
            gdiData.bitmap,                                       // Attached bitmap.
            0, screenHeight_,                                     // Line offset and height.
            staging.data(),                                       // Destination pointer.
            reinterpret_cast<LPBITMAPINFO>(&gdiData.bitmapInfo),  // Bitmap info.
            DIB_RGB_COLORS                                        // Setting.
        );
      }
      winRequire(winRt, dibFailure);

      {  // Critical section start.
        std::unique_lock<std::mutex> lock(threadMutex_);
        std::swap(staging, buffer_);
        acknowledged_.store(true);
      }  // Critical section end.

      cycle_.notify_one();
      std::this_thread::sleep_for(interval_.load());
    }

  } catch (const std::system_error &e) {
    threadStatus_.store(e.code().value());
    cycle_.notify_one();

  } catch (...) {              // For any std::* related exceptions.
    threadStatus_.store(0xA);  // ERROR_BAD_ENVIRONMENT.
    cycle_.notify_one();
  }
}

Screenshot::Screenshot(chr::milliseconds interval) {
  /**
   * BUG:
   * Metrics are only queried once during construction.
   * Changing the screen resolution during the program's lifetime
   * will result in undefined behavior.
   */
  screenHeight_ = GetSystemMetrics(SM_CYVIRTUALSCREEN);
  screenWidth_ = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  winRequire(screenHeight_ && screenWidth_, smFailure);

  interval_.store(interval);
  buffer_.resize(screenHeight_ * screenWidth_);

  std::unique_lock<std::mutex> lock(threadMutex_);
  acknowledged_.store(false);
  thread_ = std::thread([this] { threadExec_(); });
  cycle_.wait(lock, [this] {  // Handle startup failure.
    return threadStatus_.load() || acknowledged_.load();
  });
  const uint32_t status = threadStatus_.load();
  if (status) {
    terminate_.store(true);
    if (thread_.joinable()) {
      thread_.join();
    }
    throw std::system_error(status, std::system_category(), threadFailure);
  }
}

Screenshot::~Screenshot() {
  terminate_.store(true);
  if (thread_.joinable()) {
    thread_.join();
  }
}

chr::milliseconds Screenshot::interval() const noexcept { return interval_.load(); }
int Screenshot::screenHeight() const noexcept { return screenHeight_; }
int Screenshot::screenWidth() const noexcept { return screenWidth_; }
void Screenshot::setInterval(std::chrono::milliseconds interval) { interval_.store(interval); }

void Screenshot::take(std::vector<Rgba> &output) const {
  if (threadStatus_.load()) {
    throw std::system_error(threadStatus_.load(), std::system_category(), threadFailure);
  }

  output.resize(screenHeight_ * screenWidth_);
  {  // Critical section start.
    std::lock_guard<std::mutex> lock(threadMutex_);
    std::copy(buffer_.begin(), buffer_.end(), output.begin());
  }  // Critical section end.
}
std::vector<Rgba> Screenshot::take() const {
  auto output = std::vector<Rgba>(screenHeight_ * screenWidth_);
  take(output);
  return output;
}

void Screenshot::takeNew(std::vector<Rgba> &output) const {
  const uint32_t statusBefore = threadStatus_.load();
  if (statusBefore) {
    throw std::system_error(statusBefore, std::system_category(), threadFailure);
  }

  output.resize(screenHeight_ * screenWidth_);
  {  // Critical section start.
    std::unique_lock<std::mutex> lock(threadMutex_);
    acknowledged_.store(false);
    cycle_.wait(lock, [this] { return acknowledged_.load() || threadStatus_.load(); });
    const uint32_t statusAfter = threadStatus_.load();
    if (statusAfter) {
      throw std::system_error(statusAfter, std::system_category(), threadFailure);
    }
    std::copy(buffer_.begin(), buffer_.end(), output.begin());
  }  // Critical section end.
}
std::vector<Rgba> Screenshot::takeNew() const {
  auto output = std::vector<Rgba>(screenHeight_ * screenWidth_);
  takeNew(output);
  return output;
}

}  // namespace wsr