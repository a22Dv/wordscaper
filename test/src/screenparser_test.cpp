#include "visual/screenparser.hpp"

#include <iostream>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/opencv.hpp>

int main() {
  cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
  try {
    wsr::ScreenParser parser = {};
    parser.parse();
  } catch (std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}