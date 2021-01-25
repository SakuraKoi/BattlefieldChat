#pragma once
#include "Pointer.h"

extern HANDLE hProcess;
extern uintptr_t moduleBaseAddr;
extern uintptr_t messageCaveAddr;

class ChatOpenPointer : public Pointer {
public:
    ChatOpenPointer(HANDLE pProcessHandle, uintptr_t pBaseModuleAddress) : Pointer(pProcessHandle, pBaseModuleAddress) {}
    bool refreshPointer();
};

class ChatLengthPointer : public Pointer {
public:
    ChatLengthPointer(HANDLE pProcessHandle, uintptr_t pBaseModuleAddress) : Pointer(pProcessHandle, pBaseModuleAddress) {}
    bool refreshPointer();
};

class ChatMessagePointer : public Pointer {
public:
    ChatMessagePointer(HANDLE pProcessHandle, uintptr_t pBaseModuleAddress) : Pointer(pProcessHandle, pBaseModuleAddress) {}
    bool refreshPointer();
};