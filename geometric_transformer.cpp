#include "geometric_transformer.h"
#include <QDebug>
#include <math.h>
#include <algorithm>
using namespace std;
Geometric_Transformer::Geometric_Transformer()
{

}

void Geometric_Transformer::translate(int w,int h,QImage*& im){
    QImage* nim = new QImage(w + im->width(),h + im->height(),QImage::Format_RGB888);
    for(int i = 0;i < nim->height();i++){
        for(int j = 0; j < nim->width();j++){
            if(i - h >= 0 && j - w >= 0)nim->setPixelColor(j,i,im->pixel(j - w,i - h));
            else {
                nim->setPixelColor(j,i,qRgb(255,255,255));
            }
        }
    }
    delete im;
    im = nim;
    nim = nullptr;
}

void Geometric_Transformer::Horizontal_Mirror(QImage *&im){
    for(int i = 0;i < im->height();i++){
        for(int j = 0; j < im->width() / 2;j++){
            QRgb l = im->pixel(j,i);
            im->setPixelColor(j,i,im->pixelColor(im->width() - 1 - j,i));
            im->setPixelColor(im->width() - 1 - j,i,l);
        }
    }

}

void Geometric_Transformer::Vertical_Mirror(QImage *&im){
    for(int i = 0;i < im->height() / 2;i++){
        for(int j = 0; j < im->width();j++){
            QRgb top = im->pixel(j,i);
            im->setPixelColor(j,i,im->pixelColor(j,im->height() - 1 - i));
            im->setPixelColor(j,im->height() - 1 - i,top);
        }
    }
}
void Geometric_Transformer::Rotate(double angle, QImage *&im){
    //大写X和Y表示的是以图像中心为坐标系，小写的表示设备坐标系
    int srcX1, srcX2, srcX3, srcX4;
    int srcY1, srcY2, srcY3, srcY4;

    int halfw = im->width() / 2;
    int halfh = im->height() / 2;

    //左上角
    srcX1 = 0 - halfw;
    srcY1 = 0 + halfh;

    //右上角
    srcX2 = im->width() - 1 - halfw;
    srcY2 = 0 + halfh;

    //左下角
    srcX3 = 0 - halfw;
    srcY3 = 1 - im->height() + halfh;

    //右下角
    srcX4 = im->width() - 1 - halfw;
    srcY4 = 1 - im->height() + halfh;


    double fSin = sin(angle);
    double fCos = cos(angle);

    double tranX1, tranX2, tranX3, tranX4;
    double tranY1, tranY2, tranY3, tranY4;
    tranX1 = fCos * srcX1 - fSin * srcY1;
    tranY1 = fSin * srcX1 + fCos * srcY1;

    tranX2 = fCos * srcX2 - fSin * srcY2;
    tranY2 = fSin * srcX2 + fCos * srcY2;

    tranX3 = fCos * srcX3 - fSin * srcY3;
    tranY3 = fSin * srcX3 + fCos * srcY3;

    tranX4 = fCos * srcX4 - fSin * srcY4;
    tranY4 = fSin * srcX4 + fCos * srcY4;

    int outWidth = (int)(max(fabs(tranX4 - tranX1), fabs(tranX3 - tranX2)) + 0.5) + 1;
    int outHeight = (int)(max(fabs(tranY1 - tranY4),fabs(tranY2 - tranY3)) + 0.5) + 1;
    //以上是计算新图像的大小

    //下面是通过逆变换做旋转
    QImage* nim = new QImage(outWidth,outHeight,QImage::Format_RGB888);
    int nhalfw = nim->width() / 2;
    int nhalfh = nim->height() / 2;

    double num1 = halfw * fCos + halfh * fSin - nhalfw;
    double num2 = -halfw * fSin + halfh * fCos - nhalfh;

    for(int i = 0;i < nim->height();i++){
        double nxtY = i + num2;
        for(int j = 0;j < nim->width();j++){
            double nxtX = j + num1;
            //旋转前在图像中心坐标系下的坐标
            int prex,prey;
            prex = (int)(nxtX * fCos - nxtY * fSin);
            prey = (int)(fSin * nxtX + nxtY * fCos);
            if(prex >= 0 && prex < im->width() && prey >= 0 && prey < im->height()){
                nim->setPixelColor(j,i,im->pixelColor(prex,prey));
            }
            else{
                nim->setPixelColor(j,i,qRgb(255,255,255));
            }
        }
    }
    delete im;
    im = nim;
    nim = nullptr;
}

