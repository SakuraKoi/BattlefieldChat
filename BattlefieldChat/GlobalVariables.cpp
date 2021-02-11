#include "GlobalVariables.h"
#include "Preprocessor.h"

bool allowExceedLimit = false;
DWORD pid = -1;
HWND gameWindow;
Preprocessor preprocessor = TraditionalChinesePreprocessor();