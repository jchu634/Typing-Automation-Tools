#include <windows.h>
#include <shellapi.h> // Required for Shell_NotifyIcon
#include <string>         // For std::string
#include <thread>         // For std::this_thread::sleep_for
#include <chrono>         // For std::chrono::milliseconds
#include <iostream>       // For console output (can be removed for true background app)
#include <sstream>        // For std::stringstream (word counting)
#include <iterator>       // For std::istream_iterator (word counting)
#include <algorithm>      // For std::distance (word counting)
#include <stdexcept>      // For std::runtime_error (though not used in this version)

#include "key_sim_lib.h";

#define WM_MY_TRAY_MESSAGE WM_APP + 1

// Global variables for the window and tray icon
HWND g_hWnd;
NOTIFYICONDATA g_nid;

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int countWordsInString(std::string const& str)
{
    std::stringstream stream(str);
    return std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>());
}

std::string fetchClipboardContents() {
    std::string clipboardTextString = " ";
    if (OpenClipboard(NULL)) {
        HANDLE hData = GetClipboardData(CF_TEXT);
        if (hData != NULL) {
            char* clipboardText = static_cast<char*>(GlobalLock(hData));
            if (clipboardText) {
                clipboardTextString = clipboardText;
                GlobalUnlock(hData);
            }
        }
        CloseClipboard();
    }
    return clipboardTextString;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 1. Register a Window Class
    const char CLASS_NAME[] = "HiddenWindow";

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // Small invisible cursor
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // No visible background

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
    g_nid.uID = 100; // Unique ID for our icon
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION); // Replace with your app's icon
    lstrcpy(g_nid.szTip, "Clipboard Paster"); // Tooltip text
    g_nid.uCallbackMessage = WM_MY_TRAY_MESSAGE; // Custom message for tray interactions

    if (!Shell_NotifyIcon(NIM_ADD, &g_nid)) {
        MessageBox(nullptr, "Failed to add icon to system tray!", "Error", MB_ICONERROR);
        // You might want to handle this gracefully, maybe exit
    }

    if (RegisterHotKey(NULL, 1, MOD_WIN, 0x75)) {
        std::cout << "Hotkey registered. Press WIN + F6" << std::endl;
    }
    else {
        std::cerr << "Failed to register hotkey. Error: " << GetLastError() << std::endl;
        return 1;
    }

    // Default values
    int initialDelayMs = { 1000 };      // 1 seconds
    int keyPressDelayMs = { 20 };       // 20 ms per key    
    std::string textToType = " ";
    // Message loop
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            //std::cout << "Hotkey pressed!" << std::endl;
            textToType = fetchClipboardContents();
            if (!textToType.empty()) {

                std::cout << textToType << std::endl;
                keyPressDelayMs = 5;

                // Delay required: otherwise beginning of string is often clipped 
                std::this_thread::sleep_for(std::chrono::milliseconds(initialDelayMs));
                simulateTyping(textToType, keyPressDelayMs);
                std::cout << "Text typed" << std::endl;
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
    case WM_CREATE:
        // Initialization here if needed
        break;

    case WM_MY_TRAY_MESSAGE:
        switch (LOWORD(lParam)) {
       
        case WM_RBUTTONUP:
            // Right-click on tray icon: e.g., show context menu
        {
            POINT pt;
            GetCursorPos(&pt);

            HMENU hMenu = CreatePopupMenu();
            InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, 1, "Settings");
            InsertMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, 2, "Exit");

            // Set the default menu item
            SetMenuDefaultItem(hMenu, 1, FALSE);

            // Bring the window to the foreground before showing the menu
            // This is crucial for the menu to disappear when clicking elsewhere
            SetForegroundWindow(hwnd);

            UINT clicked = TrackPopupMenu(hMenu,
                TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RETURNCMD | TPM_NONOTIFY,
                pt.x, pt.y, 0, hwnd, nullptr);

            DestroyMenu(hMenu);

            switch (clicked) {
            case 1:
                MessageBox(hwnd, "Settings selected!", "Menu", MB_OK);
                // Open settings dialog
                break;
            case 2:
                // Post quit message to exit the application
                PostQuitMessage(0);
                break;
            }
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}