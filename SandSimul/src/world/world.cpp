#include "world.h"
#include "iostream"
#include "random"
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"

void World::init(Input* input)
{
	this->input = input;
}

void World::updateCells()
{
	if (input->keyPressed[GLFW_KEY_1])
	{
		brush = { SAND, SOLID_MOVABLE, false };
	}
	else if (input->keyPressed[GLFW_KEY_2])
	{
		brush = { WATER, FLUID, false };
	}
	else if (input->keyPressed[GLFW_KEY_3])
	{
		brush = { WOOD, SOLID_IMMOVABLE, true };
	}
	else if (input->keyPressed[GLFW_KEY_BACKSPACE])
	{
		brush = { AIR, FLUID, true };
	}
	else if (input->keyPressed[GLFW_KEY_4])
	{
		brush = { FIRE, REACTION, false };
	}

	if (input->isleftMouseDown)
	{
		spawnCells();
		mouseDownLastFrame = true;
	}
	else 
	{
		mouseDownLastFrame = false;
	}

	for (int i = grid.size - 1; i >= 0; i--)
	{
		for (int j = grid.size - 1; j >= 0; j--)
		{

			if (grid.get(i, j).isUpdated)
				continue;

			switch (grid.get(i, j).type)
			{
			case SAND:
				updateSand(i, j);
				break;
			case WATER:
				updateWater(i, j);
				break;
			case WOOD:
				updateWood(i, j);
				break;
			case FIRE:
				updateFire(i, j);
				break;
			}

			grid.get(i, j).isUpdated = true;
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
	case AIR:
		createAir(x, y);
		break;
	case SAND:
		createSand(x, y);
		break;
	case WATER:
		createWater(x, y);
		break;
	case WOOD:
		createWood(x, y);
		break;
	case FIRE:
		createFire(x, y);
		break;
	}
}



void World::spawnCells()
{
	static double lastTime = 0;
	static double limit = 1 / 40.0;
	static double lastTimeContinuous = 0;
	static double limitContinuous = 1 / 60.0;

	double currentTime = glfwGetTime();
	double deltaTime = currentTime - lastTime;
	double deltaTimeContinuous = currentTime - lastTimeContinuous;

	static int lastCircleCenterX;
	static int lastCircleCenterY;

	float relativeX = input->mouseX / 800.0;
	float relativeY = input->mouseY / 800.0;

	int centerX = (int)(relativeX * grid.size);
	int centerY = (int)(relativeY * grid.size);

	if (!brush.continuous && deltaTime >= limit)
	{
		brushDraw(centerX, centerY);
		lastTime = currentTime;
	}
	else if (deltaTimeContinuous >= limitContinuous && brush.continuous)
	{
		brushDraw(centerX, centerY);
		
		// fill gaps 
		if (mouseDownLastFrame)
		{
			int distX = lastCircleCenterX - centerX;
			int distY = lastCircleCenterY - centerY;

			float m = 0;

			if (distX != 0)
			{
				m = (float)distY / (float)distX;
			}			

			if (distX == 0)
			{
				for (int i = 1; i <= std::abs(distY); i++)
				{
					int y = distY < 0 ? -i : i;

					brushDraw(centerX, centerY + y);
				}
			}
			else
			{
				int distance = distX;
				bool usingYDistance = false;
				if (std::abs(distX) < std::abs(distY))
				{
					distance = distY;
					usingYDistance = true;
				}

				for (int i = 1; i <= std::abs(distance); i++)
				{
					int x, y;
					if (usingYDistance)
					{
						y = distY < 0 ? -i : i;
						x = std::round(float(y) / m);
					}
					else
					{
						x = distX < 0 ? -i : i;
						y = std::round(m * (float)x);
					}

					brushDraw(centerX + x,centerY + y);
				}
			}

			lastTimeContinuous = currentTime;
		}

		
		lastCircleCenterX = centerX;
		lastCircleCenterY = centerY;
	}
}

void World::createSand(int x, int y)
{
	CellData sand =
	{
		SAND,
		SOLID_MOVABLE,
		types::color8{.r = 247, .g = 235, .b = 178 },
		false,
		false
	};

	float f = types::genRandom(200, 255) / 255.0f;

	sand.color = sand.color * f;
	grid.set(x, y, sand);
}

void World::updateWater(int x, int y)
{
	int dispersion = 5;
	CellData currentCell = grid.get(x, y);
	if (y < grid.size - 1 && !currentCell.isUpdated)
	{
		int randomOffset = types::genRandom(-1, 1);

		CellData cellBelow = grid.get(x, y + 1);
		CellData cellDiagonal = grid.get(x + randomOffset, y + 1);
		CellData cellSide = grid.get(x + randomOffset, y);

		currentCell.isUpdated = true;

		if (cellBelow.type == AIR)
		{
			grid.set(x, y, cellBelow);
			grid.set(x, y + 1, currentCell);

		}
		else if (x + randomOffset < grid.size && x + randomOffset >= 0)
		{
			if (cellDiagonal.type == AIR)
			{
				// disperse diagonally 
				disperseFluid(x, y, dispersion, randomOffset, 1);
			}
			else if (cellSide.type == AIR)
			{
				// disperse to the sides
				disperseFluid(x, y, dispersion, randomOffset, 0);
			}
		}
		
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				CellData& cell = grid.get(x + i, y + j);

				if (!grid.isOutOfBounds(x + i, y + j) && cell.isIgnited)
				{
					cell.isIgnited = false;
					cell.color.r /= 1.5;
				}
			}
		}		
	}
}

