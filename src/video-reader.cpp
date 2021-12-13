#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

int main(int argc, char* argv[]) {
  cv::VideoCapture video(argv[1]);
  if (!video.isOpened()) return 1;
  
  cv::Mat frame;
  
  while (true) {
    video >> frame;
    if (frame.empty()) break;
    
    cv::imshow("Rick", frame);
    
    char key = cv::waitKey(33);
    if (key == 27) break;
  }
  
  cv::destroyAllWindows();
}