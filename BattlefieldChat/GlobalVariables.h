#pragma once
#include <Windows.h>
#include "Preprocessor.h"
#include "BattlefieldChat.h"
#include "InputDialog.h"

extern bool allowExceedLimit;
extern bool fullscreenSupport;

extern DWORD pid = -1;
extern HWND gameWindow;

extern Preprocessor preprocessor;

extern BattlefieldChat* mainWindow;
extern InputDialog* inputWindow;
