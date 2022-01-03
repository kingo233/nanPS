#include "gray_histogram.h"
#include "ui_gray_histogram.h"
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QGridLayout>
#include <QBoxLayout>
#include <QChartView>
Gray_histogram::Gray_histogram(QImage*& im,QWidget* parent):
    QWidget(parent),ui(new Ui::Gray_histogram),im(im)
{
    ui->setupUi(this);
    caculate();
    QHBoxLayout *all_layout = new QHBoxLayout();
    QHBoxLayout *r_layout = new QHBoxLayout();
    QHBoxLayout *g_layout = new QHBoxLayout();
    QHBoxLayout *b_layout = new QHBoxLayout();
    ui->tab_all->setLayout(all_layout);
    ui->tab_r->setLayout(r_layout);
    ui->tab_g->setLayout(g_layout);
    ui->tab_b->setLayout(b_layout);
    QChartView *all_view = new QChartView(all_chart);
    QChartView *r_view = new QChartView(r_chart);
    QChartView *g_view = new QChartView(g_chart);
    QChartView *b_view = new QChartView(b_chart);
    all_layout->addWidget(all_view);
    r_layout->addWidget(r_view);
    g_layout->addWidget(g_view);
    b_layout->addWidget(b_view);
}

Gray_histogram::~Gray_histogram()
{
    delete ui;
    delete all_chart;
    delete r_chart;
    delete g_chart;
    delete b_chart;
}

void Gray_histogram::caculate(){
    int rcnt[256] = {0};
    int gcnt[256] = {0};
    int bcnt[256] = {0};
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

    all_chart = new QChart();
    r_chart = new QChart();
    g_chart = new QChart();
    b_chart = new QChart();
    QBarSeries *all_series = new QBarSeries(all_chart);
    QBarSeries *r_series = new QBarSeries(r_chart);
    QBarSeries *g_series = new QBarSeries(g_chart);
    QBarSeries *b_series = new QBarSeries(b_chart);
    QBarSet* all_set = new QBarSet("all");
    QBarSet* r_set = new QBarSet("r");
    QBarSet* g_set = new QBarSet("g");
    QBarSet* b_set = new QBarSet("b");
    double multi = im->height() * im->width();
    for(int i = 0;i < 256;i++){
        *all_set << (rcnt[i] + gcnt[i] + bcnt[i]) / multi;
        *r_set << rcnt[i] / multi;
        *g_set << gcnt[i] / multi;
        *b_set << bcnt[i] / multi;
    }
    all_series->append(all_set);
    r_series->append(r_set);
    g_series->append(g_set);
    b_series->append(b_set);
    all_chart->addSeries(all_series);
    r_chart->addSeries(r_series);
    g_chart->addSeries(g_series);
    b_chart->addSeries(b_series);
    all_chart->createDefaultAxes();
    r_chart->createDefaultAxes();
    b_chart->createDefaultAxes();
    all_chart->legend()->hide();
    r_chart->legend()->hide();
    g_chart->legend()->hide();
    b_chart->legend()->hide();
}

