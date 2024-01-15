#include <iostream>
#include <locale>
#include <codecvt>
#include "Utils.h"

int getSystemTitleHeight() {
    return (GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));
}

bool isFullscreenWindow(HWND window) {
    QUERY_USER_NOTIFICATION_STATE state;
    SHQueryUserNotificationState(&state);
    return state == QUNS_RUNNING_D3D_FULL_SCREEN;
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

    int bufferSize = iNewLen + 2;
    wchar_t* sDst = new wchar_t[bufferSize];
    if (iNewLen > 0) {
        LCMapString(lcidSrc, LCMAP_TRADITIONAL_CHINESE, sCht.c_str(), sCht.length(), sDst, iNewLen);
    }
    sDst[iNewLen] = '\0';
    
    std::wstring out = std::wstring(sDst);
    delete[] sDst;
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

enum AccentState {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_INVALID_STATE = 4
};
struct AccentPolicy {
    AccentState AccentState;
    int AccentFlags;
    int GradientColor;
    int AnimationId;
};
enum WindowCompositionAttribute {
    WCA_UNDEFINED = 0,
    WCA_NCRENDERING_ENABLED = 1,
    WCA_NCRENDERING_POLICY = 2,
    WCA_TRANSITIONS_FORCEDISABLED = 3,
    WCA_ALLOW_NCPAINT = 4,
    WCA_CAPTION_BUTTON_BOUNDS = 5,
    WCA_NONCLIENT_RTL_LAYOUT = 6,
    WCA_FORCE_ICONIC_REPRESENTATION = 7,
    WCA_EXTENDED_FRAME_BOUNDS = 8,
    WCA_HAS_ICONIC_BITMAP = 9,
    WCA_THEME_ATTRIBUTES = 10,
    WCA_NCRENDERING_EXILED = 11,
    WCA_NCADORNMENTINFO = 12,
    WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
    WCA_VIDEO_OVERLAY_ACTIVE = 14,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
    WCA_DISALLOW_PEEK = 16,
    WCA_CLOAK = 17,
    WCA_CLOAKED = 18,
    WCA_ACCENT_POLICY = 19,
    WCA_FREEZE_REPRESENTATION = 20,
    WCA_EVER_UNCLOAKED = 21,
    WCA_VISUAL_OWNER = 22,
    WCA_LAST = 23
};
struct WindowCompositionAttributeData {
    WindowCompositionAttribute Attribute;
    int* Data;
    int SizeOfData;
};

typedef int* (*_SetWindowCompositionAttribute)(HWND hwnd, WindowCompositionAttributeData* data);
void blurWindow(HWND window) {
    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (hUser) {
        _SetWindowCompositionAttribute setWindowCompositionAttribute = (_SetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        if (setWindowCompositionAttribute) {
            AccentPolicy accent = { ACCENT_ENABLE_BLURBEHIND,0, 0, 0 };
            WindowCompositionAttributeData data;
            data.Attribute = WCA_ACCENT_POLICY;
            data.Data = reinterpret_cast<int*>(&accent);
            data.SizeOfData = sizeof(accent);
            setWindowCompositionAttribute(window, &data);
        }
    }
}