#define SFML_STATIC
#include <SFML/Graphics.hpp>
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#include <Windows.h>
#include <sstream>
#include <vector>
#include <fstream>
#include <filesystem>

// Function to wrap text to fit within a given width, preserving existing line breaks
std::string wrapText(const std::string& text, const sf::Font& font, unsigned int fontSize, float maxWidth) {
    std::string result;
    std::istringstream lines(text);
    std::string line;
    
    sf::Text tempText("", font, fontSize);
    
    while (std::getline(lines, line)) {
        if (line.empty()) {
            result += "\n";
            continue;
        }
        
        std::istringstream words(line);
        std::string word;
        std::string currentLine;
        
        while (words >> word) {
            std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
            tempText.setString(testLine);
            
            if (tempText.getLocalBounds().width <= maxWidth) {
                currentLine = testLine;
            } else {
                if (!currentLine.empty()) {
                    result += currentLine + "\n";
                    currentLine = word;
                } else {
                    result += word + "\n";
                    currentLine = "";
                }
            }
        }
        
        if (!currentLine.empty()) {
            result += currentLine;
        }
        result += "\n";
    }
    
    return result;
}

// Completely rewritten cursor position calculation to match SFML text rendering
sf::Vector2f calculateCursorPosition(const std::string& text, size_t cursorPos, const sf::Font& font, unsigned int fontSize, sf::Vector2f startPos, float lineSpacing) {
    if (cursorPos > text.length()) {
        cursorPos = text.length();
    }
    
    // Create a temporary text object to get exact metrics
    sf::Text tempText("", font, fontSize);
    tempText.setLineSpacing(lineSpacing);
    tempText.setPosition(startPos);
    
    std::string textUpToCursor = text.substr(0, cursorPos);
    
    // Split text into lines exactly as SFML does
    std::vector<std::string> lines;
    std::stringstream ss(textUpToCursor);
    std::string line;
    
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }
    
    // Handle case where cursor is at the very beginning
    if (lines.empty()) {
        lines.push_back("");
    }
    
    // Get the current line (last line in our split)
    std::string currentLine = lines.back();
    int lineNumber = lines.size() - 1;
    
    // Calculate X position using the exact line content
    tempText.setString(currentLine);
    float lineWidth = tempText.getLocalBounds().width;
    
    // Calculate Y position using SFML's actual line height calculation
    float actualLineHeight = fontSize * lineSpacing;
    
    float x = startPos.x + lineWidth;
    float y = startPos.y + lineNumber * actualLineHeight;
    
    return sf::Vector2f(x, y);
}

// Function to block common copy shortcuts and screenshot keys
bool isBlockedKey(const sf::Event::KeyEvent& key) {
    if (key.control) {
        return (key.code == sf::Keyboard::C || 
                key.code == sf::Keyboard::V || 
                key.code == sf::Keyboard::A || 
                key.code == sf::Keyboard::X);
    }
    
    if (key.code == sf::Keyboard::F12 || key.code == sf::Keyboard::F10) {
        return true;
    }
    
    if (key.system) {
        return true;
    }
    
    return false;
}

