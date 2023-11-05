#ifndef EVENT_H
#define EVENT_H
#include "GameObject.h"
#include "GameWindow.h"
#include <unordered_map>

class Event : public GameObject {

public:

	Event();

	static const int objectType = 7;
	struct variant {
		enum Type {
			TYPE_INT,
			TYPE_FLOAT,
			TYPE_BOOLP,
			TYPE_GAMEOBJECT,
			TYPE_GAMEWINDOW
		};
		variant::Type m_Type;
		union {
			int m_asInt;
			float m_asFloat;
			bool* m_asBoolP;
			GameObject* m_asGameObject;
			GameWindow* m_asGameWindow;
		};
	};

	int priority = 0;

	std::string type;

	std::unordered_map<std::string, variant> parameters;

	std::string toString() override;

	std::shared_ptr<GameObject> constructSelf(std::string self) override;

	std::shared_ptr<GameObject> makeTemplate() override;

	int getObjectType() override;
};
#endif
