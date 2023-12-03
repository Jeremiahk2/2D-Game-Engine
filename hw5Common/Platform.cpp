#include "Platform.h"

Platform::Platform() : sf::RectangleShape(), GameObject(true, true, true) {
    guid = "platform" + std::to_string(*GameObject::getCurrentGUID());
    (*GameObject::getCurrentGUID())++;
    game_objects.push_back(this);
}

Platform::Platform(bool stationary, bool collidable, bool drawable) : sf::RectangleShape(), GameObject(stationary, collidable, drawable)
{
    passthrough = false;

    guid = "platform" + std::to_string(*GameObject::getCurrentGUID());
    (*GameObject::getCurrentGUID())++;
    game_objects.push_back(this);
}

void Platform::setPassthrough(bool passthrough) {
    this->passthrough = passthrough;
}

void Platform::move(float offsetX, float offsetY) {
    float roundedX = std::roundf(100 * offsetX) / 100.f;
    float roundedY = std::roundf(100 * offsetY) / 100.f;
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::RectangleShape::move(roundedX, roundedY);
}

void Platform::move(const sf::Vector2f offset) {
    float roundedX = std::roundf(100 * offset.x) / 100.f;
    float roundedY = std::roundf(100 * offset.y) / 100.f;
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::RectangleShape::move(roundedX, roundedY);
}

void Platform::setPosition(float x, float y) {
    float roundedX = std::roundf(100 * x) / 100.f;
    float roundedY = std::roundf(100 * y) / 100.f;
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::RectangleShape::setPosition(roundedX, roundedY);
}

void Platform::setPosition(const sf::Vector2f position) {
    float roundedX = std::roundf(100 * position.x) / 100.f;
    float roundedY = std::roundf(100 * position.y) / 100.f;
    std::lock_guard<std::mutex> lock(innerMutex);
    sf::RectangleShape::setPosition(roundedX, roundedY);
}

sf::Vector2f Platform::getPosition() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return sf::RectangleShape::getPosition();
}

sf::FloatRect Platform::getGlobalBounds() {
    std::lock_guard<std::mutex> lock(innerMutex);
    return sf::RectangleShape::getGlobalBounds();
}

std::mutex* Platform::getMutex() {
    return &innerMutex;
}

/**
    * Create a string representation of the class. To work with GameWindow, this should match with constructSelf
    */
std::string Platform::toString() {
    std::stringstream stream;
    char space = ' ';

    stream << getObjectType() << space << getPosition().x << space << getPosition().y << space << getSize().x << space << getSize().y
        << space << (int)getFillColor().r << space << (int)getFillColor().g << space << (int)getFillColor().b;
    std::string line;
    std::getline(stream, line);
    return line;
}

/**
* Creates a new object using the string provided. To work with GameWindow, this should match toString.
*/
std::shared_ptr<GameObject> Platform::constructSelf(std::string self) {
    Platform* c = new Platform;
    int type;
    float x;
    float y;
    float sizeX;
    float sizeY;
    int r;
    int g;
    int b;
    int matches = sscanf_s(self.data(), "%d %f %f %f %f %d %d %d", &type, &x, &y, &sizeX, &sizeY, &r, &g, &b);
    if (matches != 8 || type != getObjectType()) {
        throw std::invalid_argument("Type was not correct for character or string was formatted wrong.");
    }

    c->setPosition(x, y);
    sf::Vector2f size(sizeX, sizeY);
    c->setSize(size);
    sf::Color color(r, g, b);
    c->setFillColor(color);

    GameObject* go = (GameObject*)c;
    std::shared_ptr<GameObject> ptr(go);
    return ptr;
}

/**
* Creates a blank GameObject. Useful for when you need an instance of the object without knowing what type it is.
*/
std::shared_ptr<GameObject> Platform::makeTemplate() {
    std::shared_ptr<GameObject> ptr(new Platform);
    return ptr;
}

/**
* Return the type of the class. Type should be a static const variable so that it is constant across all versions.
* type should be unique for each version of GameObject that you use with GameWindow.
*/
int Platform::getObjectType() {
    return this->objectType;
}

//Script stuff

void Platform::setPlatformGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    v8::String::Utf8Value utf8_str(info.GetIsolate(), value->ToString(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
    static_cast<Platform*>(ptr)->guid = *utf8_str;
}

void Platform::getPlatformGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    std::string guid = static_cast<Platform*>(ptr)->guid;
    v8::Local<v8::String> v8_guid = v8::String::NewFromUtf8(info.GetIsolate(), guid.c_str()).ToLocalChecked();
    info.GetReturnValue().Set(v8_guid);
}

void Platform::setPlatformX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<Platform*>(ptr)->setPosition(value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked(), static_cast<Platform*>(ptr)->getPosition().y);
}

void Platform::getPlatformX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    float x_val = static_cast<Platform*>(ptr)->getPosition().x;
    info.GetReturnValue().Set(x_val);
}

void Platform::setPlatformY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<Platform*>(ptr)->setPosition(static_cast<Platform*>(ptr)->getPosition().x, value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked());
}

void Platform::getPlatformY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    float y_val = static_cast<Platform*>(ptr)->getPosition().y;
    info.GetReturnValue().Set(y_val);
}

/**
 * IMPORTANT: Pay close attention to the definition of the std::vector in this
 * example implementation. The v8helpers::expostToV8 will assume you have
 * instantiated this exact type of vector and passed it in. If you don't the
 * helper function will not work.
 */
v8::Local<v8::Object> Platform::exposeToV8(v8::Isolate* isolate, v8::Local<v8::Context>& context, std::string context_name)
{
    std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
    v.push_back(v8helpers::ParamContainer("guid", getPlatformGUID, setPlatformGUID));
    v.push_back(v8helpers::ParamContainer("x", getPlatformX, setPlatformX));
    v.push_back(v8helpers::ParamContainer("y", getPlatformY, setPlatformY));
    return v8helpers::exposeToV8(guid, this, v, isolate, context, context_name);
}