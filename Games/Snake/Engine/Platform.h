#ifndef PLATFORM_H
#define PLATFORM_H

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <mutex>
#include "GameObject.h"

/**
* A class for platforms, a type of RectangleShape.
*/
class Platform : public GameObject, public sf::RectangleShape
{
private:
    /**
    * Should the player pass through the Platform when jumping up into it.
    */
    bool passthrough;

    /**
    * Inner mutex for making inner-object actions thread safe
    */
    std::mutex innerMutex;

    //SCRIPTING STUFF

    v8::Isolate* isolate;
    v8::Global<v8::Context>* context;

    static void setPlatformGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void getPlatformGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type

    static void setPlatformX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void getPlatformX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type
    static void setPlatformY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void getPlatformY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type

    
public:
    /**
    * object type of Platform
    */
    static const int objectType = 2;

    /**
    * Constructs a new platform (empty RectangleShape)
    */
    Platform();
    /**
    * Constructs a new platform, setting GameObject appropriately.
    */
    Platform(bool stationary, bool collidable, bool drawable);

    /**
    * Set the passthrough setting on this platform.
    */
    void setPassthrough(bool passthrough);

    /**
    * Move the platform (thread safe)
    */
    void move(float offsetX, float offsetY);
    /**
    * Move the platform (thread safe)
    */
    void move(const sf::Vector2f offset);
    /**
    * set the position of the platform (thread safe);
    */
    void setPosition(float x, float y);
    /**
    * set the position of the platform (thread safe);
    */
    void setPosition(const sf::Vector2f position);
    /**
    * Return the position of the platform (thread safe)
    */
    sf::Vector2f getPosition();
    /**
    * Return the bounding box of the platform (thread safe)
    */
    sf::FloatRect getGlobalBounds();
    /**
    *Return the pointer to the inner mutex. Useful for sublasses.
    */
    std::mutex* getMutex();

    /**
    * Create a string representation of the class. To work with GameWindow, this should match with constructSelf
    */
    std::string toString() override;

    /**
    * Creates a new object using the string provided. To work with GameWindow, this should match toString.
    */
    std::shared_ptr<GameObject> constructSelf(std::string self) override;

    /**
    * Creates a blank GameObject. Useful for when you need a copy of the object without knowing what type it is.
    */
    std::shared_ptr<GameObject> makeTemplate() override;

    /**
    * Return the type of the class. Type should be a static const variable so that it is constant across all versions.
    * type should be unique for each version of GameObject that you use with GameWindow.
    */
    int getObjectType() override;

    //Script Stuff

    std::string guid;

    /**
     * This function will make this class instance accessible to scripts in
     * the given context.
     *
     * IMPORTANT: Please read this definition of this function in
     * GameObject.cpp. The helper function I've provided expects certain
     * parameters which you must use in order to take advance of this
     * convinience.
     */
    v8::Local<v8::Object> exposeToV8(v8::Isolate* isolate, v8::Local<v8::Context>& context, std::string context_name = "default") override;
};

#endif