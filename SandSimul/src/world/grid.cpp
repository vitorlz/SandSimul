#include "grid.h"


Grid::Grid()
	: cellGrid(size* size, CellData()), cellTexture(size* size, types::color8{.r = 255, .g = 255, .b = 255})
{
}

CellData& Grid::get(int x, int y)
{
	return cellGrid[y * size + x];
}

void Grid::set(int x, int y, CellData data)
{
	cellGrid[y * size + x] = data;
}

std::vector<CellData>& Grid::getGrid()
{
	return cellGrid;
}

std::vector<types::color8>& Grid::getCellTexture()
{
	return cellTexture;
}

void Grid::setCellTextureColor(int x, int y, types::color8 color)
{
	cellTexture[y * size + x] = color;
}

bool Grid::isOutOfBounds(int x, int y)
{
	return !(x < size && x >= 0 && y < size && y >= 0);
}
