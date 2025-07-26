#include <windows.h>
#include <shellapi.h>     // Required for Shell_NotifyIcon
#include <string>         // For std::string
#include <thread>         // For std::this_thread::sleep_for
#include <chrono>         // For std::chrono::milliseconds
#include <iostream>       // For console output (can be removed for true background app)
#include <sstream>        // For std::stringstream (word counting)
#include <iterator>       // For std::istream_iterator (word counting)
#include <algorithm>      // For std::distance (word counting)
#include <stdexcept>      // For std::runtime_error (though not used in this version)
#include "key_sim_lib.h"
#include "resource.h"

#define WM_MY_TRAY_MESSAGE WM_APP + 1

// Global variables for the window and tray icon
struct AppSettings {
    int initialDelayMs = 1000; // 1 second
    int keyPressDelayMs = 5;  // 20 ms
};

HWND g_hWnd;
NOTIFYICONDATA g_nid;
AppSettings g_settings; // Global settings instance

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void ShowSettingsDialog(HWND hParent);
std::wstring fetchClipboardContents();


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 1. Register a Window Class
    const char CLASS_NAME[] = "HiddenWindow";

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);    // Small invisible cursor
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);  // No visible background
   

    RegisterClassEx(&wc);

    // 2. Create the Hidden Window
    g_hWnd = CreateWindowEx(
        WS_EX_TOOLWINDOW,       // Prevent from appearing in taskbar
        CLASS_NAME,             // Window class name
        "Clipboard Paster",     // Window title (can be anything, won't be seen)
        WS_POPUP,               // Popup style (no border, title bar)
        CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, // Position and size (0x0 effectively)
        nullptr,                // Parent window
        nullptr,                // Menu
        hInstance,              // Instance handle
        nullptr                 // Additional application data
    );


    if (g_hWnd == nullptr) {
        MessageBox(nullptr, "Window Creation Failed!", "Error", MB_ICONERROR);
        return 1;
    }

    // Don't call ShowWindow(g_hWnd, nCmdShow); as we want it hidden

    // 3. Add to System Tray
    g_nid.cbSize = sizeof(NOTIFYICONDATA);
    g_nid.hWnd = g_hWnd;
    g_nid.uID = 100;                                                    // Unique ID for our icon
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAYICON));   // Use custom icon
    lstrcpy(g_nid.szTip, "Clipboard Paster");                           // Tooltip text
    g_nid.uCallbackMessage = WM_MY_TRAY_MESSAGE;                        // Custom message for tray interactions

    if (!Shell_NotifyIcon(NIM_ADD, &g_nid)) {
        MessageBox(nullptr, "Failed to add icon to system tray!", "Error", MB_ICONERROR);
		// Exit if we can't add the icon
        return 0;
    }

    if (RegisterHotKey(NULL, 1, MOD_WIN, 0x75)) {
        std::cout << "Hotkey registered. Press WIN + F6" << std::endl;
    }
    else {
        std::cerr << "Failed to register hotkey. Error: " << GetLastError() << std::endl;
        return 1;
    }

    std::wstring textToType = L"";

    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            //std::cout << "Hotkey pressed!" << std::endl;
            textToType = fetchClipboardContents();
            if (!textToType.empty()) {

                std::wcout << textToType << std::endl;

                // Delay required: otherwise beginning of string is often clipped 
                std::this_thread::sleep_for(std::chrono::milliseconds(g_settings.initialDelayMs));
                simulateTyping(textToType, g_settings.keyPressDelayMs);
                std::wcout << L"Text typed" << std::endl;
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup: Remove from System Tray before exiting
    Shell_NotifyIcon(NIM_DELETE, &g_nid);

    return 0;
}


// 4. Handle Window Messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // Initialization here if needed
            break;
        }

        case WM_MY_TRAY_MESSAGE: {
            switch (LOWORD(lParam)) {
                // Right-click on tray icon:
                case WM_RBUTTONUP: {
                    POINT pt;
                    GetCursorPos(&pt);

                    HMENU hMenu = CreatePopupMenu();

                    // Menu IDs should be unique and non-zero
                    InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, 1, "Settings");
                    InsertMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, 2, "Exit");

                    SetMenuDefaultItem(hMenu, 2, FALSE);

                    // Bring the window to the foreground before showing the menu
                    // This is crucial for the menu to disappear when clicking elsewhere
                    SetForegroundWindow(hwnd);

                    UINT clicked = TrackPopupMenu(
                        hMenu,
                        TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD | TPM_NONOTIFY,
                        pt.x, pt.y, 0, hwnd, nullptr);

                    DestroyMenu(hMenu);

                    switch (clicked) {
                        case 1: { // Settings
                            ShowSettingsDialog(hwnd);
                            break;
                        }
                        case 2: { // Exit
                            PostQuitMessage(0);
                            break;
                        }
                    }
                    break; // Break from WM_RBUTTONUP
                }
            }
            return 0;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        default: {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }
    return 0;
}

INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INITDIALOG: {
        // Set initial values in the edit controls
        SetDlgItemInt(hDlg, IDC_EDIT_INITIAL_DELAY, g_settings.initialDelayMs, FALSE);
        SetDlgItemInt(hDlg, IDC_EDIT_KEY_DELAY, g_settings.keyPressDelayMs, FALSE);
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK: {
            BOOL ok1 = FALSE, ok2 = FALSE;
            int initialDelay = GetDlgItemInt(hDlg, IDC_EDIT_INITIAL_DELAY, &ok1, FALSE);
            int keyDelay = GetDlgItemInt(hDlg, IDC_EDIT_KEY_DELAY, &ok2, FALSE);

            // Simple validation: only update if both are valid and positive
            if (ok1 && ok2 && initialDelay >= 0 && keyDelay >= 0) {
                g_settings.initialDelayMs = initialDelay;
                g_settings.keyPressDelayMs = keyDelay;
            }
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void ShowSettingsDialog(HWND hParent) {
    DialogBoxParam(
        GetModuleHandle(nullptr),
        MAKEINTRESOURCE(101),
        hParent,
        SettingsDlgProc,
        0
    );
}

std::wstring fetchClipboardContents() {
    std::wstring clipboardTextString = { L" " };
    if (OpenClipboard(NULL)) {
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData != NULL) {
            wchar_t* clipboardText = static_cast<wchar_t*>(GlobalLock(hData));
            if (clipboardText) {
                clipboardTextString = clipboardText;
                GlobalUnlock(hData);
            }
        }
        CloseClipboard();
    }
    return clipboardTextString;
}