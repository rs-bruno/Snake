// Snake.cpp : Define el punto de entrada de la aplicación.
//

#include "Snake.h"

#define MAX_LOADSTRING 100

// Utilities
template<class Interface>
inline void SafeRelease(
    Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// TODO: replace by a function that takes an objetive client size and returns a windows size
bool TryGetBorderHeader(HWND m_hwnd, int& border, int& header)
{
    // Get the window and client dimensions
    RECT wndRect, clRect;
    if (!GetWindowRect(m_hwnd, &wndRect))
        return false;
    if (!GetClientRect(m_hwnd, &clRect))
        return false;

    // Edges
    border = (wndRect.right - wndRect.left) - clRect.right;
    header = (wndRect.bottom - wndRect.top) - clRect.bottom;
    return true;
}

// Main app implementation
MainApp::MainApp() :
    m_hwnd(NULL),
    m_pDirect2dFactory(NULL),
    m_pRenderTarget(NULL),
    m_pLightSlateGrayBrush(NULL),
    m_pCornflowerBlueBrush(NULL)
{}
MainApp::~MainApp()
{
    SafeRelease(&m_pDirect2dFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pCornflowerBlueBrush);
}
void MainApp::RunMessageLoop()
{
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
HRESULT MainApp::Initialize()
{
    HRESULT hr;

    // Initialize device-independent resources, such
    // as the Direct2D factory.
    hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr))
    {
        // Register the window class.
        WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = MainApp::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = NULL;
        wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
        wcex.lpszClassName = L"MainWindowClass";

        RegisterClassEx(&wcex);

        // In terms of using the correct DPI, to create a window at a specific size
        // like this, the procedure is to first create the window hidden. Then we get
        // the actual DPI from the HWND (which will be assigned by whichever monitor
        // the window is created on). Then we use SetWindowPos to resize it to the
        // correct DPI-scaled size, then we use ShowWindow to show it.

        m_hwnd = CreateWindow(
            L"MainWindowClass",
            L"Snake",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            this);

        if (m_hwnd)
        {
            // Because the SetWindowPos function takes its size in pixels, we
            // obtain the window's DPI, and use it to scale the window size.
            float dpi = GetDpiForWindow(m_hwnd);

            int blockSizePx = static_cast<int>(ceil(_gameController.GetBlockSize() * (dpi / 96.f)));
            int xPx = _gameController.GetWorldSizeX() * blockSizePx;
            int yPx = _gameController.GetWorldSizeY() * blockSizePx;
            int border, header;
            TryGetBorderHeader(m_hwnd, border, header);

            SetWindowPos(
                m_hwnd,
                NULL,
                NULL,
                NULL,
                xPx + border,
                yPx + header,
                SWP_NOMOVE);
            ShowWindow(m_hwnd, SW_SHOWNORMAL);
            UpdateWindow(m_hwnd);
        }
    }

    return hr;
}
HRESULT MainApp::CreateDeviceIndependentResources()
{
    HRESULT hr = S_OK;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

    if (!SUCCEEDED(hr))
        return hr;

    // Create DirectWrite factory.
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&m_pDirectWriteFactory));

    return hr;
}
HRESULT MainApp::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
        );

        // Create a Direct2D render target.
        hr = m_pDirect2dFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
        );

        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::LightSlateGray),
                &m_pLightSlateGrayBrush
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
                &m_pCornflowerBlueBrush
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Red),
                &m_pRedBrush
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::GreenYellow),
                &m_pGreenBrush
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::DarkSlateBlue),
                &m_pBlueBrush
            );
        }
        if (SUCCEEDED(hr))
        {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::AntiqueWhite),
                &m_pWhiteBrush
            );
        }

        if (SUCCEEDED(hr))
        {
            float dpi = GetDpiForWindow(m_hwnd);
            hr = m_pDirectWriteFactory->CreateTextFormat(
                L"Arial",
                NULL,
                DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                12.0f * dpi / 72.0f, // 12pt font size
                L"en-US",
                &m_pTextFormat
            );
        }
    }

    return hr;
}
void MainApp::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pCornflowerBlueBrush);
    SafeRelease(&m_pRedBrush);
    SafeRelease(&m_pGreenBrush);
    SafeRelease(&m_pBlueBrush);
    SafeRelease(&m_pWhiteBrush);
}
HRESULT MainApp::OnRender()
{
    HRESULT hr = S_OK;

    hr = CreateDeviceResources();
    if (SUCCEEDED(hr))
    {
        _gameController.Update();

        m_pRenderTarget->BeginDraw();

        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

        int blockSize = _gameController.GetBlockSize();
        int width = _gameController.GetWorldSizeX() * blockSize;
        int height = _gameController.GetWorldSizeY() * blockSize;

        // Draw the fruits.
        for (auto& fruit : _gameController._fruits)
        {
            auto fruitBlock = fruit.first;
            auto fuitColor =
                fruit.second == Fruit::GROWTH_FRUIT ? m_pRedBrush :
                fruit.second == Fruit::LIFE_FRUIT ? m_pGreenBrush :
                fruit.second == Fruit::SLOW_FRUIT ? m_pBlueBrush :
                m_pLightSlateGrayBrush;
            D2D1_RECT_F rectangle1 = D2D1::RectF(
                fruitBlock.x * blockSize,
                fruitBlock.y * blockSize,
                (fruitBlock.x + 1) * blockSize,
                (fruitBlock.y + 1) * blockSize
            );
            m_pRenderTarget->FillRectangle(&rectangle1, fuitColor);
        }

        // Draw the snake body.
        for (auto& snakeBlock : _gameController._snakeBody)
        {
            D2D1_RECT_F rectangle1 = D2D1::RectF(
                snakeBlock.x * blockSize,
                snakeBlock.y * blockSize,
                (snakeBlock.x + 1) * blockSize,
                (snakeBlock.y + 1) * blockSize
            );
            auto snakeColor = _gameController.GameOver() ? m_pRedBrush : m_pCornflowerBlueBrush;
            m_pRenderTarget->FillRectangle(&rectangle1, snakeColor);
        }

        // Draw a grid.
        for (int x = 0; x < width; x += blockSize)
        {
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
                D2D1::Point2F(static_cast<FLOAT>(x), height),
                m_pLightSlateGrayBrush,
                0.75f
            );
        }
        for (int y = 0; y < height; y += blockSize)
        {
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
                D2D1::Point2F(width, static_cast<FLOAT>(y)),
                m_pLightSlateGrayBrush,
                0.5f
            );
        }

        // Draw text over the frame.
        D2D1_RECT_F layoutRect = D2D1::RectF(0.f, 0.f, 500.f, 100.f);
        wstringstream fps;
        fps << std::setprecision(4) << _gameController.GetUpdateRate() 
            << L"\n"
            << _gameController.GetLives()
            << L"\n"
            << _gameController._snakeBody.size();
        m_pRenderTarget->DrawText(
            fps.str().c_str(),
            fps.str().length(),
            m_pTextFormat,
            layoutRect,
            m_pWhiteBrush
        );

        hr = m_pRenderTarget->EndDraw();

        if (hr == D2DERR_RECREATE_TARGET)
        {
            hr = S_OK;
            DiscardDeviceResources();
        }
    }

    return hr;
}
void MainApp::OnResize(UINT width, UINT height)
{
    if (m_pRenderTarget)
    {
        // Note: This method can fail, but it's okay to ignore the
        // error here, because the error will be returned again
        // the next time EndDraw is called.
        m_pRenderTarget->Resize(D2D1::SizeU(width, height));

        float dpi = GetDpiForWindow(m_hwnd);
        int dipxW = static_cast<int>(ceil(width / (dpi / 96.f)));
        int dipxH = static_cast<int>(ceil(height / (dpi / 96.f)));
        int blockSize = _gameController.GetBlockSize();
        _gameController.ResizeWorld(dipxW / blockSize, dipxH / blockSize);
    }
}
void MainApp::OnResizing(WPARAM wParam, LPRECT rectp)
{
    // Get the window and client dimensions
    RECT wndRect, clRect;
    GetWindowRect(m_hwnd, &wndRect);
    GetClientRect(m_hwnd, &clRect);

    // Edges
    int border, header;
    TryGetBorderHeader(m_hwnd, border, header);

    // Offered client width and height
    int width = (rectp->right - rectp->left) - border;
    int height = (rectp->bottom - rectp->top) - header;

    float dpi = GetDpiForWindow(m_hwnd);
    int blockSizePx = static_cast<int>(ceil(_gameController.GetBlockSize() * (dpi / 96.f)));
    int minClientDim = 15 * blockSizePx;

    switch (wParam)
    {
    case WMSZ_RIGHT:
    case WMSZ_BOTTOM:
    case WMSZ_BOTTOMRIGHT:
        if (width < minClientDim)
            width = minClientDim;
        if (height < minClientDim)
            height = minClientDim;
        width = width - (width % blockSizePx);
        height = height - (height % blockSizePx);
        rectp->right = (rectp->left) + width + border;
        rectp->bottom = (rectp->top) + height + header;
        break;
    default:
        *rectp = wndRect;
        break;
    }
}

