#include "GlobalVariables.h"

bool allowExceedLimit = false;
bool fullscreenSupport = false;

DWORD pid = -1;
HWND gameWindow;

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