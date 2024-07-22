#pragma once

#include "framework.h"
#include "resource.h"
#include "GameController.h"

// Main app class
class MainApp
{
private:
    HWND m_hwnd;
    ID2D1Factory* m_pDirect2dFactory;
    ID2D1HwndRenderTarget* m_pRenderTarget;
    ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
    ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
    ID2D1SolidColorBrush* m_pRedBrush;
    ID2D1SolidColorBrush* m_pGreenBrush;
    ID2D1SolidColorBrush* m_pBlueBrush;
    ID2D1SolidColorBrush* m_pWhiteBrush;
    IDWriteFactory* m_pDirectWriteFactory;
    IDWriteTextFormat* m_pTextFormat;
    GameController _gameController;
public:
    MainApp();
    ~MainApp();

    // Register the window class and call methods for instantiating drawing resources
    void Initialize();

    // Process and dispatch messages
    void RunMessageLoop();

private:
    // Initialize device-dependent resources.
    void CreateDeviceResources();

    // Release device-dependent resource.
    void DiscardDeviceResources();

    // Draw content.
    void OnRender();

    // Resize the window
    void OnResizing(
        WPARAM wParam,
        LPRECT prect
    );

    // Resize the render target and world representation
    void OnResize(
        UINT width,
        UINT height
    );

    // The windows procedure.
    static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );
};