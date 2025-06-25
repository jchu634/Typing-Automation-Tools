#include <windows.h>
#include <string>
#include <thread>
#include <chrono>
#include <iostream>
#include "key_sim_lib.h";
#include <vector>   // For std::vector
#include <algorithm> // For std::find_if
#include <stdexcept> // For std::runtime_error


// --- Command-Line Parsing and Application Logic ---

void printUsage(const std::string& appName) {
    std::cout << "Usage: " << appName << " [OPTIONS] <text_to_type>" << std::endl;
    std::cout << "\nSimulates typing a given text using Windows API." << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -l, --lorem-ipsum  Type sample Lorem Ipsum text instead" << std::endl;
    std::cout << "  -d, --delay <ms>   Initial delay in milliseconds before typing begins (default: 5000)." << std::endl;
    std::cout << "  -wpm <ms>          Words Per minute speed to type text (default: 60)." << std::endl;
    std::cout << "  -h, --help         Display this help message." << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << appName << " \"Hello, World!\"" << std::endl;
    std::cout << "  " << appName << " -d 2000 \"My secret password\"" << std::endl;
    std::cout << "  " << appName << " -k 50 \"Lorem ipsum...\"" << std::endl;
    std::cout << "\nRemember to switch to the target application/text field during the initial delay." << std::endl;
}

int countWordsInString(std::string const& str)
{
    std::stringstream stream(str);
    return std::distance(std::istream_iterator<std::string>(stream), std::istream_iterator<std::string>());
}

int main(int argc, char* argv[]) {
    std::string appName = (argc > 0) ? argv[0] : "key_sim_tool";

    // Default values
    int initialDelayMs = {5000
};  // 5 seconds
    int keyPressDelayMs = { 20 };   // 20 ms per key
	int wpm = { 50 };           // Default WPM
    std::string textToType = "";

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(appName);
            return 0;
        }
        else if (arg == "-d" || arg == "--delay") {
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
        else if (arg == "-wpm") {
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
        else if (arg == "-l" || arg == "--lorem-ipsum") {
			textToType = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
        }
        else if (textToType.empty()) { // Assume the first non-option argument is the text
            textToType = arg;
        }
        else {
            // Handle multiple non-option arguments, or unrecognized options
            std::cerr << "Error: Unrecognized argument or too many text arguments: " << arg << std::endl;
            printUsage(appName);
            return 1;
        }
    }

    if (textToType.empty()) {
        std::cerr << "Error: No text to type provided." << std::endl;
        printUsage(appName);
        return 1;
    }

	keyPressDelayMs = countWordsInString(textToType) > 0 ? 60000 / (countWordsInString(textToType) * wpm) : 20;

    std::cout << "Starting key press simulation for: \"" << textToType << "\"" << std::endl;
    std::cout << "Initial delay: " << initialDelayMs << "ms, Key delay: " << keyPressDelayMs << "ms" << std::endl;
    std::cout << "Switch to a text editor or a field where you want the text to be typed NOW." << std::endl;

    if (initialDelayMs > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(initialDelayMs));
    }

    simulateTyping(textToType, keyPressDelayMs);

    std::cout << "Text typing simulation complete." << std::endl;

    // Keep console open briefly if run directly without a debugger attached
    if (IsDebuggerPresent() == 0) { // Check if debugger is attached
        std::cout << "Press Enter to exit..." << std::endl;
        std::cin.ignore();
    }

    return 0;
}