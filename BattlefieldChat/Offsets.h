#pragma once
#include "Pointer.h"

extern HANDLE hProcess;
extern uintptr_t moduleBaseAddr;
extern uintptr_t messageCaveAddr;

class ChatOpenPointer : public Pointer {
public:
    ChatOpenPointer() : Pointer(hProcess, moduleBaseAddr) {}
    bool refreshPointer();
};

class ChatLengthPointer : public Pointer {
public:
    ChatLengthPointer() : Pointer(hProcess, moduleBaseAddr) {}
    bool refreshPointer();
};

class ChatMessagePointer : public Pointer {
public:
    ChatMessagePointer() : Pointer(hProcess, moduleBaseAddr) {}
    bool refreshPointer();
};