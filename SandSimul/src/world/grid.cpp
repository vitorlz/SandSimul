#include "grid.h"


Grid::Grid(int size)
	: size(size), grid(size* size, CellData())
{
}

CellData& Grid::get(int x, int y)
{
	return grid[y * size + x];
}

void Grid::set(int x, int y, CellData data)
{
	grid[y * size + x] = data;
}

std::vector<CellData> Grid::getGrid()
{
	return grid;
}