// Function to save text to file
bool saveToFile(const std::string& content, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << content;
            file.close();
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

int main() {
    // Enable high DPI awareness for better text quality
    SetProcessDPIAware();
    
    // Create window with anti-aliasing from the start
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4; // Reduced to 4 for better performance
    
    sf::RenderWindow window(sf::VideoMode(1400, 900), "TechMantra1.0 - C Programming Challenge", sf::Style::Default, settings);
    window.setFramerateLimit(60);

    // Enhanced screenshot blocking
    HWND hwnd = window.getSystemHandle();
    SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE);
    
    RegisterHotKey(hwnd, 1, MOD_CONTROL | MOD_ALT, VK_DELETE);
    RegisterHotKey(hwnd, 2, MOD_ALT, VK_TAB);
    RegisterHotKey(hwnd, 3, MOD_WIN, VK_TAB);

    sf::Font font;
    bool fontLoaded = false;
    
    // Try to load better fonts for improved quality
    if (font.loadFromFile("C:/Windows/Fonts/consola.ttf")) {
        fontLoaded = true;
    } else if (font.loadFromFile("C:/Windows/Fonts/calibri.ttf")) {
        fontLoaded = true;
    } else if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        fontLoaded = true;
    }
    
    if (!fontLoaded) {
        return -1;
    }

    // Problem code in C that needs debugging
    std::string problemCode = 
        "// Debug this C code to make it work correctly\n"
        "#include <stdio.h>\n"
        "#include <stdlib.h>\n\n"
        "int main() {\n"
        "    int numbers[] = {1, 2, 3, 4, 5};\n"
        "    int size = 5;\n"
        "    int sum = 0;\n"
        "    int i;\n"
        "    \n"
        "    // Bug: Loop condition is wrong\n"
        "    for(i = 0; i <= size; i++) {\n"
        "        sum += numbers[i];\n"
        "    }\n"
        "    \n"
        "    printf(\"Sum: %d\\n\", sum);\n"
        "    \n"
        "    // Bug: Division by zero\n"
        "    int average = sum / 0;\n"
        "    printf(\"Average: %d\\n\", average);\n"
        "    \n"
        "    return 0;\n"
        "}\n";

    // Split window into three panels
    const float panelWidth = window.getSize().x / 3;
    const float padding = 15.f;
    const float headerHeight = 35.f;
    const float mainHeaderHeight = 30.f;
    
    // Instructions panel (left side)
    std::string instructions =
        "DEBUG CHALLENGE\n\n"
        "Fix the bugs in the C code!\n\n"
        "RULES:\n"
        "- View buggy code in middle panel\n"
        "- Write corrected code in right panel only\n"
        "- Copy/paste disabled for security\n"
        "- Screenshots blocked\n\n"
        "CONTROLS:\n"
        "- F5: Save solution as solution.c file\n"
        "- F6: Clear solution panel\n"
        "- ESC: Exit application\n"
        "- Click on solution panel to start coding\n\n"
        "BUGS TO FIND:\n"
        "1. Array bounds overflow in loop\n"
        "2. Division by zero error\n"
        "3. Check loop condition carefully\n\n"
        "HINT: Look at the loop condition\n"
        "and mathematical operations carefully!";

    std::string wrappedInstructions = wrapText(instructions, font, 12, panelWidth - padding * 2);
    
    sf::Text instructionText(wrappedInstructions, font, 12);
    instructionText.setFillColor(sf::Color(200, 200, 255));
    instructionText.setLineSpacing(1.3f);

    // Problem code panel (middle) - READ ONLY
    sf::Text codeText(problemCode, font, 11);
    codeText.setFillColor(sf::Color::White);
    codeText.setLineSpacing(1.2f);

    // Solution panel (right side) - EDITABLE ONLY
    std::string solutionCode = "// Write your corrected C code here:\n\n";
    
    sf::Text solutionText("", font, 11);
    solutionText.setFillColor(sf::Color(150, 255, 150));
    solutionText.setLineSpacing(1.2f);

    // Cursor with proper height matching actual line height
    float actualLineHeight = 11 * 1.2f;
    sf::RectangleShape cursor(sf::Vector2f(2, actualLineHeight));
    cursor.setFillColor(sf::Color::Green);
    
    size_t cursorPosition = solutionCode.length();
    bool showCursor = true;
    sf::Clock cursorClock;
    bool needsCursorUpdate = true;
    
    // Scroll offsets
    float leftScrollOffset = 0.f;
    float middleScrollOffset = 0.f;
    float rightScrollOffset = 0.f;
    const float scrollSpeed = 20.f;
    
    // Status and header texts
    sf::Text headerText("TechMantra C Programming Challenge", font, 16);
    headerText.setFillColor(sf::Color(255, 215, 0));
    headerText.setPosition(padding, 8);
    
    sf::Text statusText("Click on solution panel to start coding!", font, 11);
    statusText.setFillColor(sf::Color::Yellow);
    statusText.setPosition(padding, window.getSize().y - 25);

    // Panel headers
    sf::Text leftHeader("INSTRUCTIONS", font, 13);
    leftHeader.setFillColor(sf::Color::Cyan);
    leftHeader.setPosition(padding, mainHeaderHeight);
    
    sf::Text middleHeader("BUGGY CODE (READ ONLY)", font, 13);
    middleHeader.setFillColor(sf::Color::Red);
    middleHeader.setPosition(panelWidth + padding, mainHeaderHeight);
    
    sf::Text rightHeader("YOUR SOLUTION (EDITABLE)", font, 13);
    rightHeader.setFillColor(sf::Color::Green);
    rightHeader.setPosition(panelWidth * 2 + padding, mainHeaderHeight);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Mouse click - only allow clicking in solution panel
            if (event.type == sf::Event::MouseButtonPressed) {
                float mouseX = event.mouseButton.x;
                if (mouseX > panelWidth * 2) {
                    statusText.setString("Editing solution panel...");
                    statusText.setFillColor(sf::Color::Green);
                } else {
                    statusText.setString("You can only edit in the solution panel!");
                    statusText.setFillColor(sf::Color::Red);
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (isBlockedKey(event.key)) {
                    statusText.setString("Action blocked for security!");
                    statusText.setFillColor(sf::Color::Red);
                    continue;
                }
                
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                    continue;
                }
                
                // F5 to save solution
                if (event.key.code == sf::Keyboard::F5) {
                    std::string filename = "solution.c";
                    if (saveToFile(solutionCode, filename)) {
                        statusText.setString("Solution saved to " + filename);
                        statusText.setFillColor(sf::Color::Green);
                    } else {
                        statusText.setString("Failed to save file!");
                        statusText.setFillColor(sf::Color::Red);
                    }
                    continue;
                }
                
                // F6 to clear solution panel
                if (event.key.code == sf::Keyboard::F6) {
                    solutionCode = "// Write your corrected C code here:\n\n";
                    cursorPosition = solutionCode.length();
                    needsCursorUpdate = true;
                    statusText.setString("Solution panel cleared");
                    statusText.setFillColor(sf::Color::Yellow);
                    continue;
                }
                
                // Handle backspace (only for solution panel)
                if (event.key.code == sf::Keyboard::BackSpace) {
                    if (cursorPosition > 0) {
                        solutionCode.erase(cursorPosition - 1, 1);
                        cursorPosition--;
                        needsCursorUpdate = true;
                        statusText.setString("Editing...");
                        statusText.setFillColor(sf::Color::Yellow);
                    }
                    continue;
                }
                
                // Handle Enter
                if (event.key.code == sf::Keyboard::Enter) {
                    solutionCode.insert(cursorPosition, "\n");
                    cursorPosition++;
                    needsCursorUpdate = true;
                    continue;
                }
                
                // Handle Tab for indentation
                if (event.key.code == sf::Keyboard::Tab && !event.key.shift) {
                    solutionCode.insert(cursorPosition, "    ");
                    cursorPosition += 4;
                    needsCursorUpdate = true;
                    continue;
                }
                
                // Arrow keys for cursor movement
                if (event.key.code == sf::Keyboard::Left && cursorPosition > 0) {
                    cursorPosition--;
                    needsCursorUpdate = true;
                    continue;
                }
                if (event.key.code == sf::Keyboard::Right && cursorPosition < solutionCode.length()) {
                    cursorPosition++;
                    needsCursorUpdate = true;
                    continue;
                }
                
                // Improved Up/Down arrow handling
                if (event.key.code == sf::Keyboard::Up) {
                    std::string textBeforeCursor = solutionCode.substr(0, cursorPosition);
                    size_t currentLineStart = textBeforeCursor.find_last_of('\n');
                    if (currentLineStart == std::string::npos) currentLineStart = 0;
                    else currentLineStart++;
                    
                    size_t columnPos = cursorPosition - currentLineStart;
                    
                    if (currentLineStart > 0) {
                        size_t prevLineEnd = currentLineStart - 1;
                        std::string textBeforePrevLine = solutionCode.substr(0, prevLineEnd);
                        size_t prevLineStart = textBeforePrevLine.find_last_of('\n');
                        if (prevLineStart == std::string::npos) prevLineStart = 0;
                        else prevLineStart++;
                        
                        size_t prevLineLength = prevLineEnd - prevLineStart;
                        cursorPosition = prevLineStart + std::min(columnPos, prevLineLength);
                        needsCursorUpdate = true;
                    }
                    continue;
                }
                
                if (event.key.code == sf::Keyboard::Down) {
                    size_t nextLineStart = solutionCode.find('\n', cursorPosition);
                    if (nextLineStart != std::string::npos) {
                        nextLineStart++;
                        
                        std::string textBeforeCursor = solutionCode.substr(0, cursorPosition);
                        size_t currentLineStart = textBeforeCursor.find_last_of('\n');
                        if (currentLineStart == std::string::npos) currentLineStart = 0;
                        else currentLineStart++;
                        
                        size_t columnPos = cursorPosition - currentLineStart;
                        
                        size_t nextLineEnd = solutionCode.find('\n', nextLineStart);
                        if (nextLineEnd == std::string::npos) nextLineEnd = solutionCode.length();
                        
                        size_t nextLineLength = nextLineEnd - nextLineStart;
                        cursorPosition = nextLineStart + std::min(columnPos, nextLineLength);
                        needsCursorUpdate = true;
                    }
                    continue;
                }
            }

            // Handle text input (only for solution panel)
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode >= 32 && event.text.unicode < 127) {
                    char inputChar = static_cast<char>(event.text.unicode);
                    solutionCode.insert(cursorPosition, 1, inputChar);
                    cursorPosition++;
                    needsCursorUpdate = true;
                    statusText.setString("Typing...");
                    statusText.setFillColor(sf::Color::Yellow);
                }
            }

            // Handle mouse wheel scrolling
            if (event.type == sf::Event::MouseWheelScrolled) {
                float mouseX = event.mouseWheelScroll.x;
                if (mouseX < panelWidth) {
                    leftScrollOffset += event.mouseWheelScroll.delta * scrollSpeed;
                } else if (mouseX < panelWidth * 2) {
                    middleScrollOffset += event.mouseWheelScroll.delta * scrollSpeed;
                } else {
                    rightScrollOffset += event.mouseWheelScroll.delta * scrollSpeed;
                    needsCursorUpdate = true;
                }
            }
        }

        // Cursor blinking with proper timing
        if (cursorClock.getElapsedTime().asMilliseconds() > 500) {
            showCursor = !showCursor;
            cursorClock.restart();
        }

        // Update text content only when necessary
        solutionText.setString(solutionCode);

        // Calculate text bounds for proper scrolling
        sf::FloatRect leftBounds = instructionText.getLocalBounds();
        sf::FloatRect middleBounds = codeText.getLocalBounds();
        sf::FloatRect rightBounds = solutionText.getLocalBounds();
        
        // Available content area height
        float contentAreaHeight = window.getSize().y - headerHeight - mainHeaderHeight - 60;
        
        // Clamp scroll offsets
        float leftMaxScroll = std::max(0.f, leftBounds.height - contentAreaHeight);
        float middleMaxScroll = std::max(0.f, middleBounds.height - contentAreaHeight);
        float rightMaxScroll = std::max(0.f, rightBounds.height - contentAreaHeight);
        
        leftScrollOffset = std::max(-leftMaxScroll, std::min(0.f, leftScrollOffset));
        middleScrollOffset = std::max(-middleMaxScroll, std::min(0.f, middleScrollOffset));
        rightScrollOffset = std::max(-rightMaxScroll, std::min(0.f, rightScrollOffset));

        // Set text positions
        float textStartY = mainHeaderHeight + headerHeight + 10;
        
        instructionText.setPosition(padding, textStartY + leftScrollOffset);
        codeText.setPosition(panelWidth + padding, textStartY + middleScrollOffset);
        solutionText.setPosition(panelWidth * 2 + padding, textStartY + rightScrollOffset);

        // Calculate cursor position with exact alignment to text
        if (needsCursorUpdate) {
            sf::Vector2f startPos = sf::Vector2f(panelWidth * 2 + padding, textStartY + rightScrollOffset);
            sf::Vector2f cursorPos = calculateCursorPosition(solutionCode, cursorPosition, font, 11, startPos, 1.2f);
            cursor.setPosition(cursorPos.x, cursorPos.y);
            needsCursorUpdate = false;
        }

        // Render
        window.clear(sf::Color(10, 10, 15));
        
        // Draw panel separators
        sf::RectangleShape separator1(sf::Vector2f(2, window.getSize().y));
        separator1.setPosition(panelWidth - 1, 0);
        separator1.setFillColor(sf::Color(60, 60, 60));
        
        sf::RectangleShape separator2(sf::Vector2f(2, window.getSize().y));
        separator2.setPosition(panelWidth * 2 - 1, 0);
        separator2.setFillColor(sf::Color(60, 60, 60));
        
        // Draw panel backgrounds
        sf::RectangleShape leftBg(sf::Vector2f(panelWidth - 5, window.getSize().y - 80));
        leftBg.setPosition(3, textStartY - 5);
        leftBg.setFillColor(sf::Color(20, 20, 30, 80));
        
        sf::RectangleShape middleBg(sf::Vector2f(panelWidth - 8, window.getSize().y - 80));
        middleBg.setPosition(panelWidth + 3, textStartY - 5);
        middleBg.setFillColor(sf::Color(30, 20, 20, 80));
        
        sf::RectangleShape rightBg(sf::Vector2f(panelWidth - 8, window.getSize().y - 80));
        rightBg.setPosition(panelWidth * 2 + 3, textStartY - 5);
        rightBg.setFillColor(sf::Color(20, 30, 20, 80));
        
        window.draw(leftBg);
        window.draw(middleBg);
        window.draw(rightBg);
        window.draw(separator1);
        window.draw(separator2);
        
        // Draw texts
        window.draw(headerText);
        window.draw(leftHeader);
        window.draw(middleHeader);
        window.draw(rightHeader);
        window.draw(instructionText);
        window.draw(codeText);
        window.draw(solutionText);
        window.draw(statusText);
        
        // Draw cursor perfectly aligned with text
        if (showCursor) {
            window.draw(cursor);
        }

        window.display();
    }

    // Cleanup
    UnregisterHotKey(hwnd, 1);
    UnregisterHotKey(hwnd, 2);
    UnregisterHotKey(hwnd, 3);

    return 0;
}