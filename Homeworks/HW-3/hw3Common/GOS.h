#ifndef GOS_H
#define GOS_H
#define INITIAL_CAPACITY 10

#include "GameObject.h"
#include <map>
#include <iostream>
#include <memory>
#include <iterator>
#include <cstddef>

class GOS {
private:
	std::map<int, std::shared_ptr<GameObject>> map;

	GameObject::ObjectStruct* contents;

	int last = 0;
	
	int size = 0;

	int capacity = INITIAL_CAPACITY;

	void doubleCapacity();

public:
	GOS();

	void addStruct(std::shared_ptr<GameObject::ObjectStruct> objectStruct);

	void addTemplate(std::shared_ptr<GameObject> plate);

	GameObject::ObjectStruct* getdata();

};

#endif
