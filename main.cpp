#include <opencv2/opencv.hpp>
#include "windows.h"
#include <iostream>

using namespace cv;

int main()
{
    SetConsoleOutputCP(CP_UTF8);

    VideoCapture cap(0);            // открыть камеру №0
    if (!cap.isOpened())
        return 1;

    Mat frame, blurred, negative;

    double freq = getTickFrequency();   // сколько тиков таймера в одной секунде
    double tInput = 0, tProcess = 0, tShow = 0;     // время стадий за текущую секунду
    int frames = 0;                                 // кадров за текущую секунду
    double sumInput = 0, sumProcess = 0, sumShow = 0; // время стадий за всё время
    int totalFrames = 0;                            // кадров за всё время
    int64 startAll = getTickCount();    // момент запуска программы
    int64 startSec = startAll;          // момент начала текущей секунды

    while (true) {
        int64 t0 = getTickCount();  // замер: начало ввода
        cap.read(frame);            // получить кадр
        if (frame.empty())
            break;
        int64 t1 = getTickCount();  // замер: конец ввода, начало преобразования

        //Библиотечное преобразование: размытие фильтром 15x15
        blur(frame, blurred, Size(15, 15));

        // Ручное преобразование: негатив (255 - значение)
        negative = frame.clone();
        for (int y = 0; y < negative.rows; y++) {
            uchar *ptr = negative.ptr<uchar>(y);        // начало строки y
            for (int x = 0; x < negative.cols; x++) {
                ptr[3*x]     = 255 - ptr[3*x];          // Blue
                ptr[3*x + 1] = 255 - ptr[3*x + 1];      // Green
                ptr[3*x + 2] = 255 - ptr[3*x + 2];      // Red
            }
        }
        int64 t2 = getTickCount();  // замер: конец преобразования, начало показа

        imshow("original", frame);  //показать изображение в окне с этим именем
        imshow("blur", blurred);
        imshow("negative", negative);

        char c = waitKey(1);        //ждёт нажатия клавиши до 1 мс (для замера скорости)
        int64 t3 = getTickCount();  // замер: конец показа

        // замер: накапливаем время каждой стадии
        tInput   += t1 - t0;
        tProcess += t2 - t1;
        tShow    += t3 - t2;
        frames++;

        if (c == 27)
            break;         // Esc — выход

        // замер: раз в секунду выводим FPS и доли времени
        double elapsed = (t3 - startSec) / freq;    // сколько секунд прошло
        if (elapsed >= 1.0) {
            double total = t3 - startSec;           // вся секунда в тиках
            std::cout << "FPS: "          << frames / elapsed
                      << " | ввод: "      << 100 * tInput   / total << "%"
                      << " | преобраз.: " << 100 * tProcess / total << "%"
                      << " | показ: "     << 100 * tShow    / total << "%"
                      << std::endl;

            sumInput += tInput; sumProcess += tProcess; sumShow += tShow;
            totalFrames += frames;
            tInput = tProcess = tShow = 0;
            frames = 0;
            startSec = t3;
        }
    }

    // замер: итог за всё время работы
    sumInput += tInput; sumProcess += tProcess; sumShow += tShow;
    totalFrames += frames;
    double totalTime = (getTickCount() - startAll) / freq;

    std::cout << "\n===== ИТОГ =====" << std::endl;
    std::cout << "Кадров: " << totalFrames << " за " << totalTime << " с" << std::endl;
    std::cout << "Средний FPS: " << totalFrames / totalTime << std::endl;

    double totalTicks = sumInput + sumProcess + sumShow;   // всё время работы цикла в тиках

    std::cout << "Доля времени:" << std::endl;
    std::cout << "  ввод:           " << 100 * sumInput   / totalTicks << " %" << std::endl;
    std::cout << "  преобразование: " << 100 * sumProcess / totalTicks << " %" << std::endl;
    std::cout << "  показ:          " << 100 * sumShow    / totalTicks << " %" << std::endl;


    cap.release();          //закрыть камеру и освободить её чтобы ею могли пользовать другие
    destroyAllWindows();   //закрыть все окна созданные imshow
    return 0;
}