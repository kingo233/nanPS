#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "geometric_transformer.h"
#include <QPainter>
#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QInputDialog>
#include "gray_histogram.h"
#include "gray_transformer.h"

;
#pragma pack(push)
#pragma pack(1)
struct BMPFILEHEAD
{
    char bfType[2];
    unsigned int bfSize;
    int bReserved;
    unsigned int bfOffBits;
};
#pragma pack(pop)


;
#pragma pack(push)
#pragma pack(1)
struct BITMAPFILEHEADER
{
    unsigned int ciSize;//BITMAPFILEHEADER所占的字节数
    unsigned int ciWidth;//宽度
    int ciHeight;//高度
    char ciPlanes[2];//目标设备的位平面数，值为1
    short ciBitCount;//每个像素的位数
    int ciCompress;//压缩说明
    int ciSizeImage;//用字节表示的图像大小，该数据必须是4的倍数
    int ciXPelsPerMeter;//目标设备的水平像素数/米
    int ciYPelsPerMeter;//目标设备的垂直像素数/米
    int ciClrUsed; //位图使用调色板的颜色数
    int ciClrImportant; //指定重要的颜色数，当该域的值等于颜色数时（或者等于0时），表示所有颜色都一样重要
};
#pragma pack(pop)

;
#pragma pack(push)
#pragma pack(1)
struct RGB
{
    unsigned char b,g,r;
};
#pragma pack(pop)


;
#pragma pack(push)
#pragma pack(1)
struct RGBQUAD
{
    unsigned char    rgbBlue;
    unsigned char    rgbGreen;
    unsigned char    rgbRed;
    unsigned char    rgbReserved;
};
#pragma pack(pop)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    imgloaded = false;
    image = nullptr;
    ihp = nullptr;
    fhp = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
    if(image != nullptr)delete image;
    if(ihp != nullptr) delete (BITMAPFILEHEADER*)ihp;
    if(fhp != nullptr) delete (BMPFILEHEAD*)fhp;
}
void MainWindow::readbmp(QString filename){
    QFile* fileno = new QFile(filename);
    if(!fileno->open(QIODevice::ReadOnly)){
        qDebug() << "打开失败";
    }
    QDataStream stream(fileno);
    BMPFILEHEAD * fh = new BMPFILEHEAD;
    BITMAPFILEHEADER * ih = new BITMAPFILEHEADER;
    stream.readRawData((char*)fh,14);
    stream.readRawData((char*)ih,40);
    fhp = fh; ihp = ih;

    if(ih->ciHeight < 0){
        //TODO
    }
    RGBQUAD* tsb = nullptr;
    if(ih->ciBitCount != 24){
        //有调色板
        int tiaolines = 1 << ih->ciBitCount;
        tsb = new RGBQUAD[tiaolines];
        stream.readRawData((char*)tsb,tiaolines * sizeof(RGBQUAD));//256 * 4

    }

    //qDebug() <<tsb[255].rgbRed << tsb[255].rgbGreen << tsb[255].rgbBlue;
    stream.device()->seek(fh->bfOffBits);
    image = new QImage(ih->ciWidth,ih->ciHeight,QImage::Format_RGB888);

    if(tsb != nullptr){
        char* perline = new char[ih->ciWidth];
        int skip = 4 - (((ih->ciWidth * ih->ciBitCount) >> 3) & 3);
        skip %= 4;
        for(int i = 0;i < ih->ciHeight;i++){
            stream.readRawData(perline,ih->ciWidth);
            stream.skipRawData(skip);

            for(int j = 0; j < ih->ciWidth;j++){

                RGBQUAD t = tsb[(unsigned char)perline[j]];
                //qDebug() << (int)perline[j];
                image->setPixelColor(j,ih->ciHeight - i - 1,qRgb(t.rgbRed,t.rgbGreen,t.rgbBlue));
            }
        }
        delete [] perline;
        delete tsb;
    }
    else{
        RGB* pixels = new RGB[ih->ciWidth];
        int skip = 4 - (((ih->ciWidth * ih->ciBitCount) >> 3) & 3);
        skip %= 4;
        for(int i = 0;i < ih->ciHeight;i++){
            stream.readRawData((char*)pixels,ih->ciWidth * sizeof(RGB));
            stream.skipRawData(skip); //跳过对齐用的00字节
            for(int j = 0; j < ih->ciWidth;j++){
                (*image).setPixelColor(j,ih->ciHeight - i - 1,qRgb(pixels[j].r,pixels[j].g,pixels[j].b));
            }
        }
        delete [] pixels;
    }

    delete fileno;
    imgloaded = true;
}
void MainWindow::paintEvent(QPaintEvent *){
    if(image != nullptr){
        QPainter painter(this);
        int w = ui->horizontalScrollBar->value();
        int h = ui->verticalScrollBar->value();
        //工具栏有高度26
        painter.drawImage(QPoint(0,26),image->copy(w,h,image->width() - w,image->height() - h));
    }
}
void MainWindow::on_openfile_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this,"打开图片",".","bmp files (*.bmp)");
    if(filename != "") {
        if(image != nullptr){
            delete image;
            image = nullptr;
        }
        readbmp(filename);
    }
    else return;
    update();

}

void MainWindow::on_gray_triggered()
{
    if(image == nullptr)return;
    Gray_Transformer::gray(image);
    update();
}

