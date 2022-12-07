#include "arduino.h"


arduino::arduino()
{

}
arduino::~arduino()
{

}

QString arduino::press_shutter(cv::Mat& cap){

    frame=cap;

    Mat img;
     img = frame;

     if (img.empty()) {
             return "";
     }
     Mat img_gray;
     cvtColor(img, img_gray, COLOR_BGR2GRAY);
     blur(img_gray, img_gray, Size(5, 5));


     Mat img_sobel;
     Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT);


     Mat img_threshold;
     threshold(img_sobel, img_threshold, 0, 255, THRESH_OTSU + THRESH_BINARY);



     Mat element = getStructuringElement(MORPH_RECT, Size(17,3));
     morphologyEx(img_threshold, img_threshold, MORPH_CLOSE, element);


     vector<vector<Point>> contours;
     findContours(img_threshold, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
     vector<vector<Point>>::iterator itc = contours.begin();
     vector<RotatedRect> rects;

     while (itc != contours.end()){
        RotatedRect mr = minAreaRect(Mat(*itc));
        if( !verifySizes(mr)){
            itc = contours.erase(itc);
        } else{
            ++itc;
            rects.push_back(mr);
        }
     }

     Mat result;
     img.copyTo(result);
     drawContours(result, contours, -1, Scalar(255, 0, 0), 1);

     string title;
     for(int i=0 ;i< rects.size();i++){
         circle(result, rects[i].center, 3, Scalar(0,255,0),-1);
         float minSize=(rects[i].size.width < rects[i].size.height)?rects[i].size.width:rects[i].size.height;

         srand(time(NULL));
         Mat mask;

         mask.create(img.rows +2, img.cols+2, CV_8UC1);
         mask = Scalar::all(0);
         int loDiff = 30;
         int upDiff = 30;
         int connectivity = 4;
         int newMaskVal = 255;
         int NumSeeds = 10;
         Rect ccomp;
         int flags = connectivity + (newMaskVal << 8)+FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;
         for(int j=0; j<NumSeeds;j++){
             Point seed;
             seed.x=rects[i].center.x+rand()%(int)minSize-(minSize/2);
             seed.y=rects[i].center.y+rand()%(int)minSize-(minSize/2);
             circle(result, seed, 1, Scalar(0, 255, 255), -1);
             int area = floodFill(img, mask, seed, Scalar(255,0,0), &ccomp, Scalar(loDiff, loDiff, loDiff), Scalar(upDiff, upDiff, upDiff), flags);
         }

         vector<Point> pointsInterest;
         Mat_<uchar>::iterator itMask = mask.begin<uchar>();
         Mat_<uchar>::iterator end = mask.end<uchar>();
         for(;itMask!=end;++itMask)
             if(*itMask==255)
                 pointsInterest.push_back(itMask.pos());

         RotatedRect minRect = minAreaRect(pointsInterest);

         if(verifySizes(minRect)){
             Point2f rect_points[4]; minRect.points(rect_points);
             for(int j = 0; j<4;j++)
                 line(result, rect_points[j], rect_points[(j+1)%4], Scalar(0,0,255),1,8);

             float r = (float)minRect.size.width / (float)minRect.size.height;
             float angle = minRect.angle;
             if(r<1)
                 angle=270+angle;
             Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1);

             Mat img_rotated;
             warpAffine(img, img_rotated, rotmat, img.size(), INTER_CUBIC);

             Size rect_size = minRect.size;
             if(r<1)
                 swap(rect_size.width, rect_size.height);
             Mat img_crop;
             getRectSubPix(img_rotated, rect_size, minRect.center, img_crop);

             Mat resultResized;
             resultResized.create(200,723,CV_8UC3);
             cv::resize(img_crop, resultResized, resultResized.size(), 0, 0, INTER_CUBIC);

             Mat grayResult;
             cvtColor(resultResized, grayResult, COLOR_BGR2GRAY);
             blur(grayResult, grayResult, Size(3, 3));
             //equalizeHist(grayResult, grayResult);
             imwrite("grayResult.png",grayResult);
             imwrite("grayResult1.png",grayResult);
       }
     }
    char *outText;
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();

    if (api->Init(NULL, "kor")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }

    Pix *pimage = pixRead("grayResult.png");
    if(pimage){
        api->SetImage(pimage);
        outText = api->GetUTF8Text();

        std::string str=outText, tmp, tmp1, tmp2;
        std::string str_data="０１２３４５６７８９";
        std::string str_data1="0123456789";
        std::string plate;
        std::u16string x;


        size_t i = 0;
        size_t j = 0;
        size_t len = 0;
        size_t len_j= 0 ;

        while (i < str.size()) {
            int char_size = 0;

            if ((str[i] & 0b11111000) == 0b11110000) {
              char_size = 4;
            } else if ((str[i] & 0b11110000) == 0b11100000) {
              char_size = 3;
            } else if ((str[i] & 0b11100000) == 0b11000000) {
              char_size = 2;
            } else if ((str[i] & 0b10000000) == 0b00000000) {
              char_size = 1;
            } else {
              std::cout << "이상한 문자 발견!" << std::endl;
              char_size = 1;
            }

            tmp = str.substr(i, char_size);
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
            std::u16string wstr = convert.from_bytes(tmp);

            for (char16_t c : wstr) {
             // 유니코드 상에서 한글의 범위
            if ((0xAC00 <= c && c <= 0xD7A3)) {
                plate+=tmp;
                }
             }
            while (j < str_data.size()) {
              int char_size_j = 0;

              if ((str_data[j] & 0b11111000) == 0b11110000) {
                char_size_j = 4;
              } else if ((str_data[j] & 0b11110000) == 0b11100000) {
                char_size_j = 3;
              } else if ((str_data[j] & 0b11100000) == 0b11000000) {
                char_size_j = 2;
              } else if ((str_data[j] & 0b10000000) == 0b00000000) {
                char_size_j = 1;
              } else {
                std::cout << "이상한 문자 발견!" << std::endl;
                char_size_j = 1;
              }
                tmp1=str_data.substr(j, char_size_j);
                tmp2=str_data1[j/3];

                if(tmp==tmp1 || tmp == tmp2 ) plate+=(str_data1[j/3]);

                j += char_size_j;
                len_j++;
            }

            i += char_size;
            len++;
            j=0;
        }
        //remove("grayResult.png");
        return QString::fromStdString(plate);
    }else{
        qDebug()<<"imgprocessing error";
        return QString::fromStdString("");
    }

}
bool arduino::verifySizes(RotatedRect mr){

    float error = 0.4;
    float aspect = 5.1;
    int min = 15 * aspect * 15;
    int max = 1000 * aspect * 1000;

    float rmin = aspect-aspect*error;
    float rmax = aspect+aspect*error;

    int area = mr.size.width *mr.size.width;
    float r = (float)mr.size.width / (float)mr.size.height;
    if(r<1)
        r = (float)mr.size.height / (float)mr.size.width;

    if((area < min || area > max) || (r < rmin || r > rmax)){
        return false;
    } else {
        return true;
    }
}

