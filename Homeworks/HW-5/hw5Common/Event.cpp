#include "Event.h"

std::unordered_map<std::string, Event*> Event::events;

Event::Event() : GameObject(false, false, false)
{
    std::lock_guard<std::mutex> lock(innerMutex);
    guid = "event" + std::to_string(*GameObject::getCurrentGUID());
    (*GameObject::getCurrentGUID())++;
    events.insert_or_assign( guid, this );
}

Event::~Event() {
    if (context != nullptr) {
        context->Reset();
    }
}

std::string Event::toString()
{
	
    std::stringstream stream;
    char space = ' ';

    stream << getObjectType() << space << time << space << order << space << type << space;

    for (const auto& [key, value] : parameters) {
        if (value.m_Type == Event::variant::TYPE_INT) {
            stream << key << space << Event::variant::TYPE_INT << space << value.m_asInt << ".";
        }
        else if (value.m_Type == Event::variant::TYPE_FLOAT){
            stream << key << space << Event::variant::TYPE_FLOAT << space << value.m_asFloat << ".";
        }
        else if (value.m_Type == Event::variant::TYPE_BOOLP) {
            stream << key << space << Event::variant::TYPE_BOOLP << space << *(value.m_asBoolP) << ".";
        }
        else if (value.m_Type == Event::variant::TYPE_STRING) {
            stream << key << space << Event::variant::TYPE_STRING << space << value.m_asString << ".";
        }
    }
    std::string line;
    getline(stream, line);
    return line;
}

std::shared_ptr<GameObject> Event::constructSelf(std::string self)
{
    int pos = 0;
    int newPos = 0;
    Event *e = new Event();
    int objectType;
    int time;
    int order;
    char* type = (char*)malloc(self.size() + 1);

    //Get the time and the order of the event.
    if (sscanf_s(self.data(), "%d %d %d %s%n", &objectType, &time, &order, type, (unsigned int)self.size() + 1, &pos) != 4) {
        throw std::invalid_argument("Failed to read string. Not an event");
    }
    e->time = time;
    e->order = order;
    e->type = type;

    char* current = (char*)malloc(self.size() + 1);
    //Scan through each variant
    while (sscanf_s(self.data() + pos, "%[^.]%n", current, (unsigned int)(self.size() + 1), &newPos) == 1) {

        char* key = (char*)malloc(strlen(current) + 1);
        Event::variant::Type type;
        int innerPos = 0;

        //Get the key and the type of value
        int matches = sscanf_s(current, "%s %d%n", key,(unsigned int)strlen(current) + 1, & type, &innerPos);
        //Throw if key or type is invalid
        if (matches != 2) {
            free(key);
            throw std::invalid_argument("Failed to read string. Type must be the first value.");
        }
        //If it's an int
        if (key && type == Event::variant::TYPE_INT) {
            int value;
            sscanf_s(current + innerPos, "%d", &value);
            Event::variant intVariant;
            intVariant.m_Type = Event::variant::TYPE_INT;
            intVariant.m_asInt = value;
            e->parameters.insert({ std::string(key), intVariant });
        }
        //If it's a float
        else if (key && type == Event::variant::TYPE_FLOAT) {
            float value;
            sscanf_s(current + innerPos, "%f", &value);
            Event::variant floatVariant;
            floatVariant.m_Type = Event::variant::TYPE_FLOAT;
            floatVariant.m_asFloat = value;
            e->parameters.insert({ std::string(key), floatVariant });
        }
        //If it's a boolean
        else if (key && type == Event::variant::TYPE_BOOLP) {
            int value;
            sscanf_s(current + innerPos, "%d", &value);
            bool *boolValue = new bool(value);
            Event::variant boolVariant;
            boolVariant.m_Type = Event::variant::TYPE_BOOLP;
            boolVariant.m_asBoolP = boolValue;
            e->parameters.insert({ std::string(key), boolVariant });
        }
        //If it's a string
        else if (key && type == Event::variant::TYPE_STRING) {
            char* value = (char *)malloc(strlen(current) + 1);
            sscanf_s(current + innerPos, " %[^.]", value, (unsigned int)strlen(current) + 1);
            Event::variant stringVariant;
            stringVariant.m_Type = Event::variant::TYPE_STRING;
            stringVariant.m_asString = value;
            e->parameters.insert({ std::string(key), stringVariant });
        }


        //update position and skip past comma
        pos += newPos + 1;
    }
    free(current);
    std::shared_ptr<GameObject> rtnptr(e);
    return rtnptr;
}

