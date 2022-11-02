#include "neuron.h"

namespace s21 {
Neuron::Neuron(int previous_layer_size) { InitializeWeights(previous_layer_size); }

double Neuron::generateRandomNumber(double min, double max) {
    double random = (double)rand() / RAND_MAX;
    return min + random * (max - min);
}

void Neuron::InitializeWeights(int previous_layer_size) {
    for (int i = 0; i < previous_layer_size + 1; ++i) {
        weights.push_back(generateRandomNumber(-1.0, 1.0));
    }
}
}  // namespace s21
