#include <windows.h>
#include <string>
#include <shellapi.h> // For ShellExecute
#include <string>     // For std::string, which is useful even in a mostly C-style app
#include <filesystem> // C++17 for path manipulation (optional, but good practice)
#include <iostream>   // For console output (for debugging/status)

// Define a unique ID for your hotkey
#define MY_HOTKEY_ID 100

// Define the hotkey combination: Ctrl + Alt + 'K' (for Key or Kwik)
// You can change 0x4B to another virtual key code.
// E.g., 0x46 for 'F', 0x41 for 'A', VK_SPACE for space bar.
#define HOTKEY_MODIFIERS (MOD_CONTROL | MOD_ALT)
#define HOTKEY_VIRTUAL_KEY 0x4B // VK_K or 0x4B

// --- Path to your WPM-Simulator.exe ---
// IMPORTANT: This path needs to be correct relative to where WPM-Hotkey-Launcher.exe
// will be run from, or be an absolute path.
// When building with CMake, both executables will likely be in the same output directory
// (e.g., out/build/x64-debug/).
// So, "WPM-Simulator.exe" should suffice if they are in the same directory.
const wchar_t* WPM_SIMULATOR_EXE = L"WPM-Simulator.exe";

// --- Text to type when hotkey is pressed ---
// You can make this configurable via an INI file or registry if more complex.
// For now, hardcoding a Lorem Ipsum snippet for the hotkey version.
const wchar_t* TEXT_TO_TYPE_HOTKEY = L"\"Hotkey triggered! This text is typed from the WPM Simulator launcher.\" -wpm 80";


// Forward declaration for the window procedure
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc = { 0 };
    HWND hwnd;
    MSG Msg;

    // Register the window class
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"WPMHotkeyLauncherClass"; // Use wide string literal

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    // Create the hidden window
    hwnd = CreateWindowEx(
        0,                 // Optional window style
        L"WPMHotkeyLauncherClass", // Window class name (wide string)
        L"WPM Hotkey Launcher",    // Window title (wide string, can be anything, it's hidden)
        WS_OVERLAPPEDWINDOW, // Window style (can be WS_POPUP for even less overhead)
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, // Position and size (doesn't matter much for a hidden window)
        NULL,              // Parent window handle
        NULL,              // Menu handle
        hInstance,         // Instance handle
        NULL               // Additional application data
    );

    if (hwnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 1;
    }

    // Register the hotkey
    if (!RegisterHotKey(hwnd, MY_HOTKEY_ID, HOTKEY_MODIFIERS, HOTKEY_VIRTUAL_KEY)) {
        std::string errorMsg = "Hotkey Registration Failed! It might be in use by another application. Error code: " + std::to_string(GetLastError());
        MessageBoxA(NULL, errorMsg.c_str(), "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 1; // Indicate failure
    }

    // Hide the window immediately. SW_HIDE will prevent it from showing up.
    ShowWindow(hwnd, SW_HIDE);
    UpdateWindow(hwnd); // Ensures the window state is applied

    // Acknowledge that the launcher is running (e.g., to a console if it has one, or a log)
    // For a hidden window, this won't be seen unless you're debugging or logging.
    // For a production app, consider a tray icon.
    // std::cout << "WPM Hotkey Launcher is running. Press Ctrl+Alt+K to activate." << std::endl;
    // For a true GUI app, output to Debug console or a log file.

    // Message loop
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    // Unregister the hotkey before exiting
    UnregisterHotKey(hwnd, MY_HOTKEY_ID);

    return (int)Msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_HOTKEY:
        if (wParam == MY_HOTKEY_ID) {
            // Hotkey pressed! Launch your command-line tool.
            // Construct the full command line for WPM-Simulator.exe
            // The arguments include the text and potentially -wpm or -d
            // Example: WPM-Simulator.exe "Your Text Here" -wpm 80
            std::wstring commandLine = std::wstring(L"\"") + WPM_SIMULATOR_EXE + L"\" " + TEXT_TO_TYPE_HOTKEY;

            // Use ShellExecute to launch the WPM-Simulator.exe
            // It's good practice to provide the current working directory
            // or ensure WPM_SIMULATOR_EXE is a full path.
            // ShellExecute returns an HINSTANCE (or 32 if fails).
            int result = (int)ShellExecuteW(
                NULL,                // Parent window handle
                L"open",             // Operation to perform
                WPM_SIMULATOR_EXE,   // File to open (executable path)
                TEXT_TO_TYPE_HOTKEY, // Parameters to pass to the executable
                NULL,                // Default directory (current working directory)
                SW_SHOWNORMAL        // How the application window is to be shown (can be SW_HIDE if your tool is console-based)
            );

            if (result <= 32) { // ShellExecute returns values <= 32 to indicate an error
                std::string errorMsg = "Failed to launch WPM-Simulator.exe! Error code: " + std::to_string(GetLastError()) +
                    "\nMake sure '" + std::string(TEXT_TO_TYPE_HOTKEY) + "' is accessible.";
                MessageBoxA(NULL, errorMsg.c_str(), "Launch Error", MB_ICONERROR | MB_OK);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0); // Post a quit message to the message loop
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}