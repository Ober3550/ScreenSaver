#include "imgui.h"
#include "imgui-SFML.h"
#include "zlib.h"

#include <SFML/Graphics.hpp>
#include <iostream>

static sf::Texture logo;
class ScreenSaver {
public:
    ScreenSaver() {
        sprite.setTexture(logo);
    }
    void draw(sf::RenderWindow& window) {
        if (active) {
            window.draw(sprite);
        }
    }
    void update(sf::RenderWindow& window) {
        // Artificially reduce the frequency of updating the frames
        if (updateNum >= updateFreq) {
            sf::Vector2u windowSize = window.getSize();
            sf::Vector2f current = sprite.getPosition();
            // Reset outside of the frame
            if (frame >= reset) {
                current.x = float(windowSize.x) / -2.0 + startPos.x;
                current.y = float(windowSize.y) / -2.0 + startPos.y;
                frame = 0;
            }
            else {
                // If velocity plus current position moves outside the screen change the direction
                // But don't change the direction if it's moving onto the screen
                if ((current.x + velocity.x < float(windowSize.x) / -2.0 && velocity.x < 0)|| 
                    (current.x + velocity.x + logoSize.x > float(windowSize.x) / 2.0 && velocity.x > 0)) {
                    velocity.x *= -1;
                }
                if ((current.y + velocity.y < float(windowSize.y) / -2.0 && velocity.y < 0) || 
                    (current.y + velocity.y + logoSize.y > float(windowSize.y) / 2.0 && velocity.y > 0)) {
                    velocity.y *= -1;
                }
                // Apply the velocity to the position
                current.x += velocity.x;
                current.y += velocity.y;
            }
            sprite.setPosition(current);
            frame++;
            updateNum = 0;
        }
        updateNum++;
    }
    // Some getters and setters 
    // To show or hide the screensaver
    void setInactive() {
        active = false;
    }
    void setActive() {
        active = true;
    }
    bool getActive() {
        return active;
    }
private:
    bool active = true;
    sf::Sprite   sprite;
    sf::Vector2f logoSize      = { 100,100 };
    sf::Vector2f startPos      = { -100,-100 };
    sf::Vector2f startVelocity = { 5, 6 };
    sf::Vector2f velocity = startVelocity;
    int updateNum  = 0;
    int updateFreq = 0;
    int reset      = 200;
    int frame      = reset; // Reset on the first frame
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(500, 500), "ScreenSaver");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    
    // Views are SFML's rendering ports 
    // and need to be resized when the window resizes
    sf::View centreView;
    sf::Vector2u size = window.getSize();
    centreView.setSize(sf::Vector2f(size.x, size.y));
    centreView.setCenter(0, 0);
    window.setView(centreView);

    logo.loadFromFile("assets/logo.png");
    ScreenSaver screensaver = ScreenSaver();
    sf::Clock deltaClock;
    while (window.isOpen()) {
        window.clear();
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Resized) {
                size = window.getSize();
                centreView.setSize(sf::Vector2f(size.x, size.y));
                centreView.setCenter(0, 0);
                window.setView(centreView);
            }
            else if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());
        //ImGui::ShowDemoWindow();
        screensaver.update(window);
        screensaver.draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}