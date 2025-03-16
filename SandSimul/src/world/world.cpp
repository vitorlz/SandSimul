#include "world.h"
#include "iostream"
#include "random"

void World::init(Input* input)
{
	this->input = input;
}

void World::updateCells()
{
	int offsetArray[3] = { -1, 0, 1 };

	int randomOffset = offsetArray[rand() % 3];

	if (input->isleftMouseDown)
	{
		int mouseX = input->mouseX;
		int mouseY = input->mouseY;

		if (input->mouseX > 800)
		{
			mouseX = 800;
		}
		else if (input->mouseX < 0)
		{
			mouseX = 0;
		}

		if (input->mouseY > 800)
		{
			mouseY = 800;
		}
		else if (input->mouseY < 0)
		{
			mouseY = 0;
		}

		float relativeX = mouseX / 800.0;
		float relativeY = mouseY / 800.0;

		int brushSize = 5;

		CellData sand = { SAND, types::color8{.r = 247, .g = 235, .b = 178 }, 0, 0 };
		
		for (int i = -brushSize; i <= brushSize; i++) {
			for (int j = -brushSize; j <= brushSize; j++) {
				float distanceSquared = i * i + j * j;
				float radiusSquared = brushSize * brushSize;

				if (distanceSquared <= radiusSquared + brushSize * 0.5f) {
					int gridX = (int)(relativeX * grid.size) + i;
					int gridY = (int)(relativeY * grid.size) + j;

					if (gridX >= 0 && gridX < grid.size && gridY >= 0 && gridY < grid.size) {

						if (grid.get(gridX, gridY).type == AIR)
						{
							float f = (200 + rand() % (255 - 200 + 1)) / 255.0f;

							//std::cout << "Before: (" << (int)sand.color.r << ", " << (int)sand.color.g << ", " << (int)sand.color.b << ")" << std::endl;

							if (sand.color.r == 247)
							{
								sand.color = sand.color * f;
							}
							
							// Apply multiplication


							// Print color after multiplication
							//std::cout << "After: (" << (int)sand.color.r << ", " << (int)sand.color.g << ", " << (int)sand.color.b << ")" << std::endl;

							grid.set(gridX, gridY, sand);
						}
						
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

						grid.get(i, j).padding1 = 1;

						if (grid.get(i, j + 1).type == AIR)
						{
							CellData current = grid.get(i, j);

							grid.set(i, j, grid.get(i, j + 1));

							grid.set(i, j + 1, current);


						}
						else if (i + randomOffset < grid.size && i + randomOffset >= 0)
						{

							if (grid.get(i + randomOffset, j + 1).type == AIR)
							{
								

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

	for (int i = grid.size - 1; i >= 0; i--)
	{
		for (int j = grid.size - 1; j >= 0; j--)
		{
			CellData& cell = grid.get(i, j);

			cell.padding1 = 0;

			float f = cell.padding2 / 255;

			grid.setCellTextureColor(i, j, cell.color);
		}
	}

	
}


