#pragma once 
#include "../util/input.h"
#include <vector>
#include "grid.h"

class World
{
public:
	void init(Input* input);
	void updateCells();

	Input* input;
	Grid grid;

private:
	void updateSand();
};