// DX3D_Framework.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "DX3D_Framework.h"

#define MAX_LOADSTRING 100
#define PI 3.1415926
// 정점(Vertex) : 3차원 공간에 있는 한 점

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND hWnd;                                      // 윈도우 기본 핸들

ID3D11Device* device;   // CPU
ID3D11DeviceContext* deviceContext; // GPU
// 하드웨어 정보부터 알아야 DX를 할 수 있다.
// CPU와 GPU가 있다. CPU와 GPU를 연결한다.

IDXGISwapChain* swapChain; // 백버퍼 관리 인터페이스
ID3D11RenderTargetView* renderTargetView; // GPU쪽 관련 메모리를 접근하는 용도. 중재역활
// 렌더타겟뷰 = 도화지를 만들어서 스왑체인형태로 만드는것.

ID3D11VertexShader* vertexShader; // 버텍스 쉐이더
ID3D11PixelShader* pixelShader;     // 픽셀 쉐이더
ID3D11InputLayout* inputLayout;     // 인풋 레이아웃
ID3D11Buffer* vertexBuffer;         // 버텍스 하나 넘겨서 출력이 가능하다.
ID3D11Buffer* indexBuffer;          // 인덱스 버퍼를 사용한다.
ID3D11Buffer* constantBuffer;       // 월드버퍼

void InitDevice();      // 시도
void Render();          // 렌더
void ReleaseDevice();   // 메모리 해제하는 것.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DX3DFRAMEWORK, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DX3DFRAMEWORK));

    MSG msg = {};
    InitDevice();
    // 기본 메시지 루프입니다:
    while (msg.message != WM_QUIT) // 메세지에 종료가 들어오지 않으면 true를 반환한다.
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // 메세지 들어왔을때만 처리하고 아닐때는 버림.
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }   // 게임프로그래밍에서 많이 쓰는 루프방법.
        else
        {
            // Update
            // 이러면 프레임이 잘 나온다.
            Render();
        }

    }
    ReleaseDevice();
}

