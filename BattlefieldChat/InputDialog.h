#include <string>
#include <Windows.h>

#pragma once
class InputDialog
{
public:
	std::wstring showInputDialog(std::string currentText, HWND relativeTo, int mode);
};

