#include "controller.h"

namespace s21 {
Controller* Controller::instance_ = nullptr;
Controller::Controller() { network_ = new GraphNetwork; }
Controller::~Controller() {
    if (network_) delete network_;
}
Controller* Controller::GetInstance() {
    if (!instance_) instance_ = new Controller;
    return instance_;
}
void Controller::InitNetwork(std::vector<int> config, double learning_rate, bool is_graph) {
    if (network_) delete network_;
    if (is_graph)
        network_ = new GraphNetwork;
    else
        network_ = new MatrixNetwork;
    network_->InitNetwork(config, learning_rate);
}
void Controller::Train(int numEpochs, std::string filename, bool cross_validation, int number_of_groups_k,
                       int test_gropu_number) {
    network_->Train(numEpochs, filename, cross_validation, number_of_groups_k, test_gropu_number);
}
void Controller::Test(double part_of_test_data, std::string filename) {
    network_->Test(part_of_test_data, filename);
}
void Controller::TestOneLetter(std::vector<double> letter) { network_->TestOneLetter(letter); }
bool Controller::SaveWeights(std::string filename) { return network_->SaveWeights(filename); }
bool Controller::LoadWeights(std::string filename) { return network_->LoadWeights(filename); }
const double& Controller::GetAverageAccuracy() { return network_->GetAverageAccuracy(); }
const std::vector<double>& Controller::GetPrecisions() { return network_->GetPrecisions(); }
const std::vector<double>& Controller::GetRecalls() { return network_->GetRecalls(); }
const std::vector<double>& Controller::GetFMeasures() { return network_->GetFMeasures(); }
const double& Controller::GetTimeSpentOnTesting() { return network_->GetTimeSpentOnTesting(); }
const double& Controller::GetTimeSpentOnTraining() { return network_->GetTimeSpentOnTraining(); }
const std::vector<double>& Controller::GetMseForTraining() { return network_->GetMseForTraining(); }
const std::vector<double>& Controller::GetMseForTesting() { return network_->GetMseForTesting(); }
const double& Controller::GetAverageMse() { return network_->GetAverageMse(); }
const char& Controller::GetAnswer() { return network_->GetAnswer(); }
const bool& Controller::GetIsTrained() { return network_->GetIsTrained(); }
}  // namespace s21