void InitDevice()
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {}; 
    swapChainDesc.BufferDesc.Width = WIN_WIDTH; // 디바이스 크기
    swapChainDesc.BufferDesc.Height = WIN_HEIGHT; // 디바이스 크기
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60; // 프레임
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1; // 프레임 디폴트값으로 저장해준다.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.SampleDesc.Count = 1;   
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.Windowed = true;


    D3D11CreateDeviceAndSwapChain(
        nullptr,                    // 1. 어답터 : 창관리어뎁터, null이 들어가면 기본이 사용됨
        D3D_DRIVER_TYPE_HARDWARE,   // 2. 드라이버 타입 : 하드웨어 인지 가상드라이브인지 등 을 설정한다.
        0,                          // 3. 소프트웨어 타입 : 드라이버타입 에서 소프트웨어 유형을 설정했을때만 지정한다.
        D3D11_CREATE_DEVICE_DEBUG,  // 4. 플레그 : 현재 진행하는 디바이스가 디버그인지 일반인지 지정한다.
        nullptr,                    // 5. 피쳐레벨 : 어플리케이션이 하드웨어 성능을 얼마나 쓰는지 지정
        0,                          // 6. 피쳐레벨 : 위 값이 있을경우 배열의 개수를 지정한다.
        D3D11_SDK_VERSION,          // 7. SDK버전 : 현재 SDK버전 을 기입한다.
        &swapChainDesc,             // 8. 스왑체인 : 현재 스왑체인의 기입사항을 지정해준다.
        &swapChain,                 // 9. 스왑체인인터페이스 : 인터페이스 포인터를 기입한다.
        &device,                    // 10. 디바이스(CPU) : CPU 인터페이스 포인터를 기입한다.
        nullptr,                    // 11. 디바이스수준 : CPU기능수준을 기입한다. null일경우 사용X
        &deviceContext              // 12. 디바이스(GPU) : GPU 인터페이스 포인터를 기입한다.
    );

    ID3D11Texture2D* backBuffer;
    //백버퍼 정보는 스왑체인이 들고있다.

    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    // IID = interface ID
    // 보이드포인터 = 버튼을 받는데 어떤 버튼인지 모를때 voidpointer를 사용한다.
    // 보이드포인터 이기 때문에 뭐를 받는지 몰라서. 그 포인터의 IID를 받아오는 것이다.
    device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    // 렌더타겟뷰를 백버퍼로 사용한다. 렌더타겟뷰의 주소를 전달.
    backBuffer->Release();
    // 벡버퍼를 그래픽카드로 넘겨주면 쓸모가 없기 떄문에 삭제해준다.
    deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
    // 3D를 넣으려면 깊이값도 넣어야 하지만, nullptr로 아직 넘어간다. 아웃펏 머지 설정한것
    /////////////////////////////////////////////////////////////////////////////////

    D3D11_VIEWPORT viewPort;
    viewPort.TopLeftX = 0;
    viewPort.TopLeftY = 0;
    viewPort.Width = WIN_WIDTH;
    viewPort.Height = WIN_HEIGHT;
    viewPort.MinDepth = 0.0f;
    viewPort.MaxDepth = 1.0f;
    // ndc좌표계를 스크린으로 쭉 늘려주는 역할을 하는게 뷰포트이다.

    deviceContext->RSSetViewports(1, &viewPort);

    DWORD flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
    // 쉐이더 로드하는 일반적인 옵션이다.

    ID3DBlob* blob;// 로드하는 그래픽의 데이터
    D3DCompileFromFile(L"Shaders/VertexShader.hlsl", nullptr, nullptr,
        "VS", "vs_5_0", flags, 0, &blob, nullptr);

    device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(),
        nullptr, &vertexShader);

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
        D3D11_INPUT_PER_VERTEX_DATA, 0},
        // 포지션 인풋을 잡아준다.
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,// 앞에 몃바이트를 썼나? 표시
        D3D11_INPUT_PER_VERTEX_DATA, 0} // 컬러 인풋을 잡아준다
    };  // 인스턴싱할때 바꾼다.

    UINT layoutSize = ARRAYSIZE(layoutDesc);

    device->CreateInputLayout(layoutDesc, layoutSize,
        blob->GetBufferPointer(), blob->GetBufferSize(), &inputLayout);

    blob->Release();

    D3DCompileFromFile(L"Shaders/VertexShader.hlsl", nullptr, nullptr,
        "PS", "ps_5_0", flags, 0, &blob, nullptr);

    device->CreatePixelShader
    (blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader);

    //Vertex vertex(0,0); // 이대로는 못한다 vram에 넘겨야함
    vector<Vertex> vertices;
    vertices.emplace_back(-1, -1, -1, 1, 0, 0);
    vertices.emplace_back(-1, +1, -1, 0, 1, 0);
    vertices.emplace_back(+1, -1, -1, 0, 0, 1);
    vertices.emplace_back(+1, +1, -1, 1, 1, 0);

    //vertices.emplace_back(-1, -1, +1, 1, 0, 1);
    //vertices.emplace_back(-1, +1, +1, 0, 1, 1);
    //vertices.emplace_back(+1, -1, +1, 1, 1, 1);
    //vertices.emplace_back(+1, +1, +1, 0, 0, 0);

    // emplace_back 과 push_back의 차이점.
    // push_back은 한번 생성해서 복사되어서 대입. 복사대입이 된다.
    // emplace_back은 바로 그자리에 생성된다. 이동대입이 된다.
    // new로 만들어서 넣는. 포인터 대입방식은 push_back과 emplace_back은 차이가 없다.
    // 
    // 인덱스 버퍼를 쓰는방법

    { // VertexBuffer
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT; // 허가권한
        bufferDesc.ByteWidth = sizeof(Vertex) * vertices.size();
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA subData = {};
        subData.pSysMem = vertices.data();

        device->CreateBuffer(&bufferDesc, &subData, &vertexBuffer);
    }
    vector<UINT> indices =
    {
        //Front
        0, 1, 2 ,2, 1 ,3
        //UP
        //1, 5, 3, 3, 5, 7
    };

    { // IndexBuffer
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT; // 허가권한
        bufferDesc.ByteWidth = sizeof(UINT) * indices.size();
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER; // 인덱스 버퍼 형이라고 기입

        D3D11_SUBRESOURCE_DATA subData = {};
        subData.pSysMem = indices.data();

        device->CreateBuffer(&bufferDesc, &subData, &indexBuffer);
    }

    { // ConstatntBuffer
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.Usage = D3D11_USAGE_DEFAULT; // 허가권한
        bufferDesc.ByteWidth = sizeof(WVP);
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // 상수 버퍼 형이라고 기입

        device->CreateBuffer(&bufferDesc, nullptr, &constantBuffer);
    }

    wvp.world = XMMatrixIdentity(); // 단위행렬이라고 지정한다.
    // 단위행렬 = 대각선으로 1111이 들어간 행렬상 영향을 안끼치는 행렬이다.

    XMVECTOR forcus = XMVectorSet(0, 0, 0, 0); // 카메라가 바라보는 위치
    XMVECTOR eye = XMVectorSet(3, 3, -3, 0); // 카메라 위치
    XMVECTOR up = XMVectorSet(0, 1, 0, 0); // 카메라 업벡터

    wvp.view = XMMatrixLookAtLH(eye, forcus, up);// LeftHand 왼손좌표계 라는뜻

    // fov, 종횡비, 
    wvp.projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, // 시야각도
        (float)WIN_WIDTH / WIN_HEIGHT,          // 종횡비
        0.1f, 1000.0f);                         // 절도체 잡기.

}

