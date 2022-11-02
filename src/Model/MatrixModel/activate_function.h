#ifndef ACTIVATE_FUNCTION_H_
#define ACTIVATE_FUNCTION_H_

#include <iostream>
#include <vector>

#include "math.h"
namespace s21 {
class ActivateFunction {
public:
    void Use(std::vector<double>& value, int n);
    void UseDer(std::vector<double>& value, int n);
    double UseDer(double value);
};
}  // namespace s21

#endif  // ACTIVATE_FUNCTION_H_
