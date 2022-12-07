#ifndef CAR_H
#define CAR_H

#pragma once
#include <ctime>
#include <fstream>
#include <ctime>
#include <vector>
#include <QDebug>

using namespace std;

class car
{
private:
    string license_plate;
    int into_ptimeH, into_ptimeM;
    int fare;
    vector<string> default_plate;

public:
    car(string license_plate, int into_ptimeH, int into_ptimeM);
    void writeData();
    int readData(string str);
    void readData();
    void deleteData(string str);
    int rate_check();
    string GetLicense_plate();
    int GetParkingtime();
    int Getinto_timeH();
    int Getinto_timeM();
    int Get_Count();
    void Set_car(string license_plate);
    string Get_default_License_plate(int i);

    ~car();
};

#endif // CAR_H
