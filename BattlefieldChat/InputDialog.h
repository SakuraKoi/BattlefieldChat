#include <string>
#include <Windows.h>

#define INPUT_BUFFER_SIZE 256

#pragma once
class InputDialog {
public:
    std::wstring showInputDialog(std::wstring currentText, HWND relativeTo, int mode);

    typedef bool(*_CallbackValidateInput) (IN std::wstring input);
    _CallbackValidateInput callbackValidateInput;
};