void Render()
{
    float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    // 디바이스 컨텍스트, 렌더타겟뷰를 컬러로 초기화 하겠다.

    //static float angle = 0.0f;
    //angle += 0.0001;

    //wvp.world = XMMatrixRotationY(angle);

    WVP temp; // 전치행렬로 바꾸어서 넘겨야 쉐이더에서는 정상적으로 연산한다.
    temp.world = XMMatrixTranspose(wvp.world);
    temp.view = XMMatrixTranspose(wvp.view);
    temp.projection = XMMatrixTranspose(wvp.projection);

    deviceContext->UpdateSubresource(constantBuffer, 0, nullptr, &temp, 0, 0);
    deviceContext->VSSetConstantBuffers(0, 1, &constantBuffer); // VS에 지정한다.


    //Render
    UINT stride = sizeof(Vertex); // 정점 하나
    UINT offset = 0; // 어디서부터 잡을껀지

    deviceContext->IASetInputLayout(inputLayout);
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0); // 인덱스 버퍼 세팅법

    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // 선 그림
    deviceContext->VSSetShader(vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pixelShader, nullptr, 0);

    //deviceContext->DrawIndexed(12, 0, 0);
    deviceContext->DrawIndexed(6, 0, 0);
    swapChain->Present(0, 0);
}

void ReleaseDevice()
{
    device->Release();
    deviceContext->Release();

    swapChain->Release();
    renderTargetView->Release();
    // 할당된 것들을 전부 릴리즈 해준다.
}


//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DX3DFRAMEWORK));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DX3DFRAMEWORK);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) // 시작부분 중요한 부분
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
    RECT rc = { 0, 0, WIN_WIDTH, WIN_HEIGHT };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false); // 윈도우의 Rect를 조절하겠다. Adjust = 조절하다.
    // LPRect = RECT*
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,// 전역으로 사용한다.
        // WIN_START_X, WIN_START_Y,  // 윈도우 창 시작위치 잡아주는것
        CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);

    SetMenu(hWnd, nullptr); // 윈도우 창의 메뉴를 사용하지 않겠다.
    // hWnd핸들을 넣어서 해당 매뉴의 매뉴를 띄우지 않겠다.

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
