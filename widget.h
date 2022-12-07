#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QDebug>
#include <QMessageBox>
#include <QListWidget>
#include "arduino.h"
#include "car.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QImage imdisplay;
    QTimer *Timer;
    VideoCapture cap;



private slots:
    void on_check_btn_clicked();

    void on_pay_btn_clicked();

    void on_end_btn_clicked();

    void handleError(QSerialPort::SerialPortError error);

    void handleError_2nd(QSerialPort::SerialPortError error);

    void readData();

    void readData_2nd();

    void on_search_btn_clicked();

    void parking_area_status(vector<QString> status);

    void on_parking_info_itemClicked(QListWidgetItem *item);

private:
    Ui::Widget *ui;
    void DisplayImage();

    void setSerial();

    void setSerial_2nd();

    QSerialPort* m_serialPort = nullptr;
    QByteArray serial_data;
    QString serial_buffer;
    
    QSerialPort* m_serialPort_2nd;
    QByteArray serial_data_2nd;
    QString serial_buffer_2nd;
    
    arduino* arduino_;
    vector<QString> plateList;
    car* live_car;
    bool into_car=false;
    bool out_car=false;

    QPalette lblpal;

    QMutex mutex;

};
#endif // WIDGET_H
