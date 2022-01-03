#ifndef GRAY_HISTOGRAM_H
#define GRAY_HISTOGRAM_H

#include <QWidget>
#include <QImage>
#include <QtCharts/QChart>
using namespace QtCharts;

namespace Ui {
class Gray_histogram;
}

class Gray_histogram : public QWidget
{
    Q_OBJECT

public:
    explicit Gray_histogram(QImage*& im,QWidget* parent = nullptr);
    ~Gray_histogram();

private:
    Ui::Gray_histogram *ui;
    QImage*& im;
    QChart *all_chart,*r_chart,*g_chart,*b_chart;

    void caculate();
};

#endif // GRAY_HISTOGRAM_H
