#ifndef MATRIX_NETWORK_H_
#define MATRIX_NETWORK_H_

#include <chrono>
#include <fstream>
#include <numeric>
#include <vector>

#include "../abstract_network.h"
#include "Matrix.h"
#include "activate_function.h"
#include "math.h"

namespace s21 {
class MatrixNetwork : public Network {
public:
    void InitNetwork(const std::vector<int>& config, double learning_rate);
    void Train(int epochs, std::string filename, bool cross_validation, int number_of_groups_k,
               int test_gropu_number);
    void Test(double part_of_test_data, std::string filename);
    void TestOneLetter(std::vector<double> letter);
    bool SaveWeights(std::string filename);
    bool LoadWeights(std::string filename);

    const double& GetAverageAccuracy();
    const std::vector<double>& GetPrecisions();
    const std::vector<double>& GetRecalls();
    const std::vector<double>& GetFMeasures();
    const double& GetTimeSpentOnTesting();
    const double& GetTimeSpentOnTraining();
    const std::vector<double>& GetMseForTraining();
    const std::vector<double>& GetMseForTesting();
    const double& GetAverageMse();
    const char& GetAnswer();

private:
    int layers_count_;
    std::vector<int> size;
    ActivateFunction actFunc;
    std::vector<Matrix> weights;
    std::vector<std::vector<double>> bios;
    std::vector<std::vector<double>> neurons_val, neurons_err;
    std::vector<double> neurons_bios_val;
    double learning_rate_;
    std::vector<std::vector<double>> training_letters_;
    std::vector<std::vector<double>> testing_letters_;
    std::vector<double> expected_values_;
    int input_neurons_count_;
    int output_neurons_count_;
    int iterator_;
    double time_spent_on_training_;
    double time_spent_on_testing_;
    double average_accuracy_;
    std::vector<double> precisions_;
    std::vector<double> recalls_;
    std::vector<double> f_measures_;
    std::vector<double> mse_for_training_;
    std::vector<double> mse_for_testing_;
    double average_mse_;
    char answer_;

    void SetInput(const std::vector<double>& values);
    void ClearVectors(bool precisions, bool recalls, bool f_measures, bool mse_for_testing,
                      bool mse_for_training);
    void ForwardProp(const std::vector<std::vector<double>>& letters);
    int SearchMaxIndex(const std::vector<double>& value);
    void BackProp();
    void UpdateWeights();
    std::vector<std::vector<double>> ParseData(std::string path);
    void InitCrossValidation(const int number_of_groups_k, const int test_group_number,
                             int& cross_validation_j_min, int& cross_validation_j_max, int& epochs_count,
                             std::vector<std::vector<int>>& precisions_for_letters_vector,
                             std::vector<std::vector<int>>& recalls_for_letters_vector);
    void PushBackMse(std::vector<std::vector<double>>& letters, double& sum, double& mse, double& total_mse,
                     std::vector<double>& mse_vector, const int j);
    void UpdateExpectedValues(std::vector<std::vector<double>>& letters);
    void CalculateTestingResults(const int correct_answers_sum, const double letters_size,
                                 std::vector<std::vector<int>>& precisions_for_letters_vector,
                                 std::vector<std::vector<int>>& recalls_for_letters_vector);
    double CalculateArithmeticMean(const std::vector<int>& vector);
    double CalculateArithmeticMean(const std::vector<double>& vector);
};
}  // namespace s21

#endif
