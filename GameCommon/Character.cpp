#include "Character.h"

Character::Character() : GameObject(false, false, true), Sprite() {
    /**
    ART FOR SANTA PROVIDED BY Marco Giorgini THROUGH OPENGAMEART.ORG
    License: CC0 (Public Domain) @ 12/28/21
    https://opengameart.org/content/santas-rats-issue-c64-sprites
    */
    if (!texture.loadFromFile("Santa_standing.png")) {
        std::cout << "Failed";
    }
    setTexture(texture);
    setScale(.1f, .1f);
    setSpeed(CHAR_SPEED);
    setGravity(GRAV_SPEED);
    std::lock_guard<std::mutex> lock(innerMutex);
    guid = "character" + std::to_string(*GameObject::getCurrentGUID());
    (*GameObject::getCurrentGUID())++;
    game_objects.push_back(this);
}

Character::Character(bool stationary, bool collidable, bool drawable) : GameObject(stationary, collidable, drawable), Sprite(){
    /**
    ART FOR SANTA PROVIDED BY Marco Giorgini THROUGH OPENGAMEART.ORG
    License: CC0 (Public Domain) @ 12/28/21
    https://opengameart.org/content/santas-rats-issue-c64-sprites
    */
    sf::Texture charTexture;
    if (!charTexture.loadFromFile("Santa_standing.png")) {
        std::cout << "Failed";
    }
    setTexture(charTexture);
    setSpeed(CHAR_SPEED);
    setGravity(GRAV_SPEED);
    std::lock_guard<std::mutex> lock(innerMutex);
    guid = "character" + std::to_string(*GameObject::getCurrentGUID());
    (*GameObject::getCurrentGUID())++;
    game_objects.push_back(this);
}

bool Character::isConnecting() {
    return connecting != 0;
}

void Character::setConnecting(int connecting) {
    this->connecting = connecting;
}

void Character::move(float offsetX, float offsetY) {
    float roundedX = std::roundf(100 * offsetX) / 100.f;
    float roundedY = std::roundf(100 * offsetY) / 100.f;
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::Sprite::move(offsetX, offsetY);
}

void Character::move(const sf::Vector2f offset) {
    float roundedX = std::roundf(100 * offset.x) / 100.f;
    float roundedY = std::roundf(100 * offset.y) / 100.f;
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::Sprite::move(offset);
}

void Character::setPosition(float x, float y) {
    float roundedX = std::roundf(100 * x) / 100.f;
    float roundedY = std::roundf(100 * y) / 100.f;
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::Sprite::setPosition(x, y);
}

void Character::setPosition(const sf::Vector2f position) {
    float roundedX = std::roundf(100 * position.x) / 100.f;
    float roundedY = std::roundf(100 * position.y) / 100.f;
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::Sprite::setPosition(position);
}

sf::Vector2f Character::getPosition() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return sf::Sprite::getPosition();
}

sf::FloatRect Character::getGlobalBounds() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return sf::Sprite::getGlobalBounds();
}

sf::Vector2f Character::getSpeed() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return speed;
}

void Character::setSpeed(float speed) {
    std::lock_guard<std::mutex> lock(innerMutex);
    this->speed.x = speed;
}

bool Character::isJumping() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return jumping;
}

bool Character::isDead()
{
    return dead;
}

void Character::died()
{
    this->dead = true;
}

void Character::setJumping(bool jump) {
    std::lock_guard<std::mutex> lock(innerMutex);
    this->jumping = jump;
}

float Character::getGravity() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return gravity;
}

void Character::setGravity(float gravity) {
    std::lock_guard<std::mutex> lock(innerMutex);
    this->gravity = gravity;
}

void Character::setID(int id) {
    std::lock_guard<std::mutex> lock(innerMutex);
    this->id = id;
}

int Character::getID() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return id;
}

void Character::setSpawnPoint(SpawnPoint spawn)
{
    this->spawn = spawn;
}

void Character::respawn() {
    setPosition(spawn.getPosition());
    dead = false;
}

int Character::getObjectType() {
    return this->objectType;
}

float Character::getJumpSpeed()
{
    return jumpSpeed;
}

int Character::getJumpTime()
{
    return jumpTime;
}

