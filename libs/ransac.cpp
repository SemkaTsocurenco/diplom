#include "../include/ransac.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Конструктор класса
Ransac::Ransac(const string& videoPath, const string& configPath)
    : videoPath(videoPath), configFile(configPath) {
    cap.open(videoPath);
    if (!cap.isOpened()) {
        cerr << "Не удалось открыть видеофайл: " << videoPath << endl;
        exit(-1);
    }

    cout<<"aaa\n";
    if (!loadConfig()) {
        points = {
            Point2f(100, 100),
            Point2f(cap.get(CAP_PROP_FRAME_WIDTH) - 100, 100),
            Point2f(cap.get(CAP_PROP_FRAME_WIDTH) - 100, cap.get(CAP_PROP_FRAME_HEIGHT) - 100),
            Point2f(100, cap.get(CAP_PROP_FRAME_HEIGHT) - 100)
        };
    }

cout<<"aaa1\n";
    namedWindow("Birdview", WINDOW_AUTOSIZE);
    cout<<"aaa2\n";

    createTrackbars();
    cout<<"aaa3\n";

}

// Загрузка конфигурации из YAML файла
bool Ransac::loadConfig() {
    FileStorage fs(configFile, FileStorage::READ);
    FileNode pts = fs["points"];

    points.resize(4);  
    for (int i = 0; i < 4; i++) {
        // Читаем координаты
        pts[i][0] >> points[i].x;
        pts[i][1] >> points[i].y;
    }
    fs["width"] >> width;
    fs["height"] >> height;
    fs["maxTrials"] >> maxTrials;
    fs["distanceThreshold"] >> distanceThreshold;
    fs["minInliers"] >> minInliers;
    fs.release();
    return true;
}



// Сохранение конфигурации в YAML файл
void Ransac::saveConfig() {
    FileStorage fs(configFile, FileStorage::WRITE);
    fs << "points" << "[";
    for (const auto& point : points) {
        fs << "[" << point.x << point.y << "]";
    }
    fs << "]";
    fs << "width" << width;
    fs << "height" << height;
    fs << "maxTrials" << maxTrials;
    fs << "distanceThreshold" << distanceThreshold;
    fs << "minInliers" << minInliers;
    fs.release();
}

// Обновление birdview
void Ransac::updateBirdview() {
    vector<Point2f> dstPoints = {
        Point2f(0, 0),
        Point2f(width - 1, 0),
        Point2f(width - 1, height - 1),
        Point2f(0, height - 1)
    };

    Mat transformMatrix = getPerspectiveTransform(points, dstPoints);
    warpPerspective(frame, birdview, transformMatrix, Size(width, height));
}

// Обнаружение линий методом RANSAC
void Ransac::detectLaneLines() {
    Mat gray;
    cvtColor(birdview, gray, COLOR_BGR2GRAY);
    Canny(gray, gray, 50, 150);

    vector<Vec4i> lines;
    HoughLinesP(gray, lines, 1, CV_PI / 180, minInliers, distanceThreshold, maxTrials);

    Mat result = birdview.clone();
    for (const auto& line : lines) {
        cv::line(result, Point(line[0], line[1]), Point(line[2], line[3]), Scalar(0, 255, 0), 2);
    }

    imshow("Detected Lines", result);
}


// Callback функция для трекбаров
void Ransac::onTrackbar(int, void* userdata) {
    auto* processor = reinterpret_cast<Ransac*>(userdata);

    // Обновляем значения в points на основе значений трекбаров
    for (int i = 0; i < 4; i++) {
        processor->points[i].x = static_cast<float>(processor->xTrackbars[i]);
        processor->points[i].y = static_cast<float>(processor->yTrackbars[i]);
    }

    // Обновляем birdview и ищем линии
    processor->updateBirdview();
    processor->detectLaneLines();
}


// Создание трекбаров
void Ransac::createTrackbars() {
    int maxCols = frame.cols > 0 ? frame.cols - 1 : 1000;
    int maxRows = frame.rows > 0 ? frame.rows - 1 : 1000;

    // Инициализируем векторы трекбаров
    xTrackbars.resize(4);
    yTrackbars.resize(4);

    for (int i = 0; i < 4; i++) {
        // Инициализируем трекбары значениями из points
        xTrackbars[i] = static_cast<int>(points[i].x);
        yTrackbars[i] = static_cast<int>(points[i].y);

        // Создаём трекбары для каждой координаты
        createTrackbar("X" + std::to_string(i), "Birdview", &xTrackbars[i], maxCols, onTrackbar, this);
        createTrackbar("Y" + std::to_string(i), "Birdview", &yTrackbars[i], maxRows, onTrackbar, this);
    }

    // Трекбары для параметров RANSAC
    createTrackbar("width", "Birdview", &width, 500, onTrackbar, this);
    createTrackbar("height", "Birdview", &height, 500, onTrackbar, this);

    createTrackbar("Max Trials", "Birdview", &maxTrials, 500, onTrackbar, this);
    createTrackbar("Distance Threshold", "Birdview", &distanceThreshold, 50, onTrackbar, this);
    createTrackbar("Min Inliers", "Birdview", &minInliers, 200, onTrackbar, this);
}


// Запуск обработки видео
void Ransac::run() {
    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        updateBirdview();
        detectLaneLines();

        char key = (char)waitKey(10);
        if (key == 'q'){
            saveConfig();
            break;
        } 
    }
}

// Функция для вызова из динамической библиотеки
extern "C" void findLines(std::string videoPath) {
    string configPath = "../res/ransac_parameters.yaml";

    Ransac processor(videoPath, configPath);
    processor.run();
}