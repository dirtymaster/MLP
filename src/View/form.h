#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QtWidgets>
#include <iostream>

QT_BEGIN_NAMESPACE
extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality,
                                          bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

#include "../Controller/controller.h"

namespace Ui {
class Form;
}

class Form : public QWidget {
    Q_OBJECT
public:
    explicit Form(QWidget *parent = nullptr);

private:
    Ui::Form *ui;
    QTimer *timer;
    QPixmap m_pixmap;
    QPoint m_lastPos;
    s21::Controller *controller;
    bool network_is_trained = false;

    void draw(const QPoint &pos);
    void clearWindow();
    void SavePixels();
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
};

#endif  // FORM_H
