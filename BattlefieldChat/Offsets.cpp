#include "Offsets.h"
#include "Utils.h"

HANDLE hProcess;
uintptr_t moduleBaseAddr;
uintptr_t messageCaveAddr;

bool isBattlefieldChatOpen() {
    uintptr_t ptr = readPointer(hProcess, moduleBaseAddr, 0x39f1e50);
    if (ptr == 0) return false;

    ptr = readPointer(hProcess, ptr, 0x8);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x28);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x0);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x20);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x18);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x28);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x38);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x40);
    if (ptr == 0) return false;
    unsigned char value = readByte(hProcess, ptr, 0x30);
    return value == 1;
}


bool writeBattlefieldChatLength(int length) {
    uintptr_t ptr = readPointer(hProcess, moduleBaseAddr, 0x3A2CA60);
    if (ptr == 0) return false;

    ptr = readPointer(hProcess, ptr, 0x20);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x38);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x18);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x10);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x30);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x20);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0xB8);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x10);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x10);
    if (ptr == 0) return false;
    return writeLong(hProcess, ptr, 0x188, messageCaveAddr + length);
}

bool writeBattlefieldChatPointer() {
    uintptr_t ptr = readPointer(hProcess, moduleBaseAddr, 0x3a327e0);
    if (ptr == 0) return false;

    ptr = readPointer(hProcess, ptr, 0x20);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x18);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x38);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x8);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x68);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0xb8);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x10);
    if (ptr == 0) return false;
    ptr = readPointer(hProcess, ptr, 0x10);
    if (ptr == 0) return false;
    return writePointer(hProcess, ptr, 0x180, messageCaveAddr);
}

bool writeBattlefieldChatMessage(std::string str) {
    return writeString(hProcess, messageCaveAddr, str.c_str(), str.size());
}
