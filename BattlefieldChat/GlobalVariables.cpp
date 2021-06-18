﻿#include "GlobalVariables.h"

QString currentVersion = "8";
QString addtionalVersion = "";

bool allowExceedLimit = false;
bool fullscreenSupport = false;
bool translateKeepOriginal = false;

int translatorTimeout = 5000L;

DWORD pid = -1;
HWND gameWindow;

UpdateCheckerTask* updateCheckerTask;
CountLoaderTask* countLoaderTask;

Preprocessor SINGLETON_PREPROCESSOR_NOP;
TraditionalChinesePreprocessor SINGLETON_PREPROCESSOR_TRAD;
PinyinPreprocessor SINGLETON_PREPROCESSOR_PINYIN;
TranslateToEnglishPreprocessor SINGLETON_PREPROCESSOR_ENGLISH;

Preprocessor* preprocessor = &SINGLETON_PREPROCESSOR_TRAD;

BattlefieldChat* mainWindow;
InputDialog* inputWindow;
WorkerThread* workerThread;

QNetworkAccessManager* network;

QSettings* settings = new QSettings(QSettings::NativeFormat, QSettings::UserScope, "ArukasNetwork", "BattlefieldChat");