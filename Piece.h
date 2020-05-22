#pragma once

#include "resource.h"
#include "Point2D.h"
#include "Matrix.h"

class Piece
{
public:
	Piece();
	~Piece();

	void InitializeD2D(ID2D1HwndRenderTarget* m_pRenderTarget);
	void Activate();
	bool Advance(Matrix* stackCells);
	void GoLeft(Matrix* stackCells);
	void GoRight(Matrix* stackCells);
	void Rotate(Matrix* stackCells);
	void Draw(ID2D1HwndRenderTarget* m_pRenderTarget);

	bool LeftWallCollision();
	bool RightWallCollision();
	bool StackCollision(Matrix* stackCells);
	Point2D GetPosition();
	Matrix* GetCells();

private:
	Point2D position;
	Matrix* cells;

	bool waiting; // Is the current piece or the next piece?

	bool cellsTemplates[7][4][4] = { // This is where we define our pieces templates
						{ { false, false, false, false },
						  { false, false, false, false },
						  { true,  true,  true,  true  },
						  { false, false, false, false } },

						{ { false, false, false, false },
						  { false, true,  true,  true  },
						  { false, true,  false, false },
						  { false, false, false, false } },

						{ { false, false, false, false },
						  { false, true,  false, false },
						  { false, true,  true,  true  },
						  { false, false, false, false } },

						{ { false, false, false, false },
						  { false, true,  true,  true  },
						  { false, false, true,  false },
						  { false, false, false, false } },

						{ { false, false, false, false },
						  { false, true,  true,  false },
						  { false, true,  true,  false },
						  { false, false, false, false } },

						{ { false, false, false, false },
						  { false, true,  true,  false },
						  { false, false, true,  true  },
						  { false, false, false, false } },

						{ { false, false, false, false },
						  { false, false, true,  true  },
						  { false, true,  true,  false },
						  { false, false, false, false } }
	};

	ID2D1SolidColorBrush* m_pRedBrush;

};

