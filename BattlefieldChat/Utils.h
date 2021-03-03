#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#pragma once

#define REPORTERROR ReportError(__FUNCTION__)

int getSystemTitleHeight();

bool isFullscreenWindow(HWND window);
bool isBorderlessWindow(HWND window);

uintptr_t getModuleBaseAddress(DWORD processId, const wchar_t* modName);

std::wstring CHS2CHT(std::wstring sCht);
std::string WStrToStr(std::wstring src);
std::wstring StrToWStr(std::string src);
std::wstring ReplaceWCSWithPattern(__in const std::wstring& message, __in const std::wstring& pattern, __in const std::wstring& replace);

void press(BYTE key, int delay);

void blurWindow(HWND window);

typedef LONG(NTAPI* _NtSuspendProcess) (IN HANDLE ProcessHandle);
typedef LONG(NTAPI* _NtResumeProcess) (IN HANDLE ProcessHandle);
extern _NtSuspendProcess NtSuspendProcess;
extern _NtResumeProcess NtResumeProcess;

bool loadNtDll();
void freeNtDll();