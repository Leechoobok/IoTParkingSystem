#ifndef ARDUINO_H
#define ARDUINO_H

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QWidget>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <QThread>
#include <QMutex>
#include <codecvt>
#include <algorithm>
#include <cstdlib>

using namespace cv;
using namespace std;


class arduino : public QThread
{
public:
    arduino();
    ~arduino();
    QString press_shutter(cv::Mat& cap);
    bool verifySizes(RotatedRect mr);
    QString verify(QString plate);
    QString frequency(vector<QString> plateList);
    vector<QString> led_status(QString led);

    Mat frame;



private:



};

#endif // ARDUINO_H
