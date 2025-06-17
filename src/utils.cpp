#include "utils.hpp"

int randRange(int min, int max) {
	srand(time(0));
	return rand() % (max - min - 1);
}

double SafeDivide(double numerator, double denominator, double fallback) {
    return (fabs(denominator) < 0.0001) ? fallback : (numerator / denominator);
}