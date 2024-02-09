#include "CBox.h"

CBox::CBox() {
    object = NULL;

}

CBox::CBox(GameObject *object) {
    this->object = object;
}

bool CBox::isStatic() {
    return object->isStatic();
}

//sf::FloatRect CBox::getCBox() {
//    return object->getGlobalBounds();
//}

GameObject *CBox::getObject() {
    return object;
}