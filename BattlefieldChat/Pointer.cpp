#include "Pointer.h"

bool Pointer::refreshPointer() {
    return false;
}

bool Pointer::readBoolean() {
    bool value;
    if (ReadProcessMemory(hProcess, (LPCVOID)pointer, &value, sizeof(value), 0))
        return value;
    return false;
}

uintptr_t Pointer::readAddress(uintptr_t offset) {
    uintptr_t value;
    if (ReadProcessMemory(hProcess, (LPCVOID)pointer + offset, &value, sizeof(value), 0))
        return value;
    return 0;
}

bool Pointer::writeAddress(uintptr_t offset, uintptr_t address) {
    uintptr_t final_address = pointer + offset;
    SIZE_T size = sizeof(address);
    DWORD oldprotect;
    if (!VirtualProtectEx(hProcess, (LPVOID)final_address, size, PAGE_EXECUTE_READWRITE, &oldprotect))
        return false;
    if (!WriteProcessMemory(hProcess, (LPVOID)final_address, (LPCVOID)&address, size, nullptr))
        return false;
    if (!VirtualProtectEx(hProcess, (LPVOID)final_address, size, oldprotect, &oldprotect))
        return false;
    return true;
}

bool Pointer::writeStringPtr(std::string data) {
    uintptr_t address = readAddress();
    if (address == 0) throw 1;
    SIZE_T size = data.size();
    DWORD oldprotect;
    if (!VirtualProtectEx(hProcess, (LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldprotect))
        return false;
    if (!WriteProcessMemory(hProcess, (LPVOID)address, data.c_str(), size, nullptr))
        return false;
    if (!VirtualProtectEx(hProcess, (LPVOID)address, size, oldprotect, &oldprotect))
        return false;
    return true;
}

bool Pointer::writeString(std::string data) {
    SIZE_T size = data.size();
    DWORD oldprotect;
    if (!VirtualProtectEx(hProcess, (LPVOID)pointer, size, PAGE_EXECUTE_READWRITE, &oldprotect))
        return false;
    if (!WriteProcessMemory(hProcess, (LPVOID)pointer, data.c_str(), size, nullptr))
        return false;
    if (!VirtualProtectEx(hProcess, (LPVOID)pointer, size, oldprotect, &oldprotect))
        return false;
    return true;
}

Pointer::Pointer(HANDLE pProcessHandle, uintptr_t pBaseModuleAddress) {
    hProcess = pProcessHandle;
    baseModuleAddress = pBaseModuleAddress;
}

bool Pointer::offset(uintptr_t offset) {
    pointer += offset;
    return (pointer = readAddress()) != 0;
}