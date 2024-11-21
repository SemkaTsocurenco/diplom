#include "../include/main.hpp"

int low_threshold, high_threshold; // Параметры для Canny

// Функция для загрузки параметров Canny из YAML-файла
void loadCannyParams(const std::string& filename) {
    std::cout << "Загрузка параметров Canny\n";
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для чтения." << std::endl;
        return;
    }
    // Загрузка пороговых значений Canny
    fs["Low_Threshold"] >> low_threshold;
    fs["High_Threshold"] >> high_threshold;
    fs.release();
    std::cout << "Загрузка параметров Canny - DONE\n";
}

// Функция для сохранения параметров Canny в YAML-файл
void saveCannyParams(const std::string& filename) {
    std::cout << "Сохранение параметров Canny\n";
    cv::FileStorage fs(filename, cv::FileStorage::WRITE);
    if (!fs.isOpened()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filename << " для записи." << std::endl;
        return;
    }
    // Сохранение пороговых значений Canny
    fs << "Low_Threshold" << low_threshold;
    fs << "High_Threshold" << high_threshold;
    fs.release();
    std::cout << "Сохранение параметров Canny - DONE\n";
}

void processCanny(const cv::Mat& frame, cv::Mat& output) {
    cv::Mat gray, edges;
    // Преобразование в оттенки серого
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    // Применение фильтра Canny
    cv::Canny(gray, edges, low_threshold, high_threshold);

    // Поиск контуров
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Создаем пустое изображение для заливки
    output = cv::Mat::zeros(edges.size(), CV_8UC3);

    // Заполнение контуров белым цветом
    cv::drawContours(output, contours, -1, cv::Scalar(255, 255, 255), cv::FILLED);
}


// Функция-обработчик для ползунков
void on_trackbar(int, void*) {}

// // Основная функция поиска линий
// void findLines(std::string vidname) {
//     cv::Mat frame;
//     loadCannyParams("../res/canny_parameters.yaml");
//     cv::VideoCapture cap(vidname);

//     if (!cap.isOpened()) {
//         std::cerr << "Ошибка: Не удалось открыть видео!" << std::endl;
//         return;
//     }

//     while (true) {
//         cap >> frame;
//         if (frame.empty()) {
//             std::cerr << "Ошибка: Не удалось захватить кадр!" << std::endl;
//             break;
//         }

//         if (cv::waitKey(30) == 'q') break;

//         // Создаем окна
//         cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
//         cv::namedWindow("Canny", cv::WINDOW_AUTOSIZE);

//         // Ползунки для настройки Canny
//         cv::createTrackbar("Low Threshold", "Canny", &low_threshold, 255, on_trackbar);
//         cv::createTrackbar("High Threshold", "Canny", &high_threshold, 255, on_trackbar);

//         // Обработка кадра
//         cv::Mat edges;
//         processCanny(frame, edges);

//         // Отображение исходного кадра и результатов Canny
//         cv::imshow("Original", frame);
//         cv::imshow("Canny", edges);
//     }

//     saveCannyParams("../res/canny_parameters.yaml");
//     cap.release();
//     cv::destroyAllWindows();
// }

extern "C" void findLines(const std::string imagesDir) {

    std::string outputDir = "../images_canny";
    if (!fs::exists(outputDir)) {
        fs::create_directory(outputDir);
    }
    cv::Mat edges;
;
    int index = 0;
    for (const auto& entry : fs::directory_iterator(imagesDir)) {
        while (entry.is_regular_file() && entry.path().extension() == ".png") {
            cv::Mat frame = cv::imread(entry.path().string());
            if (frame.empty()) {
                std::cerr << "Не удалось загрузить изображение: " << entry.path() << std::endl;
                continue;
            }

           
            // Создаем окна
            cv::namedWindow("Original", cv::WINDOW_AUTOSIZE);
            cv::namedWindow("Canny", cv::WINDOW_AUTOSIZE);

            // Ползунки для настройки Canny
            cv::createTrackbar("Low Threshold", "Canny", &low_threshold, 255, on_trackbar);
            cv::createTrackbar("High Threshold", "Canny", &high_threshold, 255, on_trackbar);

            // Обработка кадра
            processCanny(frame, edges);

            // Отображение исходного кадра и результатов Canny
            cv::imshow("Original", frame);
            cv::imshow("Canny", edges);


            char key = (char)cv::waitKey(10);

            if (key == 'q') {
                break;
            }
        }
    
        std::string outputFilename = outputDir + "/Images_" + std::to_string(index++) + ".png";
        cv::imwrite(outputFilename, edges);
        cv::waitKey();

    }

}