#include "widget.h"
#include "./ui_widget.h"



Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    QPixmap area;
    QLabel* p[8] = {ui->P0, ui->P1, ui->P2, ui->P3, ui->P4, ui->P5, ui->P6, ui->P7};

    serial_buffer="";
    serial_buffer_2nd="";
    m_serialPort = new QSerialPort();
    m_serialPort_2nd = new QSerialPort();
    arduino_ = new arduino();
    setSerial();
    setSerial_2nd();
    live_car = new car("test", 0, 0);
    live_car->readData();
    area.load("area.png");
    ui->lbl_area->setPixmap(area);
    lblpal.setColor(QPalette::Window, Qt::black);
    lblpal.setColor(QPalette::WindowText, Qt::white);


    for(int i=0;i<8;i++){
        p[i]->setAutoFillBackground(true);
        p[i]->setPalette(lblpal);
    }
    cap.open(0);
    m_serialPort_2nd->write("L");
}

Widget::~Widget()
{
    if (m_serialPort->isOpen()) // 시리얼 포트 열렸으면
        m_serialPort->close(); // 시리얼 닫아라
    if (m_serialPort_2nd->isOpen()) // 시리얼 포트 열렸으면
        m_serialPort_2nd->close(); // 시리얼 닫아라
    cap.release();
    delete ui;
    delete live_car;
}


void Widget::on_check_btn_clicked()
{
//요금조
    ui->parking_info->clear();
    ui->parking_info->setEnabled(1);
    ui->pay_btn->setEnabled(1);
    ui->check_btn->setEnabled(0);
    ui->lbl_title->setText("차량번호\t\t\t이용시간\t\t\t이용요금");
    ui->parking_info->addItem(QString::fromStdString(live_car->GetLicense_plate())+"\t\t       "+QString::number(live_car->GetParkingtime())+"분\t\t"+"              "+QString::number(live_car->rate_check())+"원");
    /*QString tmp = ui->InputText->text();
    int r_data = live_car->readData(tmp.toStdString());
    if(r_data==1){

    }*/


}

void Widget::on_pay_btn_clicked()
{
   /* cap >> frame;
    namedWindow("sample");
    imshow("sample", frame);
    waitKey(0);
    destroyWindow("sample");*/
    //정산
    QString tmp = ui->InputText->text();
    ui->parking_info->clear();
    ui->img_plate->clear();
    ui->pay_btn->setEnabled(0);
    ui->parking_info->setEnabled(0);
    ui->check_btn->setEnabled(0);
    QString tmp_plate = QString::fromStdString(live_car->GetLicense_plate());
    QString tmp_delpng =  tmp_plate.right(4)+QString::number(live_car->Getinto_timeH())+".png";
    remove(tmp_delpng.toLocal8Bit().data());
    live_car->deleteData(tmp.toStdString());



}

void Widget::on_end_btn_clicked()
{
     QWidget::close();
}

void Widget::setSerial(){
    const auto infos = QSerialPortInfo::availablePorts();
    m_serialPort->setPortName(infos[0].portName());
    m_serialPort->setBaudRate(QSerialPort::Baud9600);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);
    if(m_serialPort->open(QIODevice::ReadWrite)){
        connect(m_serialPort, &QSerialPort::readyRead, this,  &Widget::readData);
        connect(m_serialPort, &QSerialPort::errorOccurred, this, &Widget::handleError);
        qDebug() << "Serial connect!";
    }else{
        QMessageBox::critical(this, QObject::tr("Error"), m_serialPort->errorString());
    }
}

void Widget::setSerial_2nd()
{
    const auto infos = QSerialPortInfo::availablePorts();
    m_serialPort_2nd->setPortName(infos[1].portName());
    m_serialPort_2nd->setBaudRate(QSerialPort::Baud9600);
    m_serialPort_2nd->setDataBits(QSerialPort::Data8);
    m_serialPort_2nd->setParity(QSerialPort::NoParity);
    m_serialPort_2nd->setStopBits(QSerialPort::OneStop);
    m_serialPort_2nd->setFlowControl(QSerialPort::NoFlowControl);
    if(m_serialPort_2nd->open(QIODevice::ReadWrite)){
        connect(m_serialPort_2nd, &QSerialPort::readyRead, this,  &Widget::readData_2nd);
        connect(m_serialPort_2nd, &QSerialPort::errorOccurred, this, &Widget::handleError_2nd);
        qDebug() << "Serial connect!";
    }else{
        QMessageBox::critical(this, QObject::tr("Error"), m_serialPort_2nd->errorString());
    }
}



