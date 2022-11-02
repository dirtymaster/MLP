#include "msegraphwindow.h"

#include "ui_msegraphwindow.h"

msegraphwindow::msegraphwindow(QWidget *parent) : QWidget(parent), ui(new Ui::msegraphwindow) {
    ui->setupUi(this);
    controller = s21::Controller::GetInstance();
}

msegraphwindow::~msegraphwindow() { delete ui; }

void msegraphwindow::PrintGraph() {
    ui->widget->clearGraphs();

    double y_min = 0.0, y_max = 0.5, x_min = 0.0, x_max;
    x_max = controller->GetMseForTraining().size();
    ui->widget->xAxis->setRange(x_min, x_max);
    ui->widget->yAxis->setRange(y_min, y_max);

    QVector<double> x, y;
    for (int counter = x_min; counter < x_max; ++counter) {
        x.push_back(counter);
        y.push_back(controller->GetMseForTraining()[counter]);
    }

    ui->widget->addGraph();
    ui->widget->graph(0)->addData(x, y);
    ui->widget->replot();
}
