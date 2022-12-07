#include "car.h"

#define pass 1;
#define fail 0;

car::car(string license_plate, int into_ptimeH, int into_ptimeM)
{
    this->license_plate = license_plate;
    this->into_ptimeH = into_ptimeH;
    this->into_ptimeM = into_ptimeM;
}

void car::writeData()
{
    ofstream fout;
    const char* file = "parking_car.dat";
    fout.open(file, ios::out|ios::binary|ios::app);
    if(!fout){
        //cout << "파일 열기 오류";
        //messagebox 파일 오류 출력
    }
    fout.write(this->license_plate.c_str(), 20);
    fout.write((char*)&this->into_ptimeH, sizeof(this->into_ptimeH));
    fout.write((char*)&this->into_ptimeM, sizeof(this->into_ptimeM));
    fout.write("\n",1);
    fout.close();
    this->default_plate.push_back(this->license_plate);
}
int car::readData(string str)
{
    ifstream fin;
    const char* file = "parking_car.dat";
    char plate[20],tmp;
    fin.open(file, ios::in);
    if(!fin){
        //cout << "파일 열기 오류";
        //messagebox 파일 오류 출력
    }
    while(!fin.eof()){
        fin.read((char *)&plate[0], 20);
        fin.read((char*)&this->into_ptimeH, sizeof(this->into_ptimeH));
        fin.read((char*)&this->into_ptimeM, sizeof(this->into_ptimeM));
        //fin.seekg(1,ios::cur);
        fin.read((char*)&tmp,sizeof(tmp));
        this->license_plate=plate;
        if(this->license_plate == str ){
            break;

        }
        if(fin.get() == EOF){
            break;
        }
        else fin.seekg(-1,ios::cur);
    }

    fin.close();
    if (this->license_plate == str){
        return pass;
    } else{
        return fail;
    };
}

void car::readData()
{
    ifstream fin;
    const char* file = "parking_car.dat";
    char plate[20], tmp;
    fin.open(file, ios::in);
    if(!fin){
        return;
        //cout << "파일 열기 오류";
        //messagebox 파일 오류 출력
    }
    while(!fin.eof()){
        fin.read((char *)&plate[0], 20);
        fin.read((char*)&this->into_ptimeH, sizeof(this->into_ptimeH));
        fin.read((char*)&this->into_ptimeM, sizeof(this->into_ptimeM));
        //fin.seekg(1,ios::cur);
        fin.read((char*)&tmp,sizeof(tmp));
        this->license_plate=plate;
        this->default_plate.push_back(this->license_plate);
        if(fin.get() == EOF){
            break;
        }
        else fin.seekg(-1,ios::cur);

    }
    fin.close();
}

void car::deleteData(string str)
{
    ifstream fin;
    ofstream fout;
    const char* file = "parking_car.dat";
    const char* t_file = "tmp.dat";
    char plate[20], tmp;
    fin.open(file, ios::in);
    if(!fin){
        //cout << "파일 열기 오류";
        //messagebox 파일 오류 출력
    }
    fout.open(t_file, ios::out|ios::binary);
    if(!fout){
        //cout << "파일 열기 오류";
        //messagebox 파일 오류 출력
    }

    while(!fin.eof()){
        fin.read((char *)&plate[0], 20);
        fin.read((char*)&this->into_ptimeH, sizeof(this->into_ptimeH));
        fin.read((char*)&this->into_ptimeM, sizeof(this->into_ptimeM));
        //fin.seekg(1,ios::cur);
        fin.read((char*)&tmp,sizeof(tmp));
        this->license_plate=plate;

        if(this->license_plate == str ){
            continue ;
        }else{
            fout.write(this->license_plate.c_str(), 20);
            fout.write((char*)&this->into_ptimeH, sizeof(this->into_ptimeH));
            fout.write((char*)&this->into_ptimeM, sizeof(this->into_ptimeM));
            fout.write("\n",1);
            if(fin.get() == EOF) break;
            else fin.seekg(-1,ios::cur);
        }
    }
    fin.close();
    fout.close();
    remove("parking_car.dat");
    rename("tmp.dat", "parking_car.dat");
    for(int i=0; i<(int)default_plate.size();i++){
        if(str==this->default_plate.at(i))
            this->default_plate.erase(this->default_plate.begin()+i);
    }
}

int car::rate_check()
{
    int chk_timeA, chk_timeB;
    time_t timer;
    struct tm* t;

    time(&timer);
    t = localtime(&timer);
    chk_timeA = (t->tm_hour * 60) + t->tm_min;
    chk_timeB = (this->into_ptimeH * 60) + this->into_ptimeM;
    this->fare = (chk_timeA - chk_timeB) / 10 * 800;
    return fare;
}

string car::GetLicense_plate()
{
    string str;

    str = this->license_plate;

    return str;
}

int car::GetParkingtime()
{
    int chk_time, chk_timeA, chk_timeB;
    time_t timer;
    struct tm* t;

    time(&timer);
    t = localtime(&timer);
    chk_timeA = (t->tm_hour * 60) + t->tm_min;
    chk_timeB = (this->into_ptimeH * 60) + this->into_ptimeM;
    chk_time = chk_timeA-chk_timeB;
    return chk_time;
}

int car::Getinto_timeH()
{
    int into_t_h;
    into_t_h = this->into_ptimeH;
    return into_t_h;
}

int car::Getinto_timeM()
{
    int into_t_m;
    into_t_m = this->into_ptimeM;
    return into_t_m;
}

int car::Get_Count()
{
        return this->default_plate.size();
}
void car::Set_car(string license_plate)
{
    time_t timer;
    struct tm* t;
    time(&timer);
    t = localtime(&timer);
    this->license_plate = license_plate;
    this->into_ptimeH = t->tm_hour;
    this->into_ptimeM = t->tm_min;
}

string car::Get_default_License_plate(int i)
{
    return this->default_plate.at(i);
}
car::~car()
{

}
