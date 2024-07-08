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
    GameController _gameController;
public:
    MainApp();
    ~MainApp();

    // Register the window class and call methods for instantiating drawing resources
    HRESULT Initialize();

    // Process and dispatch messages
    void RunMessageLoop();

private:
    // Initialize device-independent resources.
    HRESULT CreateDeviceIndependentResources();

    // Initialize device-dependent resources.
    HRESULT CreateDeviceResources();

    // Release device-dependent resource.
    void DiscardDeviceResources();

    // Updates the game state, returns true if something changed
    bool OnUpdate();

    // Draw content.
    HRESULT OnRender();

    // Resize the render target and world representation
    void OnResize(
        UINT width,
        UINT height
    );

    // Resize the window
    void OnResizing(
        WPARAM wParam,
        LPRECT prect
    );

    // The windows procedure.
    static LRESULT CALLBACK WndProc(
        HWND hWnd,
        UINT message,
        WPARAM wParam,
        LPARAM lParam
    );
};