void MainWindow::on_profile_triggered()
{
    if(image == nullptr)return;
    BMPFILEHEAD * fh = (BMPFILEHEAD*)fhp;
    BITMAPFILEHEADER * ih = (BITMAPFILEHEADER*)ihp;
    QMessageBox::about(NULL, "关于该图片",
                       QString("图片大小：") +
                       QString::number(fh->bfSize) + QString("字节\r\n") +
                       QString("宽度：") +
                       QString::number(ih->ciWidth) + QString("像素\r\n") +
                       QString("高度：") +
                       QString::number(ih->ciHeight) + QString("像素"));
}

void MainWindow::on_translate_triggered()
{
    if(image == nullptr)return;
    QDialog dialog(this);
    dialog.setMinimumSize(QSize(200,150));
    QFormLayout form(&dialog);
    form.addRow(new QLabel("请输入参数:"));
    // Value1
    QString value1 = QString("宽: ");
    QSpinBox *spinbox1 = new QSpinBox(&dialog);
    spinbox1->setMaximum(9999);
    form.addRow(value1, spinbox1);
    // Value2
    QString value2 = QString("高: ");
    QSpinBox *spinbox2 = new QSpinBox(&dialog);
    spinbox2->setMaximum(9999);
    form.addRow(value2, spinbox2);
    // Add Cancel and OK button
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // Process when OK button is clicked
    if (dialog.exec() == QDialog::Accepted) {
        //qDebug() << spinbox1->value() << spinbox2->value();
        Geometric_Transformer::translate(spinbox1->value(),spinbox2->value(),image);
    }
}

void MainWindow::on_Horizontal_Mirror_triggered()
{
    if(image == nullptr)return;
    Geometric_Transformer::Horizontal_Mirror(image);
    update();
}

void MainWindow::on_Vertical_Mirror_triggered()
{
    if(image == nullptr)return;
    Geometric_Transformer::Vertical_Mirror(image);
    update();
}

void MainWindow::on_Rotate_triggered()
{
    if(image == nullptr)return;
    bool ok;

    double angle = QInputDialog::getDouble(this,"角度","弧度制角度",0,-7,7,2,&ok,Qt::Dialog,0.01);
    if(ok){
        Geometric_Transformer::Rotate(angle,image);
        update();
    }
}


void MainWindow::on_verticalScrollBar_valueChanged(int value)
{
    update();
}

void MainWindow::on_horizontalScrollBar_valueChanged(int value)
{
    update();
}

void MainWindow::on_Shrink_triggered()
{
    if(image == nullptr)return;
    bool ok;

    int k = QInputDialog::getInt(this,"倍数","原图的百分比",100,1,100,1,&ok);
    if(ok){
        Geometric_Transformer::Shrink(k,image);
        update();
    }
}

void MainWindow::on_Enlarge_triggered()
{
    if(image == nullptr)return;
    bool ok;
    int k = QInputDialog::getInt(this,"倍数","原图的百分比",100,100,1000,1,&ok);
    if(ok){
        Geometric_Transformer::Enlarge(k,image);
        update();
    }
}

void MainWindow::on_saveas_triggered()
{
    if(image == nullptr)return;
    QString filename = QFileDialog::getSaveFileName(this,"保存图片",".","Images (*.bmp *.png *.jpg)");
    if(filename != ""){
        image->save(filename);
    }
}



void MainWindow::on_Horizontal_Shear_triggered()
{
    if(image == nullptr)return;
    bool ok;

    int k = QInputDialog::getInt(this,"角度","与y轴的错切角度",0,-89,89,1,&ok);
    if(ok){
        Geometric_Transformer::Horizontal_Shear(k,image);
        update();
    }

}

void MainWindow::on_Vertical_Shear_triggered()
{
    if(image == nullptr)return;
    bool ok;

    int k = QInputDialog::getInt(this,"角度","与x轴的错切角度",0,-89,89,1,&ok);
    if(ok){
        Geometric_Transformer::Vertical_Shear(k,image);
        update();
    }
}

void MainWindow::on_Histogram_triggered()
{
    if(image == nullptr)return;
    Gray_histogram* gray = new Gray_histogram(image);
    gray->show();
}

void MainWindow::on_Histogram_Equalize_triggered()
{
    if(image == nullptr)return;
    Gray_Transformer::Histogram_Equalize(image);
    update();
}

void MainWindow::on_HSI_Histogram_Equalize_triggered()
{
    if(image == nullptr)return;
    Gray_Transformer::HSI_Histogram_Equalize(image);
    update();
}

void MainWindow::on_avg_filter_triggered()
{
    if(image == nullptr)return;
    Gray_Transformer::avg_filter(image);
    update();
}



void MainWindow::on_high_freq_filter_triggered()
{
    if(image == nullptr)return;
    Gray_Transformer::high_freq_filter(image);
    update();
}

void MainWindow::on_Gradient_sharpen_triggered()
{
    if(image == nullptr)return;
    Gray_Transformer::Gradient_sharpen(image);
    update();
}

void MainWindow::on_Edge_detection_triggered()
{
    if(image == nullptr)return;
    Gray_Transformer::Edge_detection(image);
    update();
}

void MainWindow::on_Image_segmentation_triggered()
{
    if(image == nullptr)return;
    Gray_Transformer::Image_segmentation(image);
    update();
}

void MainWindow::on_Add_Gaussian_Noise_triggered()
{
    if(image == nullptr)return;
    Gray_Transformer::Add_Gaussian_Noise(image);
    update();
}

void MainWindow::on_Maximum_entropy_segmentation_triggered()
{
    if(image == nullptr)return;
    Gray_Transformer::Maximum_entropy_segmentation(image);
    update();
}
