#include "framework.h"
#include "Matrix.h"
#include "Stack.h"
#include "Piece.h"
#include "Engine.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "Windowscodecs.lib")

Engine::Engine() : m_pDirect2dFactory(NULL), m_pRenderTarget(NULL)
{
	// Constructor
	// Initialize your game elements here

    srand(time(NULL));
	
    stack = new Stack();

    activePiece = new Piece();
    activePiece->Activate();
    waitingPiece = new Piece();

    autoFallDelay = 1;
    autoFallAccumulated = 0;
    keyPressDelay = 0.07;
    keyPressAccumulated = 0;

}

Engine::~Engine()
{
    // Destructor

    SafeRelease(&m_pDirect2dFactory);
    SafeRelease(&m_pRenderTarget);

    // Safe-release your game elements here
    delete stack;
    delete waitingPiece;
    delete activePiece;
}

HRESULT Engine::InitializeD2D(HWND m_hwnd)
{
    // Initializes Direct2D, for drawing
    D2D1_SIZE_U size = D2D1::SizeU(RESOLUTION_X, RESOLUTION_Y);
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
    m_pDirect2dFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(m_hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
        &m_pRenderTarget
    );

    // Initialize the D2D part of your game elements here
    InitializeTextAndScore();
    stack->InitializeD2D(m_pRenderTarget);
    activePiece->InitializeD2D(m_pRenderTarget);
    waitingPiece->InitializeD2D(m_pRenderTarget);

    return S_OK;
}

void Engine::InitializeTextAndScore()
{
    DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(m_pDWriteFactory),
        reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
    );

    m_pDWriteFactory->CreateTextFormat(
        L"Verdana",
        NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        20,
        L"", //locale
        &m_pTextFormat
    );

    m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

    m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        &m_pWhiteBrush
    );
}

void Engine::KeyUp(WPARAM wParam)
{
    // If keyup, un-set the keys flags
    // Don't do any logic here, you want to control the actual logic in the Logic method below
    if (wParam == VK_DOWN)
        downPressed = false;

    if (wParam == VK_LEFT)
        leftPressed = false;

    if (wParam == VK_RIGHT)
        rightPressed = false;

    if (wParam == VK_SPACE || wParam == VK_UP)
        spacePressed = false;
}

void Engine::KeyDown(WPARAM wParam)
{
	// If keyup, set the keys flags
	// Don't do any logic here, you want to control the actual logic in the Logic method below
    if (wParam == VK_DOWN)
        downPressed = true;

    if (wParam == VK_LEFT)
        leftPressed = true;

    if (wParam == VK_RIGHT)
        rightPressed = true;

    if (wParam == VK_SPACE || wParam == VK_UP)
        spacePressed = true;
}

void Engine::MousePosition(int x, int y)
{
    // Campture mouse position when the mouse moves
    // Don't do any logic here, you want to control the actual logic in the Logic method below
}

void Engine::MouseButtonUp(bool left, bool right)
{
    // If mouse button is released, set the mouse flags
    // Don't do any logic here, you want to control the actual logic in the Logic method below
}

void Engine::MouseButtonDown(bool left, bool right)
{
    // If mouse button is pressed, set the mouse flags
    // Don't do any logic here, you want to control the actual logic in the Logic method below
}

