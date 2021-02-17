#pragma once
#include <Windows.h>
#include <QNetworkAccessManager>
#include "Preprocessor.h"
#include "BattlefieldChat.h"
#include "InputDialog.h"
#include "WorkerThread.h"

extern bool allowExceedLimit;
extern bool fullscreenSupport;

extern DWORD pid;
extern HWND gameWindow;

extern Preprocessor SINGLETON_PREPROCESSOR_NOP;
extern TraditionalChinesePreprocessor SINGLETON_PREPROCESSOR_TRAD;
extern PinyinPreprocessor SINGLETON_PREPROCESSOR_PINYIN;
extern TranslateToEnglishPreprocessor SINGLETON_PREPROCESSOR_ENGLISH;

extern Preprocessor* preprocessor;

extern BattlefieldChat* mainWindow;
extern InputDialog* inputWindow;
extern WorkerThread* workerThread;

extern QNetworkAccessManager* network;