#pragma once
#include <string>
#include <cstdint>
#include <Windows.h>

class Pointer {
public:
    uintptr_t pointer = 0;

    bool refreshPointer();

    bool readBoolean();

    uintptr_t readAddress();
    bool writeAddress(uintptr_t address);

    bool writeStringPtr(std::string data);
    bool writeString(std::string data);

    Pointer(HANDLE pProcessHandle, uintptr_t pBaseModuleAddress);
protected: 
    HANDLE hProcess;
    uintptr_t baseModuleAddress;

    bool offset(uintptr_t offset);
};

