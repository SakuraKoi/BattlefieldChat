#include <string>
#include <Windows.h>

#pragma once
class InputDialog
{
public:
	std::wstring showInputDialog(std::wstring currentText, HWND relativeTo, int mode);
};

