#include "VideoProcessor.hpp"
#include <iostream>

VideoProcessor::VideoProcessor() : windowName("Lab 6 - OpenCV"), currentMode(0), brightness(50), faceMode(false) {
    fd = new FaceDetector("deploy.prototxt", "res10_300x300_ssd_iter_140000.caffemodel");
}

VideoProcessor::~VideoProcessor() {
    delete fd;
    if (cap.isOpened()) {
        cap.release();
    }
    cv::destroyAllWindows();
}

void VideoProcessor::onMouse(int event, int x, int y, int flags, void* userdata) {
    VideoProcessor* vp = reinterpret_cast<VideoProcessor*>(userdata);
    vp->handleMouse(event, x, y, flags);
}

void VideoProcessor::handleMouse(int event, int x, int y, int flags) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        std::cout << "Mouse Click: (" << x << ", " << y << ")" << std::endl;
        currentMode = (currentMode + 1) % 4;
    }
}

bool VideoProcessor::initialize(int cameraId) {
    cap.open(cameraId, cv::CAP_V4L2);
    if (!cap.isOpened()) {
        std::cerr << "Camera Error!" << std::endl;
        return false;
    }
    
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(windowName, onMouse, this);
    cv::createTrackbar("Brightness", windowName, &brightness, 100);
    
    return true;
}

void VideoProcessor::run() {
    cv::Mat frame, processedFrame;
    
    std::cout << "\n1 - RGB\n2 - Grayscale\n3 - Blur\n4 - Canny Edge\nLMB - Switch\nf/F - Face Detection\nESC/q - Exit\n\n";

    double fps = 0.0;
    int frameCount = 0;
    int64 startTick = cv::getTickCount();

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            break;
        }

        switch (currentMode) {
            case 1:
                cv::cvtColor(frame, processedFrame, cv::COLOR_BGR2GRAY);
                cv::cvtColor(processedFrame, processedFrame, cv::COLOR_GRAY2BGR);
                break;
            case 2:
                cv::GaussianBlur(frame, processedFrame, cv::Size(15, 15), 0);
                break;
            case 3:
                cv::cvtColor(frame, processedFrame, cv::COLOR_BGR2GRAY);
                cv::Canny(processedFrame, processedFrame, 50, 150);
                cv::cvtColor(processedFrame, processedFrame, cv::COLOR_GRAY2BGR);
                break;
            default:
                processedFrame = frame.clone();
                break;
        }

        processedFrame.convertTo(processedFrame, -1, 1.0, brightness - 50);

        if (faceMode) {
            fd->updateFrame(frame);
            for (const auto& face : fd->getFaces()) {
                cv::rectangle(processedFrame, face, cv::Scalar(0, 255, 0), 2);
            }
        }

        frameCount++;
        int64 currentTick = cv::getTickCount();
        double timeElapsed = (currentTick - startTick) / cv::getTickFrequency();
        if (timeElapsed >= 1.0) {
            fps = frameCount / timeElapsed;
            frameCount = 0;
            startTick = currentTick;
        }
        cv::putText(processedFrame, "FPS: " + std::to_string((int)fps), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);

        cv::imshow(windowName, processedFrame);

        char key = (char)cv::waitKey(30);
        if (key == 27 || key == 'q' || key == 'Q') {
            break;
        } else if (key == 'f' || key == 'F') {
            faceMode = !faceMode;
        } else if (key == '1') {
            currentMode = 0;
        } else if (key == '2') {
            currentMode = 1;
        } else if (key == '3') {
            currentMode = 2;
        } else if (key == '4') {
            currentMode = 3;
        }
    }
}
