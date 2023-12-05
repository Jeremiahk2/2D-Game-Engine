#include "PubThread.h"


PubThread::PubThread(Timeline *timeline, int *highScore, std::mutex* m, EventManager *manager) {
    this->timeline = timeline;
    this->highScore = highScore;
    this->mutex = m;
    this->manager = manager;
}

void PubThread::run() {

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

        v8::Local<v8::Context> default_context = v8::Context::New(isolate, NULL, global);
        v8::Context::Scope default_context_scope(default_context); // enter the context
        //For raising events, pass an array that is stored in the manager, and can be accessed from the script. Pop the elements off to get the arguments.
        ScriptManager* sm = new ScriptManager(isolate, default_context);

        sm->addScript("move_platform", "scripts/move_platform.js");

        zmq::context_t context(2);
        zmq::socket_t pubSocket(context, zmq::socket_type::pub);
        pubSocket.bind("tcp://localhost:5555");

        int64_t tic = 0;
        int64_t currentTic = 0;
        float ticLength;
        int moves = 0;
        while (true) {
            ticLength = timeline->getRealTicLength();
            currentTic = timeline->getTime();

            if (currentTic > tic) {
                //Construct return string
                std::string rtnString;
                std::stringstream stream;
                {
                    //Add high score to stream
                    std::lock_guard<std::mutex> lock(*mutex);
                    stream << *highScore;
                }

                std::getline(stream, rtnString);
                //Send the string to the players
                zmq::message_t rtnMessage(rtnString.size() + 1);
                memcpy(rtnMessage.data(), rtnString.data(), rtnString.size() + 1);
                pubSocket.send(rtnMessage, zmq::send_flags::none);
                tic = currentTic;
            }
        }
    }
}