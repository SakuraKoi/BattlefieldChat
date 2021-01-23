#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#pragma once

#define REPORTERROR ReportError(__FUNCTION__)

void setTextAlignment(HWND hwnd, int textalignment);
void ReportError(const char* CallingFunction);

int getSystemTitleHeight();

bool isFullscreenWindow(HWND window);
bool isBorderlessWindow(HWND window);

uintptr_t getModuleBaseAddress(DWORD processId, const wchar_t* modName);

uintptr_t readPointer(HANDLE hProcess, uintptr_t address, int offset);
unsigned char readByte(HANDLE hProcess, uintptr_t address, int offset);
bool writeString(HANDLE hProcess, uintptr_t address, const char* data, int size);
int readInt(HANDLE hProcess, uintptr_t address, int offset);
bool writeInt(HANDLE hProcess, uintptr_t address, int offset, int data);

std::wstring CHS2CHT(std::wstring sCht);
std::string WStrToStr(std::wstring src);
std::wstring StrToWStr(std::string src);