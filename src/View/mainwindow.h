#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtOpenGLWidgets/qopenglwidget.h>

#include <QFileDialog>
#include <QMainWindow>
#include <QVector>

#include "form.h"
#include "msegraphwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

#include "../Controller/controller.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    void UpdateTestingResultsAfterTraining();
    void ChangeAvailabilityOfButtonsAfterTraining();
    void UpdateTestingResultsAfterTesting();
    void ChangeAvailabilityOfButtonsAfterRestart();
    void ChangeAvailabilityOfButtonsAfterLoadingWeights();
    int QStringToInt(QString str);

    Ui::MainWindow* ui;
    msegraphwindow* msegraph;
    bool test_finished;
    std::string training_dataset_path_;
    std::string testing_dataset_path_;
    std::string graph_weights_path_;
    std::string matrix_weights_path_;
    s21::Controller* controller;

private slots:
    void on_train_network_clicked();
    void on_define_letter_clicked();
    void on_start_test_clicked();
    void on_precision_line_edit_textChanged();
    void on_recall_line_edit_textChanged();
    void on_f_measure_line_edit_textChanged();
    void on_open_mse_graph_clicked();
    void on_define_letter_from_bmp_clicked();
    void on_restart_training_clicked();
    void on_cross_validation_stateChanged();
    void on_number_of_groups_k_valueChanged();
    void on_save_weights_clicked();
    void on_load_weights_clicked();
    void on_matrix_model_clicked();
    void on_graph_model_clicked();
};
#endif  // MAINWINDOW_H
