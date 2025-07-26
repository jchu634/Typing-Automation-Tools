#include "key_sim_lib.h"
#include <thread>
#include <chrono>

extern "C" {
    /**
     * @brief Simulates typing a string using Windows SendInput.
     *
     * @param text The string to type.
     * @param key_delay_ms Delay in milliseconds between each key press.
     */
    KEY_SIM_API int simulateTyping(const std::wstring& text, int& key_delay_ms) {
        if (text.empty()) {
            return 0; // Nothing to type
        }

        //for (char c : text) {
        for (int i = 0; i < text.length(); i++) {
            INPUT input[2];
            ZeroMemory(input, sizeof(input));

            // Key down event
            input[0].type = INPUT_KEYBOARD;
            input[0].ki.wVk = 0;
            input[0].ki.wScan = (short)text[i];
            input[0].ki.dwFlags = KEYEVENTF_UNICODE;
            if (SendInput(1, &input[0], sizeof(INPUT)) != 1) {
                return 1; // Indicate error
            }
            
            // Key up event
            input[1].type = INPUT_KEYBOARD;
            input[1].ki.wVk = 0;
            input[1].ki.wScan = (short)text[i];
            input[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
            if (SendInput(1, &input[1], sizeof(INPUT)) != 1) {
                return 1; // Indicate error
            }

            if (key_delay_ms > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(key_delay_ms));
            }
        }
        return 0; // Success
    }
}