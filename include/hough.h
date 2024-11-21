#ifndef HOUGH_H
#define HOUGH_H

#include <opencv2/opencv.hpp>
#include <string>

class Hough {
public:
    Hough(const std::string& imagesDir, const std::string& configPath);
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

    // Параметры Hough
    int Rho;
    int Theta;
    int Threshold;

    // Методы
    bool loadConfig();
    void saveConfig();
    void createTrackbars();
    void updateBirdview();
    void detectLaneLines();
    static void onTrackbar(int, void* userdata);
};

extern "C" void findLines(std::string vidname);

#endif // HOUGH_H