void Widget::readData(){


    QStringList bufferSplit = serial_buffer.split(",");
    QMessageBox msgbox;
    int pic_num=0;

    if(bufferSplit.length() < 3){
        serial_data = m_serialPort->readAll();
        serial_buffer += QString::fromStdString(serial_data.toStdString());
    }else{
        while(bufferSplit[1] == "in" && plateList.size()<5){
            Mat frame;
            cap >> frame;
            QString tmp_plate = arduino_->press_shutter(frame);
            QString plate = arduino_->verify(tmp_plate);
            if(plate.isEmpty()){
                if(pic_num==10){
                    msgbox.information(this, "인식실패", "다시 시도해 주세요!");
                    serial_buffer="";
                    return;
                }
                pic_num++;
                continue;
            }else{
                plateList.push_back(plate);
                QString tmp = QString::number(plateList.size()-1)+".png";
                rename("grayResult.png",tmp.toLocal8Bit().data());
                into_car = true;
            }
        }
        while(bufferSplit[1] == "out" && plateList.size()<5){
            Mat frame;
            cap >> frame;
            QString tmp_plate = arduino_->press_shutter(frame);
            QString plate = arduino_->verify(tmp_plate);
            if(plate.isEmpty()){
                if(pic_num==10){
                    msgbox.information(this, "인식실패", "다시 시도해 주세요!");
                    serial_buffer="";
                    return;
                }
                pic_num++;
                 continue;
            }else{
                plateList.push_back(plate);
                QString tmp = QString::number(plateList.size()-1)+".png";
                rename("grayResult.png",tmp.toLocal8Bit().data());
                out_car = true;
            }
        }

        if(into_car){

            QString frequency_plate = arduino_->frequency(plateList);

            for(int i=0; i<live_car->Get_Count();i++)
            {
                if(frequency_plate.toStdString()==live_car->Get_default_License_plate(i)){
                    msgbox.information(this, "오류", "이미 입차된 차량입니다!");
                    serial_buffer="";
                    into_car=false;
                    return;

                }
            }
            live_car->Set_car(frequency_plate.toStdString());
            qDebug()<<frequency_plate;
            QString tmp_savepng =  frequency_plate.right(4)+QString::number(live_car->Getinto_timeH())+".png";
            rename("result.png",tmp_savepng.toLocal8Bit().data());
            live_car->writeData();

            QByteArray send_Data;
            QString tmp = "I";
            send_Data=tmp.toUtf8().left(1);
            m_serialPort->write(send_Data.data());
            send_Data="t"+frequency_plate.toUtf8().right(4);
            m_serialPort->write(send_Data.data());
            into_car=false;

            Mat img_plate;
            QString img_dir(QString::fromStdString(live_car->GetLicense_plate()));

            img_dir=img_dir.right(4);
            img_dir=img_dir+QString::number(live_car->Getinto_timeH())+".png";

            img_plate = imread(img_dir.toStdString().c_str());
            cv::resize(img_plate, img_plate, Size(ui->img_plate->width(), ui->img_plate->height()), 0, 0, INTER_LINEAR);
            cv::cvtColor(img_plate,img_plate,cv::COLOR_BGR2RGB);
            QImage imdisplay((uchar*)img_plate.data, img_plate.cols, img_plate.rows, img_plate.step, QImage::Format_RGB888);
            ui->img_plate->setPixmap(QPixmap::fromImage(imdisplay));


            ui->parking_info->addItem("입차완료");

            plateList.clear();
        }else if(out_car){

            QString frequency_plate = arduino_->frequency(plateList);


            //나가는 차량 번호 검증(정산한 차량번호)
            QByteArray send_Data;
            QString tmp = "O";
            send_Data=tmp.toUtf8().left(1);
            m_serialPort->write(send_Data.data());

            send_Data="t"+frequency_plate.toUtf8().right(4);
            m_serialPort->write(send_Data.data());
            Mat img_plate;
            img_plate = imread("result.png");
            cv::resize(img_plate, img_plate, Size(ui->img_plate->width(), ui->img_plate->height()), 0, 0, INTER_LINEAR);
            cv::cvtColor(img_plate,img_plate,cv::COLOR_BGR2RGB);
            QImage imdisplay((uchar*)img_plate.data, img_plate.cols, img_plate.rows, img_plate.step, QImage::Format_RGB888);
            ui->img_plate->setPixmap(QPixmap::fromImage(imdisplay));
            ui->parking_info->setEnabled(1);
            ui->parking_info->addItem("출차완료");
            plateList.clear();
            out_car=false;
            remove("result.png");
        }

            serial_buffer="";
    }

}

void Widget::readData_2nd()
{
    QStringList bufferSplit = serial_buffer_2nd.split(",");
    QMessageBox msgbox;


    if(bufferSplit.length() < 3){
        serial_data_2nd = m_serialPort_2nd->readAll();
        serial_buffer_2nd += QString::fromStdString(serial_data_2nd.toStdString());

    }else{
        if(bufferSplit[1].left(1) == "l"){
            vector<QString> status;
            status = arduino_->led_status(bufferSplit[1].right(8));
            parking_area_status(status);

        }
    serial_buffer_2nd="";
    }


}
void Widget::handleError(QSerialPort::SerialPortError error){
    if(error == QSerialPort::ResourceError){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), m_serialPort->errorString());
        if (m_serialPort->isOpen()) // 시리얼 포트 열렸으면
            m_serialPort->close(); // 시리얼 닫아라
    }

}

