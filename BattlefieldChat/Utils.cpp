#include <iostream>
#include <locale>
#include <codecvt>
#include "Utils.h"

void setTextAlignment(HWND hwnd, int intTextAlignment) {
    LONG_PTR s;
    LONG_PTR textalignment = GetWindowLongPtr(hwnd, GWL_STYLE);
    if (textalignment != intTextAlignment) {
        if (intTextAlignment == 0) {
            s = GetWindowLongPtr(hwnd, GWL_STYLE);
            s = s & ~(SS_LEFT);
            SetWindowLongPtr(hwnd, GWL_STYLE, (LONG_PTR)s);
        }
        else if (intTextAlignment == 1) {
            s = GetWindowLongPtr(hwnd, GWL_STYLE);
            s = s & ~(SS_CENTER);
            SetWindowLongPtr(hwnd, GWL_STYLE, (LONG_PTR)s);
        }
        else if (intTextAlignment == 2) {
            s = GetWindowLongPtr(hwnd, GWL_STYLE);
            s = s & ~(SS_RIGHT);
            SetWindowLongPtr(hwnd, GWL_STYLE, (LONG_PTR)s);
        }

        textalignment = intTextAlignment;

        if (textalignment == 0) {
            s = GetWindowLongPtr(hwnd, GWL_STYLE);
            s = s | (SS_LEFT);
            SetWindowLongPtr(hwnd, GWL_STYLE, (LONG_PTR)s);
        }
        else if (textalignment == 1) {
            s = GetWindowLongPtr(hwnd, GWL_STYLE);
            s = s | (SS_CENTER);
            SetWindowLongPtr(hwnd, GWL_STYLE, (LONG_PTR)s);
        }
        else if (textalignment == 2) {
            s = GetWindowLongPtr(hwnd, GWL_STYLE);
            s = s | (SS_RIGHT);
            SetWindowLongPtr(hwnd, GWL_STYLE, (LONG_PTR)s);
        }
        SetWindowPos(hwnd, 0, 0, 0, 0, 0,
            SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_DRAWFRAME);
    }
}

void ReportError(const char* CallingFunction) {
    DWORD error = GetLastError();
    LPVOID lpMsgBuf;
    DWORD bufLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf,
        0, NULL);

    std::cout << "Error in " << CallingFunction << " : " << (wchar_t*)lpMsgBuf;
}

int getSystemTitleHeight() {
    return (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));
}

bool isFullscreenWindow(HWND window) {
    return window == GetDesktopWindow() || window == GetShellWindow();
}

bool isBorderlessWindow(HWND window) {
    RECT windowRect, clientRect;
    GetWindowRect(window, &windowRect);
    GetClientRect(window, &clientRect);

    return windowRect.left == clientRect.left
        && windowRect.right == clientRect.right
        && windowRect.top == clientRect.top
        && windowRect.bottom == clientRect.bottom;
}

uintptr_t getModuleBaseAddress(DWORD processId, const wchar_t* modName) {
    uintptr_t modBaseAddr = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hSnapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnapshot, &modEntry)) {
            do {
                if (_wcsicmp(modEntry.szModule, modName) == 0) {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnapshot, &modEntry));
        }
    }
    CloseHandle(hSnapshot);
    return modBaseAddr;
}

std::wstring CHS2CHT(std::wstring sCht) {
    LCID lcidSrc = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
    const int iNewLen = LCMapString(lcidSrc, LCMAP_TRADITIONAL_CHINESE, sCht.c_str(), sCht.length(), NULL, 0);

    wchar_t sDst[91];
    if (iNewLen > 0) {
        LCMapString(lcidSrc, LCMAP_TRADITIONAL_CHINESE, sCht.c_str(), sCht.length(), sDst, iNewLen);
    }
    sDst[iNewLen] = '\0';

    std::wstring out = std::wstring(sDst);
    return out;
}

std::string WStrToStr(std::wstring src) {
    int num_chars = WideCharToMultiByte(CP_UTF8, 0, src.c_str(), src.length(), NULL, 0, NULL, NULL);
    std::string strTo;
    if (num_chars > 0) {
        strTo.resize(num_chars);
        WideCharToMultiByte(CP_UTF8, 0, src.c_str(), src.length(), &strTo[0], num_chars, NULL, NULL);
    }
    return strTo;
}

std::wstring StrToWStr(std::string src) {
    int num_chars = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), src.length(), NULL, 0);
    std::wstring wstrTo;
    if (num_chars) {
        wstrTo.resize(num_chars);
        MultiByteToWideChar(CP_UTF8, 0, src.c_str(), src.length(), &wstrTo[0], num_chars);
    }
    return wstrTo;
}

std::wstring ReplaceWCSWithPattern(__in const std::wstring& message, __in const std::wstring& pattern, __in const std::wstring& replace) {
    std::wstring result = message;
    std::wstring::size_type pos = 0;
    std::wstring::size_type offset = 0;

    while ((pos = result.find(pattern, offset)) != std::wstring::npos) {
        result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
        offset = pos + replace.size();
    }

    return result;
}

void press(BYTE key, int delay) {
    Sleep(delay);
    keybd_event(key, MapVirtualKey(key, 0U), 0, 0);
    Sleep(delay);
    keybd_event(key, MapVirtualKey(key, 0U), KEYEVENTF_KEYUP, 0);
}

HINSTANCE hNtDll;
_NtSuspendProcess NtSuspendProcess = NULL;
_NtResumeProcess NtResumeProcess = NULL;

bool loadNtDll() {
    hNtDll = LoadLibrary(L"ntdll.dll");
    if (hNtDll == NULL) return false;
    NtSuspendProcess = (_NtSuspendProcess)GetProcAddress(hNtDll, "NtSuspendProcess");
    NtResumeProcess = (_NtResumeProcess)GetProcAddress(hNtDll, "NtResumeProcess");
    return true;
}

void freeNtDll() {
    FreeLibrary(hNtDll);
}