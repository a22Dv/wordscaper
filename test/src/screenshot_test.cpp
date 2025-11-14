#include "visual/screenshot.hpp"
#include "utilities/profiling.hpp"

#include <opencv2/highgui.hpp>
#include <vector>

int main() {
  WSR_PROFILESCOPE();
  wsr::Screenshot ss = {};

  std::vector<wsr::Rgba> shot = ss.take();
  cv::Mat matS = {ss.screenHeight(), ss.screenWidth(), CV_8UC4, shot.data()};
  cv::imshow("Screenshot", matS);
  cv::waitKey(0);

  std::vector<wsr::Rgba> shotN = ss.takeNew();
  cv::Mat matSN = {ss.screenHeight(), ss.screenWidth(), CV_8UC4, shotN.data()};
  cv::imshow("Screenshot", matSN);
  cv::waitKey(0);
}