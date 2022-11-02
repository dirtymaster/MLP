#include "graph_network.h"

namespace s21 {
void GraphNetwork::InitNetwork(const std::vector<int>& config, double learning_rate) {
    layers_.clear();
    for (int i = 0; i < (int)(config.size() - 1); ++i) {
        layers_.push_back(Layer(config[i], config[i + 1]));
    }
    input_neurons_count_ = config[0];
    output_neurons_count_ = config.back();
    learning_rate_ = learning_rate;
    expected_values_.resize(output_neurons_count_);
    ClearVectors(true, true, true, true, true);
    answer_ = '?';
    is_trained_ = false;
}

void GraphNetwork::ClearVectors(bool precisions, bool recalls, bool f_measures, bool mse_for_testing,
                                bool mse_for_training) {
    if (precisions) precisions_.clear();
    if (recalls) recalls_.clear();
    if (f_measures) f_measures_.clear();
    if (mse_for_testing) mse_for_testing_.clear();
    if (mse_for_training) mse_for_training_.clear();
}

std::vector<std::vector<double>> GraphNetwork::ParseData(std::string filename) {
    std::cout << "Парсинг файла..." << std::endl;
    std::vector<std::vector<double>> letters;
    std::vector<double> letter;
    FILE* file = fopen(filename.c_str(), "r");
    if (!file) throw std::invalid_argument("No such file!");
    int digit, i = 0;
    while (fscanf(file, "%d,", &digit) != EOF) {
        if (i % (input_neurons_count_ + 1) == 0) {
            if (i != 0) {
                letters.push_back(letter);
                letter.clear();
            }
            letter.push_back(digit);
        } else {
            letter.push_back(digit / 255.);
        }
        ++i;
    }
    fclose(file);
    return letters;
}

std::vector<double> GraphNetwork::ForwardProp(std::vector<std::vector<double>>& letters) {
    std::vector<double> inputs;
    inputs.resize(input_neurons_count_);
    for (int i = 1; i <= input_neurons_count_; ++i) {
        inputs[i] = letters[iterator_][i];
    }
    for (int i = 0; i < (int)(layers_.size()); ++i) {
        Layer& layer = layers_[i];
        std::vector<double> new_inputs;
        for (Neuron& n : layer.neurons) {
            double activation = Activate(n.weights, inputs);
            n.output = Transfer(activation);
            new_inputs.push_back(n.output);
        }
        inputs = new_inputs;
    }
    return inputs;  // output layer outputs
}

double GraphNetwork::Activate(std::vector<double> weights, std::vector<double> input) {
    double activation = weights.back();  // bias
    for (int i = 0; i < (int)(weights.size() - 1); ++i) {
        activation += weights[i] * input[i];
    }
    return activation;
}

double GraphNetwork::Transfer(double activation) { return 1.0 / (1.0 + exp(-activation)); }

double GraphNetwork::TransferDerivative(double output) { return output * (1 - output); }

void GraphNetwork::BackProp() {
    UpdateExpectedValues(training_letters_);

    for (int i = layers_.size() - 1; i >= 0; --i) {
        Layer& layer = layers_[i];
        std::vector<double> errors;
        if (i != (int)(layers_.size() - 1)) {
            for (int j = 0; j < (int)(layer.neurons.size()); ++j) {
                double error = 0.0;
                for (Neuron& n : layers_[i + 1].neurons) {
                    error += (n.weights[j] * n.delta);
                }
                errors.push_back(error);
            }
        } else {
            for (int j = 0; j < (int)(layer.neurons.size()); ++j) {
                Neuron& n = layer.neurons[j];
                errors.push_back((double)expected_values_[j] - n.output);  // expected - actual
            }
        }
        for (int j = 0; j < (int)layer.neurons.size(); ++j) {
            Neuron& n = layer.neurons[j];
            // gradient / derivative part of back prop
            n.delta = errors[j] * TransferDerivative(n.output);
        }
    }
}

void GraphNetwork::UpdateExpectedValues(std::vector<std::vector<double>>& letters) {
    for (int i = 0; i < output_neurons_count_; ++i) {
        expected_values_[i] = letters[iterator_][0] == i + 1 ? 1 : 0;
    }
}

void GraphNetwork::UpdateWeights() {
    std::vector<double> inputs;

    for (int i = 0; i < (int)layers_.size(); ++i) {
        if (i != 0) {
            for (Neuron& n : layers_[i - 1].neurons) {
                inputs.push_back(n.output);
            }
        }
        for (Neuron& n : layers_[i].neurons) {
            for (int j = 0; j < (int)(inputs.size()); ++j) {
                n.weights[j] += learning_rate_ * n.delta * inputs[j];
            }
            n.weights.back() += learning_rate_ * n.delta;
        }
        inputs.clear();
    }
}

void GraphNetwork::Train(int epochs_count, std::string filename, bool cross_validation,
                         int number_of_groups_k, int test_group_number) {
    if (training_letters_.empty()) {
        training_letters_ = ParseData(filename);
    }
    unsigned int start_training_time = clock(), start_testing_time = 0;
    std::cout << "Начало обучения..." << std::endl;
    double sum = 0, mse = 0, total_mse = 0, cross_validation_mse = 0, correct_answers_sum = 0;
    ClearVectors(true, true, true, true, false);
    int cross_validation_j_min, cross_validation_j_max;
    std::vector<std::vector<int>> precisions_for_letters_vector;
    std::vector<std::vector<int>> recalls_for_letters_vector;
    if (cross_validation) {
        InitCrossValidation(number_of_groups_k, test_group_number, cross_validation_j_min,
                            cross_validation_j_max, epochs_count, precisions_for_letters_vector,
                            recalls_for_letters_vector);
    }
    for (int i = 0; i < epochs_count; ++i) {
        total_mse = 0;
        iterator_ = 0;
        for (int j = 0; j < (int)(training_letters_.size()); ++j) {
            ForwardProp(training_letters_);
            // **************CROSS VALIDATION**************
            if (cross_validation) {
                if (j >= cross_validation_j_min && j < cross_validation_j_max) {
                    if (start_testing_time == 0) start_testing_time = clock();
                    int index = CalculateMaxIndex();
                    int coincidence = (training_letters_[iterator_][0] == index + 1) ? 1 : 0;
                    correct_answers_sum += coincidence;
                    precisions_for_letters_vector[index].push_back(coincidence);
                    recalls_for_letters_vector[training_letters_[iterator_][0] - 1].push_back(coincidence);
                    UpdateExpectedValues(training_letters_);
                    PushBackMse(sum, mse, cross_validation_mse, mse_for_testing_, j - cross_validation_j_min);
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
            if ((j + 1) % 15000 == 0) {
                std::cout << "Обучено " << j + 1 << " букв " << i + 1 << "-й эпохи..." << std::endl;
                std::cout << "Текущее значение MSE: " << total_mse / (j + 1) << std::endl;
            }
            PushBackMse(sum, mse, total_mse, mse_for_training_, j);
            ++iterator_;
        }
        std::cout << i + 1
                  << "-я эпоха закончена. Среднее значение MSE: " << total_mse / training_letters_.size()
                  << std::endl;
    }
    if (cross_validation) {
        CalculateTestingResults(correct_answers_sum, (training_letters_.size() / number_of_groups_k),
                                precisions_for_letters_vector, recalls_for_letters_vector);
    }
    is_trained_ = true;
    time_spent_on_training_ = (clock() - start_training_time) / CLOCKS_PER_SEC - time_spent_on_testing_;
}

void GraphNetwork::InitCrossValidation(const int number_of_groups_k, const int test_group_number,
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

void GraphNetwork::PushBackMse(double& sum, double& mse, double& total_mse, std::vector<double>& mse_vector,
                               const int j) {
    sum = 0;
    for (int output_cnt = 0; output_cnt < output_neurons_count_; ++output_cnt) {
        double tmp = expected_values_[output_cnt] - layers_.back().neurons[output_cnt].output;
        sum += tmp * tmp;
    }
    mse = sum / output_neurons_count_;
    total_mse += mse;
    mse_vector.push_back(total_mse / (j + 1));
}

int GraphNetwork::CalculateMaxIndex() {
    double max = -1;
    int index = -1;
    for (int output_cnt = 0; output_cnt < output_neurons_count_; ++output_cnt) {
        if (layers_.back().neurons[output_cnt].output > max) {
            max = layers_.back().neurons[output_cnt].output;
            index = output_cnt;
        }
    }
    return index;
}

void GraphNetwork::Test(double part_of_test_data, std::string filename) {
    unsigned int start_time = clock();
    if (testing_letters_.empty()) {
        testing_letters_ = ParseData(filename);
    }
    double sum = 0.0, total_mse = 0.0, mse = 0.0, correct_answers_sum = 0.0;
    iterator_ = 0;
    ClearVectors(true, true, true, true, false);
    std::vector<std::vector<int>> precisions_for_letters_vector;
    precisions_for_letters_vector.resize(output_neurons_count_);
    std::vector<std::vector<int>> recalls_for_letters_vector;
    recalls_for_letters_vector.resize(output_neurons_count_);
    for (int j = 0; j < testing_letters_.size() * part_of_test_data; ++j) {
        ForwardProp(testing_letters_);
        int index = CalculateMaxIndex();
        int coincidence = (testing_letters_[iterator_][0] == index + 1) ? 1 : 0;
        correct_answers_sum += coincidence;
        precisions_for_letters_vector[index].push_back(coincidence);
        recalls_for_letters_vector[testing_letters_[iterator_][0] - 1].push_back(coincidence);
        for (int output_cnt = 0; output_cnt < output_neurons_count_; ++output_cnt) {
            expected_values_[output_cnt] = testing_letters_[iterator_][0] == output_cnt + 1 ? 1 : 0;
        }
        PushBackMse(sum, mse, total_mse, mse_for_testing_, j);
        ++iterator_;
        if ((j + 1) % 3000 == 0) {
            std::cout << "Протестировано " << j + 1 << " букв..." << std::endl;
        }
    }
    iterator_ = 0;
    std::cout << "Тест завершен." << std::endl;
    CalculateTestingResults(correct_answers_sum, (testing_letters_.size() * part_of_test_data),
                            precisions_for_letters_vector, recalls_for_letters_vector);
    time_spent_on_testing_ = (clock() - start_time) / CLOCKS_PER_SEC;
    std::cout << "time spent: " << time_spent_on_testing_ << " sec" << std::endl;
}

void GraphNetwork::CalculateTestingResults(const int correct_answers_sum, const double letters_size,
                                           std::vector<std::vector<int>>& precisions_for_letters_vector,
                                           std::vector<std::vector<int>>& recalls_for_letters_vector) {
    average_accuracy_ = correct_answers_sum / letters_size;
    std::cout << "average accuracy: " << average_accuracy_ << std::endl;
    for (int i = 0; i < output_neurons_count_; ++i) {
        precisions_.resize(output_neurons_count_);
        precisions_[i] = CalculateArithmeticMean(precisions_for_letters_vector[i]);
        std::cout << "precision for letter " << (char)(i + 97) << ": " << precisions_[i] << std::endl;
    }
    for (int i = 0; i < output_neurons_count_; ++i) {
        recalls_.resize(output_neurons_count_);
        recalls_[i] = CalculateArithmeticMean(recalls_for_letters_vector[i]);
        std::cout << "recall for letter " << (char)(i + 97) << ": " << recalls_[i] << std::endl;
    }
    for (int i = 0; i < output_neurons_count_; ++i) {
        f_measures_.resize(output_neurons_count_);
        f_measures_[i] = 2 * (precisions_[i] * recalls_[i] / (precisions_[i] + recalls_[i]));
        std::cout << "f measure for letter " << (char)(i + 97) << ": " << f_measures_[i] << std::endl;
    }
    average_mse_ = CalculateArithmeticMean(mse_for_testing_);
    std::cout << "average mse: " << average_mse_ << std::endl;
}

double GraphNetwork::CalculateArithmeticMean(const std::vector<int>& vector) {
    return std::accumulate(vector.begin(), vector.end(), 0.0) / vector.size();
}
double GraphNetwork::CalculateArithmeticMean(const std::vector<double>& vector) {
    return std::accumulate(vector.begin(), vector.end(), 0.0) / vector.size();
}

void GraphNetwork::TestOneLetter(std::vector<double> letter) {
    iterator_ = 0;
    std::vector<std::vector<double>> tmp_letters;
    tmp_letters.push_back(letter);
    ForwardProp(tmp_letters);
    int index = CalculateMaxIndex();
    answer_ = (char)(index + 97);
}

bool GraphNetwork::SaveWeights(std::string filename) {
    std::fstream clear_file(filename, std::ios::out);
    clear_file.close();
    FILE* file = fopen(filename.c_str(), "w");
    if (!file) {
        return false;
    }
    fprintf(file, "%d ", (int)layers_.size() + 1);
    fprintf(file, "%d ", input_neurons_count_);
    for (int i = 0; i < (int)layers_.size(); ++i) {
        fprintf(file, "%d ", (int)layers_[i].neurons.size());
    }
    for (int i = 0; i < (int)layers_.size(); ++i) {
        for (int j = 0; j < (int)layers_[i].neurons.size(); ++j) {
            for (int k = 0; k < (int)layers_[i].neurons[j].weights.size(); ++k) {
                fprintf(file, "%lf ", layers_[i].neurons[j].weights[k]);
            }
        }
    }
    fclose(file);
    return true;
}

bool GraphNetwork::LoadWeights(std::string filename) {
    FILE* file = fopen(filename.c_str(), "r");
    if (!file) {
        return false;
    }
    std::vector<int> config;
    int layers_count;
    fscanf(file, "%d ", &layers_count);
    for (int i = 0; i < layers_count; ++i) {
        int neurons_count;
        fscanf(file, "%d ", &neurons_count);
        config.push_back(neurons_count);
    }
    InitNetwork(config, learning_rate_);
    for (int i = 0; i < (int)layers_.size(); ++i) {
        for (int j = 0; j < (int)layers_[i].neurons.size(); ++j) {
            for (int k = 0; k < (int)layers_[i].neurons[j].weights.size(); ++k) {
                fscanf(file, "%lf ", &layers_[i].neurons[j].weights[k]);
            }
        }
    }
    fclose(file);
    is_trained_ = true;
    return true;
}

const double& GraphNetwork::GetAverageAccuracy() { return average_accuracy_; }
const std::vector<double>& GraphNetwork::GetPrecisions() { return precisions_; }
const std::vector<double>& GraphNetwork::GetRecalls() { return recalls_; }
const std::vector<double>& GraphNetwork::GetFMeasures() { return f_measures_; }
const double& GraphNetwork::GetTimeSpentOnTesting() { return time_spent_on_testing_; }
const double& GraphNetwork::GetTimeSpentOnTraining() { return time_spent_on_training_; }
const std::vector<double>& GraphNetwork::GetMseForTraining() { return mse_for_training_; }
const std::vector<double>& GraphNetwork::GetMseForTesting() { return mse_for_testing_; }
const double& GraphNetwork::GetAverageMse() { return average_mse_; }
const char& GraphNetwork::GetAnswer() { return answer_; }
}  // namespace s21
