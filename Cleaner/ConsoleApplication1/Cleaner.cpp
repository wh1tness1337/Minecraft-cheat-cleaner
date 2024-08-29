#include <iostream>
#include <cstdlib>
#include <string>
#include <Windows.h>
#include <tlhelp32.h>
#include <chrono>
#include <thread>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <ShlObj_core.h>
#include <Shlwapi.h>
#include <ctime>


bool createLocalCMPackageFolder() {
    TCHAR localAppData[MAX_PATH];
    if (FAILED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, localAppData))) {
        return false;
    }

    std::wstring packageFolderPath = std::wstring(localAppData) + L"\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\LocalCM";

    if (PathFileExists(packageFolderPath.c_str())) {

        return true;
    }

    if (!CreateDirectory(packageFolderPath.c_str(), NULL)) {

        return false;
    }

    if (!SetFileAttributes(packageFolderPath.c_str(), FILE_ATTRIBUTE_HIDDEN)) {

        return false;
    }
    return true;
}


namespace fs = std::filesystem;
void closeProcesses(const std::vector<std::wstring>& processes) {
    while (true) {
        HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap == INVALID_HANDLE_VALUE) {
            return;
        }
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);
        if (!Process32First(hProcessSnap, &pe32)) {
            CloseHandle(hProcessSnap);
            return;
        }
        do {
            for (const auto& process : processes) {
                if (std::wstring(pe32.szExeFile) == process) {
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                    if (hProcess) {
                        TerminateProcess(hProcess, 0);
                        CloseHandle(hProcess);
                    }
                }
            }
        } while (Process32Next(hProcessSnap, &pe32));

        CloseHandle(hProcessSnap);

        std::this_thread::sleep_for(std::chrono::seconds(1));
 }
}


void WindowSetup() {
    HWND consoleWindow = GetConsoleWindow();
    RECT rect;
    GetWindowRect(consoleWindow, &rect);
    SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_SIZEBOX);
    SetWindowPos(consoleWindow, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_FRAMECHANGED);
    HMENU hMenu = GetSystemMenu(consoleWindow, FALSE);
    EnableMenuItem(hMenu, SC_MAXIMIZE, MF_GRAYED);
    RECT rrect;
    GetWindowRect(consoleWindow, &rrect);
    SetWindowLong(consoleWindow, GWL_STYLE, GetWindowLong(consoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX);
    SetWindowPos(consoleWindow, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_FRAMECHANGED);
    int newWidth = 560;
    int newHeight = 300;
    SetWindowPos(consoleWindow, NULL, 0, 0, newWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER);
}


