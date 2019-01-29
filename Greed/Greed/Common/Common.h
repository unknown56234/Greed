#pragma once
#include<cassert>
#include<string_view>
#include<functional>
#include<array>
#include<vector>
#include<map>
#include<filesystem>

#include<dxgi1_6.h>
#include<wrl.h>
#include<d3d12.h>
#include"../ThirdParty/D3DX12/d3dx12.h"
#include<d3dcompiler.h>

#include<DirectXMath.h>

#include<pix3.h>

#include"../ThirdParty/DirectXTex/include/DirectXTex.h"

#include"../ThirdParty/imgui/imgui.h"
#include"../ThirdParty/imgui/imgui_impl_win32.h"
#include"../ThirdParty/imgui/imgui_impl_dx12.h"

#include"../ThirdParty/tinygltf/tiny_gltf.h"

#include"../ThirdParty/chaiscript/chaiscript.hpp"

#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"runtimeobject.lib")

#ifdef _DEBUG
#pragma comment(lib,"Greed/ThirdParty/DirectXTex/lib/x64/Debug/DirectXTex.lib")
#else
#pragma comment(lib,"Greed/ThirdParty/DirectXTex/lib/x64/Release/DirectXTex.lib")
#endif

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using Microsoft::WRL::ComPtr;

//assertの実装コピペしてちょい書き換え
//TODO 気に食わないからなんとかする
#ifdef _DEBUG
#define panic(message) (void)(                                                       \
            (_wassert(_CRT_WIDE(message), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)), 0) \
        )
#else
#define panic(message)
#endif

#include"Resource/Resource.h"
#include"Window/Window.h"