#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"
#include <nlohmann/json.hpp>
#include <mutex>
#include <string>
using json = nlohmann::json;

class GameObject : public sf::RectangleShape {

private:
    std::mutex *innerMutex;

    bool updates = false;

public:
	GameObject(bool updates);

    void move(float offsetX, float offsetY);

    void move(const sf::Vector2f offset);

    void setPosition(float x, float y);

    void setPosition(const sf::Vector2f position);

    sf::Vector2f getPosition();

    sf::FloatRect getGlobalBounds();

	virtual std::string toString();

    bool doesUpdate();

    std::mutex* getMutex();

    void setUpdates(bool newValue);
};
