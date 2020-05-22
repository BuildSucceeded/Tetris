#pragma once
class Matrix
{
public:
	Matrix(int sizeX, int sizeY);
	~Matrix();

	bool Get(int x, int y);
	void Set(int x, int y, bool value);
	int GetXSize();
	int GetYSize();
private:
	int xSize;
	int ySize;

	bool** matrix;
};

