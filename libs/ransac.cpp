#include "../include/ransac.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem> // Для работы с директориями

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

// Конструктор класса
Ransac::Ransac(const std::string& imagesDir, const std::string& configPath)
    : imagesDir(imagesDir), configFile(configPath) {

    if (!fs::exists(imagesDir) || !fs::is_directory(imagesDir)) {
        cerr << "Директория не найдена: " << imagesDir << endl;
        exit(-1);
    }

    loadConfig();

    namedWindow("Birdview", WINDOW_AUTOSIZE);
    namedWindow("Canny", WINDOW_AUTOSIZE);

    createTrackbars();
}

// Загрузка конфигурации из YAML файла
bool Ransac::loadConfig() {
    FileStorage fs(configFile, FileStorage::READ);

    points.resize(4);
    fs["xOffsettop"] >> xOffsettop;
    fs["xOffsetdown"] >> xOffsetdown;
    fs["topY"] >> topY;
    fs["bottomY"] >> bottomY;

    fs["width"] >> width;
    fs["height"] >> height;
    fs["maxTrials"] >> maxTrials;
    fs["distanceThreshold"] >> distanceThreshold;
    fs["minInliers"] >> minInliers;
    fs["threshold"] >> threshold;
    fs["size"] >> size;

    fs.release();
    return true;
}

// Сохранение конфигурации в YAML файл
void Ransac::saveConfig() {
    FileStorage fs(configFile, FileStorage::WRITE);

    fs << "xOffsettop" << xOffsettop;
    fs << "xOffsetdown" << xOffsetdown;
    fs << "topY" << topY;
    fs << "bottomY" << bottomY;
    fs << "width" << width;
    fs << "height" << height;
    fs << "maxTrials" << maxTrials;
    fs << "distanceThreshold" << distanceThreshold;
    fs << "minInliers" << minInliers;
    fs << "threshold" << threshold;
    fs << "size" << size;

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

    frameSize = frame.size();
    Mat transformMatrix = getPerspectiveTransform(points, dstPoints);
    InverTtransformMatrix = getPerspectiveTransform(dstPoints, points);

    warpPerspective(frame, birdview, transformMatrix, Size(width, height));
}

// Обнаружение линий методом RANSAC
void Ransac::detectLaneLines() {
    Mat gray;
    cvtColor(birdview, gray, COLOR_BGR2GRAY);

    Canny(gray, gray, size, threshold);

    vector<Vec4i> lines;
    HoughLinesP(gray, lines, 1, CV_PI / 180, minInliers, distanceThreshold, maxTrials);

    Mat result = Mat::zeros(birdview.size(), birdview.type());
    
    for (const auto& line : lines) {
        cv::line(birdview, Point(line[0], line[1]), Point(line[2], line[3]), Scalar(0, 255, 0), 2);
        cv::line(result, Point(line[0], line[1]), Point(line[2], line[3]), Scalar(255, 255, 255), 2);
    }
    warpPerspective(result, invertBird, InverTtransformMatrix, frame.size());

    imshow("Birdview", birdview);
    imshow("invert", invertBird);
    imshow("Canny", gray);
    cv::waitKey();
}

// Callback функция для трекбаров
void Ransac::onTrackbar(int, void* userdata) {
    auto* processor = reinterpret_cast<Ransac*>(userdata);

    int centerX = processor->frame.cols / 2;

    // Обновляем точки
    processor->points[0].x = centerX - processor->xOffsettop; // Левая верхняя
    processor->points[0].y = processor->topY;
    processor->points[1].x = centerX + processor->xOffsettop; // Правая верхняя
    processor->points[1].y = processor->topY;

    processor->points[2].x = centerX + processor->xOffsetdown; // Правая нижняя
    processor->points[2].y = processor->bottomY;
    processor->points[3].x = centerX - processor->xOffsetdown; // Левая нижняя
    processor->points[3].y = processor->bottomY;

    processor->updateBirdview();
    processor->detectLaneLines();

}


void Ransac::createTrackbars() {
    int maxCols = frame.cols > 0 ? frame.cols - 1 : 1000;
    int maxRows = frame.rows > 0 ? frame.rows - 1 : 1000;

    createTrackbar("X Offset top", "Birdview", &xOffsettop, maxCols+400, onTrackbar, this);
    createTrackbar("X Offset down", "Birdview", &xOffsetdown, maxCols+400, onTrackbar, this);

    createTrackbar("Top Y", "Birdview", &topY, maxRows, onTrackbar, this);
    createTrackbar("Bottom Y", "Birdview", &bottomY, maxRows, onTrackbar, this);

    // Параметры RANSAC
    createTrackbar("width", "Birdview", &width, 500, onTrackbar, this);
    createTrackbar("height", "Birdview", &height, 500, onTrackbar, this);

    createTrackbar("Max Trials", "Birdview", &maxTrials, 500, onTrackbar, this);
    createTrackbar("Distance Threshold", "Birdview", &distanceThreshold, 50, onTrackbar, this);
    createTrackbar("Min Inliers", "Birdview", &minInliers, 200, onTrackbar, this);

    createTrackbar("Canny threshold", "Canny", &threshold, 1000, onTrackbar, this);
    createTrackbar("Canny size", "Canny", &size, 500, onTrackbar, this);
}

// Запуск обработки изображений
void Ransac::run() {
    string outputDir = "images_ransac";
    if (!fs::exists(outputDir)) {
        fs::create_directory(outputDir);
    }

    int index = 0;
    for (const auto& entry : fs::directory_iterator(imagesDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".png") {
            frame = imread(entry.path().string());
            if (frame.empty()) {
                cerr << "Не удалось загрузить изображение: " << entry.path() << endl;
                continue;
            }

            updateBirdview();
            detectLaneLines();

            // Сохранение результата
            string outputFilename = outputDir + "/Images_" + to_string(index++) + ".png";
            imwrite(outputFilename, invertBird);

            char key = (char)waitKey(10);
            saveConfig();

            if (key == 'q') {
                break;
            }
        }
    }
}

// Функция для вызова из динамической библиотеки
extern "C" void findLines(const std::string imagesDir) {
    const std::string& configPath = "../res/ransac_parameters.yaml";

    Ransac processor(imagesDir, configPath);
    processor.run();
}
