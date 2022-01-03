#include "gray_transformer.h"
#include "math.h"
#include <algorithm>
#include <QDebug>
#include <QtMath>
using namespace std;

const double pi_3 = 3.1415926 / 3;
Gray_Transformer::Gray_Transformer()
{

}

void Gray_Transformer::rgb2hsi(int r, int g, int b, double &h, double &s, double &i){
    double dr = 1.0 * r / 255;
    double dg = 1.0 * g / 255;
    double db = 1.0 * b / 255;
    i = (dr + dg + db) / 3;
    if(i < 0 || i > 1)qDebug() << "i" << i;
    s = 1 - (3 * min({dr,dg,db}) / (dr + dg + db));
    if(s < 0 || s > 1)qDebug() << "s" << s;
    double temp1 = (2 * dr - dg - db) / 2;
    double temp2 = sqrt((dr - dg) * (dr - dg) + (dr - db) * (dg - db));
    if(fabs(temp2) < 1e-6)temp2 += 1e-4;
    h = acos(temp1 / temp2);
    if(h < 0 || h > 6 * pi_3)qDebug() << "h" << h;
}

void Gray_Transformer::hsi2rgb(double h, double s, double i, int &r, int &g, int &b){
    double dr,dg,db;
    double temp;
    const double eps = 1e-4;
    if(h >= 0 && h <= pi_3 * 2){
        //小于120度
        db = i * (1 - s);
        temp = cos(pi_3 - h);
        if(fabs(temp) < 1e-6)temp += 1e-4;
        dr = i * (1 + (s * cos(h)) / temp);
        dg = 3 * i - db - dr;
    }
    else if(h >= 0 && h < 4 * pi_3){
        h -= pi_3 * 2;
        dr = i * (1 - s);
        temp = cos(pi_3 - h);
        if(fabs(temp) < 1e-6)temp += 1e-4;
        dg = i * (1 + (s * cos(h)) / temp);
        db = 3 * i - dg - dr;
    }
    else if(h >= 0 && h < 6 * pi_3){
        h -=  pi_3 * 4;
        dg = i * (1 - s);
        temp = cos(pi_3 - h);
        if(fabs(temp) < 1e-6)temp += 1e-4;
        db = i * (1 + (s * cos(h)) / temp);
        dr = 3 * i - dg - db;
    }
    else{
        db = dg = dr = i * (1 - s);
    }
    r = (int)(dr * 255 + 0.5);
    g = (int)(dg * 255 + 0.5);
    b = (int)(db * 255 + 0.5);
}


void Gray_Transformer::gray(QImage *&im){
    for(int i = 0;i < im->height();i++){
        for(int j = 0; j < im->width();j++){
            QRgb t = im->pixel(j,i);
            //int gray = qGray(t); //algorithm 1
            int gray = (qBlue(t) + qRed(t) + qGreen(t)) / 3;
            im->setPixelColor(j,i,qRgb(gray,gray,gray));
        }
    }
}

