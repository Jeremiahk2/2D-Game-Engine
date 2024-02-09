#include "GOS.h"

GOS::GOS() {
	contents = (GameObject::ObjectStruct *)malloc(INITIAL_CAPACITY);
}

void GOS::doubleCapacity() {
	contents = (GameObject::ObjectStruct *)realloc(contents, capacity * 2);
	capacity *= 2;
}

void GOS::addStruct(std::shared_ptr<GameObject::ObjectStruct> objectStruct) {
	std::shared_ptr<GameObject> templatePtr;
	try {
		templatePtr = map.at(objectStruct->type);
	}
	catch (std::out_of_range) {
		std::cout << "Template not found" << std::endl;
	}

	int size = templatePtr->getStructSize();

	while (this->size + size > capacity) {
		doubleCapacity();
	}

	memcpy(contents + last, objectStruct.get(), size);
	last += size;
}

void GOS::addTemplate(std::shared_ptr<GameObject> plate) {
	map.insert({plate->getType(), plate});
}
