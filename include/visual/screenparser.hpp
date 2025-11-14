/**
 * screenparser.hpp
 *
 * Declaration for the ScreenParser class.
 * Extracts game state from a given screenshot.
 */

#pragma once

#include <utility>
#include <variant>
#include <optional>

// Third-party.
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

#include "utilities/types.hpp"
#include "visual/screenshot.hpp"

namespace wsr {

/**
 * Layout:
 * `'\0' (0x0)` - Out of bounds cells.
 * `' ' (0x20)` - Empty cells.
 * `'A'-'Z'` - Characters.
 */
struct LevelGrid {
  std::vector<char> grid = {};  // Row-major order.
  size_t gridHeight = {};
  size_t gridWidth = {};

  char &operator[](Point point);
  const char &operator[](Point point) const;
};

struct Letter {
  char letter = {};
  cv::Rect location = {};
};

struct Level {
  cv::Rect letterWheel = {};
  cv::Rect gridLocation = {};
  std::vector<Letter> letters = {};
  LevelGrid grid = {};
};

struct MainMenu {
  cv::Rect levelButton = {};
};

struct Game {
  cv::Rect windowLocation = {};
  GameState associatedState = GameState::UNKNOWN;
  std::variant<MainMenu, Level> game{};
};

class ScreenParser {
  Screenshot screenshot_ = {};
  std::vector<Rgba> screenBuffer_ = {};
  cv::Mat screenMat_ = {};
  GameState state_ = GameState::UNKNOWN;
  cv::Rect lastKnownGameWindowLocation_ = {};
  std::vector<cv::Mat> letterTemplates_ = {};

  std::pair<double, Letter> matchLetter_(const cv::Mat& mat, cv::Rect location);
  std::optional<MainMenu> getMainMenu_();
  std::optional<Level> getLevel_();
 public:
  ScreenParser();

  const std::vector<Rgba> &screenBuffer() const noexcept;
  
  std::optional<Game> parse();
};

}  // namespace wsr