void World::disperseFluid(int x, int y, int dispersion, int dirX, int dirY)
{
	// iterate through the cells in the direction defined by dirX and dirY until there is no air or up to 
	// the value of dispersion, then swap with the last air cell found
	CellData currentCell = grid.get(x, y);
	CellData cellSide;

	glm::vec2 lastAirCellPos{};
	int i = 1;
	while (i <= dispersion)
	{
		int xOffset = dirX * i;
		int yOffset = dirY * i;

		cellSide = grid.get(x + xOffset, y + yOffset);

		if (cellSide.type == AIR)
		{
			lastAirCellPos = glm::vec2(x + xOffset, y + yOffset);

			// if did not find non-air cells and reached max dispersion or end of the map, swap.
			if (i == dispersion || x + xOffset == grid.size - 1 || x + xOffset == 0 
				|| y + yOffset == grid.size - 1 || y + yOffset == 0)
			{	
				grid.set(x, y, cellSide);
				grid.set(lastAirCellPos.x, lastAirCellPos.y, currentCell);

				break;
			}
		}
		else 
		{
			// found non-air cell: swap with last air cell.
			grid.set(x, y, grid.get(lastAirCellPos.x, lastAirCellPos.y));
			grid.set(lastAirCellPos.x, lastAirCellPos.y, currentCell);

			break;
		}
		i++;
	}
}

void World::brushDraw(int centerX, int centerY)
{
	for (int i = -brushSize; i <= brushSize; i++) {
		for (int j = -brushSize; j <= brushSize; j++) {
			float distanceSquared = i * i + j * j;
			float radiusSquared = brushSize * brushSize;

			// randomize cell position in circle
			if ((int)distanceSquared % (rand() % 2 + 1) == 0 && !brush.continuous)
				continue;

			if (distanceSquared <= radiusSquared + brushSize * 0.5f) {
				int gridX = centerX + i;
				int gridY = centerY + j;

				if (!grid.isOutOfBounds(gridX, gridY)) {

					if (grid.get(gridX, gridY).type == AIR || brush.type == AIR)
					{
						createCell(gridX, gridY, brush.type);
					}
				}
			}
		}
	}

}

void World::createWater(int x, int y)
{
	CellData water =
	{
		.type = WATER,
		.kind = FLUID,
		.color = types::color8{ 43, 153, 235 },
		.isFlammable = false,
		.isIgnited = false,
		.isUpdated = false
	};

	float f = types::genRandom(230, 255) / 255.0f;

	water.color = water.color * f;
	grid.set(x, y, water);
}


void World::updateWood(int x, int y)
{

	CellData& currentCell = grid.get(x, y);

	if (currentCell.isIgnited)
	{
		if (types::genRandom(1, 100) == 5)
		{
			int randX = x + types::genRandom(-1, 1);
			int randY = y + types::genRandom(-1, 1);

			if (!grid.isOutOfBounds(randX, randY))
			{
				if (grid.get(randX, randY).isFlammable)
				{
					grid.get(randX, randY).color.r *= 1.5;
					grid.get(randX, randY).isIgnited = true;
				}
			}	
		}
		else if (types::genRandom(1, 1000) == 5 && currentCell.isIgnited)
		{
			createAir(x, y);
		}
	}
	
}

void World::createWood(int x, int y)
{
	CellData wood =
	{
		.type = WOOD,
		.kind = SOLID_IMMOVABLE,
		.color = types::color8{ 51, 27, 12 },
		.isFlammable = true,
		.isIgnited = false,
		.isUpdated = false
	};

	float f = types::genRandom(200, 255) / 255.0f;

	wood.color = wood.color * f;
	grid.set(x, y, wood);
}

void World::updateFire(int x, int y)
{
	int dispersion = 2;
	CellData currentCell = grid.get(x, y);

	if (y > 0)
	{
		CellData cellAbove = grid.get(x, y - 1);

		int randomOffset = types::genRandom(-1, 1);

		currentCell.isUpdated = true;

		if ((cellAbove.type == AIR) && types::genRandom(1, 2) == 2)
		{
			createAir(x, y);
			grid.set(x, y - 1, currentCell);

		}
		else if (x + randomOffset < grid.size && x + randomOffset >= 0)
		{
			CellData cellDiagonal = grid.get(x + randomOffset, y - 1);
			CellData cellSide = grid.get(x + randomOffset, y);

			if (cellDiagonal.isFlammable)
			{	
				CellData& cellD = grid.get(x + randomOffset, y - 1);

				cellD.color = currentCell.color;
				cellD.isIgnited = true;

				createAir(x, y);
			}
			else if (cellSide.isFlammable)
			{

				CellData& cellS = grid.get(x + randomOffset, y);
				cellS.color = currentCell.color;
				cellS.isIgnited = true;

				createAir(x, y);
			}
			else if (cellDiagonal.type == AIR)
			{
				// disperse diagonally 
				disperseFluid(x, y, dispersion, randomOffset, -1);
			}
			else if (cellSide.type == AIR)
			{
				// disperse to the sides
				disperseFluid(x, y, dispersion, randomOffset, 0);
			}
		}
		
	}
	else 
	{
		createAir(x, y);
	}

}

void World::createFire(int x, int y)
{
	CellData fire =
	{
		.type = FIRE,
		.kind = REACTION,
		.color = types::color8{ 237, 76, 7 },
		.isFlammable = false,
		.isIgnited = false,
		.isUpdated = false
	};

	float f = types::genRandom(150, 255) / 255.0f;

	fire.color = fire.color * f;
	grid.set(x, y, fire);
}


void World::createAir(int x, int y)
{
	CellData air =
	{
		.type = AIR,
		.kind = FLUID,
		.color = types::color8{ 255, 255, 255 },
		.isFlammable = false,
		.isIgnited = false,
		.isUpdated = false
	};

	grid.set(x, y, air);
}

