#include <windows.h>      // Core Windows API functions
#include <shellapi.h>     // For Shell_NotifyIcon and NOTIFYICONDATA (tray icon)
#include <string>         // For std::string
#include <thread>         // For std::this_thread::sleep_for
#include <chrono>         // For std::chrono::milliseconds
#include <iostream>       // For console output (can be removed for true background app)
#include <sstream>        // For std::stringstream (word counting)
#include <iterator>       // For std::istream_iterator (word counting)
#include <algorithm>      // For std::distance (word counting)
#include <stdexcept>      // For std::runtime_error (though not used in this version)

#include "key_sim_lib.h";




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
int main(int argc, char* argv[]) {

	/* Cannot use some hotkeys, as a subset may already be in use and interfer with the program.
    if (RegisterHotKey(NULL, 1, MOD_WIN | MOD_ALT | MOD_SHIFT, 0x56)) {
       std::cout << "Hotkey registered. Press Shift + Win + Alt + V" << std::endl;
    */
    if (RegisterHotKey(NULL, 1, MOD_WIN, 0x75)){
        std::cout << "Hotkey registered. Press WIN + F6" << std::endl;
    }else {
        std::cerr << "Failed to register hotkey. Error: " << GetLastError() << std::endl;
		return 1;
    }

    // Default values
    int initialDelayMs = { 1000 };      // 1 seconds
    int keyPressDelayMs = { 20 };       // 20 ms per key    
    std::string textToType = " ";

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            std::cout << "Hotkey pressed!" << std::endl;
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

    UnregisterHotKey(NULL, 1);
    return 0;
}
