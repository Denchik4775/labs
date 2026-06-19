#ifndef VIDEO_PROCESSOR_HPP
#define VIDEO_PROCESSOR_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include "FaceDetector.hpp"

class VideoProcessor {
private:
FaceDetector* fd;
bool faceMode;
    cv::VideoCapture cap;
    std::string windowName;
    int currentMode;
    int brightness;

    static void onMouse(int event, int x, int y, int flags, void* userdata);
    void handleMouse(int event, int x, int y, int flags);

public:
    VideoProcessor();
    ~VideoProcessor();

    bool initialize(int cameraId = 0);
    void run();
};

#endif
