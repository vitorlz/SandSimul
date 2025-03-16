#include "world.h"
#include "iostream"
#include "random"
#include <GLFW/glfw3.h>

void World::init(Input* input)
{
	this->input = input;
}

void World::updateCells()
{
	if (input->keyPressed[GLFW_KEY_1])
	{
		brushType = SAND;
	}
	else if (input->keyPressed[GLFW_KEY_2])
	{
		brushType = WATER;
	}

	if (input->isleftMouseDown)
	{
		spawnCells();
	}

	for (int i = grid.size - 1; i >= 0; i--)
	{
		for (int j = grid.size - 1; j >= 0; j--)
		{
			switch (grid.get(i, j).type)
			{
			case SAND:
				updateSand(i, j);
				break;
			case WATER:
				updateWater(i, j);
				break;
			}
		}
	}

	for (int i = grid.size - 1; i >= 0; i--)
	{
		for (int j = grid.size - 1; j >= 0; j--)
		{
			CellData& cell = grid.get(i, j);
			cell.isUpdated = false;
			
			grid.setCellTextureColor(i, j, cell.color);
		}
	}
}

void World::updateSand(int x, int y)
{
	CellData currentCell = grid.get(x, y);
	if (y < grid.size - 1 && !currentCell.isUpdated)
	{
		CellData cellBelow = grid.get(x, y + 1);

		int randomOffset = types::genRandom(-1, 1);
		currentCell.isUpdated = true;

		if (cellBelow.type == AIR || cellBelow.type == WATER)
		{
			grid.set(x, y, cellBelow);
			grid.set(x, y + 1, currentCell);
			
		}
		else if (x + randomOffset < grid.size && x + randomOffset >= 0)
		{
			CellData cellDiagonal = grid.get(x + randomOffset, y + 1);

			if (cellDiagonal.type == AIR || cellDiagonal.type == WATER)
			{
				grid.set(x, y, cellDiagonal);
				grid.set(x + randomOffset, y + 1, currentCell);
			}
		}
	}
}

void World::createCell(int x, int y, CellType type)
{
	switch (type)
	{
	case SAND:
		createSand(x, y);
		break;
	case WATER:
		createWater(x, y);
		break;
	}
}

static double lastTime = 0;
static double limit = 1 / 20.0;

void World::spawnCells()
{
	double currentTime = glfwGetTime();
	double deltaTime = currentTime - lastTime;

	if (deltaTime >= limit)
	{
		int mouseX = input->mouseX;
		int mouseY = input->mouseY;

		float relativeX = input->mouseX / 800.0;
		float relativeY = input->mouseY / 800.0;

		for (int i = -brushSize; i <= brushSize; i++) {
			for (int j = -brushSize; j <= brushSize; j++) {
				float distanceSquared = i * i + j * j;
				float radiusSquared = brushSize * brushSize;

				// randomize cell position in circle
				if ((int)distanceSquared % (rand() % 3 + 1) == 0)
					continue;
				
				if (distanceSquared <= radiusSquared + brushSize * 0.5f) {
					int gridX = (int)(relativeX * grid.size) + i;
					int gridY = (int)(relativeY * grid.size) + j;

					if (gridX >= 0 && gridX < grid.size && gridY >= 0 && gridY < grid.size) {

						if (grid.get(gridX, gridY).type == AIR)
						{
							createCell(gridX, gridY, brushType);
						}
					}
				}
			}
		}

		lastTime = currentTime;
	}
}

void World::createSand(int x, int y)
{
	CellData sand =
	{
		SAND,
		types::color8{.r = 247, .g = 235, .b = 178 },
		false
	};

	float f = types::genRandom(200, 255) / 255.0f;

	sand.color = sand.color * f;
	grid.set(x, y, sand);
}

void World::updateWater(int x, int y)
{
	CellData currentCell = grid.get(x, y);
	if (y < grid.size - 1 && !currentCell.isUpdated)
	{
		CellData cellBelow = grid.get(x, y + 1);

		int randomOffset = types::genRandom(-1, 1);
		currentCell.isUpdated = true;

		if (cellBelow.type == AIR)
		{
			grid.set(x, y, cellBelow);
			grid.set(x, y + 1, currentCell);

		}
		else if (x + randomOffset < grid.size && x + randomOffset >= 0)
		{
			CellData cellDiagonal = grid.get(x + randomOffset, y + 1);
			CellData cellSide = grid.get(x + randomOffset, y);

			if (cellDiagonal.type == AIR)
			{
				grid.set(x, y, cellDiagonal);
				grid.set(x + randomOffset, y + 1, currentCell);
			}
			else if (cellSide.type == AIR)
			{
				grid.set(x, y, cellSide);
				grid.set(x + randomOffset, y, currentCell);
			}
		}
	}
}

void World::createWater(int x, int y)
{
	CellData water =
	{
		WATER,
		types::color8{ 43, 153, 235 },
		false
	};

	float f = types::genRandom(230, 255) / 255.0f;

	water.color = water.color * f;
	grid.set(x, y, water);
}


