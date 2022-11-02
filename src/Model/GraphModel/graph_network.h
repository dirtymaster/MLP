#ifndef GRAPH_NETWORK_H_
#define GRAPH_NETWORK_H_

#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>

#include "../abstract_network.h"
#include "layer.h"

namespace s21 {
class GraphNetwork : public Network {
public:
    void InitNetwork(const std::vector<int>& config, double learning_rate);
    void Train(int epochs_count, std::string filename, bool cross_validation, int number_of_groups_k,
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
    std::vector<Layer> layers_;
    double learning_rate_;
    std::vector<std::vector<double>> training_letters_;
    std::vector<std::vector<double>> testing_letters_;
    int iterator_;  // iterator_ for letters
    std::vector<double> expected_values_;
    int input_neurons_count_;
    int output_neurons_count_;
    double average_accuracy_;
    std::vector<double> precisions_;
    std::vector<double> recalls_;
    std::vector<double> f_measures_;
    double time_spent_on_testing_;
    double time_spent_on_training_;
    std::vector<double> mse_for_training_;
    std::vector<double> mse_for_testing_;
    double average_mse_;
    char answer_ = '?';

    std::vector<std::vector<double>> ParseData(std::string filename);
    std::vector<double> ForwardProp(std::vector<std::vector<double>>& letters);
    double Activate(std::vector<double>, std::vector<double>);  // dot product
    double Transfer(double);
    double TransferDerivative(double);
    void UpdateExpectedValues(std::vector<std::vector<double>>& letters);
    void ClearVectors(bool precisions, bool recalls, bool f_measures, bool mse_for_testing,
                      bool mse_for_training);
    void BackProp();
    void UpdateWeights();
    void InitCrossValidation(const int number_of_groups_k, const int test_group_number,
                             int& cross_validation_j_min, int& cross_validation_j_max, int& epochs_count,
                             std::vector<std::vector<int>>& precisions_for_letters_vector,
                             std::vector<std::vector<int>>& recalls_for_letters_vector);
    void PushBackMse(double& sum, double& mse, double& total_mse, std::vector<double>& mse_vector,
                     const int j);
    int CalculateMaxIndex();
    void CalculateTestingResults(const int correct_answers_sum, const double letters_size,
                                 std::vector<std::vector<int>>& precisions_for_letters_vector,
                                 std::vector<std::vector<int>>& recalls_for_letters_vector);
    double CalculateArithmeticMean(const std::vector<int>& vector);
    double CalculateArithmeticMean(const std::vector<double>& vector);
};
}  // namespace s21

#endif  // GRAPH_NETWORK_H_