std::shared_ptr<GameObject> Event::makeTemplate()
{
	return std::shared_ptr<GameObject>(new Event);
}

int Event::getObjectType()
{
	return objectType;
}

/**
 * IMPORTANT: Pay close attention to the definition of the std::vector in this
 * example implementation. The v8helpers::expostToV8 will assume you have
 * instantiated this exact type of vector and passed it in. If you don't the
 * helper function will not work.
 */
v8::Local<v8::Object> Event::exposeToV8(v8::Isolate* isolate, v8::Local<v8::Context>& context, std::string context_name)
{
    std::vector<v8helpers::ParamContainer<v8::AccessorGetterCallback, v8::AccessorSetterCallback>> v;
    v.push_back(v8helpers::ParamContainer("guid", getEventGUID, setEventGUID));
    v.push_back(v8helpers::ParamContainer("type", getEventType, setEventType));
    v.push_back(v8helpers::ParamContainer("time", getEventTime, setEventTime));
    v.push_back(v8helpers::ParamContainer("order", getEventOrder, setEventOrder));
    return v8helpers::exposeToV8(guid, this, v, isolate, context, context_name);
}

void Event::setEventGUID(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    v8::String::Utf8Value utf8_str(info.GetIsolate(), value->ToString(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
    static_cast<Event*>(ptr)->guid = *utf8_str;
}

void Event::getEventGUID(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    std::string guid = static_cast<Event*>(ptr)->guid;
    v8::Local<v8::String> v8_guid = v8::String::NewFromUtf8(info.GetIsolate(), guid.c_str()).ToLocalChecked();
    info.GetReturnValue().Set(v8_guid);
}

void Event::setEventType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    v8::String::Utf8Value utf8_str(info.GetIsolate(), value->ToString(info.GetIsolate()->GetCurrentContext()).ToLocalChecked());
    static_cast<Event*>(ptr)->type = *utf8_str;
}

void Event::getEventType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    std::string guid = static_cast<Event*>(ptr)->type;
    v8::Local<v8::String> v8_type = v8::String::NewFromUtf8(info.GetIsolate(), guid.c_str()).ToLocalChecked();
    info.GetReturnValue().Set(v8_type);
}

void Event::setEventTime(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<Event*>(ptr)->time = value->Int32Value(info.GetIsolate()->GetCurrentContext()).ToChecked();
}

void Event::getEventTime(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    float time_val = static_cast<Event*>(ptr)->time;
    info.GetReturnValue().Set(time_val);
}

void Event::setEventOrder(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    static_cast<Event*>(ptr)->order = value->Int32Value(info.GetIsolate()->GetCurrentContext()).ToChecked();
}

void Event::getEventOrder(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info)
{
    v8::Local<v8::Object> self = info.Holder();
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(self->GetInternalField(0));
    void* ptr = wrap->Value();
    float order_val = static_cast<Event*>(ptr)->order;
    info.GetReturnValue().Set(order_val);
}


/**
 * Factory method for allowing javascript to create instances of native game
 * objects
 *
 * NOTE: Like with the setters above, this static function does have a return
 * type (and object), but the return value is placed in the function callback
 * parameter, not the native c++ return type.
 */
void Event::ScriptedGameObjectFactory(const v8::FunctionCallbackInfo<v8::Value>& args)
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
    GameObject* new_object = new Event;
    v8::Local<v8::Object> v8_obj = new_object->exposeToV8(isolate, context);
    args.GetReturnValue().Set(handle_scope.Escape(v8_obj));
}