#include <atlconv.h>
#include "Utils.h"

#include "InputDialog.h"

#define CLASSNAME L"InputDialog"

static HBRUSH hbrBkgnd = NULL;

HFONT m_hFont = NULL;
HWND  m_hWndInputBox = NULL;
HWND  m_hWndEdit = NULL;
wchar_t m_String[320];

int width;
int height = 20;
int posX;
int posY;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void initializeDialog(HWND relativeTo, int mode);

// mode = 0 -> popup dialog (for battlefield in fullscreen mode)
// mode = 1 -> overlay (for battlefield in windowed mode)
// mode = 2 -> overlay content (for battlefield in borderless mode)
std::wstring InputDialog::showInputDialog(std::string currentText, HWND relativeTo, int mode) {
	initializeDialog(relativeTo, mode);

	HINSTANCE hInst = GetModuleHandle(NULL);

	WNDCLASSEX wcex;

	if (!GetClassInfoEx(hInst, CLASSNAME, &wcex))
	{
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = (WNDPROC)WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = CLASSNAME;
		wcex.hIconSm = NULL;

		if (RegisterClassEx(&wcex) == 0)
			REPORTERROR;

	}

	RECT clientRect;
	clientRect.left = 0;
	clientRect.top = 0;
	clientRect.right = width;
	clientRect.bottom = height;
	
	DWORD style = mode == 0 ? WS_POPUPWINDOW | WS_CAPTION : WS_POPUP;
	DWORD exStyle = WS_EX_TOPMOST | (mode == 0 ? WS_EX_DLGMODALFRAME : WS_EX_APPWINDOW);
	if (mode == 0)
		AdjustWindowRectEx(&clientRect, style, false, 0);

	m_hWndInputBox = CreateWindowEx(exStyle, CLASSNAME,
		L"BattlefieldChat",
		style,
		posX, posY,
		width, clientRect.bottom - clientRect.top,
		NULL,
		NULL,
		NULL,
		NULL);
	if (m_hWndInputBox == NULL)
	{
		REPORTERROR;
		return std::wstring();
	}

	setTextAlignment(m_hWndEdit, SS_LEFTNOWORDWRAP);
	SetForegroundWindow(m_hWndInputBox);

	USES_CONVERSION;
	wchar_t* currentWText = A2W(currentText.c_str());

	SendMessage(m_hWndEdit, EM_SETSEL, 0, -1);
	SendMessage(m_hWndEdit, EM_REPLACESEL, 0, (LPARAM)currentWText);
	SendMessage(m_hWndEdit, EM_SETSEL, 0, -1);
	SetFocus(m_hWndEdit);

	ShowWindow(m_hWndInputBox, SW_SHOW);
	UpdateWindow(m_hWndInputBox);

	BOOL ret = 0;

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_KEYDOWN)
		{
			if (msg.wParam == VK_ESCAPE)
			{
				SendMessage(m_hWndInputBox, WM_DESTROY, 0, 0);
				ret = 0;
			}
			if (msg.wParam == VK_RETURN)
			{
				int nCount = GetWindowTextLength(m_hWndEdit);
				nCount++;
				GetWindowText(m_hWndEdit, m_String, nCount);
				SendMessage(m_hWndInputBox, WM_DESTROY, 0, 0);
				ret = 1;
			}
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return ret ? std::wstring(m_String) : std::wstring();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HINSTANCE m_hInst = NULL;
	switch (message)
	{
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		if (hbrBkgnd == NULL)
		{
			hbrBkgnd = CreateSolidBrush(RGB(255, 255, 255));
		}
		SetTextColor(hdcStatic, RGB(0, 0, 0));
		SetBkColor(hdcStatic, RGB(255, 255, 255));

		return (INT_PTR)hbrBkgnd;
	}
	break;
	case WM_CREATE:
		LOGFONT lfont;
		memset(&lfont, 0, sizeof(lfont));
		lstrcpy(lfont.lfFaceName, L"ËÎÌו");
		lfont.lfHeight = 16;
		lfont.lfWeight = FW_NORMAL;
		lfont.lfItalic = FALSE;
		lfont.lfCharSet = DEFAULT_CHARSET;
		lfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lfont.lfQuality = DEFAULT_QUALITY;
		lfont.lfPitchAndFamily = DEFAULT_PITCH;
		m_hFont = CreateFontIndirect(&lfont);
		m_hInst = GetModuleHandle(NULL);

		m_hWndEdit = CreateWindowEx(WS_EX_STATICEDGE,
			L"edit", L"",
			WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOHSCROLL,
			0, 0, width, height,
			hWnd,
			NULL,
			m_hInst,
			NULL);

		if (m_hWndEdit == NULL)
		{
			REPORTERROR;
			return NULL;
		}

		SendMessage(m_hWndEdit, WM_SETFONT, (WPARAM)m_hFont, 0);
		SendMessage(m_hWndEdit, EM_LIMITTEXT, 90, 0);

		SetFocus(m_hWndEdit);
		break;
	case WM_DESTROY:
		DestroyWindow(hWnd);
		PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		if (wParam == WA_INACTIVE) {
			SendMessage(m_hWndInputBox, WM_DESTROY, 0, 0);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void initializeDialog(HWND relativeTo, int mode)
{
	if (mode == 0) { // fullscreen
		RECT r;
		if (!GetWindowRect(GetDesktopWindow(), &r))
			throw "Cannot get window rect";

		width = 320;
		posX = (r.right - 60) / 2;
		posY = (r.bottom - 320) / 2;
	}
	else {
		RECT r;
		if (!GetWindowRect(relativeTo, &r))
			throw "Cannot get window rect";

		if (mode == 1) { // windowed 
			width = r.right - r.left - 24;
			posX = r.left + 12;
			posY = r.top + getSystemTitleHeight() + 12 ;
		}
		else if (mode == 2) { // borderless
			width = r.right - r.left - 24;
			posX = r.left + 12;
			posY = r.top + 12;
		}
		else {
			throw "Invalid mode";
		}
	}
}