void Gray_Transformer::Histogram_Equalize(QImage *&im){
    int rcnt[256] = {0};
    int gcnt[256] = {0};
    int bcnt[256] = {0};
    int imsize = im->width() * im->height();
    for(int i = 0;i < im->height();i++){
        for(int j = 0;j < im->width();j++){
            int r = qRed(im->pixel(j,i));
            int g = qGreen(im->pixel(j,i));
            int b = qBlue(im->pixel(j,i));
            rcnt[r]++;
            gcnt[g]++;
            bcnt[b]++;
        }
    }
    double rf[256] = {0};
    double gf[256] = {0};
    double bf[256] = {0};
    //计算频率和累加概率
    for(int i = 0; i < 256;i++){
        rf[i] = 1.0 * rcnt[i] / imsize;
        gf[i] = 1.0 * gcnt[i] / imsize;
        bf[i] = 1.0 * bcnt[i] / imsize;
        //概率
        if(i){
            rf[i] += rf[i - 1];
            gf[i] += gf[i - 1];
            bf[i] += bf[i - 1];
        }
        //累计概率
        rcnt[i] = (int)(rf[i] * 255 + 0.5);
        gcnt[i] = (int)(gf[i] * 255 + 0.5);
        bcnt[i] = (int)(bf[i] * 255 + 0.5);
    }
    for(int i = 0;i < im->height();i++){
        for(int j = 0;j < im->width();j++){
            int r = qRed(im->pixel(j,i));
            int g = qGreen(im->pixel(j,i));
            int b = qBlue(im->pixel(j,i));
            int nr = rcnt[r];
            int ng = gcnt[g];
            int nb = bcnt[b];
            im->setPixelColor(j,i,qRgb(nr,ng,nb));
        }
    }
}
void Gray_Transformer::HSI_Histogram_Equalize(QImage *&im){
    vector<vector<double>> dh(im->height());
    vector<vector<double>> ds(im->height());
    vector<vector<double>> di(im->height());
    int icnt[256] = {0};
    double ifrequncy[256];
    for(int i = 0;i < im->height();i++){
        for(int j = 0;j < im->width();j++){
            int r = qRed(im->pixel(j,i));
            int g = qGreen(im->pixel(j,i));
            int b = qBlue(im->pixel(j,i));
            dh[i].push_back(0);
            ds[i].push_back(0);
            di[i].push_back(0);
            rgb2hsi(r,g,b,dh[i][j],ds[i][j],di[i][j]);
            icnt[(int)(di[i][j] * 255 + 0.5)]++;
        }
    }
    int imsize = im->width() * im->height();
    for(int i = 0;i < 256;i++){
        ifrequncy[i] = 1.0 * icnt[i] / imsize;
        if(i){
            ifrequncy[i] += ifrequncy[i - 1];
        }
        icnt[i] = (int)(ifrequncy[i] * 255 + 0.5);
    }
    for(int i = 0;i < im->height();i++){
        for(int j = 0;j < im->width();j++){
            int origin_i = (int)(di[i][j] * 255 + 0.5);
            int now_i = icnt[origin_i];
            di[i][j] = 1.0 * now_i / 255;
            int r,g,b;
            hsi2rgb(dh[i][j],ds[i][j],di[i][j],r,g,b);
            im->setPixelColor(j,i,qRgb(r,g,b));
        }
    }
}

void Gray_Transformer::avg_filter(QImage *&im){
    for(int i = 0;i < im->height();i++){
        for(int j = 0;j < im->width();j++){
            if(i && i < im->height() - 1 && j && j < im->width() - 1){
                QRgb q1,q2,q3,q4,q5,q6,q7,q8,q9;
                q1 = im->pixel(j - 1,i - 1);
                q2 = im->pixel(j,i - 1);
                q3 = im->pixel(j + 1,i - 1);
                q4 = im->pixel(j - 1,i);
                q5 = im->pixel(j,i);
                q6 = im->pixel(j + 1,i);
                q7 = im->pixel(j - 1,i + 1);
                q8 = im->pixel(j,i + 1);
                q9 = im->pixel(j + 1,i + 1);
                int r,g,b;
                r = (qRed(q1) + qRed(q2) +qRed(q3) +qRed(q4) +qRed(q5) +qRed(q6) +qRed(q7) +qRed(q8) + qRed(q9)) / 9;
                g = (qGreen(q1) + qGreen(q2) +qGreen(q3) +qGreen(q4) +qGreen(q5) +qGreen(q6) +qGreen(q7) +qGreen(q8) + qGreen(q9)) / 9;
                b = (qBlue(q1) + qBlue(q2) +qBlue(q3) +qBlue(q4) +qBlue(q5) +qBlue(q6) +qBlue(q7) +qBlue(q8) + qBlue(q9)) / 9;
                im->setPixel(j,i,qRgb(r,g,b));
            }
        }
    }
}

