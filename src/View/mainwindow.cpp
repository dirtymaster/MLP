#include "mainwindow.h"

#include <QValidator>

#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    controller = s21::Controller::GetInstance();
    msegraph = new msegraphwindow;
    std::string dir_path;
    test_finished = false;
    dir_path = QCoreApplication::applicationDirPath().toStdString();
    dir_path = dir_path.replace(dir_path.size() - 3, 3, "");
    //    dir_path = "/Users/pilafber/Desktop/Projects/CPP7_MLP-0/";  // для билда в qt
    training_dataset_path_ = dir_path + "resources/emnist-letters-train.csv";
    testing_dataset_path_ = dir_path + "resources/emnist-letters-test.csv";
    graph_weights_path_ = dir_path + "resources/graph_weights.txt";
    matrix_weights_path_ = dir_path + "resources/matrix_weights.txt";
}

MainWindow::~MainWindow() {
    delete ui;
    delete controller;
    delete msegraph;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

void MainWindow::on_train_network_clicked() {
    const int input_neurons_count = 784;
    const int output_neurons_count = 26;
    if (!controller->GetIsTrained()) {
        std::vector<int> config;
        config.push_back(input_neurons_count);
        for (int i = 0; i < ui->hidden_layers_count->value(); ++i) {
            config.push_back(ui->hidden_neurons_number->value());
        }
        config.push_back(output_neurons_count);
        if (ui->graph_model->isChecked()) {
            controller->InitNetwork(config, ui->learning_rate->value(), true);
        } else {
            controller->InitNetwork(config, ui->learning_rate->value(), false);
        }
    }

    controller->Train(ui->epochs_count->value(), training_dataset_path_, ui->cross_validation->isChecked(),
                      ui->number_of_groups_k->value(), ui->test_group_number->value());

    UpdateTestingResultsAfterTraining();
    ChangeAvailabilityOfButtonsAfterTraining();
}

void MainWindow::UpdateTestingResultsAfterTraining() {
    ui->time_spent_on_training->setText(QString::number(controller->GetTimeSpentOnTraining()));
    if (ui->cross_validation->isChecked()) {
        ui->time_spent->setText(QString::number(controller->GetTimeSpentOnTesting()));
        ui->average_mse->setText(QString::number(controller->GetAverageMse(), 'f', 2));
        ui->average_accuracy->setText(QString::number(controller->GetAverageAccuracy(), 'f', 2));
        if (ui->precision_line_edit->text().isEmpty()) ui->precision_line_edit->setText("a");
        ui->precision_for_letter->setText(QString::number(
            controller->GetPrecisions()[QStringToInt(ui->precision_line_edit->text())], 'f', 2));
        if (ui->recall_line_edit->text().isEmpty()) ui->recall_line_edit->setText("a");
        ui->recall_for_letter->setText(
            QString::number(controller->GetRecalls()[QStringToInt(ui->recall_line_edit->text())], 'f', 2));
        if (ui->f_measure_line_edit->text().isEmpty()) ui->f_measure_line_edit->setText("a");
        ui->f_measure_for_letter->setText(QString::number(
            controller->GetFMeasures()[QStringToInt(ui->f_measure_line_edit->text())], 'f', 2));
    }
}

void MainWindow::ChangeAvailabilityOfButtonsAfterTraining() {
    ui->define_letter->setEnabled(true);
    ui->start_test->setEnabled(true);
    ui->open_mse_graph->setEnabled(true);
    ui->define_letter_from_bmp->setEnabled(true);
    ui->restart_training->setEnabled(true);
    ui->graphicsView->setEnabled(true);
    ui->save_weights->setEnabled(true);
    ui->load_weights->setEnabled(false);
    ui->graph_model->setEnabled(false);
    ui->matrix_model->setEnabled(false);
    ui->epochs_count->setEnabled(false);
    ui->hidden_layers_count->setEnabled(false);
    ui->hidden_neurons_number->setEnabled(false);
    ui->learning_rate->setEnabled(false);
    ui->cross_validation->setEnabled(false);
    ui->number_of_groups_k->setEnabled(false);
    ui->test_group_number->setEnabled(false);
    ui->train_network->setText("Дообучить нейросеть");
    ui->start_test->setFocus();
}

void MainWindow::on_define_letter_clicked() { ui->answer->setText((QChar)controller->GetAnswer()); }

void MainWindow::on_start_test_clicked() {
    if (ui->cross_validation->isChecked()) {
        ui->epochs_count->setValue(1);
    }

    controller->Test(ui->test_file_part->value(), testing_dataset_path_);
    UpdateTestingResultsAfterTesting();
    test_finished = true;
}

void MainWindow::UpdateTestingResultsAfterTesting() {
    ui->average_accuracy->setText(QString::number(controller->GetAverageAccuracy(), 'f', 2));
    ui->average_mse->setText(QString::number(controller->GetAverageMse(), 'f', 2));
    ui->time_spent->setText(QString::number(controller->GetTimeSpentOnTesting()));

    if (ui->precision_line_edit->text().isEmpty()) ui->precision_line_edit->setText("a");
    ui->precision_for_letter->setText(
        QString::number(controller->GetPrecisions()[QStringToInt(ui->precision_line_edit->text())], 'f', 2));
    if (ui->recall_line_edit->text().isEmpty()) ui->recall_line_edit->setText("a");
    ui->recall_for_letter->setText(
        QString::number(controller->GetRecalls()[QStringToInt(ui->recall_line_edit->text())], 'f', 2));
    if (ui->f_measure_line_edit->text().isEmpty()) ui->f_measure_line_edit->setText("a");
    ui->f_measure_for_letter->setText(
        QString::number(controller->GetFMeasures()[QStringToInt(ui->f_measure_line_edit->text())], 'f', 2));
}

int MainWindow::QStringToInt(QString str) {
    QByteArray ba = str.toLocal8Bit();
    char *tmp = ba.data();
    if (tmp[0] < 97) {
        tmp[0] += 32;
    }
    return tmp[0] - 97;
}

void MainWindow::on_precision_line_edit_textChanged() {
    if (test_finished) {
        if (ui->precision_line_edit->text().contains(QRegularExpression("[А-Яа-я]"))) {
            ui->precision_line_edit->clear();
        } else if (controller->GetIsTrained() && !ui->precision_line_edit->text().isEmpty()) {
            ui->precision_for_letter->setText(QString::number(
                controller->GetPrecisions()[QStringToInt(ui->precision_line_edit->text())], 'f', 2));
        }
    }
}

void MainWindow::on_recall_line_edit_textChanged() {
    if (test_finished) {
        if (ui->recall_line_edit->text().contains(QRegularExpression("[А-Яа-я]"))) {
            ui->recall_line_edit->clear();
        } else if (controller->GetIsTrained() && !ui->recall_line_edit->text().isEmpty()) {
            ui->recall_for_letter->setText(QString::number(
                controller->GetRecalls()[QStringToInt(ui->recall_line_edit->text())], 'f', 2));
        }
    }
}

void MainWindow::on_f_measure_line_edit_textChanged() {
    if (test_finished) {
        if (ui->f_measure_line_edit->text().contains(QRegularExpression("[А-Яа-я]"))) {
            ui->f_measure_line_edit->clear();
        } else if (controller->GetIsTrained() && !ui->f_measure_line_edit->text().isEmpty()) {
            ui->f_measure_for_letter->setText(QString::number(
                controller->GetFMeasures()[QStringToInt(ui->f_measure_line_edit->text())], 'f', 2));
        }
    }
}

void MainWindow::on_open_mse_graph_clicked() {
    msegraph->show();
    msegraph->PrintGraph();
}

void MainWindow::on_define_letter_from_bmp_clicked() {
    QImage img = QImage(
        QFileDialog::getOpenFileName(this, tr("Open File"), "/path/to/file/", tr("Object Files (*.bmp)")));
    if (img.size().width() != img.size().height() || img.size().width() > 512 || img.size().height() > 512 ||
        img.size().width() < 28 || img.size().height() < 28) {
        QMessageBox::about(this, "Ошибка",
                           "Выберите квадратное изображение .bmp не меньше "
                           "28x28 и не больше 512x512");
    } else {
        img = img.scaled(28, 28);
        img = img.transformed(QTransform().rotate(-90));

        std::vector<double> Pixels;
        Pixels.push_back(-1);
        for (int i = img.size().width() - 1; i >= 0; i--) {
            for (int j = 0; j < img.size().height(); j++) {
                Pixels.push_back(img.pixelColor(j, i).black() / 255.);
            }
        }

        controller->TestOneLetter(Pixels);
        ui->answer->setText((QChar)controller->GetAnswer());
    }
}

void MainWindow::on_restart_training_clicked() { ChangeAvailabilityOfButtonsAfterRestart(); }

void MainWindow::ChangeAvailabilityOfButtonsAfterRestart() {
    ui->define_letter->setEnabled(false);
    ui->start_test->setEnabled(false);
    ui->open_mse_graph->setEnabled(false);
    ui->define_letter_from_bmp->setEnabled(false);
    ui->restart_training->setEnabled(false);
    ui->graphicsView->setEnabled(false);
    ui->save_weights->setEnabled(false);
    ui->train_network->setEnabled(true);
    ui->load_weights->setEnabled(true);
    ui->graph_model->setEnabled(true);
    ui->matrix_model->setEnabled(true);
    ui->epochs_count->setEnabled(true);
    ui->hidden_layers_count->setEnabled(true);
    ui->hidden_neurons_number->setEnabled(true);
    ui->learning_rate->setEnabled(true);
    ui->cross_validation->setEnabled(true);
    ui->number_of_groups_k->setEnabled(true);
    ui->test_group_number->setEnabled(true);
    ui->train_network->setFocus();
    ui->time_spent_on_training->setText("...");
    ui->time_spent->setText("...");
    ui->average_accuracy->setText("...");
    ui->precision_for_letter->setText("...");
    ui->recall_for_letter->setText("...");
    ui->f_measure_for_letter->setText("...");
    ui->average_mse->setText("...");
    ui->answer->setText("");
    ui->train_network->setText("Обучить нейросеть");
}

void MainWindow::on_cross_validation_stateChanged() {
    if (ui->cross_validation->isChecked()) {
        ui->epochs_count->setValue(1);
    }
}

void MainWindow::on_number_of_groups_k_valueChanged() {
    ui->test_group_number->setMaximum(ui->number_of_groups_k->value());
}

void MainWindow::on_save_weights_clicked() {
    bool error = false;
    if (ui->graph_model->isChecked()) {
        if (!controller->SaveWeights(graph_weights_path_)) error = true;
    } else {
        if (!controller->SaveWeights(matrix_weights_path_)) error = true;
    }
    if (error) {
        QMessageBox::about(this, "Ошибка", "Не удалось сохранить веса");
    } else {
        ui->save_weights->setEnabled(false);
    }
}

void MainWindow::on_load_weights_clicked() {
    bool error = false;
    if (ui->graph_model->isChecked()) {
        if (!controller->LoadWeights(graph_weights_path_)) error = true;
    } else {
        if (!controller->LoadWeights(matrix_weights_path_)) error = true;
    }
    if (error) {
        QMessageBox::about(this, "Ошибка", "Не удалось загрузить веса");
    } else {
        ChangeAvailabilityOfButtonsAfterLoadingWeights();
    }
}

void MainWindow::ChangeAvailabilityOfButtonsAfterLoadingWeights() {
    ui->define_letter->setEnabled(true);
    ui->start_test->setEnabled(true);
    ui->define_letter_from_bmp->setEnabled(true);
    ui->restart_training->setEnabled(true);
    ui->graphicsView->setEnabled(true);
    ui->train_network->setEnabled(false);
    ui->load_weights->setEnabled(false);
    ui->graph_model->setEnabled(false);
    ui->matrix_model->setEnabled(false);
    ui->epochs_count->setEnabled(false);
    ui->hidden_layers_count->setEnabled(false);
    ui->hidden_neurons_number->setEnabled(false);
    ui->learning_rate->setEnabled(false);
    ui->cross_validation->setEnabled(false);
    ui->number_of_groups_k->setEnabled(false);
    ui->test_group_number->setEnabled(false);
    ui->start_test->setFocus();
}

void MainWindow::on_matrix_model_clicked() { ui->hidden_layers_count->setMaximum(1); }

void MainWindow::on_graph_model_clicked() { ui->hidden_layers_count->setMaximum(5); }
