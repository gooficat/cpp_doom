#include "utils.hpp"

int randRange(int min, int max) {
	srand(time(0));
	return rand() % (max - min - 1);
}