void Widget::handleError_2nd(QSerialPort::SerialPortError error)
{
    if(error == QSerialPort::ResourceError){
        QMessageBox::critical(nullptr, QObject::tr("Critical Error"), m_serialPort_2nd->errorString());
        if (m_serialPort_2nd->isOpen()) // 시리얼 포트 열렸으면
            m_serialPort_2nd->close(); // 시리얼 닫아라
    }
}




void Widget::on_search_btn_clicked()
{
//주차현황
    //QString tmp = ui->InputText->text();
    QMessageBox msgbox;
    int r_data;
    ui->img_plate->clear();
    ui->parking_info->clear();
    ui->parking_info->setEnabled(1);
    ui->pay_btn->setEnabled(0);
    ui->check_btn->setEnabled(0);
    ui->InputText->clear();
    ui->lbl_title->setText("차량번호\t\t\t입차시간");
    for(int i=0;i<live_car->Get_Count();i++){
        QString tmp = QString::fromStdString(live_car->Get_default_License_plate(i));
        r_data = live_car->readData(tmp.toStdString());
        if(r_data==1){
            ui->parking_info->addItem(QString::fromStdString(live_car->GetLicense_plate())+"\t\t"+QString::number(live_car->Getinto_timeH())+"시"+QString::number(live_car->Getinto_timeM())+"분");
        }
    }
    /*
    if(tmp.isEmpty()){
        msgbox.information(this, "오류", "차량번호를 입력해 주세요");
    }
    else{
        r_data = live_car->readData(tmp.toStdString());
        if (r_data==1){
            ui->img_plate->clear();
            ui->parking_info->clear();
            ui->parking_info->setEnabled(1);
            ui->pay_btn->setEnabled(0);
            ui->check_btn->setEnabled(1);

            ui->lbl_title->setText("차량번호\t\t\t입차시간");
            ui->parking_info->addItem(QString::fromStdString(live_car->GetLicense_plate())+"\t\t"+QString::number(live_car->Getinto_timeH())+"시"+QString::number(live_car->Getinto_timeM())+"분");

            Mat img_plate;
            QString img_dir(QString::fromStdString(live_car->GetLicense_plate()));

            img_dir=img_dir.right(4);
            img_dir=img_dir+QString::number(live_car->Getinto_timeH())+".png";

            img_plate = imread(img_dir.toStdString().c_str());
            cv::resize(img_plate, img_plate, Size(ui->img_plate->width(), ui->img_plate->height()), 0, 0, INTER_LINEAR);
            cv::cvtColor(img_plate,img_plate,cv::COLOR_BGR2RGB);
            QImage imdisplay((uchar*)img_plate.data, img_plate.cols, img_plate.rows, img_plate.step, QImage::Format_RGB888);
            ui->img_plate->setPixmap(QPixmap::fromImage(imdisplay));

        } else{
            msgbox.information(this, "오류", "주차된 차량이 없습니다.");
        }
    //차량 검색 함수 돌아갑시다아아~~
    }*/
}

void Widget::parking_area_status(vector<QString> status)
{
    QLabel* p[8] = {ui->P0, ui->P1, ui->P2, ui->P3, ui->P4, ui->P5, ui->P6, ui->P7};
    for(int i=0; i<(int)status.size();i++){
        if(status[i]=="0"){
            lblpal.setColor(QPalette::Window, Qt::black);
            lblpal.setColor(QPalette::WindowText, Qt::white);
            p[i]->setAutoFillBackground(true);
            p[i]->setPalette(lblpal);
            p[i]->clear();

        }else if(status[i]=="1"){
            lblpal.setColor(QPalette::Window, Qt::white);
            lblpal.setColor(QPalette::WindowText, Qt::blue);
            p[i]->setAutoFillBackground(true);
            p[i]->setPalette(lblpal);
            p[i]->setText("full");
        }

    }
    ui->img_plate->clear();
    ui->parking_info->clear();
}

void Widget::on_parking_info_itemClicked(QListWidgetItem *item)
{
    QString tmp;
    string tmp_slice;
    ui->pay_btn->setEnabled(0);
    ui->check_btn->setEnabled(1);
    tmp = ui->parking_info->currentItem()->text();
    tmp_slice = tmp.toStdString();
    ui->InputText->setText(QString::fromStdString(tmp_slice.substr(0,tmp_slice.find("\t"))));
    QString tmp_1 = ui->InputText->text();

    int r_data = live_car->readData(tmp_1.toStdString());
    if (r_data){
        Mat img_plate;
        QString img_dir(QString::fromStdString(live_car->GetLicense_plate()));

        img_dir=img_dir.right(4);
        img_dir=img_dir+QString::number(live_car->Getinto_timeH())+".png";

        img_plate = imread(img_dir.toStdString().c_str());
        cv::resize(img_plate, img_plate, Size(ui->img_plate->width(), ui->img_plate->height()), 0, 0, INTER_LINEAR);
        cv::cvtColor(img_plate,img_plate,cv::COLOR_BGR2RGB);
        QImage imdisplay((uchar*)img_plate.data, img_plate.cols, img_plate.rows, img_plate.step, QImage::Format_RGB888);
        ui->img_plate->setPixmap(QPixmap::fromImage(imdisplay));
    }

}