std::string Character::toString()
{
    std::stringstream stream;
    char space = ' ';

    stream << getObjectType() << space << isConnecting() << space << getID() << space << getPosition().x << space << getPosition().y;
    std::string line;
    std::getline(stream, line);
    return line;
}

std::shared_ptr<GameObject> Character::constructSelf(std::string self)
{

    Character *c = new Character;
    int type;
    int connecting;
    int id;
    float x;
    float y;

    int matches = sscanf_s(self.data(), "%d %d %d %f %f", &type, &connecting, &id, &x, &y);
    if (matches != 5 || type != getObjectType()) {
        throw std::invalid_argument("Type was not correct for character or string was formatted wrong.");
    }


    c->setPosition(x, y);
    c->setConnecting(connecting);
    c->setID(id);

    std::shared_ptr<GameObject> ptr(c);
    return ptr;
}

std::shared_ptr<GameObject> Character::makeTemplate()
{
    std::shared_ptr<GameObject> ptr(new Character);
    return ptr;
}


// ############################ SCRIPTING STUFF ################################################################

/** Definitions of static class members */


/**
 * IMPORTANT: Pay close attention to the definition of the std::vector in this
 * example implementation. The v8helpers::expostToV8 will assume you have
 * instantiated this exact type of vector and passed it in. If you don't the
 * helper function will not work.
 */
v8::Local<v8::Object> Character::exposeToV8(v8::Isolate* isolate, v8::Local<v8::Context>& context, std::string context_name)
{
    std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
    v.push_back(v8helpers::ParamContainer("guid", getCharacterGUID, setCharacterGUID));
    v.push_back(v8helpers::ParamContainer("x", getCharacterX, setCharacterX));
    v.push_back(v8helpers::ParamContainer("y", getCharacterY, setCharacterY));
    return v8helpers::exposeToV8(guid, this, v, isolate, context, context_name);
}

/**
 * Implementations of static setter and getter functions
 *
 * IMPORTANT: These setter and getter functions will set and get values of v8
 * callback data structures. Note their return type is void regardless of
 * whether they are setter or getter.
 *
 * Also keep in mind that the function signature must match this exactly in
 * order for v8 to accept these functions.
 */

void Character::setCharacterGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    v8::String::Utf8Value utf8_str(info.GetIsolate(), value->ToString(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
    static_cast<Character*>(ptr)->guid = *utf8_str;
}

void Character::getCharacterGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    std::string guid = static_cast<Character*>(ptr)->guid;
    v8::Local<v8::String> v8_guid = v8::String::NewFromUtf8(info.GetIsolate(), guid.c_str()).ToLocalChecked();
    info.GetReturnValue().Set(v8_guid);
}

void Character::setCharacterX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<Character*>(ptr)->setPosition(value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked(), static_cast<Character*>(ptr)->getPosition().y);
}

void Character::getCharacterX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    float x_val = static_cast<Character*>(ptr)->getPosition().x;
    info.GetReturnValue().Set(x_val);
}

void Character::setCharacterY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<Character*>(ptr)->setPosition(static_cast<Character*>(ptr)->getPosition().x, value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked());
}

void Character::getCharacterY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    float y_val = static_cast<Character*>(ptr)->getPosition().y;
    info.GetReturnValue().Set(y_val);
}

/**
 * Factory method for allowing javascript to create instances of native game
 * objects
 *
 * NOTE: Like with the setters above, this static function does have a return
 * type (and object), but the return value is placed in the function callback
 * parameter, not the native c++ return type.
 */
void Character::ScriptedGameObjectFactory(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::EscapableHandleScope handle_scope(args.GetIsolate());
    v8::Context::Scope context_scope(context);

    std::string context_name("default");
    if (args.Length() == 1)
    {
        v8::String::Utf8Value str(args.GetIsolate(), args[0]);
        context_name = std::string(v8helpers::ToCString(str));
#if GO_DEBUG
        std::cout << "Created new object in context " << context_name << std::endl;
#endif
    }
    GameObject* new_object = new Character(false, false, false);
    v8::Local<v8::Object> v8_obj = new_object->exposeToV8(isolate, context);
    args.GetReturnValue().Set(handle_scope.Escape(v8_obj));
}