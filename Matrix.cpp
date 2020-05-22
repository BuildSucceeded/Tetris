#include "Matrix.h"

Matrix::Matrix(int sizeX, int sizeY)
{
    // Initializes the matrix
    xSize = sizeX;
    ySize = sizeY;

    matrix = new bool* [ySize];
    for (int i = 0; i < ySize; ++i)
    {
        matrix[i] = new bool[xSize];
        for (int j = 0; j < xSize; ++j)
            matrix[i][j] = false;
    }
}

Matrix::~Matrix()
{
    // Destroys the matrix
    for (int i = 0; i < ySize; ++i)
    {
        delete[] matrix[i];
    }
    delete[] matrix;
}

bool Matrix::Get(int x, int y)
{
    // Returns the value of an element
    return matrix[y][x];
}

void Matrix::Set(int x, int y, bool value)
{
    // Sets the value of an element
    matrix[y][x] = value;
}

int Matrix::GetXSize()
{
    return xSize;
}

int Matrix::GetYSize()
{
    return ySize;
}