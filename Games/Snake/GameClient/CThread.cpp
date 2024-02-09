#include "CThread.h"


bool CThread::isBusy()
{
    std::lock_guard<std::mutex> lock(*mutex);  // this locks the mutuex until the variable goes out of scope (i.e., when the function returns in this case)
    return *busy;
}

CThread::CThread(bool* upPressed, GameWindow* window, Timeline* timeline, bool* stopped,
    std::mutex* m, std::condition_variable* cv, bool* busy, EventManager *em)
{
    this->mutex = m;
    this->cv = cv;
    this->stop = stopped;
    this->line = timeline;
    this->window = window;
    this->upPressed = upPressed;
    this->busy = busy;
    this->em = em;
}

void CThread::run() {

    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.release());
    v8::V8::InitializeICU();
    v8::V8::Initialize();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate* isolate = v8::Isolate::New(create_params);

    { // anonymous scope for managing handle scope
        v8::Isolate::Scope isolate_scope(isolate); // must enter the virtual machine to do stuff
        v8::HandleScope handle_scope(isolate);

        //Best practice to isntall all global functions in the context ahead of time.
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
        // Bind the global 'print' function to the C++ Print callback.
        global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, v8helpers::Print));
        // Bind the global static factory function for creating new GameObject instances
        global->Set(isolate, "makeEvent", v8::FunctionTemplate::New(isolate, Event::ScriptedGameObjectFactory));
        // Bind the global static function for retrieving object handles
        global->Set(isolate, "gethandle", v8::FunctionTemplate::New(isolate, ScriptManager::getHandleFromScript));

        global->Set(isolate, "raise", v8::FunctionTemplate::New(isolate, EventManager::raiseEventFromScript));

        global->Set(isolate, "moreArgs", v8::FunctionTemplate::New(isolate, ScriptManager::getNextArg));

        // Declare and load a font
        sf::Font font;
        font.loadFromFile("superstar_memesbruh03.ttf");

        // Create a text
        sf::Text personal("Length: 1", font);
        personal.setCharacterSize(30);
        personal.setStyle(sf::Text::Regular);
        personal.setFillColor(sf::Color::Yellow);
        personal.setPosition(250.f, 600.f);

        // Create a text
        sf::Text highScore("High Score: 1", font);
        highScore.setCharacterSize(30);
        highScore.setStyle(sf::Text::Regular);
        highScore.setFillColor(sf::Color::Yellow);
        highScore.setPosition(450, 600.f);



        v8::Local<v8::Context> default_context = v8::Context::New(isolate, NULL, global);
        v8::Context::Scope default_context_scope(default_context); // enter the context
        //For raising events, pass an array that is stored in the manager, and can be accessed from the script. Pop the elements off to get the arguments.
        ScriptManager* sm = new ScriptManager(isolate, default_context);

        sm->addScript("handle_death", "scripts/handle_death.js");
        sm->addScript("move_character", "scripts/move_character.js");

        std::string type("collision");
        std::list<std::string> types;
        types.push_back(type);
        em->registerEvent(types, new CollisionHandler);

        type = "movement";
        types.clear();
        types.push_back(type);
        em->registerEvent(types, new MovementHandler);

        type = "gravity";
        types.clear();
        types.push_back(type);
        em->registerEvent(types, new GravityHandler(em, window, sm));

        type = "spawn";
        types.clear();
        types.push_back(type);
        em->registerEvent(types, new SpawnHandler(window));

        type = "death";
        types.clear();
        types.push_back(type);
        em->registerEvent(types, new DeathHandler(em, sm));

        type = "Client_Closed";
        types.clear();
        types.push_back(type);
        em->registerEvent(types, new ClosedHandler(em));

        int64_t tic = 0;
        int64_t currentTic;
        float ticLength;
        window->setActive(true);

        Character* character = (Character*)window->getPlayableObject();

        //  Prepare our context and socket
        zmq::context_t context(2);
        zmq::socket_t subSocket(context, zmq::socket_type::sub);

        zmq::socket_t reqSocket(context, zmq::socket_type::req);

        //Connect and get your own port.
        reqSocket.connect("tcp://localhost:5556");

        //Send the request to the server.
        char initRecvString[MESSAGE_LIMIT];
        sprintf_s(initRecvString, "Connect");

        zmq::message_t initRequest(strlen(initRecvString) + 1);
        memcpy(initRequest.data(), initRecvString, strlen(initRecvString) + 1);
        reqSocket.send(initRequest, zmq::send_flags::none);

        //Receive the reply from the server, should contain our port and ID
        zmq::message_t initReply;
        auto r = reqSocket.recv(initReply, zmq::recv_flags::none);;
        char* initReplyString = (char*)initReply.data();
        int initId = -1;
        int port = -1;
        int matches = sscanf_s(initReplyString, "%d %d", &initId, &port);

        //Exit if we didn't get a proper reply
        if (matches != 2) {
            exit(2);
        }
        //Set our character's ID and configure port string
        character->setID(initId);
        char portString[21];
        sprintf_s(portString, "%s%d", "tcp://localhost:", port);

        //Disconnect from main server process.
        reqSocket.disconnect("tcp://localhost:5556");
        //Bind to your unique port provided by the server.
        reqSocket.bind(portString);
        //Conflate messages to avoid getting behind.
        subSocket.set(zmq::sockopt::conflate, "");
        subSocket.connect("tcp://localhost:5555");
        subSocket.set(zmq::sockopt::subscribe, "");

        int moves = 0;
        bool direction = false;
        float jumpTime = JUMP_TIME;
        //For movement on top of horizontal platforms.
        float nonScalableTicLength = line->getNonScalableTicLength();

        while (!(*stop)) {
            currentTic = line->getTime();
            if (currentTic > tic) {
                //Get information from server

                bool erase = false;
                {
                    std::lock_guard<std::mutex> lock(*mutex);
                    for (const auto& [time, orderMap] : em->raised_events) {
                        if (time <= line->convertGlobal(currentTic)) {
                            for (const auto& [order, e] : orderMap) {
                                for (EventHandler* currentHandler : em->handlers.at(e.type)) {
                                    currentHandler->onEvent(e);
                                }
                            }
                            if (erase) {
                                em->raised_events.erase(em->raised_events.begin());
                            }
                            erase = true;
                        }
                        else {
                            break;
                        }
                    }
                    if (erase) {
                        em->raised_events.erase(em->raised_events.begin());
                    }
                }
                //Receive updates to nonstatic objects. Should be comma separated string.
                zmq::message_t newPlatforms;
                r = subSocket.recv(newPlatforms, zmq::recv_flags::none);
                std::string updates((char*)newPlatforms.data());
                highScore.setString("High Score: " + updates);

                //Sync with visuals
                {
                    std::lock_guard<std::mutex> lock(*mutex);
                    window->update();
                    personal.setString("Length: " + std::to_string(character->length + 1));
                    window->draw(personal);
                    window->draw(highScore);
                    window->display();

                    //Set up gravity event.
                    Event g;
                    {
                        g.time = line->convertGlobal(currentTic);
                        g.type = std::string("gravity");
                        Event::variant characterVariant;
                        characterVariant.m_Type = Event::variant::TYPE_GAMEOBJECT;
                        characterVariant.m_asGameObject = character;
                        g.parameters.insert({ "character", characterVariant });
                    }
                    //Handle gravity as well.
                    em->raise(g);

                    //Handle all events that have come up.
                    bool erase = false;

                    for (const auto& [time, orderMap] : em->raised_events) {
                        if (time <= line->convertGlobal(currentTic)) {
                            for (const auto& [order, e] : orderMap) {
                                for (EventHandler* currentHandler : em->handlers.at(e.type)) {
                                    currentHandler->onEvent(e);
                                }
                            }
                            if (erase) {
                                em->raised_events.erase(em->raised_events.begin());
                            }
                            erase = true;
                        }
                        else {
                            break;
                        }
                    }
                    if (erase) {
                        em->raised_events.erase(em->raised_events.begin());
                    }
                }
                //Send updated character information to server
                {
                    std::lock_guard<std::mutex> lock(*mutex);
                    std::string charString(std::to_string(character->length + 1));
                    zmq::message_t request(charString.size() + 1);
                    memcpy(request.data(), charString.data(), charString.size() + 1);
                    reqSocket.send(request, zmq::send_flags::none);
                }
                //Receive confirmation
                zmq::message_t reply;
                r = reqSocket.recv(reply, zmq::recv_flags::none);
                char* replyString = (char*)reply.data();
                try {
                    std::shared_ptr<Event> e(new Event);
                    //Convert to an event pointer
                    e = (std::dynamic_pointer_cast<Event>(e->constructSelf(replyString)));
                    e->time = line->convertGlobal(currentTic) + e->time;
                    //Raise event
                    em->raise(*e);
                }
                catch (std::invalid_argument) {
                    //Oops, wasn't an event.
                }
                tic = currentTic;
            }
        }
        //Tell the server we are disconnecting
        character->setConnecting(false);
        std::string charString = "-1";

        zmq::message_t request(charString.size() + 1);
        memcpy(request.data(), charString.data(), charString.size() + 1);
        reqSocket.send(request, zmq::send_flags::none);

        //Receive confirmation
        zmq::message_t reply;
        r = reqSocket.recv(reply, zmq::recv_flags::none);
        window->setActive(false);
    }
    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
}