#include "MovingPlatform.h"

    //Bound bound1 is the left/bottom bound. bound 2 is the right/top bound. Type is 0 for vertical, 1 for horizontal.
    MovingPlatform::MovingPlatform(float speed, bool type, float startx, float starty) : Platform(false, true, true)
    {
        startPos = sf::Vector2f(startx, starty);
        setPosition(startPos);
        m_type = type;
        if (type) {
            this->speed = sf::Vector2f(speed, 0);
        }
        else {
            this->speed = sf::Vector2f(0, speed);
        }
        bound1 = 0;
        bound2 = 0;

        guid = "moving" + std::to_string(*GameObject::getCurrentGUID());
        (*GameObject::getCurrentGUID())++;
        game_objects.push_back(this);
    }

    MovingPlatform::MovingPlatform() : Platform(false, true, true) {
        bound1 = 0;
        bound2 = 0;
        m_type = 0;

        guid = "moving" + std::to_string(*GameObject::getCurrentGUID());
        (*GameObject::getCurrentGUID())++;
        game_objects.push_back(this);
    }

    void MovingPlatform::setSpeed(sf::Vector2f speed) {
        std::lock_guard<std::mutex> lock(*getMutex());
        this->speed = speed;
    }

    void MovingPlatform::setMovementType(int movementType)
    {
        std::lock_guard<std::mutex> lock(*getMutex());
        this->m_type = movementType;
    }

    sf::Vector2f MovingPlatform::getSpeedVector() {
        std::lock_guard<std::mutex> lock(*getMutex());
        return speed;
    }

    float MovingPlatform::getSpeedValue() {
        std::lock_guard<std::mutex> lock(*getMutex());
        if (getMovementType()) {
            return speed.x;
        }
        return speed.y;
    }

    bool MovingPlatform::getMovementType() {
        return m_type;
    }

    sf::Vector2f MovingPlatform::getStartPos() {
        return startPos;
    }

    void MovingPlatform::setBounds(int bound1, int bound2) {
        std::lock_guard<std::mutex> lock(*getMutex());
        this->bound1 = bound1;
        this->bound2 = bound2;
    }

    sf::Vector2i MovingPlatform::getBounds() {
        std::lock_guard<std::mutex> lock(*getMutex());
        return sf::Vector2i(bound1, bound2);
    }
    void MovingPlatform::move(float x, float y) {
        float roundedX = std::roundf(100 * x) / 100.f;
        float roundedY = std::roundf(100 * y) / 100.f;
        Platform::move(roundedX, roundedY);
        lastMove = sf::Vector2f(roundedX, roundedY);
    }

    void MovingPlatform::move(sf::Vector2f v) {
        Platform::move(v);
        lastMove = v;
    }

    sf::Vector2f MovingPlatform::getLastMove() {
        std::lock_guard<std::mutex> lock(*getMutex());
        return lastMove;
    }

    std::string MovingPlatform::toString()
    {
        std::stringstream stream;
        char space = ' ';

        stream << getObjectType() << space << getPosition().x << space << getPosition().y << space << getSize().x << space << getSize().y
            << space << (int)getFillColor().r << space << (int)getFillColor().g << space << (int)getFillColor().b << space << getMovementType() 
            << space << getSpeedValue();
        std::string line;
        getline(stream, line);

        return line;
    }

    std::shared_ptr<GameObject> MovingPlatform::constructSelf(std::string self)
    {
        MovingPlatform* c = new MovingPlatform;
        int type;
        float x;
        float y;
        float sizeX;
        float sizeY;
        int r;
        int g;
        int b;
        int movementType;
        float speed;
        int matches = sscanf_s(self.data(), "%d %f %f %f %f %d %d %d %d %f", &type, &x, &y, &sizeX, &sizeY, &r, &g, &b, &movementType, &speed);
        if (matches != 10 || type != getObjectType()) {
            throw std::invalid_argument("Type was not correct for character or string was formatted wrong.");
        }

        c->setPosition(x, y);
        sf::Vector2f size(sizeX, sizeY);
        c->setSize(size);
        sf::Color color(r, g, b);
        c->setFillColor(color);
        c->setMovementType(movementType);
        if (movementType) {
            c->setSpeed(sf::Vector2f(speed, 0));
        }
        else {
            c->setSpeed(sf::Vector2f(0, speed));
        }

        GameObject* go = (GameObject*)c;
        std::shared_ptr<GameObject> ptr(go);
        return ptr;
    }

    int MovingPlatform::getObjectType() {
        return MovingPlatform::objectType;
    }
    
    //Script stuff

    void MovingPlatform::setMPlatformGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
    {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        v8::String::Utf8Value utf8_str(info.GetIsolate(), value->ToString(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
        static_cast<MovingPlatform*>(ptr)->guid = *utf8_str;
    }

    void MovingPlatform::getMPlatformGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        std::string guid = static_cast<MovingPlatform*>(ptr)->guid;
        v8::Local<v8::String> v8_guid = v8::String::NewFromUtf8(info.GetIsolate(), guid.c_str()).ToLocalChecked();
        info.GetReturnValue().Set(v8_guid);
    }

    void MovingPlatform::setMPlatformX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
    {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        static_cast<MovingPlatform*>(ptr)->setPosition(value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked(), static_cast<MovingPlatform*>(ptr)->getPosition().y);
    }

    void MovingPlatform::getMPlatformX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        float x_val = static_cast<MovingPlatform*>(ptr)->getPosition().x;
        info.GetReturnValue().Set(x_val);
    }

    void MovingPlatform::setMPlatformY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
    {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        static_cast<MovingPlatform*>(ptr)->setPosition(static_cast<MovingPlatform*>(ptr)->getPosition().x, value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked());
    }

    void MovingPlatform::getMPlatformY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        float y_val = static_cast<MovingPlatform*>(ptr)->getPosition().y;
        info.GetReturnValue().Set(y_val);
    }

    void MovingPlatform::setExposedSpeedX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
    {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        static_cast<MovingPlatform*>(ptr)->setSpeed(sf::Vector2f(value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked(), static_cast<MovingPlatform*>(ptr)->getSpeedVector().y));
    }

    void MovingPlatform::getExposedSpeedX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        float x_val = static_cast<MovingPlatform*>(ptr)->getSpeedVector().x;
        info.GetReturnValue().Set(x_val);
    }

    void MovingPlatform::setExposedSpeedY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
    {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        static_cast<MovingPlatform*>(ptr)->setSpeed(sf::Vector2f(static_cast<MovingPlatform*>(ptr)->getSpeedVector().x, value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked()));
    }

    void MovingPlatform::getExposedSpeedY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        float y_val = static_cast<MovingPlatform*>(ptr)->getSpeedVector().y;
        info.GetReturnValue().Set(y_val);
    }

    /**
 * IMPORTANT: Pay close attention to the definition of the std::vector in this
 * example implementation. The v8helpers::expostToV8 will assume you have
 * instantiated this exact type of vector and passed it in. If you don't the
 * helper function will not work.
 */
    v8::Local<v8::Object> MovingPlatform::exposeToV8(v8::Isolate* isolate, v8::Local<v8::Context>& context, std::string context_name)
    {
        std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
        v.push_back(v8helpers::ParamContainer("guid", getMPlatformGUID, setMPlatformGUID));
        v.push_back(v8helpers::ParamContainer("x", getMPlatformX, setMPlatformX));
        v.push_back(v8helpers::ParamContainer("y", getMPlatformY, setMPlatformY));
        v.push_back(v8helpers::ParamContainer("scale", getScale, setScale));
        v.push_back(v8helpers::ParamContainer("speedX", getExposedSpeedX, setExposedSpeedX));
        v.push_back(v8helpers::ParamContainer("speedY", getExposedSpeedY, setExposedSpeedY));
        return v8helpers::exposeToV8(guid, this, v, isolate, context, context_name);
    }

    void MovingPlatform::setScale(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
    {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        static_cast<MovingPlatform*>(ptr)->setCurrentScale(value->NumberValue(info.GetIsolate()->GetCurrentContext()).ToChecked());
    }

    void MovingPlatform::getScale(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
    {
        v8::Local<v8::Object> self = info.Holder();
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
        void* ptr = wrap->Value();
        float y_val = static_cast<MovingPlatform*>(ptr)->getCurrentScale();
        info.GetReturnValue().Set(y_val);
    }

    float MovingPlatform::getCurrentScale()
    {
        return currentScale;
    }

    void MovingPlatform::setCurrentScale(float scale)
    {
        currentScale = scale;
    }


    
    