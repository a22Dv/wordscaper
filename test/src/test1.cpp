#include <opencv2/highgui.hpp>

int main() {
  auto mat = cv::Mat{500, 500, CV_8UC4, cv::Scalar{0, 0, 128, 0}};
  cv::imshow("Window name", mat);
  cv::waitKey(0);
}