void Gray_Transformer::high_freq_filter(QImage *&im){
    for(int i = 0;i < im->height();i++){
        for(int j = 0;j < im->width();j++){
            if(i && i < im->height() - 1 && j && j < im->width() - 1){
                QRgb q1,q2,q3,q4,q5,q6,q7,q8,q9;
                q1 = im->pixel(j - 1,i - 1);
                q2 = im->pixel(j,i - 1);
                q3 = im->pixel(j + 1,i - 1);
                q4 = im->pixel(j - 1,i);
                q5 = im->pixel(j,i);
                q6 = im->pixel(j + 1,i);
                q7 = im->pixel(j - 1,i + 1);
                q8 = im->pixel(j,i + 1);
                q9 = im->pixel(j + 1,i + 1);
                int r,g,b;
                r = -qRed(q2) - qRed(q4) + 5 * qRed(q5) - qRed(q6) - qRed(q8);
                g = -qGreen(q2) - qGreen(q4) + 5 * qGreen(q5) - qGreen(q6) - qGreen(q8);
                b = -qBlue(q2) - qBlue(q4) + 5 * qBlue(q5) - qBlue(q6) - qBlue(q8);
                if(r < 0)r = 0;
                else if(r > 255)r = 255;
                if(g < 0)g = 0;
                else if(g > 255)g = 255;
                if(b < 0)b = 0;
                else if(b > 255)b = 255;
                im->setPixel(j,i,qRgb(r,g,b));
            }
        }
    }
}

