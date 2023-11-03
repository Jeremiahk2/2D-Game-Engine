#include "GameObject.h"

GameObject::GameObject(bool stationary, bool collidable, bool drawable)
{
    this->stationary = stationary;
    this->collidable = collidable;
    this->drawable = drawable;
}

bool GameObject::isStatic() {
    return stationary;
}

bool GameObject::isCollidable()
{
    return collidable;
}

bool GameObject::isDrawable() {
    return drawable;
}

void GameObject::setCollidable(bool collidable)
{
    this->collidable = collidable;
}

int GameObject::getObjectType()
{
    return objectType;
}

