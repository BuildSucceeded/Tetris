#include "framework.h"
#include "Matrix.h"
#include "Stack.h"
#include "Engine.h"

Stack::Stack() : m_pBlueBrush(NULL), m_pGreenBrush(NULL), m_pYellowBrush(NULL)
{
	cells = new Matrix(STACK_WIDTH, STACK_HEIGHT);
}

Stack::~Stack()
{
	delete cells;
	SafeRelease(&m_pBlueBrush);
	SafeRelease(&m_pGreenBrush);
	SafeRelease(&m_pYellowBrush);
}

void Stack::InitializeD2D(ID2D1HwndRenderTarget* m_pRenderTarget)
{
	// Creates the brushes for drawing
	m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Blue),
		&m_pBlueBrush
	);
	m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Green),
		&m_pGreenBrush
	);
	m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Yellow),
		&m_pYellowBrush
	);
}

int Stack::RemoveLines()
{
	// This removes the full rows
	int removed = 0;
	for (int i = STACK_HEIGHT - 1; i >= 0 ; i--)
	{
		bool entireLine = true;
		for (int j = 0; j < STACK_WIDTH; j++)
		{
			if (cells->Get(j, i) == false)
			{
				entireLine = false;
			}
		}
		if (entireLine)
		{
			removed++;
			for (int k = i; k > 0; k--)
			{
				for (int j = 0; j < STACK_WIDTH; j++)
				{
					cells->Set(j, k, cells->Get(j, k - 1));
				}
			}
			i++;
		}
	}
	return removed;
}

void Stack::Draw(ID2D1HwndRenderTarget* m_pRenderTarget)
{
	int padding = (RESOLUTION_Y - (STACK_HEIGHT + 1) * CELL_SIZE) / 2;

	// Drawing the walls first

	D2D1_RECT_F rectangle1 = D2D1::RectF(
		padding, padding, 
		padding + CELL_SIZE, padding + (STACK_HEIGHT + 1) * CELL_SIZE
	);
	m_pRenderTarget->FillRectangle(&rectangle1, m_pBlueBrush);

	D2D1_RECT_F rectangle2 = D2D1::RectF(
		padding, padding + STACK_HEIGHT * CELL_SIZE,
		padding + (STACK_WIDTH + 2) * CELL_SIZE, padding + (STACK_HEIGHT + 1) * CELL_SIZE
	);
	m_pRenderTarget->FillRectangle(&rectangle2, m_pBlueBrush);

	D2D1_RECT_F rectangle3 = D2D1::RectF(
		padding + (STACK_WIDTH + 1) * CELL_SIZE, padding,
		padding + (STACK_WIDTH + 2) * CELL_SIZE, padding + (STACK_HEIGHT + 1) * CELL_SIZE
	);
	m_pRenderTarget->FillRectangle(&rectangle3, m_pBlueBrush);

	// Drawing the cells

	for (int i = 0; i < STACK_HEIGHT; i++)
	{
		bool entireLine = true;
		for (int j = 0; j < STACK_WIDTH; j++)
		{
			if (cells->Get(j, i) == false)
			{
				entireLine = false;
			}
		}

		for (int j = 0; j < STACK_WIDTH; j++)
		{
			if (cells->Get(j, i) == true)
			{
				D2D1_RECT_F rectangle4 = D2D1::RectF(
					padding + (j + 1) * CELL_SIZE + 1, padding + i * CELL_SIZE + 1,
					padding + (j + 2) * CELL_SIZE - 1, padding + (i + 1) * CELL_SIZE - 1
				);
				if (entireLine)
				{
					m_pRenderTarget->FillRectangle(&rectangle4, m_pYellowBrush);
				}
				else
				{
					m_pRenderTarget->FillRectangle(&rectangle4, m_pGreenBrush);
				}
			}
		}
	}

}

Matrix* Stack::GetCells()
{
	return cells;
}