void Gray_Transformer::Gradient_sharpen(QImage *&im){
    QImage* nim = new QImage(im->width(),im->height(),QImage::Format_RGB888);
    //求出rgb三个分量的梯度，然后求三个梯度的均值
    for(int i = 0;i < im->height();i++){
        for(int j = 0; j < im->width();j++){
            if(i && j){
                QRgb q1,q2,q3;
                q1 = im->pixel(j,i - 1);
                q2 = im->pixel(j,i);
                q3 = im->pixel(j - 1,i);
                //近似梯度
                int t1 = abs(qRed(q1) - qRed(q2)) + abs(qRed(q2) - qRed(q3));
                int t2 = abs(qGreen(q1) - qGreen(q2)) + abs(qGreen(q2) - qGreen(q3));
                int t3 = abs(qBlue(q1) - qBlue(q2)) + abs(qBlue(q2) - qBlue(q3));
                int r,g,b;
                if(t1 > 30)r = min(qRed(q2) + t1,255);
                else r = qRed(q2);
                if(t2 > 30)g = min(qGreen(q2) + t2,255);
                else g = qGreen(q2);
                if(t3 > 30)b = min(qBlue(q2) + t3,255);
                else b = qBlue(q2);
                nim->setPixelColor(j,i,qRgb(r,g,b));
            }
        }
    }
    delete im;
    im = nim;
    nim = nullptr;
}
void Gray_Transformer::Edge_detection(QImage *&im){
    QImage* nim = new QImage(im->width(),im->height(),QImage::Format_RGB888);
    int r1,r2,r3,r4,g1,g2,g3,g4,b1,b2,b3,b4;
    for(int i = 0;i < im->height();i++){
        for(int j = 0;j < im->width();j++){
            if(i && j){
                QRgb q1,q2,q3,q4;
                q1 = im->pixel(j -  1,i - 1);
                q2 = im->pixel(j,i - 1);
                q3 = im->pixel(j - 1,i);
                q4 = im->pixel(j,i);
                r1 = qRed(q1);g1 = qGreen(q1);b1 = qBlue(q1);
                r2 = qRed(q2);g2 = qGreen(q2);b2 = qBlue(q2);
                r3 = qRed(q3);g3 = qGreen(q3);b3 = qBlue(q3);
                r4 = qRed(q4);g4 = qGreen(q4);b4 = qBlue(q4);
                int t1 = abs(r1 - r4) + abs(r2 - r3);
                int t2 = abs(g1 - g4) + abs(g2 - g3);
                int t3 = abs(b1 - b4) + abs(b2 - b3);
                nim->setPixelColor(j,i,qRgb(t1,t2,t3));
            }
        }
    }
    delete im;
    im = nim;
    nim = nullptr;
}
void Gray_Transformer::Image_segmentation(QImage *&im){
    int T,cnt1,cnt2,u1,u2;
    int ou1,ou2;
    cnt1 = cnt2 =  0;
    u1 = 0;u2 = 1;ou1 = 1;ou2 = 0;
    T = 127;
    while (u1 != ou1 || u2 != ou2) {
        //qDebug() << u1 << u2 << T;
        ou1 = u1;
        ou2 = u2;
        u1 = u2 = 0;
        cnt1 = cnt2 = 0;
        for(int i = 0;i < im->height();i++){
            for(int j = 0;j < im->width();j++){
                int gray = qGray(im->pixel(j,i));
                if(gray <= T){
                    u1 += gray;
                    cnt1++;
                }
                else{
                    u2 += gray;
                    cnt2++;
                }
            }
        }
        u1 /= cnt1;
        u2 /= cnt2;
        T = (u1 + u2) / 2;
    }
    for(int i = 0;i < im->height();i++){
        for(int j = 0;j < im->width();j++){
            int gray = qGray(im->pixel(j,i));
            if(gray <= T){
                im->setPixelColor(j,i,qRgb(0,0,0));
            }
            else{
                im->setPixelColor(j,i,qRgb(255,255,255));
            }
        }
    }
}
//服从参数为(mu,sigma)的正态分布
double generateGaussianNoise(double mu, double sigma)
{
    static double V1, V2, S;
    static int phase = 0;
    double X;
    double U1,U2;
    if ( phase == 0 ) {
        do {
            U1 = (double)rand() / RAND_MAX;
            U2 = (double)rand() / RAND_MAX;

            V1 = 2 * U1 - 1;
            V2 = 2 * U2 - 1;
            S = V1 * V1 + V2 * V2;
        } while(S >= 1 || S == 0);

        X = V1 * sqrt(-2 * log(S) / S);
    } else{
        X = V2 * sqrt(-2 * log(S) / S);
    }
    phase = 1 - phase;
    return mu+sigma*X;
}
void Gray_Transformer::Add_Gaussian_Noise(QImage *&im){
    for(int i = 0;i < im->height();i++){
        for(int j = 0;j < im->width();j++){
            QRgb q = im->pixel(j,i);
            int r = qRed(q) + (int)generateGaussianNoise(0,16);
            int g = qGreen(q) + (int)generateGaussianNoise(0,16);
            int b = qBlue(q) + (int)generateGaussianNoise(0,16);
            r = max(0,min(255,r));
            g = max(0,min(255,g));
            b = max(0,min(255,b));
            im->setPixelColor(j,i,qRgb(r,g,b));
        }
    }
}
void Gray_Transformer::Maximum_entropy_segmentation(QImage *&im){
    int cnt[256] = {0};
    double p[256] = {0};
    for(int i = 0;i < im->height();i++){
        for(int j = 0;j < im->width();j++){
            QRgb q = im->pixel(j,i);
            cnt[qGray(q)]++;
        }
    }
    for(int i = 0;i < 256;i++){
        p[i] = 1.0 * cnt[i] / (im->height() * im->width());
    }
    int Th = 0;
    double maxe = 0.0;
    for(int t = 100;t <= 255;t++){
        double pt = 0.0;
        for(int i = 0;i <= t;i++)pt += p[i];
        double e1,e2;
        e1 = e2 = 0.0;
        for(int i = 0;i <= t;i++){
            e1 += -(p[i] / pt) * qLn(p[i] / pt);
        }
        for(int i = t + 1;i <= 255;i++){
            e2 += -(p[i] / (1 - pt)) * qLn(p[i] / (1 - pt));
        }

        if(e1 + e2 > maxe){
            maxe = e1 + e2;
            Th = t;
        }
    }

    for(int i = 0;i < im->height();i++){
        for(int j = 0;j < im->width();j++){
            int gray = qGray(im->pixel(j,i));
            if(gray < Th){
                im->setPixelColor(j,i,qRgb(0,0,0));
            }
            else{
                im->setPixelColor(j,i,qRgb(255,255,255));
            }
        }
    }
}
