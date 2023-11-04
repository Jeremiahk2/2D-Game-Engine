#ifndef EVENT_H
#define EVENT_H
#include "GameObject.h"

class Event : public GameObject {

public:

	Event();

	static const int objectType = 7;
	struct variant {
		enum Type {
			TYPE_INT,
			TYPE_FLOAT,
			TYPE_GAMEOBJECT
		};
		variant::Type m_Type;
		union {
			int m_asInt;
			float m_asFloat;
			GameObject* m_asGameObject;
		};
	};

	std::map<std::string, variant> paramaters;

	std::string toString() override;

	std::shared_ptr<GameObject> constructSelf(std::string self) override;

	std::shared_ptr<GameObject> makeTemplate() override;

	int getObjectType() override;
};
#endif
