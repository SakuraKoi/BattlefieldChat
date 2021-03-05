#pragma once
#include <Windows.h>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QSettings>
#include "Preprocessor.h"
#include "BattlefieldChat.h"
#include "InputDialog.h"
#include "WorkerThread.h"

extern QString currentVersion;

extern bool allowExceedLimit;
extern bool fullscreenSupport;
extern bool translateKeepOriginal;

extern int translatorTimeout;

extern DWORD pid;
extern HWND gameWindow;

extern Preprocessor SINGLETON_PREPROCESSOR_NOP;
extern TraditionalChinesePreprocessor SINGLETON_PREPROCESSOR_TRAD;
extern TranslateToEnglishPreprocessor SINGLETON_PREPROCESSOR_ENGLISH;

extern Preprocessor* preprocessor;

extern BattlefieldChat* mainWindow;
extern InputDialog* inputWindow;
extern WorkerThread* workerThread;

extern QNetworkAccessManager* network;

extern QSettings* settings;

#define SETTING_KEY_bypassLimit "allowExceedLimit"
#define SETTING_KEY_fullscreenSupport "fullscreenSupport"
#define SETTING_KEY_translateKeepOriginal "translateKeepOriginal"
#define SETTING_KEY_preprocessorMode "preprocessorMode"

#define SETTING_KEY_proxyEnabled "proxyEnabled"
#define SETTING_KEY_proxyHost "proxyHost"
#define SETTING_KEY_proxyPort "proxyPort"

#define SETTING_KEY_translatorProvider "translatorProvider"
#define SETTING_KEY_translatorTimeout "translatorTimeout"

#define SETTING_KEY_translatorBaiduAppid "translatorBaiduAppid"
#define SETTING_KEY_translatorBaiduKey "translatorBaiduKey"