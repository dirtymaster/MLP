#include "matrix_network.h"

namespace s21 {
void MatrixNetwork::InitNetwork(const std::vector<int>& config, double learning_rate) {
    layers_count_ = config.size();
    size.resize(layers_count_);
    for (int i = 0; i < layers_count_; i++) size[i] = config[i];
    weights.resize(layers_count_ - 1);
    bios.resize(layers_count_ - 1);
    for (int i = 0; i < layers_count_ - 1; i++) {
        weights[i].Init(size[i + 1], size[i]);
        bios[i].resize(size[i + 1]);
        weights[i].Rand();
        for (int j = 0; j < size[i + 1]; j++) {
            bios[i][j] = Matrix::GenerateRandomNumber(-1.0, 1.0);
        }
    }
    neurons_val.resize(layers_count_);
    neurons_err.resize(layers_count_);
    for (int i = 0; i < layers_count_; i++) {
        neurons_val[i].resize(size[i]);
        neurons_err[i].resize(size[i]);
    }
    neurons_bios_val.resize(layers_count_ - 1);
    for (int i = 0; i < layers_count_ - 1; i++) neurons_bios_val[i] = 1;
    input_neurons_count_ = config[0];
    output_neurons_count_ = config.back();
    expected_values_.resize(output_neurons_count_);
    learning_rate_ = learning_rate;
    ClearVectors(true, true, true, true, true);
    is_trained_ = false;
}

void MatrixNetwork::ClearVectors(bool precisions, bool recalls, bool f_measures, bool mse_for_testing,
                                 bool mse_for_training) {
    if (precisions) precisions_.clear();
    if (recalls) recalls_.clear();
    if (f_measures) f_measures_.clear();
    if (mse_for_testing) mse_for_testing_.clear();
    if (mse_for_training) mse_for_training_.clear();
}

void MatrixNetwork::SetInput(const std::vector<double>& values) {
    for (int i = 0; i < size[0]; i++) {
        neurons_val[0][i] = values[i + 1];
    }
}

void MatrixNetwork::ForwardProp(const std::vector<std::vector<double>>& letters) {
    SetInput(letters[iterator_]);
    for (int k = 1; k < layers_count_; ++k) {
        Matrix::Multi(weights[k - 1], neurons_val[k - 1], size[k - 1], neurons_val[k]);
        Matrix::SumVector(neurons_val[k], bios[k - 1], size[k]);
        actFunc.Use(neurons_val[k], size[k]);
    }
}

int MatrixNetwork::SearchMaxIndex(const std::vector<double>& value) {
    double max = value[0];
    int prediction = 0;
    double tmp;
    for (int j = 1; j < size[layers_count_ - 1]; j++) {
        tmp = value[j];
        if (tmp > max) {
            prediction = j;
            max = tmp;
        }
    }
    return prediction;
}

void MatrixNetwork::BackProp() {
    double expect = training_letters_[iterator_][0];
    for (int i = 0; i < size[layers_count_ - 1]; i++) {
        if (i != int(expect))
            neurons_err[layers_count_ - 1][i] =
                -neurons_val[layers_count_ - 1][i] * actFunc.UseDer(neurons_val[layers_count_ - 1][i]);
        else
            neurons_err[layers_count_ - 1][i] =
                (1.0 - neurons_val[layers_count_ - 1][i]) * actFunc.UseDer(neurons_val[layers_count_ - 1][i]);
    }
    for (int k = layers_count_ - 2; k > 0; k--) {
        Matrix::Multi_T(weights[k], neurons_err[k + 1], size[k + 1], neurons_err[k]);
        for (int j = 0; j < size[k]; j++) neurons_err[k][j] *= actFunc.UseDer(neurons_val[k][j]);
    }
}

void MatrixNetwork::UpdateWeights() {
    for (int i = 0; i < layers_count_ - 1; ++i) {
        for (int j = 0; j < size[i + 1]; ++j) {
            for (int k = 0; k < size[i]; ++k) {
                weights[i](j, k) += neurons_val[i][k] * neurons_err[i + 1][j] * learning_rate_;
            }
        }
    }
    for (int i = 0; i < layers_count_ - 1; i++) {
        for (int k = 0; k < size[i + 1]; k++) {
            bios[i][k] += neurons_err[i + 1][k] * learning_rate_;
        }
    }
}

std::vector<std::vector<double>> MatrixNetwork::ParseData(std::string path) {
    std::vector<std::vector<double>> data;
    FILE* file = fopen(path.c_str(), "r");
    if (!file)
        throw std::invalid_argument("No such file!");
    else
        std::cout << "Парсинг файла..." << std::endl;
    int i = 0;
    int digit;
    std::vector<double> letter;
    while (fscanf(file, "%d,", &digit) != EOF) {
        if (i % (input_neurons_count_ + 1) == 0) {
            if (i != 0) {
                data.push_back(letter);
                letter.clear();
            }
            letter.push_back(digit - 1);
        } else {
            letter.push_back(digit / 255.);
        }
        ++i;
    }
    fclose(file);
    return data;
}

void MatrixNetwork::Train(int epochs_count, std::string filename, bool cross_validation,
                          int number_of_groups_k, int test_group_number) {
    if (training_letters_.empty()) {
        training_letters_ = ParseData(filename);
    }
    unsigned int start_training_time = clock(), start_testing_time = 0;
    std::cout << "Начало обучения..." << std::endl;
    double sum = 0.0, mse = 0.0, total_mse = 0.0, cross_validation_mse = 0.0, correct_answers_sum = 0.0;
    int cross_validation_j_min, cross_validation_j_max;
    std::vector<std::vector<int>> precisions_for_letters_vector;
    std::vector<std::vector<int>> recalls_for_letters_vector;
    ClearVectors(true, true, true, true, false);
    if (cross_validation) {
        InitCrossValidation(number_of_groups_k, test_group_number, cross_validation_j_min,
                            cross_validation_j_max, epochs_count, precisions_for_letters_vector,
                            recalls_for_letters_vector);
    }
    for (int i = 0; i < epochs_count; ++i) {
        iterator_ = 0;
        total_mse = 0;
        for (int j = 0; j < (int)training_letters_.size(); ++j) {
            ForwardProp(training_letters_);
            int idx = SearchMaxIndex(neurons_val[layers_count_ - 1]);
            // **************CROSS VALIDATION**************
            if (cross_validation) {
                if (j >= cross_validation_j_min && j < cross_validation_j_max) {
                    if (start_testing_time == 0) start_testing_time = clock();
                    if (idx == training_letters_[iterator_][0]) ++correct_answers_sum;
                    precisions_for_letters_vector[idx].push_back(
                        (training_letters_[iterator_][0] == idx) ? 1 : 0);
                    recalls_for_letters_vector[training_letters_[iterator_][0]].push_back(
                        (training_letters_[iterator_][0] == idx) ? 1 : 0);
                    PushBackMse(training_letters_, sum, mse, cross_validation_mse, mse_for_testing_,
                                j - cross_validation_j_min);
                } else {
                    if (start_testing_time != 0) {
                        time_spent_on_testing_ = (clock() - start_testing_time) / CLOCKS_PER_SEC;
                    }
                    BackProp();
                    UpdateWeights();
                }  // ********************************************
            } else {
                BackProp();
                UpdateWeights();
            }
            if ((j + 1) % 5000 == 0) {
                std::cout << "Обучено " << j + 1 << " букв " << i + 1 << "-й эпохи..." << std::endl;
                std::cout << "Текущее значение MSE: " << total_mse / (j + 1) << std::endl;
            }
            PushBackMse(training_letters_, sum, mse, total_mse, mse_for_training_, j);
            ++iterator_;
        }
    }
    if (cross_validation) {
        CalculateTestingResults(correct_answers_sum, (training_letters_.size() / number_of_groups_k),
                                precisions_for_letters_vector, recalls_for_letters_vector);
    }
    is_trained_ = true;
    time_spent_on_training_ =
        (clock() - start_training_time) / CLOCKS_PER_SEC - (cross_validation ? time_spent_on_testing_ : 0);
    std::cout << "TIME: " << time_spent_on_training_ << " sec" << std::endl;
}

void MatrixNetwork::InitCrossValidation(const int number_of_groups_k, const int test_group_number,
                                        int& cross_validation_j_min, int& cross_validation_j_max,
                                        int& epochs_count,
                                        std::vector<std::vector<int>>& precisions_for_letters_vector,
                                        std::vector<std::vector<int>>& recalls_for_letters_vector) {
    cross_validation_j_min =
        test_group_number == 1
            ? 0
            : (training_letters_.size() / number_of_groups_k) * (test_group_number - 1) - 1;
    cross_validation_j_max = test_group_number == number_of_groups_k
                                 ? training_letters_.size() - 1
                                 : training_letters_.size() / number_of_groups_k * test_group_number - 1;
    epochs_count = 1;
    precisions_for_letters_vector.resize(output_neurons_count_);
    recalls_for_letters_vector.resize(output_neurons_count_);
}

void MatrixNetwork::PushBackMse(std::vector<std::vector<double>>& letters, double& sum, double& mse,
                                double& total_mse, std::vector<double>& mse_vector, const int j) {
    UpdateExpectedValues(letters);
    sum = 0;
    for (int output_cnt = 0; output_cnt < output_neurons_count_; ++output_cnt) {
        double tmp = expected_values_[output_cnt] - neurons_val[layers_count_ - 1][output_cnt];
        sum += tmp * tmp;
    }
    mse = sum / output_neurons_count_;
    total_mse += mse;
    mse_vector.push_back(total_mse / (j + 1));
}

void MatrixNetwork::UpdateExpectedValues(std::vector<std::vector<double>>& letters) {
    for (int i = 0; i < output_neurons_count_; ++i) {
        expected_values_[i] = letters[iterator_][0] == i ? 1 : 0;
    }
}

void MatrixNetwork::CalculateTestingResults(const int correct_answers_sum, const double letters_size,
                                            std::vector<std::vector<int>>& precisions_for_letters_vector,
                                            std::vector<std::vector<int>>& recalls_for_letters_vector) {
    average_accuracy_ = correct_answers_sum / letters_size;
    for (int i = 0; i < output_neurons_count_; ++i) {
        precisions_.resize(output_neurons_count_);
        precisions_[i] = CalculateArithmeticMean(precisions_for_letters_vector[i]);
    }
    for (int i = 0; i < output_neurons_count_; ++i) {
        recalls_.resize(output_neurons_count_);
        recalls_[i] = CalculateArithmeticMean(recalls_for_letters_vector[i]);
    }
    for (int i = 0; i < output_neurons_count_; ++i) {
        f_measures_.resize(output_neurons_count_);
        f_measures_[i] = 2 * (precisions_[i] * recalls_[i] / (precisions_[i] + recalls_[i]));
    }
    average_mse_ = CalculateArithmeticMean(mse_for_testing_);
}

double MatrixNetwork::CalculateArithmeticMean(const std::vector<int>& vector) {
    return std::accumulate(vector.begin(), vector.end(), 0.0) / vector.size();
}
double MatrixNetwork::CalculateArithmeticMean(const std::vector<double>& vector) {
    return std::accumulate(vector.begin(), vector.end(), 0.0) / vector.size();
}

void MatrixNetwork::Test(double part_of_test_data, std::string filename) {
    if (testing_letters_.empty()) {
        testing_letters_ = ParseData(filename);
    }
    unsigned int start_time = clock();
    iterator_ = 0;
    double mse = 0.0, total_mse = 0.0, sum = 0.0, correct_answers_sum = 0.0;
    ClearVectors(true, true, true, true, false);
    std::vector<std::vector<int>> precisions_for_letters_vector;
    precisions_for_letters_vector.resize(output_neurons_count_);
    std::vector<std::vector<int>> recalls_for_letters_vector;
    recalls_for_letters_vector.resize(output_neurons_count_);
    for (int j = 0; j < testing_letters_.size() * part_of_test_data; ++j) {
        ForwardProp(testing_letters_);
        int idx = SearchMaxIndex(neurons_val[layers_count_ - 1]);
        if (idx == testing_letters_[iterator_][0]) ++correct_answers_sum;
        precisions_for_letters_vector[idx].push_back((testing_letters_[iterator_][0] == idx) ? 1 : 0);
        recalls_for_letters_vector[testing_letters_[iterator_][0]].push_back(
            (testing_letters_[iterator_][0] == idx) ? 1 : 0);
        if ((j + 1) % 3000 == 0) {
            std::cout << "Протестировано " << j + 1 << " букв..." << std::endl;
        }
        PushBackMse(testing_letters_, sum, mse, total_mse, mse_for_testing_, j);
        ++iterator_;
    }
    std::cout << "Тест завершен." << std::endl;
    CalculateTestingResults(correct_answers_sum, (testing_letters_.size() * part_of_test_data),
                            precisions_for_letters_vector, recalls_for_letters_vector);
    time_spent_on_testing_ = (clock() - start_time) / CLOCKS_PER_SEC;
    std::cout << "time spent: " << time_spent_on_testing_ << " sec" << std::endl;
}

void MatrixNetwork::TestOneLetter(std::vector<double> letter) {
    iterator_ = 0;
    std::vector<std::vector<double>> tmp_letters;
    tmp_letters.push_back(letter);
    ForwardProp(tmp_letters);
    int idx = SearchMaxIndex(neurons_val[layers_count_ - 1]);
    answer_ = (char)(idx + 97);
}

bool MatrixNetwork::SaveWeights(std::string filename) {
    std::fstream clear_file(filename, std::ios::out);
    clear_file.close();
    FILE* file = fopen(filename.c_str(), "w");
    if (!file) {
        return false;
    }
    fprintf(file, "%d ", layers_count_);
    for (int i = 0; i < layers_count_; ++i) {
        fprintf(file, "%d ", (int)neurons_val[i].size());
    }
    for (int i = 0; i < (int)weights.size(); ++i) {
        for (int j = 0; j < weights[i].GetRows(); ++j) {
            for (int k = 0; k < weights[i].GetCols(); ++k) {
                fprintf(file, "%lf ", weights[i](j, k));
            }
        }
    }
    fclose(file);
    return true;
}

bool MatrixNetwork::LoadWeights(std::string filename) {
    FILE* file = fopen(filename.c_str(), "r");
    if (!file) {
        return false;
    }
    std::vector<int> config;
    int layers_count;
    if (fscanf(file, "%d ", &layers_count) != 1) return false;
    for (int i = 0; i < layers_count; ++i) {
        int neurons_count;
        if (fscanf(file, "%d ", &neurons_count) != 1) return false;
        config.push_back(neurons_count);
    }
    InitNetwork(config, learning_rate_);
    for (int i = 0; i < (int)weights.size(); ++i) {
        for (int j = 0; j < weights[i].GetRows(); ++j) {
            for (int k = 0; k < weights[i].GetCols(); ++k) {
                if (fscanf(file, "%lf ", &weights[i](j, k)) != 1) return false;
            }
        }
    }
    fclose(file);
    is_trained_ = true;
    return true;
}

const double& MatrixNetwork::GetAverageAccuracy() { return average_accuracy_; }
const std::vector<double>& MatrixNetwork::GetPrecisions() { return precisions_; }
const std::vector<double>& MatrixNetwork::GetRecalls() { return recalls_; }
const std::vector<double>& MatrixNetwork::GetFMeasures() { return f_measures_; }
const double& MatrixNetwork::GetTimeSpentOnTesting() { return time_spent_on_testing_; }
const double& MatrixNetwork::GetTimeSpentOnTraining() { return time_spent_on_training_; }
const std::vector<double>& MatrixNetwork::GetMseForTraining() { return mse_for_training_; }
const std::vector<double>& MatrixNetwork::GetMseForTesting() { return mse_for_testing_; }
const double& MatrixNetwork::GetAverageMse() { return average_mse_; }
const char& MatrixNetwork::GetAnswer() { return answer_; }
}  // namespace s21
