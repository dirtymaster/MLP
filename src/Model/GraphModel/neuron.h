#ifndef NEURON_H_
#define NEURON_H_

#include <cmath>
#include <vector>

namespace s21 {
class Neuron {
public:
    double output;
    double delta;
    std::vector<double> weights;
    Neuron(int);
    void InitializeWeights(int previous_layer_size);
    double generateRandomNumber(double min, double max);
};
}  // namespace s21

#endif  // NEURON_H_
