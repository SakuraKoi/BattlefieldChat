#pragma once
#include "Pointer.h"

extern HANDLE hProcess;
extern uintptr_t moduleBaseAddr;
extern uintptr_t messageCaveAddr;

extern const intptr_t OFFSET_CHAT_MESSAGE_ADDRESS_START;
extern const intptr_t OFFSET_CHAT_MESSAGE_ADDRESS_END;

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