QString arduino::verify(QString plate)
{
    QString verify = "";
    for(int i=0;i<plate.length();i++){
        QChar tmp = plate.at(i);
        if(tmp.isDigit()){
            verify+="0";
        }else{
            verify+="1";
        }
    }
    if(verify=="0010000"||verify=="00010000"){
        return plate;
    }else
    {
        return "";
    }

}

QString arduino::frequency(vector<QString> plateList)
{
    vector<int> frequency;

       for(int i=0 ; i<(int)plateList.size();i++){
           frequency.push_back(0);
           for(int j=0;j<(int)plateList.size();j++){
               if(plateList[i]==plateList[j])
               {
                   frequency.at(i)++;
               }
           }
       }
       int max_index = max_element(frequency.begin(), frequency.end()) - frequency.begin();
       for(int i=0;i<(int)plateList.size();i++){
           if(i==max_index){
               QString tmp = QString::number(i)+".png";
               rename(tmp.toLocal8Bit().data(),"result.png");
           }else{
               QString tmp = QString::number(i)+".png";
               remove(tmp.toLocal8Bit().data());
           }
       }
       return plateList[max_index];
}

vector<QString> arduino::led_status(QString led)
{
    vector<QString> status;
    int tmp_cnt=3;
    for(int i=0;i<led.size();i++){
        if(i>3){
            status.push_back(led.at(i+tmp_cnt));
            tmp_cnt-=2;
        }else status.push_back(led.at(i));//8 elements
    }
    return status;
}