void Engine::Logic(double elapsedTime)
{
    // This is the logic part of the engine. It receives the elapsed time from the app class, in seconds.
    // It uses this value for a smooth and consistent movement, regardless of the CPU or graphics speed

    if (gameOver) // Do nothing if game over
    {
        return;
    }

    // We will need the stack in several places below
    Matrix* stackCells = stack->GetCells();

    // Due to a high FPS, we can't consider the keys at every frame because movement will be very fast
    // So we're using a delay, and if enough time has passed we take a key press into consideration
    keyPressAccumulated += elapsedTime;
    if (keyPressAccumulated > keyPressDelay)
    {
        keyPressAccumulated = 0;
        
        if (leftPressed || rightPressed || spacePressed)
        {
            // Remove any full rows
            int removed = stack->RemoveLines();
            if (removed > 0)
            {
                score += pow(2, removed) * 100;
                autoFallDelay = autoFallDelay * 0.98;
            }
        }

        // Move left or right
        if (leftPressed)
            activePiece->GoLeft(stackCells);
        if (rightPressed)
            activePiece->GoRight(stackCells);

        // Rotate
        if (spacePressed)
        {
            activePiece->Rotate(stackCells);
            spacePressed = false;
        }

        // Move down
        // On this one we will just set autoFallAccumulated to be high, because we have the down movemenet logic below
        if (downPressed)
            autoFallAccumulated = autoFallDelay + 1;
    }

    // The piece falls automatically after a delay
    autoFallAccumulated += elapsedTime;
    if (autoFallAccumulated > autoFallDelay)
    {
        autoFallAccumulated = 0;

        // Remove any full rows
        int removed = stack->RemoveLines();
        if (removed > 0)
        {
            score += pow(2, removed) * 100;
            autoFallDelay = autoFallDelay * 0.98;
        }

        // Move down the active piece
        bool isConflict = activePiece->Advance(stackCells);
        // If we have a conflict with the stack, it means we were sitting on the stack or bottom wall already
        if (isConflict)
        {
            // We add the piece to stack
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    if (activePiece->GetCells()->Get(j, i) == true)
                    {
                        int realx = activePiece->GetPosition().x + j;
                        int realy = activePiece->GetPosition().y + i;
                        stackCells->Set(realx, realy, true);
                    }
                }
            }

            // Delete active piece, activate the waiting piece and generate new waiting piece
            delete activePiece;
            activePiece = waitingPiece;
            activePiece->Activate();
            waitingPiece = new Piece();
            waitingPiece->InitializeD2D(m_pRenderTarget);

            // If we have a collision right after we generate the new piece, 
            // it means the stack is too high, so game over
            if (activePiece->StackCollision(stackCells))
                gameOver = true;
        }
    }

}

HRESULT Engine::Draw()
{
    // This is the drawing method of the engine.
    // It runs every frame

    // Draws the elements in the game using Direct2D
    HRESULT hr;

    m_pRenderTarget->BeginDraw();

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());


    m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	
	// Below you can add drawing logic for your game elements
    DrawTextAndScore();
    stack->Draw(m_pRenderTarget);
    activePiece->Draw(m_pRenderTarget);
    waitingPiece->Draw(m_pRenderTarget);
    
    hr = m_pRenderTarget->EndDraw();

    return S_OK;
}

void Engine::DrawTextAndScore()
{
    // Text and score
    int padding = (RESOLUTION_Y - (STACK_HEIGHT + 1) * CELL_SIZE) / 2;
    int centerRight = RESOLUTION_X - (RESOLUTION_X - padding - (STACK_WIDTH + 2) * CELL_SIZE) / 2;


    D2D1_RECT_F rectangle1 = D2D1::RectF(centerRight - 200, padding, centerRight + 200, padding + 100);
    m_pRenderTarget->DrawText(
        L"Next Piece",
        15,
        m_pTextFormat,
        rectangle1,
        m_pWhiteBrush
    );


    D2D1_RECT_F rectangle2 = D2D1::RectF(centerRight - 200, padding + 200, centerRight + 200, padding + 300);
    m_pRenderTarget->DrawText(
        L"Score",
        5,
        m_pTextFormat,
        rectangle2,
        m_pWhiteBrush
    );


    D2D1_RECT_F rectangle3 = D2D1::RectF(centerRight - 200, padding + 300, centerRight + 200, padding + 400);
    WCHAR scoreStr[64];
    swprintf_s(scoreStr, L"%d        ", score);
    m_pRenderTarget->DrawText(
        scoreStr,
        7,
        m_pTextFormat,
        rectangle3,
        m_pWhiteBrush
    );
}
