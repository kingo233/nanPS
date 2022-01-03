#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void readbmp(QString filename);
    void paintEvent(QPaintEvent* );

private slots:
    void on_openfile_triggered();

    void on_gray_triggered();

    void on_profile_triggered();

    void on_translate_triggered();

    void on_Horizontal_Mirror_triggered();

    void on_Vertical_Mirror_triggered();

    void on_Rotate_triggered();
    
    void on_verticalScrollBar_valueChanged(int value);

    void on_horizontalScrollBar_valueChanged(int value);

    void on_Shrink_triggered();

    void on_Enlarge_triggered();

    void on_saveas_triggered();


    void on_Horizontal_Shear_triggered();

    void on_Vertical_Shear_triggered();

    void on_Histogram_triggered();

    void on_Histogram_Equalize_triggered();

    void on_HSI_Histogram_Equalize_triggered();

    void on_avg_filter_triggered();

    void on_high_freq_filter_triggered();

    void on_Gradient_sharpen_triggered();

    void on_Edge_detection_triggered();

    void on_Image_segmentation_triggered();

    void on_Add_Gaussian_Noise_triggered();

    void on_Maximum_entropy_segmentation_triggered();

private:
    Ui::MainWindow *ui;
    bool imgloaded;
    QImage* image;
    void* fhp,*ihp;
};

#endif // MAINWINDOW_H
