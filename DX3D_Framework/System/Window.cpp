#include "framework.h"
#include "Window.h"

//상대경로 : ./ 현재폴더, ../ 상위폴더
//절대경로 : 

DXDesc Window::desc;
HINSTANCE instance;

Window::Window(DXDesc desc)
{
	WNDCLASSEX wndClass;

	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hIconSm = wndClass.hIcon;
	wndClass.hInstance = desc.instance;
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = desc.AppName.c_str();
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbSize = sizeof(WNDCLASSEX);

	WORD wHr = RegisterClassEx(&wndClass);
	assert(wHr != 0);

	desc.handle = CreateWindowEx
	(
		WS_EX_APPWINDOW,
		desc.AppName.c_str(),
		desc.AppName.c_str(),
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		(HMENU)NULL,
		desc.instance,
		NULL
	);

	RECT rect = { 0, 0, (LONG)desc.width, (LONG)desc.height };

	UINT centerX = (GetSystemMetrics(SM_CXSCREEN) - (UINT)desc.width) / 2;
	UINT centerY = (GetSystemMetrics(SM_CYSCREEN) - (UINT)desc.height) / 2;

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	//x, y, width, height
	MoveWindow
	(
		desc.handle
		, centerX, centerY
		, rect.right - rect.left, rect.bottom - rect.top
		, true
	);

	ShowWindow(desc.handle, SW_SHOWNORMAL);
	SetForegroundWindow(desc.handle);
	SetFocus(desc.handle);

	//ShowCursor(true);
	Window::desc = desc;

	::instance = desc.instance;
}

Window::~Window()
{
	DestroyWindow(desc.handle);
	UnregisterClass(desc.AppName.c_str(), desc.instance);
}

WPARAM Window::Run(IObject* mainObj)
{

	MSG msg = { 0 }; //스니핑

	this->mainObj = mainObj;
	this->mainObj->Init();

    while (msg.message != WM_QUIT) // 메세지에 종료가 들어오지 않으면 true를 반환한다.
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // 메세지 들어왔을때만 처리하고 아닐때는 버림.
        {
			if (msg.message == WM_QUIT)
				break;
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }   // 게임프로그래밍에서 많이 쓰는 루프방법.
        else
        {
            // Update
            // 이러면 프레임이 잘 나온다.
            MainRender();
        }

    }
	this->mainObj->Destroy();

	return msg.wParam;
}

HWND handle;
LRESULT Window::WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CREATE)
		::handle = handle;

	if (message == WM_CLOSE || message == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

void Window::MainRender()
{

}
