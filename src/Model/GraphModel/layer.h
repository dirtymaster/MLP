#ifndef LAYER_H_
#define LAYER_H_

#include <vector>

#include "neuron.h"

namespace s21 {
struct Layer {
    std::vector<Neuron> neurons;
    Layer(int previous_layer_size, int current_layer_size);
};
}  // namespace s21

#endif  // LAYER_H_
