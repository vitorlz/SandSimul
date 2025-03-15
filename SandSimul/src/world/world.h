#pragma once 

#include "../util/input.h"
#include <vector>

#include "grid.h"

class World
{
public:
	World(GLFWwindow* window);

	Input input;

	void updateCell();

	Grid grid;
};