// WndProc implementation
LRESULT CALLBACK MainApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    if (message == WM_CREATE)
    {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        MainApp* pDemoApp = (MainApp*)pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pDemoApp)
        );

        result = 1;
    }
    else
    {
        MainApp* pDemoApp = reinterpret_cast<MainApp*>(static_cast<LONG_PTR>(
            ::GetWindowLongPtrW(
                hwnd,
                GWLP_USERDATA
            )));

        bool wasHandled = false;

        if (pDemoApp)
        {
            switch (message)
            {
            case WM_KEYDOWN:
            {
                if (wParam == VK_SPACE) 
                {
                    pDemoApp->_gameController.ChangeState(CycleState::RUNNING);
                }
                else if (wParam == 'P')
                {
                    pDemoApp->_gameController.ChangeState(CycleState::PAUSED);
                }
                Direction changeDir =
                    wParam == 'W' ? Direction::UP :
                    wParam == 'A' ? Direction::LEFT :
                    wParam == 'S' ? Direction::DOWN :
                    wParam == 'D' ? Direction::RIGHT :
                    (Direction)(-1);
                if (changeDir != (Direction)(-1)) 
                {
                    pDemoApp->_gameController.ChangeHeadingDirection(changeDir);
                }
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_SIZING:
            {
                pDemoApp->OnResizing(wParam, (LPRECT)lParam);
            }
            result = TRUE;
            wasHandled = true;
            break;

            case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                pDemoApp->OnResize(width, height);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DISPLAYCHANGE:
            {
                InvalidateRect(hwnd, NULL, TRUE);
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_PAINT:
            {
                pDemoApp->OnRender();
            }
            result = 0;
            wasHandled = true;
            break;

            case WM_DESTROY:
            {
                PostQuitMessage(0);
            }
            result = 1;
            wasHandled = true;
            break;
            }
        }

        if (!wasHandled)
        {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

// WinMain implementation
int WINAPI WinMain(
    HINSTANCE /* hInstance */,
    HINSTANCE /* hPrevInstance */,
    LPSTR /* lpCmdLine */,
    int /* nCmdShow */
)
{
    // Use HeapSetInformation to specify that the process should
    // terminate if the heap manager detects an error in any heap used
    // by the process.
    // The return value is ignored, because we want to continue running in the
    // unlikely event that HeapSetInformation fails.
    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

    if (SUCCEEDED(CoInitialize(NULL)))
    {
        {
            MainApp app;

            if (SUCCEEDED(app.Initialize()))
            {
                app.RunMessageLoop();
            }
        }
        CoUninitialize();
    }

    return 0;
}