#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <mutex>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <v8.h>
#include "v8helpers.h"
#define OBJECT 0

class GameObject {

private:
    bool stationary = true;

    bool collidable = false;

    bool drawable = false;

    //SCRIPTING STUFF

    /** Static variable to keep count of current number of GameObject instances */
    static int current_guid;

    v8::Isolate* isolate;
    v8::Global<v8::Context>* context;

    static void setGameObjectGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void getGameObjectGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info); // note return type

public:

    static std::mutex innerMutex;

    static const int objectType = 0;



	GameObject(bool stationary, bool collidable, bool drawable);

    /**
    * Check if the object is static (Doesn't move or doesn't get send from the server)
    */
    bool isStatic();

    /**
    * Bool value for if it is drawable (inherits sf::drawable)
    */
    bool isDrawable();

    /**
    * Check if the object is collidable (inherits sf::Shape or sf::Shape)
    */
    bool isCollidable();

    /**
    * Set the collidable status of the object. 
    */
    void setCollidable(bool collidable);

    /**
    * Create a string representation of the class. To work with GameWindow, this should match with constructSelf
    */
    virtual std::string toString();

    /**
    * Creates a new object using the string provided. To work with GameWindow, this should match toString.
    */
    virtual std::shared_ptr<GameObject> constructSelf(std::string self);

    /**
    * Creates a blank GameObject. Useful for when you need a copy of the object without knowing what type it is.
    */
    virtual std::shared_ptr<GameObject> makeTemplate();

    /**
    * Return the type of the class. Type should be a static const variable so that it is constant across all versions.
    * type should be unique for each version of GameObject that you use with GameWindow.
    */
    virtual int getObjectType();

//SCRIPTING STUFF

    //~GameObject();

    std::string guid;

    /**
         * Static function to keep track of current total number of
         * gameobjects, to facilitate creating GUIDs by incrementing a counter.
         */
    static int *getCurrentGUID();

    /**
     * This function will make this class instance accessible to scripts in
     * the given context.
     *
     * IMPORTANT: Please read this definition of this function in
     * GameObject.cpp. The helper function I've provided expects certain
     * parameters which you must use in order to take advance of this
     * convinience.
     */
    virtual v8::Local<v8::Object> exposeToV8(v8::Isolate* isolate, v8::Local<v8::Context>& context, std::string context_name = "default");

    /**
     * Static function to keep track of current total number of
     * gameobjects, to facilitate creating GUIDs by incrementing a counter.
     */
    //static int getCurrentGUID();

    /**
     * Shared list of all instances of game objects...auto populated by the
     * constructor.
     */
    static std::vector<GameObject*> game_objects;

    /**
     * Factor methods for creating new instances of GameObjects.
     *
     * This is primarily a demonstration of how you can create new objects
     * in javascript. NOTE: this is embedded in this GameObject class, but
     * could (and likely should) be used for other functionality as well
     * (i.e., Events).
     */
    //static GameObject* GameObjectFactory(std::string context_name = "default");
    static void ScriptedGameObjectFactory(const v8::FunctionCallbackInfo<v8::Value>& args);

};

#endif
