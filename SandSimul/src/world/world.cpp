#include "world.h"
#include "glad/glad.h"
#include "iostream"
#include "random"
#include <GLFW/glfw3.h>

World::World(GLFWwindow* window)
	: grid(300)
{
	
	input.window = window;
}

void World::updateCell()
{
	int offsetArray[3] = { -1, 0, 1 };

	int randomOffset = offsetArray[rand() % 3];

	if (input.isleftMouseDown)
	{
		int mouseX = input.mouseX;
		int mouseY = input.mouseY;

		if (input.mouseX > 800)
		{
			mouseX = 800;
		}
		else if (input.mouseX < 0)
		{
			mouseX = 0;
		}

		if (input.mouseY > 800)
		{
			mouseY = 800;
		}
		else if (input.mouseY < 0)
		{
			mouseY = 0;
		}

		float relativeX = mouseX / 800.0;
		float relativeY = mouseY / 800.0;

		int brushSize = 5;

		CellData sand = { SAND, 255, 0, 0};
		
		for (int i = -brushSize; i <= brushSize; i++) {
			for (int j = -brushSize; j <= brushSize; j++) {
				float distanceSquared = i * i + j * j;
				float radiusSquared = brushSize * brushSize;

				if (distanceSquared <= radiusSquared + brushSize * 0.5f) {
					int gridX = (int)(relativeX * grid.size) + i;
					int gridY = (int)(relativeY * grid.size) + j;

					if (gridX >= 0 && gridX < grid.size && gridY >= 0 && gridY < grid.size) {
						sand.padding2 = 200 + rand() % (255 - 200 + 1);

						if(grid.get(gridX, gridY).type == AIR)
							grid.set(gridX, gridY, sand);
					}
				}
			}
		}
	}

	for (int i = grid.size - 1; i >= 0; i--)
	{
		for (int j = grid.size - 1; j >= 0; j--)
		{
			if (grid.get(i, j).type == SAND)
			{	
				if (j < grid.size - 1)
				{

					if (grid.get(i, j).padding1 == 0)
					{
						if (grid.get(i, j + 1).type == AIR)
						{
							grid.get(i, j).padding1 = 1;

							CellData current = grid.get(i, j);

							grid.set(i, j, grid.get(i, j + 1));

							grid.set(i, j + 1, current);


						}
						else if (i + randomOffset < grid.size && i + randomOffset >= 0)
						{

							if (grid.get(i + randomOffset, j + 1).type == AIR)
							{
								grid.get(i, j).padding1 = 1;

								CellData current = grid.get(i, j);

								grid.set(i, j, grid.get(i + randomOffset, j + 1));


								grid.set(i + randomOffset, j + 1, current);
							}

						}
					}	
				}
			}
		}
	}

	for (int i = 0; i < grid.size; i++)
	{
		for (int j = 0; j < grid.size; j++)
		{
			grid.get(i, j).padding1 = 0;
		}
	}

	
}


