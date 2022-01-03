#ifndef GEOMETRIC_TRANSFORMER_H
#define GEOMETRIC_TRANSFORMER_H

#include <QImage>
class Geometric_Transformer
{
public:
    Geometric_Transformer();
    static void translate(int w,int h,QImage*& im);
    static void Horizontal_Mirror(QImage*& im);
    static void Vertical_Mirror(QImage*& im);
    //弧度制
    static void Rotate(double angle,QImage*& im);
    static void Shrink(int k,QImage*& im);
    static void Enlarge(int k,QImage*& im);
    static double linear_insert(double x1,double y1,double x2,double y2,double x);
    //度数制
    static void Horizontal_Shear(int angle,QImage*& im);
    static void Vertical_Shear(int angle,QImage*& im);
};

#endif // GEOMETRIC_TRANSFORMER_Hs
