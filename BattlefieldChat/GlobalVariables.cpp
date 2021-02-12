#include "GlobalVariables.h"

bool allowExceedLimit = false;
bool fullscreenSupport = false;

DWORD pid = -1;
HWND gameWindow;

Preprocessor preprocessor = TraditionalChinesePreprocessor();

BattlefieldChat* mainWindow;
InputDialog* inputWindow;