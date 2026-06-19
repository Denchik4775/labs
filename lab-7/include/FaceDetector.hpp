#pragma once
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <string>

class FaceDetector {
public:
    FaceDetector(const std::string& proto, const std::string& model) {
        net = cv::dnn::readNetFromCaffe(proto, model); 
        running = true;
        worker = std::thread(&FaceDetector::process, this);
    }
    ~FaceDetector() {
        running = false;
        if (worker.joinable()) worker.join();
    }
    void updateFrame(const cv::Mat& f) {
        std::lock_guard<std::mutex> lock(mtx);
        f.copyTo(frame);
    }
    std::vector<cv::Rect> getFaces() {
        std::lock_guard<std::mutex> lock(mtx);
        return faces;
    }
private:
    cv::dnn::Net net;
    std::thread worker;
    std::mutex mtx;
    std::atomic<bool> running;
    cv::Mat frame;
    std::vector<cv::Rect> faces;

    void process() {
        while (running) {
            cv::Mat locFrame;
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (frame.empty()) continue;
                frame.copyTo(locFrame);
            }
            
            cv::Mat blob = cv::dnn::blobFromImage(locFrame, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0)); 
            net.setInput(blob); 
            cv::Mat detection = net.forward(); 
            cv::Mat detMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

            std::vector<cv::Rect> locFaces;
            for (int i = 0; i < detMat.rows; i++) {
                if (detMat.at<float>(i, 2) > 0.5) { 
                    int x1 = static_cast<int>(detMat.at<float>(i, 3) * locFrame.cols);
                    int y1 = static_cast<int>(detMat.at<float>(i, 4) * locFrame.rows);
                    int x2 = static_cast<int>(detMat.at<float>(i, 5) * locFrame.cols);
                    int y2 = static_cast<int>(detMat.at<float>(i, 6) * locFrame.rows);
                    locFaces.push_back(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)));
                }
            }
            {
                std::lock_guard<std::mutex> lock(mtx);
                faces = locFaces; 
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); 
        }
    }
};
