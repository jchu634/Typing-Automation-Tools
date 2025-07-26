#ifndef KEY_SIM_LIB_H
#define KEY_SIM_LIB_H

#include <string> // For std::string
#include <windows.h> // For INPUT, KEYEVENTF_UNICODE, etc.

// Define export/import macros for DLL
#ifdef KEY_SIM_EXPORTS
#define KEY_SIM_API __declspec(dllexport)
#else
#define KEY_SIM_API __declspec(dllimport)
#endif

// To ensure C++ name mangling doesn't prevent Python from finding the function
// We also use basic C types (char*, int) for simpler ctypes mapping
extern "C" {
    /**
     * @brief Simulates typing a string using Windows SendInput.
     *
     * @param text The string to type (C-style null-terminated string).
     * @param key_delay_ms Delay in milliseconds between each key press.
     * @return 0 on success, non-zero on error.
     */
    KEY_SIM_API int simulateTyping(const std::wstring& text, int& key_delay_ms);
    KEY_SIM_API std::wstring fetchClipboardContents();
}

#endif // KEY_SIM_LIB_H