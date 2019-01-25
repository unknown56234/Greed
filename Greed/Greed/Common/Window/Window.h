#pragma once
namespace gr
{
    namespace Common
    {
        namespace Window
        {
            class Window
            {
            public:
                void Show()
                {
                    ShowWindow(hMainWindow, SW_NORMAL);
                    UpdateWindow(hMainWindow);
                }
                void SetWindowEvent(const std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>& func)
                {
                    windowEvents.emplace_back(func);
                }
                unsigned int GetWidth() const
                {
                    return width;
                }
                unsigned int GetHeight() const
                {
                    return height;
                }
                HWND GetWindowHandle() const
                {
                    return hMainWindow;
                }
                HINSTANCE GetInstanceHandle() const
                {
                    return hInstance;
                }
                void Loop(const std::function<void(void)>& func)
                {
                    MSG msg = { 0 };
                    while (WM_QUIT != msg.message)
                    {
                        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                        {
                            TranslateMessage(&msg);
                            DispatchMessage(&msg);
                        }
                        else
                        {
                            func();
                        }
                    }
                }
                Window(HINSTANCE hInstance, unsigned int width, unsigned int height) :
                    hInstance(hInstance), width(width), height(height)
                {
                    std::string className = "Greed";
                    std::string windowName = "MainWindow:Greed";

                    WNDCLASSEX wcex;
                    wcex.cbSize = sizeof(WNDCLASSEX);
                    wcex.style = CS_HREDRAW | CS_VREDRAW;
                    wcex.lpfnWndProc = WndProcBase;
                    wcex.cbClsExtra = 0;
                    wcex.cbWndExtra = 0;
                    wcex.hInstance = hInstance;
                    wcex.hIcon = NULL;
                    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
                    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
                    wcex.lpszMenuName = NULL;
                    wcex.lpszClassName = className.data();
                    wcex.hIconSm = NULL;
                    if (!RegisterClassEx(&wcex))
                    {
                        assert("window create failed" && false);
                    }

                    RECT rc = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
                    //AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
                    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

                    //hMainWindow = CreateWindow(className.data(), windowName.data(), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
                    hMainWindow = CreateWindow(className.data(), windowName.data(), WS_OVERLAPPEDWINDOW, 0, 0, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

                    SetWindowLongPtr(hMainWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
                }
                ~Window()
                {
                    DestroyWindow(hMainWindow);
                }
                //TODO –ß‚è’l
                LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
                {
                    for (const auto&windowEvent : windowEvents)
                    {
                        windowEvent(hWnd, message, wParam, lParam);
                    }
                    return S_OK;
                }
                static LRESULT CALLBACK WndProcBase(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
                {
                    auto ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
                    if (ptr != 0)
                    {
                        reinterpret_cast<Window*>(ptr)->WndProc(hWnd, message, wParam, lParam);
                    }
                    switch (message)
                    {
                    case WM_DESTROY: {
                        PostQuitMessage(0);
                        return DefWindowProc(hWnd, message, wParam, lParam);
                        break;
                    }
                    default: {
                        return DefWindowProc(hWnd, message, wParam, lParam);
                        break;
                    }
                    }
                }
            private:
                unsigned int width{};
                unsigned int height{};
                HWND hMainWindow{};
                HINSTANCE hInstance{};
                std::vector<std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>> windowEvents{};
            };
        }
    }
}