void do_clear(const std::string& arg) {
    std::string command = "wevtutil.exe cl " + arg;
    system(command.c_str());
}
void Clear() {
    WindowSetup();
    std::string userName;
    char buffer[128];
    DWORD size = sizeof(buffer);
    GetUserNameA(buffer, &size);
    userName = buffer;
    std::string userSid = userName.substr(userName.find("\\") + 1);
    system("CHCP 866");
    system("COLOR A");
    system("CLS");
    std::vector<std::string> eventNames = {
        "AMSI/Debug","AirSpaceChannel","Analytic","Application","DirectShowFilterGraph","DirectShowPluginControl","Microsoft-Windows-Diagnosis-ScriptedDiagnosticsProvider/Operational","Microsoft-Windows-Diagnosis-WDC/Analytic","WINDOWS_MSMPEG2VDEC_CHANNEL","WINDOWS_VC1ENC_CHANNEL","WINDOWS_WMPHOTO_CHANNEL","WINDOWS_wmvdecod_CHANNEL","WMPSetup","WMPSyncEngine","Windows Networking Vpn Plugin Platform/Operational","Windows Networking Vpn Plugin Platform/OperationalVerbose","Windows PowerShell","WordChannel","muxencode",
        
    };

    for (const std::string& eventName : eventNames) {
        do_clear(eventName);
    }
    system("REG DELETE \"HKEY_CURRENT_USER\\Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\MuiCache\" /va /f");
    system("REG DELETE \"HKEY_CURRENT_USER\\Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\BagMRU\" /f");
    system("REG DELETE \"HKEY_CURRENT_USER\\Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\Shell\\Bags\" /f");
    system("REG DELETE \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell\\BagMRU\" /f");
    system("REG DELETE \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell\\Bags\" /f");
    system("REG DELETE \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\RunMRU\" /va /f");
    system("for /f \"tokens = *\" %%A in ('reg query HKEY_USERS ^| findstr /c:\"S - 1 - 5 - 21 - \" ^| findstr /c:\" - 1001\" ^| findstr /v /c:\"_Classes\"') do set \"folder = %%A\"");
    system("REG DELETE \"%folder%\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FeatureUsage\\ShowJumpView\" /f");
    system("REG DELETE \"HKEY_USERS\\%folder%\\Software\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Store\" /f");
    system("REG DELETE \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Store\" /f");
    system("REG DELETE \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\FirstFolder\" /va /f");
    system("REG DELETE \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\LastVisitedPidlMRU\" /va /f");
    system("REG DELETE \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Tracing\" /f");
    system("REG DELETE \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\LastVisitedPidlMRULegacy\" /va /f");
    system("REG DELETE \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\OpenSavePidlMRU\" /f");
    system("REG ADD \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ComDlg32\\OpenSavePidlMRU\"");
    system("REG DELETE \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist\" /f");
    system("REG ADD \"HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\UserAssist\"");
    system("REG DELETE \"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Session Manager\\AppCompatCache\" /va /f");
    system("REG DELETE \"HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Control\\Session Manager\\AppCompatCache\" /va /f");
    system("REG DELETE \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\RADAR\\HeapLeakDetection\\DiagnosedApplications\" /f");
    system("REG ADD \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\RADAR\\HeapLeakDetection\\DiagnosedApplications\"");
    system("for /F \"tokens=2\" %i IN ('whoami /user /fo table /nh') DO SET usersid=%i");
    system(("REG DELETE \"HKEY_USERS\\" + userSid + "\\Software\\Microsoft\\Windows\\CurrentVersion\\Search\\RecentApps\" /f").c_str());
    system(("REG ADD \"HKEY_USERS\\" + userSid + "\\Software\\Microsoft\\Windows\\CurrentVersion\\Search\\RecentApps\"").c_str());
    system(("REG DELETE \"HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\bam\\UserSettings\\" + userSid + "\" /va /f").c_str());
    system(("REG DELETE \"HKEY_LOCAL_MACHINE\\SYSTEM\\ControlSet001\\Services\\bam\\UserSettings\\" + userSid + "\" /va /f").c_str());
    system(("REG DELETE \"HKEY_USERS\\" + userSid + "\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Compatibility Assistant\\Store\" /va /f").c_str());
    system(("REG DELETE \"HKEY_USERS\\" + userSid + "\\Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers\" /va /f").c_str());
    system(("REG DELETE \"HKEY_USERS\\" + userSid + "\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MountPoints2\" /f").c_str());
    system(("REG ADD \"HKEY_USERS\\" + userSid + "\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MountPoints2\"").c_str());
    system("DEL /f /q %APPDATA%\\Microsoft\\Windows\\Recent\\*.*");
    system("DEL /f /q %APPDATA%\\Microsoft\\Windows\\Recent\\CustomDestinations\\*.*");
    system("DEL /f /q %APPDATA%\\Microsoft\\Windows\\Recent\\AutomaticDestinations\\*.*");
    system("DEL /f /q %systemroot%\\Panther\\*.*");
    system("DEL /f /q %systemroot%\\appcompat\\Programs\\*.txt");
    system("DEL /f /q %systemroot%\\appcompat\\Programs\\*.xml");
    system("DEL /f /q %systemroot%\\appcompat\\Programs\\Install\\*.txt");
    system("DEL /f /q %systemroot%\\appcompat\\Programs\\Install\\*.xml");
    system("DEL /f /q %systemroot%\\Prefetch\\*.pf");
    system("DEL /f /q %systemroot%\\Prefetch\\*.ini");
    system("DEL /f /q %systemroot%\\Prefetch\\*.7db");
    system("DEL /f /q %systemroot%\\Prefetch\\*.ebd");
    system("DEL /f /q %systemroot%\\Prefetch\\*.bin");
    system("DEL /f /q %systemroot%\\Prefetch\\*.db");
    system("DEL /f /q %systemroot%\\Prefetch\\ReadyBoot\\*.fx");
    system("DEL /f /q %systemroot%\\Minidump\\*.*");
}


