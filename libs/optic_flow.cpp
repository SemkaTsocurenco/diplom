#include "../include/main.hpp"

// Параметры
int flow_threshold = 10;
int maxCorners = 500;
int qualityLevel = 10;
int minDistance = 10;
int cannyThreshold1 = 50;
int cannyThreshold2 = 150;

// Загрузка параметров из YAML файла
void loadOpticalFlowParams(const std::string& filename) {
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
        return;
    }
    fs["Flow_Threshold"] >> flow_threshold;
    fs["maxCorners"] >> maxCorners;
    fs["qualityLevel"] >> qualityLevel;
    fs["minDistance"] >> minDistance;
    fs["cannyThreshold1"] >> cannyThreshold1;
    fs["cannyThreshold2"] >> cannyThreshold2;
    fs.release();
    std::cout << "Параметры оптического потока загружены." << std::endl;
}

// Сохранение параметров в YAML файл
void saveOpticalFlowParams(const std::string& filename) {
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
        return;
    }
    fs << "Flow_Threshold" << flow_threshold;
    fs << "maxCorners" << maxCorners;
    fs << "qualityLevel" << qualityLevel;
    fs << "minDistance" << minDistance;
    fs << "cannyThreshold1" << cannyThreshold1;
    fs << "cannyThreshold2" << cannyThreshold2;
    fs.release();
    std::cout << "Параметры оптического потока сохранены." << std::endl;
}

// Callback функция для трекбаров
void on_trackbar(int, void*) {}

// Метод для обнаружения дорожной разметки с использованием оптического потока
void findLines(std::string vidname) {
    cv::VideoCapture cap("/home/tsokurenkosv/Downloads/video2.mp4");

    if (!cap.isOpened()) {
        std::cerr << "Ошибка: Не удалось открыть видеофайл!" << std::endl;
        return;
    }

    loadOpticalFlowParams("../res/optical_flow_parameters.yaml");

    cv::Mat prevGray, gray, frame, edges;
    cap >> frame;
    if (frame.empty()) {
        std::cerr << "Ошибка: Не удалось захватить кадр!" << std::endl;
        return;
    }

    // Фокусируемся на нижней части кадра
    frame = frame(cv::Rect(0, frame.rows / 2, frame.cols, frame.rows / 2));
    cv::cvtColor(frame, prevGray, cv::COLOR_BGR2GRAY);

    cv::namedWindow("Optical Flow", cv::WINDOW_AUTOSIZE);
    cv::createTrackbar("Flow Threshold", "Optical Flow", &flow_threshold, 50, on_trackbar);
    cv::createTrackbar("maxCorners", "Optical Flow", &maxCorners, 2000, on_trackbar);
    cv::createTrackbar("qualityLevel", "Optical Flow", &qualityLevel, 95, on_trackbar);
    cv::createTrackbar("minDistance", "Optical Flow", &minDistance, 300, on_trackbar);
    cv::createTrackbar("Canny Thresh 1", "Optical Flow", &cannyThreshold1, 200, on_trackbar);
    cv::createTrackbar("Canny Thresh 2", "Optical Flow", &cannyThreshold2, 300, on_trackbar);

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // Фокусируемся на нижней части кадра
        frame = frame(cv::Rect(0, frame.rows / 2, frame.cols, frame.rows / 2));
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Применяем фильтр Кэнни для выделения границ
        cv::Canny(gray, edges, cannyThreshold1, cannyThreshold2);

        // Вычисление оптического потока
        std::vector<cv::Point2f> prevPoints, currPoints;
        std::vector<uchar> status;
        std::vector<float> err;
        double qualityLevelDouble = static_cast<double>(qualityLevel + 1);

        // Инициализация точек для отслеживания на выделенных границах
        cv::goodFeaturesToTrack(edges, prevPoints, maxCorners, qualityLevelDouble / 100, minDistance);
        if (prevPoints.empty()) continue;

        // Оптический поток Лукаса-Канаде
        cv::calcOpticalFlowPyrLK(prevGray, gray, prevPoints, currPoints, status, err);

        // Отображение потока
        cv::Mat flowFrame = frame.clone();
        for (size_t i = 0; i < currPoints.size(); i++) {
            if (status[i]) {
                double flow = cv::norm(currPoints[i] - prevPoints[i]);
                if (flow > flow_threshold) {
                    cv::line(flowFrame, prevPoints[i], currPoints[i], cv::Scalar(0, 255, 0), 2);
                    cv::circle(flowFrame, currPoints[i], 3, cv::Scalar(0, 0, 255), -1);
                }
            }
        }

        // Объединение изображений (оптический поток и результат Кэнни)
        cv::Mat combined;
        cv::cvtColor(edges, edges, cv::COLOR_GRAY2BGR);  // Преобразование edges в цветное изображение
        cv::hconcat(flowFrame, edges, combined);

        // Отображение объединённого изображения
        cv::imshow("Optical Flow", combined);

        if (cv::waitKey(30) == 'q') break;

        gray.copyTo(prevGray);
    }

    saveOpticalFlowParams("../res/optical_flow_parameters.yaml");
    cap.release();
    cv::destroyAllWindows();
}

