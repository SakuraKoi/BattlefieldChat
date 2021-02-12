#pragma once
#include <Windows.h>
#include "Preprocessor.h"
#include "BattlefieldChat.h"
#include "InputDialog.h"
#include "WorkerThread.h"

extern bool allowExceedLimit;
extern bool fullscreenSupport;

extern DWORD pid;
extern HWND gameWindow;

extern Preprocessor preprocessor;

extern BattlefieldChat* mainWindow;
extern InputDialog* inputWindow;
extern WorkerThread* workerThread;
