#ifndef BIRDVIEW_PROCESSOR_H
#define BIRDVIEW_PROCESSOR_H

#include <opencv2/opencv.hpp>
#include <string>

class Ransac {
public:
    Ransac(const std::string& videoPath, const std::string& configPath);
    void run();

private:
    std::string videoPath;
    std::string configFile;
    cv::VideoCapture cap;
    cv::Mat frame, birdview;
    std::vector<cv::Point2f> points;
    int width , height;

    std::vector<int> xTrackbars;
    std::vector<int> yTrackbars;


    // Параметры RANSAC
    int maxTrials;
    int distanceThreshold;
    int minInliers;

    // Методы
    bool loadConfig();
    void saveConfig();
    void createTrackbars();
    void updateBirdview();
    void detectLaneLines();
    static void onTrackbar(int, void* userdata);
};

extern "C" void findLines(std::string videoPath);

#endif // BIRDVIEW_PROCESSOR_H
