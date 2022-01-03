#ifndef GRAY_TRANSFORMER_H
#define GRAY_TRANSFORMER_H
#include <QImage>

class Gray_Transformer
{
public:
    Gray_Transformer();
    static void gray(QImage*& im);
    static void Histogram_Equalize(QImage*& im);
    static void HSI_Histogram_Equalize(QImage*& im);
    static void avg_filter(QImage*& im);
    static void high_freq_filter(QImage*& im);
    static void Gradient_sharpen(QImage*& im);
    static void Edge_detection(QImage*& im);
    static void Image_segmentation(QImage*& im);
    static void Add_Gaussian_Noise(QImage*& im);
    static void Maximum_entropy_segmentation(QImage*& im);

private:
    static void rgb2hsi(int r,int g,int b,double& h,double& s,double& i);
    static void hsi2rgb(double h,double s,double i,int& r,int& g,int& b);
};

#endif // GRAY_TRANSFORMER_H
