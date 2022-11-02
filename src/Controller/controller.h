#ifndef GRAPH_CONTROLLER_H_
#define GRAPH_CONTROLLER_H_

#include "../Model/GraphModel/graph_network.h"
#include "../Model/MatrixModel/matrix_network.h"
#include "../Model/abstract_network.h"

namespace s21 {
class Controller {
public:
    Controller();
    ~Controller();
    static Controller* GetInstance();

    void InitNetwork(std::vector<int> config, double learning_rate, bool is_graph);
    void Train(int numEpochs, std::string filename, bool cross_validation, int number_of_groups_k,
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
    const bool& GetIsTrained();

private:
    Network* network_;
    static Controller* instance_;
};
}  // namespace s21

#endif  // GRAPH_CONTROLLER_H_