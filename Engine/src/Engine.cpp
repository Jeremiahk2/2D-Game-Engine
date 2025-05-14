// Engine.cpp : Defines the entry point for the application.
//


#include "Engine.h"
#include <optional>

using namespace std;

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 200, 200 }), "SFML works!", sf::Style::None);
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
}