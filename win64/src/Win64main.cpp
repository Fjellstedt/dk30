/* ========================================================================
   $Creator: Patrik Fjellstedt $
   ======================================================================== */
#include "w_pch.h"
#include "Win64_Renderer.h"
#include <sstream>
#include <Platform.h>
#include <Memory.h>
#include <Rendering.h>
#include <Math.h>

using namespace Cryptic;

Input::InputState gInputState;
// TODO(pf): Control bitmap allocation.

static Input::AxisState *ModifyInputAxisState(Input::Axis axis, Input::Frame frame = Input::Frame::Current)
{
	return &gInputState.axes[(U32)(frame)][(U32)(axis)];
}

static Input::ButtonState *ModifyInputButtonState(int btn, Input::Frame frame = Input::Frame::Current)
{
	return &gInputState.buttons[(U32)(frame)][btn];
}

static void OnKey(WPARAM wParam, LPARAM lParam, bool bIsDown)
{
	//bool isDown = (lParam >> 30) == 1;
	bool altIsDown = GetKeyState(VK_MENU);
	bool ctrlIsDown = GetKeyState(VK_CONTROL);
	// TODO(pf): Even handle these inside the app incase we want to do some logic based on these (saving etc) ?
	if (wParam == VK_ESCAPE)
	{
		PostQuitMessage(0);
	}
	else if (wParam == VK_F4 && altIsDown)
	{
		PostQuitMessage(0);
	}
	else
	{
		ModifyInputButtonState((int)wParam)->isDown = bIsDown;
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CREATE:
		{
		}break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			OnKey(wParam, lParam, true);
			return 0;
		}break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			OnKey(wParam, lParam, false);
			return 0;
		}break;
		case WM_LBUTTONUP:
		{
			ModifyInputButtonState(SpecialButton::M1, Input::Frame::Current)->isDown = false;
			return 0;
		}break;
		case WM_RBUTTONUP:
		{
			ModifyInputButtonState(SpecialButton::M2, Input::Frame::Current)->isDown = false;
			return 0;
		}break;
		case WM_LBUTTONDOWN:
		{
			ModifyInputButtonState(SpecialButton::M1, Input::Frame::Current)->isDown = true;
			return 0;
		}break;
		case WM_RBUTTONDOWN:
		{
			ModifyInputButtonState(SpecialButton::M2, Input::Frame::Current)->isDown = true;
			return 0;
		}break;
		case WM_MOUSEMOVE:
		{
			POINT p;
			if (GetCursorPos(&p) && ScreenToClient(hwnd, &p))
			{
				// TODO(pf): Add support for picking.
				//ModifyInputAxisState(Input::Axis::Mouse)->x = (F32)p.x / PixelsPerMeter;
				//ModifyInputAxisState(Input::Axis::Mouse)->y = (F32)p.y / PixelsPerMeter;
			}
			return 0;
		}break;
		case WM_MOUSEWHEEL:
		{
			ModifyInputAxisState(Input::Axis::Mouse)->z = GET_WHEEL_DELTA_WPARAM(wParam);
		}break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

struct Win64GameCode
{
	HMODULE gameCodeDLL;
	GameEntryDLL *entry;
	GameLoopDLL *loop;
};

// TODO(pf): support dynamic loading.
static Win64GameCode LoadGameCode(const char *SourceDLLName, PlatformLayer *layer)
{
	Win64GameCode result = {};
	result.gameCodeDLL = LoadLibraryA(SourceDLLName);
	if (result.gameCodeDLL)
	{
		result.entry = (GameEntryDLL *)GetProcAddress(result.gameCodeDLL, "GameEntry");
		result.loop = (GameLoopDLL *)GetProcAddress(result.gameCodeDLL, "GameLoop");
		layer->reloaded = true;
	}
	else
	{
		result.entry = nullptr;
		result.loop = nullptr;
	}
	return result;
}

