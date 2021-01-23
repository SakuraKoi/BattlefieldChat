#include <iostream>
#include <string>
#include <Windows.h>
#include <tlhelp32.h>
#include "InputDialog.h"
#include "Offsets.h"
#include "Utils.h"

using namespace std;

DWORD pid = -1;
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
    cout << " [*] 正在预分配内存..." << endl;
    // This memory should not be freed, otherwise the game will crash (I'm too lazy to restore the pointer LOL)
    // Possible memory leaks, but the size is small and therefore not a big problem
    messageCaveAddr = (uintptr_t)VirtualAllocEx(hProcess, NULL, sizeof(char) * 91, MEM_COMMIT, PAGE_READWRITE);
    cout << " [+] 申请内存成功: 0x" << hex << messageCaveAddr << endl;
    cout.flags(f);

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
            // Convert Simplified Chinese std::wstring to Traditional Chinese std::string
            wstring trad = CHS2CHT(str);
            string converted = WStrToStr(trad);

            int length = (converted.size() / sizeof(char));

            if (length > 90) {
                press(VK_ESCAPE, 20);
                MessageBox(NULL, L"聊天消息长度不能超过90字节", L"错误", 0);
                cout << " [-] 聊天消息长度超过90字节" << endl;
                goto outer;
            }

            if (!writeBattlefieldChatMessage(converted)) {
                cout << " [-] 写入消息数据失败" << endl;
                goto outer;
            }

            if (!writeBattlefieldChatPointer()) {
                cout << " [-] 重定向消息指针失败" << endl;
                goto outer;
            }

            // It may be possible to exceed the chat message length limit? But shouldn't do so, at risk of being banned
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
    CloseHandle(hProcess);
    cout << endl << " [*] 游戏已退出, Thanks for using!" << endl;
    Sleep(3000);
    return 0;
}