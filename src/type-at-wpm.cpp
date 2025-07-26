#include <windows.h>      // Required for Windows API functions
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



// --- Command-Line Parsing and Application Logic ---

void printUsage(const std::wstring& appName) {
    std::wcout << "Usage: " << appName << " [OPTIONS] <text_to_type>" << std::endl;
    std::wcout << "\nSimulates typing a given text using Windows API." << std::endl;
    std::wcout << "\nOptions:" << std::endl;
    std::wcout << "  -l, --lorem-ipsum  Type sample Lorem Ipsum text instead" << std::endl;
    std::wcout << "  -d, --delay <ms>   Initial delay in milliseconds before typing begins (default: 5000)." << std::endl;
    std::wcout << "  -wpm <ms>          Words Per minute speed to type text (default: 60)." << std::endl;
    std::wcout << "  -h, --help         Display this help message." << std::endl;
    std::wcout << "\nExamples:" << std::endl;
    std::wcout << "  " << appName << " \"Hello, World!\"" << std::endl;
    std::wcout << "  " << appName << " -d 2000 \"My secret password\"" << std::endl;
    std::wcout << "  " << appName << " -k 50 \"Lorem ipsum...\"" << std::endl;
    std::wcout << "\nRemember to switch to the target application/text field during the initial delay." << std::endl;
}

int countWordsInString(std::wstring const& str)
{
    std::wstringstream stream(str);
    return std::distance(std::istream_iterator<std::wstring, wchar_t>(stream), std::istream_iterator<std::wstring, wchar_t>());
}

int main(int argc, wchar_t* argv[]) {
    std::wstring appName = (argc > 0) ? argv[0] : L"type-at-wpm";

    // Default values
	int initialDelayMs = { 5000 };      // Startup delay
    int keyPressDelayMs = { 0 };        // Will be set by WPM calculations
	int wpm = { 120 };                  
    std::wstring textToType = L"";

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::wstring arg = argv[i];

        if (arg == L"-h" || arg == L"--help") {
            printUsage(appName);
            return 0;
        }
        else if (arg == L"-d" || arg == L"--delay") {
            if (i + 1 < argc) {
                try {
                    initialDelayMs = std::stoi(argv[++i]);
                    if (initialDelayMs < 0) throw std::out_of_range("Negative delay not allowed.");
                }
                catch (const std::exception& e) {
                    std::cerr << "Error: Invalid value for --delay. " << e.what() << std::endl;
                    printUsage(appName);
                    return 1;
                }
            }
            else {
                std::cerr << "Error: --delay requires a value." << std::endl;
                printUsage(appName);
                return 1;
            }
        }
        else if (arg == L"-wpm") {
            if (i + 1 < argc) {
                try {
                    wpm = std::stoi(argv[++i]);
                    if (wpm <= 0) throw std::out_of_range("WPM must be at minimum 1.");
                    
                }
                catch (const std::exception& e) {
                    std::cerr << "Error: Invalid value for -wpm. " << e.what() << std::endl;
                    printUsage(appName);
                    return 1;
                }
            }
            else {
                std::cerr << "Error: -wpm requires a value." << std::endl;
                printUsage(appName);
                return 1;
            }
        }
        else if (arg == L"-l" || arg == L"--lorem-ipsum") {
			textToType = L"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
        }
        else if (textToType.empty()) { // Assume the first non-option argument is the text
            textToType = arg;
        }
        else {
            // Handle multiple non-option arguments, or unrecognized options
            std::wcerr << L"Error: Unrecognized argument or too many text arguments: " << arg << std::endl;
            printUsage(appName);
            return 1;
        }
    }

    if (textToType.empty()) {
        textToType = fetchClipboardContents();
        if (textToType.empty()) {
            std::cerr << "Error: No text provided and clipboard is empty." << std::endl;
            printUsage(appName);
            return 1;
        }
        else {
            std::cout << "No text to type provided: Using Clipboard contents" << std::endl;
        }
    }

	keyPressDelayMs = countWordsInString(textToType) > 0 ? 60000 / (countWordsInString(textToType) * wpm) : 20;
	//std::cout << "Words in text: " << countWordsInString(textToType) << std::endl;
    std::wcout << L"Starting key press simulation for: \"" << textToType << L"\"" << std::endl;
    std::cout << "Initial delay: " << initialDelayMs << "ms, WPM:" << wpm << ", Calculated Key delay : " << keyPressDelayMs << "ms" << std::endl;
    std::cout << "Switch to a text editor or a field where you want the text to be typed NOW." << std::endl;

    if (initialDelayMs > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(initialDelayMs));
    }

    simulateTyping(textToType, keyPressDelayMs);

    std::cout << "Text typing simulation complete." << std::endl;
   
    return 0;
}