void Geometric_Transformer::Shrink(int k, QImage *&im){
    double quo = 100.0 / k;
    int nw = (int)(im->width() / quo);
    int nh = (int)(im->height() / quo);
    QImage* nim = new QImage(nw,nh,QImage::Format_RGB888);
    for(int i = 0;i < nim->height();i++){
        int y = (int)(1.0 * i * quo);
        for(int j = 0; j < nim->width();j++){
            int x = (int)(1.0 * j * quo);
            if(x >= 0 && x < im->width() && y >= 0 && y < im->height())
                nim->setPixelColor(j,i,im->pixelColor(x,y));
        }
    }
    delete im;
    im = nim;
    nim = nullptr;
}

//x2 = x1 + 1 or x2 = x1 - 1
double Geometric_Transformer::linear_insert(double x1, double f1, double x2, double f2, double x){
    double temp = (x2 - x) * f1 + (x - x1) * f2;
    if(x1 < x2)return temp;
    else return -temp;
}
//双线性插值算法
void Geometric_Transformer::Enlarge(int k, QImage *&im){
    double quo = k / 100.0;
    int nw = (int)(im->width() * quo);
    int nh = (int)(im->height() * quo);
    QImage* nim = new QImage(nw,nh,QImage::Format_RGB888);
    for(int i = 0;i < nim->height();i++){
        double cy = (i + 0.5) / nh * im->height() - 0.5;
        int y1 = (int)(cy);
        for(int j = 0; j < nim->width();j++){
            double cx = (j + 0.5) / nw * im->width() - 0.5;
            int x1 = (int)(cx);

            int x2 = x1 + 1 < im->width() ? x1 + 1 : x1 - 1;
            int y2 = y1 + 1 < im->height() ? y1 + 1 : y1 - 1;
            double r1,r2,g1,g2,b1,b2;
            int r,g,b;
            r1 = linear_insert(x1,qRed(im->pixel(x1,y1)),x2,qRed(im->pixel(x2,y1)),cx);
            r2 = linear_insert(x1,qRed(im->pixel(x1,y2)),x2,qRed(im->pixel(x2,y2)),cx);
            g1 = linear_insert(x1,qGreen(im->pixel(x1,y1)),x2,qGreen(im->pixel(x2,y1)),cx);
            g2 = linear_insert(x1,qGreen(im->pixel(x1,y2)),x2,qGreen(im->pixel(x2,y2)),cx);
            b1 = linear_insert(x1,qBlue(im->pixel(x1,y1)),x2,qBlue(im->pixel(x2,y1)),cx);
            b2 = linear_insert(x1,qBlue(im->pixel(x1,y2)),x2,qBlue(im->pixel(x2,y2)),cx);
            r = (int)(linear_insert(y1,r1,y2,r2,cy) + 0.5);
            g = (int)(linear_insert(y1,g1,y2,g2,cy) + 0.5);
            b = (int)(linear_insert(y1,b1,y2,b2,cy) + 0.5);

            nim->setPixelColor(j,i,qRgb(r,g,b));
        }
    }
    delete im;
    im = nim;
    nim = nullptr;
}
void Geometric_Transformer::Horizontal_Shear(int angle, QImage *&im){
    double theta = angle * 3.1415 / 180;
    double k = tan(theta);

    int nw = (int)max(im->width() + k * im->height(),im->width() - k * im->height());

    QImage* nim = new QImage(nw,im->height(),QImage::Format_RGB888);
    for(int i = 0; i < nim->height();i++){
        int y = i;
        for(int j = 0;j < nim->width();j++){
            int x;
            if(angle < 0){
                x = (int)(j - k * y + k * im->height());
            }
            else{
                x = (int)(j - k * y);
            }

            if(x >= 0 && x < im->width()){
                nim->setPixelColor(j,i,im->pixel(x,y));
            }
            else {
                nim->setPixelColor(j,i,qRgb(255,255,255));
            }
        }
    }
    delete im;
    im = nim;
    nim = nullptr;
}
void Geometric_Transformer::Vertical_Shear(int angle, QImage *&im){
    double theta = angle * 3.1415 / 180;
    double k = tan(theta);

    int nh = (int)max(im->height() + k * im->width(),im->height() - k * im->width());

    QImage* nim = new QImage(im->width(),nh,QImage::Format_RGB888);
    for(int i = 0;i < nim->height();i++){
        for(int j = 0; j < nim->width();j++){
            int x,y;
            x = j;
            if(angle < 0){
                y = (int)(i - k * x + k * im->width());
            }
            else{
                y = (int)(i - k * x);
            }

            if(y >= 0 && y < im->height()){
                nim->setPixelColor(j,i,im->pixel(x,y));
            }
            else {
                nim->setPixelColor(j,i,qRgb(255,255,255));
            }
        }
    }
    delete im;
    im = nim;
    nim = nullptr;
}
