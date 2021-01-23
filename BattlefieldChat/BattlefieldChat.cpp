#include <iostream>
#include <string>
#include <Windows.h>
#include <tlhelp32.h>
#include "InputDialog.h"
#include "Utils.h"

using namespace std;

bool isBattlefieldChatOpen();
bool writeBattlefieldChatLength(int length);
bool writeBattlefieldChatMessage(string str);
void press(BYTE key, int delay);

DWORD pid = -1;
HANDLE hProcess;
uintptr_t moduleBaseAddr;
HWND gameWindow;

int main() {
    SetConsoleTitle(L"Battlefield 1 中文输入工具");
    cout
        << " Battlefield 1 中文输入工具" << endl
        << " Powered by.SakuraKooi (https://github.com/SakuraKoi/BattlefieldChat)" << endl
        << endl
        << " 警告: 本工具直接读写游戏内存, 存在一定的风险" << endl
        << "       USE AT YOUR OWN RISK, 作者不对损失承担责任" << endl
        << endl
        << " 注意: 游戏需要运行在无边框或窗口模式" << endl
        << endl;

    cout << " [*] 正在等待游戏启动..." << endl;
    while (true) {
        gameWindow = FindWindow(nullptr, L"Battlefield™ 1");
        if (gameWindow != 0) {
            GetWindowThreadProcessId(gameWindow, &pid);
            moduleBaseAddr = getModuleBaseAddress(pid, L"bf1.exe");
            if (pid != -1)
                if (moduleBaseAddr != 0)
                    break;
        }
        Sleep(1000);
    }

    ios::fmtflags f(cout.flags());
    cout << " [+] bf1.exe -> pid = " << pid << " 0x" << hex << moduleBaseAddr << endl;
    cout.flags(f);

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    cout << " [+] Done! 在游戏中打开聊天即可自动呼出输入框" << endl;

    InputDialog dialog;
    bool lastState = false;
    while (IsWindow(gameWindow)) {
        bool state = isBattlefieldChatOpen();
        if (!isFullscreenWindow(gameWindow) && !lastState && state) {
            cout << endl << " [+] 检测到聊天框打开" << endl;
            wstring str = dialog.showInputDialog(L"", gameWindow, isBorderlessWindow(gameWindow) ? 2 : 1);

            SetForegroundWindow(gameWindow);
            if (str.length() == 0) {
                press(VK_ESCAPE, 20);
                cout << " [-] 取消输入操作" << endl;
                goto outer;
            }
            wstring trad = CHS2CHT(str);
            string converted = WStrToStr(trad);

            int length = (converted.size() / sizeof(char));

            if (length > 90) {
                press(VK_ESCAPE, 20);
                MessageBox(NULL, L"聊天消息长度不能超过90字节", L"错误", 0);
                cout << " [-] 聊天消息长度超过90字节" << endl;
                goto outer;
            }

            // NESSARY: Type something to let battlefield initialize memory
            press(VK_SPACE, 20);

            if (!writeBattlefieldChatMessage(converted)) {
                cout << " [-] 写入消息数据失败" << endl;
                goto outer;
            }

            if (!writeBattlefieldChatLength(length)) {
                cout << " [-] 写入消息长度失败" << endl;
                goto outer;
            }

            cout << " [+] 写入消息数据成功" << endl;
            press(VK_RETURN, 20);
            cout << " [+] 模拟发送完成" << endl;
        }
    outer:
        lastState = state;
        Sleep(200);
    }
    cout << endl << " [*] 游戏已退出, Thanks for using!" << endl;
    CloseHandle(hProcess);
    Sleep(3000);
    return 0;
}

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
    int base = readInt(hProcess, ptr, 0x180);
    return writeInt(hProcess, ptr, 0x188, base + length);
}

bool writeBattlefieldChatMessage(string str) {
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
    ptr = readPointer(hProcess, ptr, 0x180);
    if (ptr == 0) return false;
    return writeString(hProcess, ptr, str.c_str(), str.size());
}

void press(BYTE key, int delay) {
    Sleep(delay);
    keybd_event(key, MapVirtualKey(key, 0U), 0, 0);
    Sleep(delay);
    keybd_event(key, MapVirtualKey(key, 0U), KEYEVENTF_KEYUP, 0);
}