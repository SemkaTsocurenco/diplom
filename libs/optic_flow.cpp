#include "../include/main.hpp"

// Параметры
int flow_threshold;
int maxCorners ;
int qualityLevel;
int minDistance;
int cannyThreshold1;
int cannyThreshold2 ;

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
void findLines(std::string imagesDir) {

    int index =0;
    loadOpticalFlowParams("../res/optical_flow_parameters.yaml");
    cv::Mat frame = cv::imread("../images/Image_0.png");
    cv::Mat prevGray, gray, edges;
        std::string outputDir = "../images_optical_flow";
    if (frame.empty()) {
        std::cerr << "Ошибка: Не удалось захватить кадр!" << std::endl;
        return;
    }


    // Размер исходного изображения
    cv::Size originalSize = frame.size();

    // Фокусируемся на нижней части кадра
    cv::Rect roi(0, frame.rows / 2, frame.cols, frame.rows / 2);
    cv::Mat frameROI = frame(roi);
    cv::cvtColor(frameROI, prevGray, cv::COLOR_BGR2GRAY);
    cv::Mat fullSizeBinary ;
    std::string outputFilename ;
    
    for (const auto& entry : fs::directory_iterator(imagesDir)) {
        while (entry.is_regular_file() && entry.path().extension() == ".png" && cv::waitKey(30) != 'q') {
            frame = cv::imread(entry.path().string());
            if (frame.empty()) {
                std::cerr << "Не удалось загрузить изображение: " << entry.path() << std::endl;
                continue;
            }

            cv::namedWindow("Optical Flow", cv::WINDOW_AUTOSIZE);
            cv::createTrackbar("Flow Threshold", "Optical Flow", &flow_threshold, 50, on_trackbar);
            cv::createTrackbar("maxCorners", "Optical Flow", &maxCorners, 2000, on_trackbar);
            cv::createTrackbar("qualityLevel", "Optical Flow", &qualityLevel, 95, on_trackbar);
            cv::createTrackbar("minDistance", "Optical Flow", &minDistance, 300, on_trackbar);
            cv::createTrackbar("Canny Thresh 1", "Optical Flow", &cannyThreshold1, 200, on_trackbar);
            cv::createTrackbar("Canny Thresh 2", "Optical Flow", &cannyThreshold2, 300, on_trackbar);

            // Фокусируемся на нижней части кадра
            frameROI = frame(roi);
            cv::cvtColor(frameROI, gray, cv::COLOR_BGR2GRAY);

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

            // Создание бинарного изображения
            cv::Mat binary = cv::Mat::zeros(frameROI.size(), CV_8UC1);
            for (size_t i = 0; i < currPoints.size(); i++) {
                if (status[i]) {
                    double flow = cv::norm(currPoints[i] - prevPoints[i]);
                    if (flow > flow_threshold) {
                        // Отметка линии в бинарном изображении
                        cv::line(binary, prevPoints[i], currPoints[i], cv::Scalar(255), 2);
                    }
                }
            }

            // Восстановление размера изображения
            fullSizeBinary = cv::Mat::zeros(originalSize, CV_8UC1);
            binary.copyTo(fullSizeBinary(roi));

            // Отображение результата
            cv::imshow("Optical Flow", fullSizeBinary);
            // Сохранение изображения

            // Отображение результата
            if (cv::waitKey(30) == 'q') break;
        }
        outputFilename = outputDir + "/Image_" + std::to_string(index++) + ".png";
        cv::imwrite(outputFilename, fullSizeBinary);
        gray.copyTo(prevGray);
        cv::waitKey();
    }
}
