#include "WorkerThread.h"

WorkerThread::WorkerThread(QObject *parent)
    : QThread(parent)
{
}

WorkerThread::~WorkerThread()
{
}

void WorkerThread::run() {
    // FIXME: Migrating to Qt
    cout
        << " Battlefield 1 中文输入工具" << endl
        << " Powered by.SakuraKooi (https://github.com/SakuraKoi/BattlefieldChat)" << endl
        << endl
        << " 警告: 尽管Fairfight不检测聊天区域的内存数据, 但仍然可能存在一定的风险" << endl
        << "       USE AT YOUR OWN RISK, 作者不对工具造成的任何损失承担任何责任" << endl
        << endl
        << " 注意: 游戏需要运行在无边框或窗口模式" << endl
        << endl;

    cout << " [*] 正在等待游戏启动..." << endl;
    while (!isInterruptionRequested()) {
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
    messageCaveAddr = (uintptr_t)VirtualAllocEx(hProcess, NULL, sizeof(char) * ((INPUT_BUFFER_SIZE + 1) * 3), MEM_COMMIT, PAGE_READWRITE);
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
    dialog.callbackValidateInput = validateInputLength;
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
                wstring replaced = replaceNonDisplayableCharacters(str);
                // Convert Simplified Chinese std::wstring to Traditional Chinese std::string
                wstring trad = CHS2CHT(replaced);
                string converted = WStrToStr(trad);

                int length = (converted.size() / sizeof(char));

                if (length > 90) {
                    if (allowExceedLimit) {
                        cout << " [!] 消息长度超过90字节, 工具可以绕过这个限制并将继续发送, 但这可能带来额外的ff风险" << endl;
                    } else {
                        press(VK_ESCAPE, 20);
                        cout << " [x] 消息长度超过90字节" << endl;
                        MessageBox(NULL, L"聊天消息长度超过游戏限制 (90字节 / 30中文)\n\n您可以通过添加 -BypassLimit 参数来绕过这个限制\n但这可能带来额外的FF风险", L"错误", 0);
                        goto outer;
                    }
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
                    goto outer;
                }

                if (!chatMessagePtr.writeAddress(messageCaveAddr)) {
                    cout << " [-] 错误: 写入指针失败 [ChatMessage]" << endl;
                    goto resume;
                }

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

                // Loop to wait for the game to clear the string
                {
                    int count = 0;
                    while (count++ <= 10) { // wait for 200ms max
                        if (!messageCavePtr.readBoolean())
                            break;
                        Sleep(20);
                    }
                }

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
                cout << " [+] 恢复指针完成" << endl;
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
}