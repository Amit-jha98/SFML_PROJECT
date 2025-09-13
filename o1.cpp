#define SFML_STATIC
#include <SFML/Graphics.hpp>
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#include <Windows.h>

int main() {
    sf::RenderWindow window(sf::VideoMode(900, 700), "TechMantra1.0 -Web App dev Instructions");

    // Block screenshots
    HWND hwnd = window.getSystemHandle();
    SetWindowDisplayAffinity(hwnd, WDA_MONITOR);

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/calibri.ttf")) {
        return -1; // Exit if font not found
    }

    // Story + Instructions (long text)
    std::string instructions =
        "Photographer Portfolio Web App\n\n"
        "Imagine this: A young photographer named Aditi has just won a local\n"
        "contest and people are asking for her portfolio online. She only has\n"
        "1 hour before her next shoot, and she asks YOU to help.\n\n"
        "Your job: Build a simple portfolio web app so Aditi can:\n"
        " - Showcase her best photos.\n"
        " - Receive messages from interested clients.\n"
        " - Edit her profile quickly (bio, name, images).\n\n"
        "What you need to do (Critical Thinking Required):\n"
        "1. Setup:\n"
        "   - Use HTML, CSS, JS (or React if preferred).\n"
        "   - Keep photos local for speed.\n\n"
        "2. Core Features:\n"
        "   - Home Page -> Photographer name, tagline, profile picture.\n"
        "   - Gallery -> Grid of 6-8 images.\n"
        "   - Contact Form -> Name, Email, Message -> send to backend.\n"
        "   - Profile Management -> Edit info + photos via backend.\n\n"
        "3. Backend Options (choose one):\n"
        "   - Node.js + Express -> /profile & /contact routes.\n"
        "   - OR Python Flask -> server.py with same endpoints.\n"
        "   - Store profile in memory (no DB needed).\n\n"
        "4. If time permits:\n"
        "   - Make it responsive (CSS Grid/Flexbox).\n"
        "   - Add social media links.\n\n"
        "Goal: In 1 hour, create a working portfolio where visitors can\n"
        "see photos, send a message, and Aditi can update her profile.\n\n"
        "Remember: The story is simple, but the real test is how YOU\n"
        "analyze the needs, design the flow, and build a working solution.\n\n";
        

    sf::Text text(instructions, font, 19);
    text.setFillColor(sf::Color::White);
    text.setPosition(20, 20);

    float scrollOffset = 0.f;          // Vertical scroll offset
    const float scrollSpeed = 30.f;    // Pixels per scroll step

    // Get text height (to clamp scrolling)
    sf::FloatRect textBounds = text.getLocalBounds();
    float textHeight = textBounds.height + 40; // include padding
    float windowHeight = window.getSize().y;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Mouse wheel scrolling
            // Mouse wheel scrolling (reversed direction)
            if (event.type == sf::Event::MouseWheelScrolled) {
                scrollOffset += event.mouseWheelScroll.delta * scrollSpeed;
            }


            // Keyboard scrolling
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Down)
                    scrollOffset -= scrollSpeed;
                if (event.key.code == sf::Keyboard::Up)
                    scrollOffset += scrollSpeed;
            }
        }

        // Clamp scroll offset so it doesn’t scroll infinitely
        if (scrollOffset > 0)
            scrollOffset = 0;
        if (scrollOffset < -(textHeight - windowHeight))
            scrollOffset = -(textHeight - windowHeight);

        // Apply scroll offset
        text.setPosition(20, 20 + scrollOffset);

        window.clear(sf::Color(25, 25, 25)); // Dark background
        window.draw(text);
        window.display();
    }

    return 0;
}


