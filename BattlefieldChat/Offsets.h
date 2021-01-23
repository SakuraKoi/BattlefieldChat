#pragma once
#include <string>
#include <Windows.h>

extern HANDLE hProcess;
extern uintptr_t moduleBaseAddr;
extern uintptr_t messageCaveAddr;

bool isBattlefieldChatOpen();
bool writeBattlefieldChatLength(int length);
bool writeBattlefieldChatMessage(std::string str);
bool writeBattlefieldChatPointer();