#include "Offsets.h"

HANDLE hProcess;
uintptr_t moduleBaseAddr;
uintptr_t messageCaveAddr;

const intptr_t OFFSET_CHAT_MESSAGE_ADDRESS_START = 0x180;
const intptr_t OFFSET_CHAT_MESSAGE_ADDRESS_END = 0x188;

bool ChatOpenPointer::refreshPointer() {
    pointer = baseModuleAddress;
    if (!offset(0x39f1e50)) return false;
    if (!offset(0x8)) return false;
    if (!offset(0x28)) return false;
    if (!offset(0x0)) return false;
    if (!offset(0x20)) return false;
    if (!offset(0x18)) return false;
    if (!offset(0x28)) return false;
    if (!offset(0x38)) return false;
    if (!offset(0x40)) return false;
    pointer += 0x30;
    return true;
}

bool ChatMessagePointer::refreshPointer() {
    pointer = baseModuleAddress;
    if (!offset(0x3a327e0)) return false;
    if (!offset(0x20)) return false;
    if (!offset(0x18)) return false;
    if (!offset(0x38)) return false;
    if (!offset(0x8)) return false;
    if (!offset(0x68)) return false;
    if (!offset(0xB8)) return false;
    if (!offset(0x10)) return false;
    if (!offset(0x10)) return false;
    return true;
}