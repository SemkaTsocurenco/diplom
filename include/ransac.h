#ifndef BIRDVIEW_PROCESSOR_H
#define BIRDVIEW_PROCESSOR_H

#include <opencv2/opencv.hpp>
#include <string>

class Ransac {
public:
    Ransac(const std::string& imagesDir, const std::string& configPath);
    void run();

private:
    int xOffsettop;          // Смещение по x от центра для точек
    int xOffsetdown;
    int topY;            // Высота верхних точек
    int bottomY;         // Высота нижних точек
    int size;
    int threshold;

    std::string videoPath;
    std::string configFile;
    const std::string& imagesDir;
    cv::VideoCapture cap;
    cv::Mat frame, birdview;
    cv::Mat invertBird;

    std::vector<cv::Point2f> points;
    int width , height;

    std::vector<int> xTrackbars;

    std::vector<int> yTrackbars;
    cv::Mat InverTtransformMatrix;
    cv::Size frameSize;

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

extern "C" void findLines(std::string vidname);

#endif // BIRDVIEW_PROCESSOR_H
