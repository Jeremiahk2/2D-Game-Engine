#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class GameObject : public sf::RectangleShape {
public:
	GameObject();
	virtual void to_json(json& j, const GameObject& g) = 0;

	virtual void from_json(const json& j, GameObject& g) = 0;
private:
};