static I64 QueryCurrentTime()
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return currentTime.QuadPart;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	const char className[] = "Engine2";
	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	// add CS_DBLCLKS to recieve double clicks. this will replace the second down message when it registers two consecutive clicks.
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(hInstance, IDC_CROSS);
	windowClass.lpszClassName = className;

	LPCSTR windowName = "game";

	if (!RegisterClassEx(&windowClass))
	{
		OutputDebugString("Registering class failed.");
		return 0;
	}

	// NOTE(pf): This is the window size, the rendering area will be smaller!
	HWND hwnd = CreateWindowEx(0, className, windowName, WS_OVERLAPPEDWINDOW,
							   300, 100, 1280, 720,
							   NULL, NULL, hInstance, NULL);

	if (hwnd == NULL)
	{
		OutputDebugString("Create window failed.");
		return 0;
	}

	ShowWindow(hwnd, nShowCmd);

	// NOTE(pf): Start of initialization.
	MSG msg = {};
	
	LARGE_INTEGER countsPerSec;
	QueryPerformanceFrequency(&countsPerSec);
	F64 frequency = (static_cast<F64>(countsPerSec.QuadPart));
	I64 currentTime = 0;
	I64 previousTime = 0;
	F64 totalTime = 0.0f;
	F64 prevTotalTime = 0.0f;
	F64 deltaTime = 0.0f;

	Win64_Renderer renderer;
	renderer.screenDim.x = 800;
	renderer.screenDim.x = 600;

	PlatformLayer platLayer{0};
	platLayer.screenWidth = renderer.screenDim.x;
	platLayer.screenHeight = renderer.screenDim.y;

	// NOTE: Memory leak tracker.
#if (DEBUG) | (_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

#if _DEBUG
	LPVOID baseAddress = reinterpret_cast<LPVOID>(TB(2));
#else
	LPVOID baseAddress = 0;
#endif
	platLayer.memorySize = MB(256);
	platLayer.memory = VirtualAlloc(baseAddress, platLayer.memorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	platLayer.shouldExitGame = false;
	Win64GameCode gameCode = LoadGameCode("game.dll", &platLayer);

	// NOTE(pf): Start of loop.

	gameCode.entry(&platLayer);
	while (!platLayer.shouldExitGame)
	{
		I64 startTime = QueryCurrentTime();
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
				case WM_QUIT:
				{
					platLayer.shouldExitGame = true;
				}break;
				default:
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}break;
			}
		}

		// GAME LOOP.

		DrawCall drawCalls = {};
		platLayer.delta = (F32)deltaTime;
		gameCode.loop(&platLayer, &drawCalls, &gInputState);

		// RENDERING.

		renderer.BeginDraw(&drawCalls);
		renderer.EndDraw();

		// INPUT PROCESSING.
		for (U32 axis = 0; axis < (U32)Input::Axis::_AxisSize; axis++)
		{
			gInputState.axes[(U32)Input::Frame::Previous][axis] = gInputState.axes[(U32)(Input::Frame::Current)][axis];
		}
		ModifyInputAxisState(Input::Axis::Mouse)->z = 0;

		for (U32 btn = 0; btn < (U32)Input::BUTTONSIZE; btn++)
		{
			gInputState.buttons[(U32)(Input::Frame::Previous)][btn] = gInputState.buttons[(U32)(Input::Frame::Current)][btn];
		}

		// END OF FRAME TIME SWAP.
		I64 endTime = QueryCurrentTime();
		deltaTime = max(((endTime - startTime) / frequency), 0);
		totalTime += deltaTime;
		if ((totalTime - prevTotalTime) >= 0.1f)
		{
			std::wostringstream outs;
			outs.precision(6);
			outs << windowName << L" FPS: " << (I64)(1.0f / deltaTime) <<
				L" Delta: " << deltaTime << L" Elapsed Time: " << (I64)totalTime;
			SetWindowTextW(hwnd, outs.str().c_str());
			prevTotalTime = totalTime;
		}

		platLayer.delta = (F32)deltaTime;
	}

	return 0;
}