std::wstring StringToWideString(const std::string& str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
    return wstr;
}



std::wstring GenerateRandomWindowTitle() {
    std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-=+_`~";
    std::string randomTitle;
    srand(static_cast<unsigned int>(time(0)));

    int length = 20;
    for (int i = 0; i < length; ++i) {
        randomTitle += alphabet[rand() % alphabet.length()];
    }

    return StringToWideString(randomTitle + "   |  Panic free version 0.1");
}
std::wstring GenerateRandomWindowTitlePrem() {
    std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-=+_`~";
    std::string randomTitle;
    srand(static_cast<unsigned int>(time(0)));

    int length = 20;
    for (int i = 0; i < length; ++i) {
        randomTitle += alphabet[rand() % alphabet.length()];
    }

    return StringToWideString(randomTitle + "   |  Panic Premium version 0.1");
}
DWORD GetProcessIdByName(const std::wstring& processName)
{
    DWORD pid = 0;
    HANDLE snapHandle;
    PROCESSENTRY32 pe32;

    snapHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapHandle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(snapHandle, &pe32))
    {
        CloseHandle(snapHandle);
        return 0;
    }

    do
    {
        if (processName == pe32.szExeFile)
        {
            pid = pe32.th32ProcessID;
            break;
        }
    } while (Process32Next(snapHandle, &pe32));

    CloseHandle(snapHandle);
    return pid;
}


int main() {

    WindowSetup();
    std::wstring randomTitle = GenerateRandomWindowTitle();
    SetConsoleTitle(randomTitle.c_str());
    createLocalCMPackageFolder();
    char* userProfile = nullptr;
    size_t len = 0;
    errno_t err = _dupenv_s(&userProfile, &len, "USERPROFILE");
    std::string horik = std::string(userProfile) + R"(\AppData\Local\Packages\Microsoft.MinecraftUWP_8wekyb3d8bbwe\RoamingState\Horion\Assets)";
    
    std::string sourcePath = std::string(userProfile) + R"(\AppData\Local\Packages\Microsoft.MinecraftUWP_8wekyb3d8bbwe\RoamingState)";
    std::string destPath = std::string(userProfile) + R"(\AppData\Local\Packages\Microsoft.MinecraftUWP_8wekyb3d8bbwe\LocalCM)";
    setlocale(LC_ALL, "ru");
    system("COLOR A");
    system("CLS");
    std::string choice;
    std::string non;
    std::cout << "  _____            _\n";
    std::cout << " |  __ \\          (_)\n";
    std::cout << " | |__) |_ _ _ __  _  ___\n";
    std::cout << " |  ___/ _` | '_ \\| |/ __| by meLdozy (wh1tness)\n";
    std::cout << " | |  | (_| | | | | | (__\n";
    std::cout << " |_|   \\__,_|_| |_|_|\\___|\n";
    std::cout << "1. Clear all\n";
    std::cout << "2. Auto-closing programs\n";
    std::cout << "3. Back folders\n";
    std::cout << "Enter choice: ";
    std::cin >> choice;
    if (choice == "1") {
        Clear();
        for (const auto& entry : fs::directory_iterator(sourcePath)) {
            std::string newFilename = "";
            for (char c : entry.path().filename().string()) {
                newFilename += c;
                newFilename += '0';
            }
            
            fs::rename(entry.path(), destPath + "\\" + newFilename);
            std::cout << "Moved: " << newFilename << "\n";
        }
        system("ECHO Successfully");
        system("PAUSE");
        main();
    }
    else if (choice == "2") {
        std::cout << "Перезапустите программу, для дольнейшего использования. (но автозакрытие работать не будет!)";
        std::vector<std::wstring> processes = { L"lastactivityview.exe", L"everything.exe", L"shellbag_analyzer_cleaner.exe", L"SystemSettings.exe" };
        closeProcesses(processes);
        return 0;
    } else if (choice == "3") {
        for (const auto& entry : fs::directory_iterator(destPath)) {
            std::string newFilename = entry.path().filename().string();
            newFilename.erase(std::remove(newFilename.begin(), newFilename.end(), '0'), newFilename.end());
            fs::rename(entry, sourcePath + "\\" + newFilename);
            std::cout << "Moved: " << newFilename << "\n";
        }
        std::cout << "successfully.\n";
        system("PAUSE");
        main();
    }
    return 0;
}
