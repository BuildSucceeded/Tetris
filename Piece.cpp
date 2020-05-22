#include "framework.h"
#include "Point2D.h"
#include "Matrix.h"
#include "Piece.h"
#include "Engine.h"

Piece::Piece() : m_pRedBrush(NULL)
{

	position.x = STACK_WIDTH / 2 - 2;
	position.y = 0;

	waiting = true;

	cells = new Matrix(4, 4);
	// Randomly select the piece type
	int pieceType = rand() % 7;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cells->Set(j, i, cellsTemplates[pieceType][i][j]);
		}
	}
}

Piece::~Piece()
{
	delete cells;
	SafeRelease(&m_pRedBrush);
}

void Piece::InitializeD2D(ID2D1HwndRenderTarget* m_pRenderTarget)
{
	// Creates a red brush for drawing
	m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red),
		&m_pRedBrush
	);
}

void Piece::Activate()
{
	waiting = false;
}

bool Piece::Advance(Matrix* stackCells)
{
	// Advances the piece down. If there is a collision, returns true and reverts the movement
	position.y += 1;

	if (StackCollision(stackCells))
	{
		position.y -= 1;
		return true;
	}

	return false;
}

void Piece::GoLeft(Matrix* stackCells)
{
	// Tries to go left, and checks for collisions with the wall or current stack
	int initialPosX = position.x;
	position.x -= 1;

	if (LeftWallCollision())
	{
		position.x = initialPosX;
		return;
	}

	if (StackCollision(stackCells))
	{
		position.x = initialPosX;
		return;
	}
}

void Piece::GoRight(Matrix* stackCells)
{
	// Tries to go right, and checks for collisions with the wall or current stack
	int initialPosX = position.x;
	position.x += 1;

	if (RightWallCollision())
	{
		position.x = initialPosX;
		return;
	}

	if (StackCollision(stackCells))
	{
		position.x = initialPosX;
		return;
	}
}

void Piece::Rotate(Matrix* stackCells)
{
	// Store initial values
	Matrix* temp = new Matrix(4, 4);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			temp->Set(j, i, cells->Get(j, i));
		}
	}
	int initialPosX = position.x;

	// Rotate
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			cells->Set(j, i, temp->Get(i, 3 - j));
		}
	}

	while(LeftWallCollision())
	{
		position.x += 1;
	};
	while (RightWallCollision())
	{
		position.x -= 1;
	};

	if (StackCollision(stackCells)) {
		// Revert
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				cells->Set(j, i, temp->Get(j, i));
			}
		}
		position.x = initialPosX;
		return;
	}
}

bool Piece::LeftWallCollision()
{
	// Returns true if we're in a collision with the left wall
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (cells->Get(j, i) == true)
			{
				int realx = position.x + j;
				if (realx < 0)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool Piece::RightWallCollision()
{
	// Returns true if we're in a collision with the right wall
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (cells->Get(j, i) == true)
			{
				int realx = position.x + j;
				if (realx >= STACK_WIDTH)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool Piece::StackCollision(Matrix* stackCells)
{
	// Returns true if we're in a collision with the bottom wall or current stack
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (cells->Get(j, i) == true)
			{
				int realx = position.x + j;
				int realy = position.y + i;
				// Check if we are colliding with the bottom
				if (realy >= STACK_HEIGHT)
				{
					return true;
				}
				// Check if we are colliding with existing stack
				if (stackCells->Get(realx, realy))
				{
					return true;
				}
			}
		}
	}
	return false;
}

void Piece::Draw(ID2D1HwndRenderTarget* m_pRenderTarget)
{
	int padding = (RESOLUTION_Y - (STACK_HEIGHT + 1) * CELL_SIZE) / 2;

	int centerRight = RESOLUTION_X - (RESOLUTION_X - padding - (STACK_WIDTH + 2) * CELL_SIZE) / 2;

	int center_x = centerRight;
	int center_y = padding + 2 * CELL_SIZE + 100;

	if (!waiting)
	{
		center_x = padding + (position.x + 1) * CELL_SIZE;
		center_y = padding + position.y * CELL_SIZE;
	}

	// Drawing the cells

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (cells->Get(j, i) == true)
			{
				D2D1_RECT_F rectangle4 = D2D1::RectF(
					center_x + j * CELL_SIZE + 1, center_y + i * CELL_SIZE + 1,
					center_x + (j + 1) * CELL_SIZE - 1, center_y + (i + 1) * CELL_SIZE - 1
				);
				m_pRenderTarget->FillRectangle(&rectangle4, m_pRedBrush);
			}
		}
	}

}

Point2D Piece::GetPosition()
{
	return position;
}

Matrix* Piece::GetCells()
{
	return cells;
}