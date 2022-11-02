#ifndef ABSTRACT_CLASS_H_
#define ABSTRACT_CLASS_H_

#include <string>
#include <vector>

namespace s21 {
class Network {
public:
    virtual ~Network() {}
    virtual void InitNetwork(const std::vector<int>& config, double learning_rate) = 0;
    virtual void Train(int epochs_count, std::string filename, bool cross_validation, int number_of_groups_k,
                       int test_gropu_number) = 0;
    virtual void Test(double part_of_test_data, std::string filename) = 0;
    virtual void TestOneLetter(std::vector<double> letter) = 0;
    virtual bool SaveWeights(std::string filename) = 0;
    virtual bool LoadWeights(std::string filename) = 0;

    virtual const double& GetAverageAccuracy() = 0;
    virtual const std::vector<double>& GetPrecisions() = 0;
    virtual const std::vector<double>& GetRecalls() = 0;
    virtual const std::vector<double>& GetFMeasures() = 0;
    virtual const double& GetTimeSpentOnTesting() = 0;
    virtual const double& GetTimeSpentOnTraining() = 0;
    virtual const std::vector<double>& GetMseForTraining() = 0;
    virtual const std::vector<double>& GetMseForTesting() = 0;
    virtual const double& GetAverageMse() = 0;
    virtual const char& GetAnswer() = 0;
    const bool& GetIsTrained() { return is_trained_; };
    void SetIsTrained(bool value) { is_trained_ = value; }

protected:
    bool is_trained_ = false;
};
}  // namespace s21

#endif  // ABSTRACT_CLASS