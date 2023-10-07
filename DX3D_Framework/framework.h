#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             

#define WIN_START_X 100
#define WIN_START_Y	100

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

#define CHECK(p) assert(SUCCEEDED(p))
#define Zero(p)  ZeroMemory(&p, sizeof(p))

#define SAFE_DELETE(p)		 { if(p) { delete(p);	   (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[](p);	   (p) = nullptr; } }
#define SAFE_RELEASE(p)		 { if(p) { (p)->Release(); (p) = nullptr; } }

#include <windows.h>
#include <string>

#include <vector>	// 연결 컨테이너

#include <map>
#include <unordered_map> // 비선형 자료구조중 정렬이 필요없을 때 사용.

#include <d3d11.h> // 다이렉트X선언
#include <d3dcompiler.h>
#include <DirectXMath.h>
// Math를 사용하는 이유
// 내부구조가 매우 어렵게 되어있다. vector처럼 구성되어있다.
// 대신 내부적으로 속도가 좀 빠르다.
// MSD에서 추천하는 라이브러리이다.

#pragma comment(lib, "d3d11.lib") // 라이브러리 추가
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace DirectX;

#include "Obejct/ObjacetInterface.h"
#include "Elements/Resources/Texture.h"

struct Vertex
{
    XMFLOAT3 pos;
    XMFLOAT4 color;

    Vertex(float x, float y, float z, float r, float g, float b)
        : pos(x, y, z), color(r, g, b, 1)
    {

    }
};

struct WVP
{
    XMMATRIX world;
    XMMATRIX view;
    XMMATRIX projection;
}wvp;
