#include "layer.h"

namespace s21 {
Layer::Layer(int previous_layer_size, int current_layer_size) {
    for (int i = 0; i < current_layer_size; ++i) {
        neurons.push_back(Neuron(previous_layer_size));
    }
}
}  // namespace s21
