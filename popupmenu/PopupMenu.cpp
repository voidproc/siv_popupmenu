#include "stdafx.h"
#include "PopupMenu.h"

namespace
{
	// メインスレッドIDを設定する
	constexpr UINT WM_USER_SET_MAINTHREADID = WM_USER + 1;

	// メニューを表示する
	constexpr UINT WM_USER_SHOWMENU = WM_USER + 2;

	// メニューが選択された
	constexpr UINT WM_USER_MENUCOMMAND = WM_USER + 3;
}

namespace
{
	// メインウィンドウのハンドルを得る
	HWND GetWindowHandle()
	{
		return static_cast<HWND>(Platform::Windows::Window::GetHWND());
	}

	// プロセスのインスタンスハンドルを得る
	HINSTANCE GetInstanceHandle()
	{
		return GetModuleHandle(nullptr);
	}

	// 元のウィンドウプロシージャ
	LONG_PTR gOriginalWindowProc = 0;

	// ウィンドウプロシージャを変更する
	void SetWindowProc(HWND hWnd, WNDPROC proc)
	{
		gOriginalWindowProc = SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(proc));
	}

	// メニュー処理用のウィンドウプロシージャ
	// Siv3D のウィンドウが作成されたスレッド (THREAD #0) で実行される
	// https://github.com/Siv3D/OpenSiv3D/blob/main/Siv3D/src/Siv3D-Platform/WindowsDesktop/Siv3D/Siv3DMain.cpp
	//
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static DWORD mainThreadId = 0;

		switch (message)
		{
		case WM_USER_SET_MAINTHREADID:
			mainThreadId = (DWORD)wParam;
			break;

		case WM_USER_SHOWMENU:
		{
			const auto hMenu = LoadMenu(GetInstanceHandle(), MAKEINTRESOURCE(wParam));

			if (hMenu == nullptr)
				break;

			const auto hSubMenu = GetSubMenu(hMenu, 0);

			const auto mousePos = Cursor::ScreenPos();

			TrackPopupMenu(hSubMenu, TPM_LEFTALIGN, mousePos.x, mousePos.y, 0, hWnd, nullptr);

			DestroyMenu(hMenu);

			break;
		}

		case WM_COMMAND:
			if (lParam == 0)
			{
				// 選択されたメニュー項目をメインスレッドに通知する
				PostThreadMessage(mainThreadId, WM_USER_MENUCOMMAND, LOWORD(wParam), 0);
			}
			break;
		}

		return CallWindowProc(reinterpret_cast<WNDPROC>(gOriginalWindowProc), hWnd, message, wParam, lParam);
	}
}

PopupMenu::PopupMenu(WORD menuResourceId)
	: menuResourceId_{ menuResourceId }
{
	const auto hWnd = GetWindowHandle();

	SetWindowProc(hWnd, WindowProc);

	// 別スレッドで実行されるウィンドウプロシージャに、必要な情報を渡しておく
	PostMessage(hWnd, WM_USER_SET_MAINTHREADID, (WPARAM)GetCurrentThreadId(), 0);
}

PopupMenu::~PopupMenu()
{
}

void PopupMenu::show()
{
	PostMessage(GetWindowHandle(), WM_USER_SHOWMENU, (WPARAM)menuResourceId_, 0);
}

Optional<WORD> PopupMenu::selectedItem() const
{
	MSG msg{};
	Optional<WORD> id;

	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		BOOL ret = GetMessage(&msg, nullptr, 0, 0);

		if (ret == -1)
		{
			//error
		}
		else if (msg.message == WM_USER_MENUCOMMAND)
		{
			id = (WORD)msg.wParam;
		}
	}

	return id;
}
