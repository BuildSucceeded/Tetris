#pragma once

#include "resource.h"
#include "Piece.h"
#include "Stack.h"

#define RESOLUTION_X 800
#define RESOLUTION_Y 600

#define CELL_SIZE 20

class Engine
{
public:
	Engine();
	~Engine();

	HRESULT InitializeD2D(HWND m_hwnd);
	void KeyUp(WPARAM wParam);
	void KeyDown(WPARAM wParam);
	void MousePosition(int x, int y);
	void MouseButtonUp(bool left, bool right);
	void MouseButtonDown(bool left, bool right);
	void Logic(double elapsedTime);
	HRESULT Draw();

private:
	ID2D1Factory* m_pDirect2dFactory;
	ID2D1HwndRenderTarget* m_pRenderTarget;

	IDWriteFactory* m_pDWriteFactory;
	IDWriteTextFormat* m_pTextFormat;
	ID2D1SolidColorBrush* m_pWhiteBrush;

	void InitializeTextAndScore();
	void DrawTextAndScore();

	Stack* stack;
	Piece* activePiece;
	Piece* waitingPiece;

	bool gameOver = false;

	bool downPressed = false;
	bool leftPressed = false;
	bool rightPressed = false;
	bool spacePressed = false;

	int score = 0;

	double autoFallDelay;
	double autoFallAccumulated;
	double keyPressDelay;
	double keyPressAccumulated;
};

