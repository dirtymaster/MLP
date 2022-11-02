#ifndef MSEGRAPHWINDOW_H
#define MSEGRAPHWINDOW_H

#include <QWidget>

#include "../Controller/controller.h"

namespace Ui {
class msegraphwindow;
}

class msegraphwindow : public QWidget {
    Q_OBJECT

public:
    explicit msegraphwindow(QWidget* parent = nullptr);
    ~msegraphwindow();
    void PrintGraph();

private:
    Ui::msegraphwindow* ui;
    s21::Controller* controller;
};

#endif  // MSEGRAPHWINDOW_H
