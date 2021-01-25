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
    cout << " [*] 正在初始化..." << endl;
    messageCaveAddr = (uintptr_t)VirtualAllocEx(hProcess, NULL, sizeof(char) * 91, MEM_COMMIT, PAGE_READWRITE);
    cout << " [+] 预分配内存成功: 0x" << hex << messageCaveAddr << endl;
    cout.flags(f);

    if (!loadNtDll()) {
        cout << " [-] 警告: NtDll 加载失败, 可能会导致意料之外的游戏崩溃" << endl;
    }

    Pointer messageCavePtr(hProcess, moduleBaseAddr);
    messageCavePtr.pointer = messageCaveAddr;
    ChatOpenPointer chatOpenPtr(hProcess, moduleBaseAddr);
    ChatLengthPointer chatLengthPtr(hProcess, moduleBaseAddr);
    ChatMessagePointer chatMessagePtr(hProcess, moduleBaseAddr);

    cout << " [+] Done! 在游戏中打开聊天即可自动呼出输入框" << endl;

    InputDialog dialog;
    bool lastState = false;
    while (IsWindow(gameWindow)) {
        if (chatOpenPtr.refreshPointer()) {
            bool state = chatOpenPtr.readBoolean();
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

                if (!chatLengthPtr.refreshPointer()) {
                    cout << " [-] 错误: 刷新指针失败 [ChatLength]" << endl;
                    goto outer;
                }

                if (!chatMessagePtr.refreshPointer()) {
                    cout << " [-] 错误: 刷新指针失败 [ChatMessage]" << endl;
                    goto outer;
                }

                uintptr_t oldAddr = chatMessagePtr.readAddress();
                if (oldAddr == 0) {
                    cout << " [-] 错误: 读取指针失败 [ChatMessage]" << endl;
                    goto outer;
                }

                // Suspend the process to avoid desynchronized memory access
                if (NtSuspendProcess != NULL)
                    NtSuspendProcess(hProcess);
                if (!messageCavePtr.writeString(converted)) {
                    cout << " [-] 错误: 写入数据失败 [ChatMessage]" << endl;
                    goto resume;
                }

                if (!chatMessagePtr.writeAddress(messageCaveAddr)) {
                    cout << " [-] 错误: 写入指针失败 [ChatMessage]" << endl;
                    goto resume;
                }

                // It may be possible to exceed the chat message length limit? But shouldn't do so, at risk of being banned
                if (!chatLengthPtr.writeAddress(messageCaveAddr + length)) {
                    cout << " [-] 错误: 写入数据失败 [ChatLength]" << endl;
                    goto resume;
                }
                // Resume the process to perform the send operation
                if (NtResumeProcess != NULL)
                    NtResumeProcess(hProcess);

                cout << " [+] 写入消息数据成功" << endl;
                press(VK_RETURN, 20);
                cout << " [+] 模拟发送完成" << endl;
                Sleep(100);
                // Then suspend the process again and restore the pointer
                if (NtSuspendProcess != NULL)
                    NtSuspendProcess(hProcess);
            resume:
                if (!chatMessagePtr.writeAddress(oldAddr)) {
                    cout << " [-] 错误: 恢复指针失败 [ChatMessage]" << endl;
                }
                if (!chatLengthPtr.writeAddress(oldAddr)) {
                    cout << " [-] 错误: 恢复指针失败 [ChatLength]" << endl;
                }
                if (NtResumeProcess != NULL)
                    NtResumeProcess(hProcess);
                // Everything done.
            }
            lastState = state;
        }
    outer:
        Sleep(200);
    }
    VirtualFreeEx(hProcess, (LPVOID)messageCaveAddr, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    freeNtDll();
    cout << endl << " [*] 游戏已退出, Thanks for using!" << endl;
    Sleep(3000);
    return 0;
}