#include "ScriptManager.h"
#include <v8.h>
#include <libplatform/libplatform.h>
#include <iostream>
#include "v8helpers.h"
#include <cstdio>
#include "GameObject.h"

void usage()
{
	// Usage
	std::cout << "\n"
		<< "JavaScript <--> C++ Integration Example\n"
		<< "---------------------------------------\n"
		<< "\n"
		<< "Commands:\n"
		<< "\th: run hello_world.js\n"
		<< "\tp: run perform_function.js\n"
		<< "\tc: run create_object.js\n"
		<< "\tm: run modify_position.js\n"
		<< "\tr: reload scripts from files\n"
		<< "\tl: list all game objects\n"
		<< "\ta: create new object and assign random position\n"
		<< "\to: run random_object.js\n"
		<< "\t?: to print this message\n"
		<< "\tq: quit"
		<< std::endl;

}

int main(int argc, char** argv)
{

    /*
     * NOTE: You must initialize v8 in main, otherwise handles will go out of scope.
     *
     * Contexts are generally on a per-thread basis, so if you would like to do 
     * script execution in a different thread, you must create a new context in
     * the other thread and give care to ensure the context doesn't go out of scope
     * before the thread's run function is called.
	 *
	 * The below v8 function calls are basically boilerplate. 
     */
    std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(platform.release());
    v8::V8::InitializeICU();
    v8::V8::Initialize();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    v8::Isolate *isolate = v8::Isolate::New(create_params);

    { // anonymous scope for managing handle scope
        v8::Isolate::Scope isolate_scope(isolate); // must enter the virtual machine to do stuff
        v8::HandleScope handle_scope(isolate);

		// Best practice to isntall all global functions in the context ahead of time.
        v8::Local<v8::ObjectTemplate> global = v8::ObjectTemplate::New(isolate);
        // Bind the global 'print' function to the C++ Print callback.
        global->Set(isolate, "print", v8::FunctionTemplate::New(isolate, v8helpers::Print));
		// Bind the global static factory function for creating new GameObject instances
		global->Set(isolate, "gameobjectfactory", v8::FunctionTemplate::New(isolate, GameObject::ScriptedGameObjectFactory));
		// Bind the global static function for retrieving object handles
		global->Set(isolate, "gethandle", v8::FunctionTemplate::New(isolate, ScriptManager::getHandleFromScript));
        v8::Local<v8::Context> default_context =  v8::Context::New(isolate, NULL, global);
		v8::Context::Scope default_context_scope(default_context); // enter the context

        ScriptManager *sm = new ScriptManager(isolate, default_context); 

		// Without parameters, these calls are made to the default context
        sm->addScript("hello_world", "scripts/hello_world.js");
        sm->addScript("perform_function", "scripts/perform_function.js");

		// Create a new context
		v8::Local<v8::Context> object_context = v8::Context::New(isolate, NULL, global);
		sm->addContext(isolate, object_context, "object_context");

		GameObject *go = new GameObject();
		go->exposeToV8(isolate, object_context);

		// With the "object_context" parameter, these scripts are put in a
		// different context than the prior three scripts
		sm->addScript("create_object", "scripts/create_object.js", "object_context");
		sm->addScript("random_object", "scripts/random_object.js", "object_context");
		sm->addScript("random_position", "scripts/random_position.js", "object_context");
		sm->addScript("modify_position", "scripts/modify_position.js", "object_context");

		// Use the following 4 lines in place of the above 4 lines as the
		// reference if you don't plan to use multiple contexts
		/* sm->addScript("create_object", "scripts/create_object.js"); */
		/* sm->addScript("random_object", "scripts/random_object.js"); */
		/* sm->addScript("random_position", "scripts/random_position.js"); */
		/* sm->addScript("modify_position", "scripts/modify_position.js"); */

		usage();

		bool reload = false;
		int c = '\n';
		do
		{
			std::cout << "v8 example: ";
			if(c!='\n')
			{
				if(c == '?')
				{
					usage();
				}
				else if(c == 'h')
				{
					sm->runOne("hello_world", reload);
					reload = false;
				}
				else if(c == 'p')
				{
					sm->runOne("perform_function", reload);
					reload = false;
				}
				else if(c =='c')
				{
					sm->runOne("create_object", reload, "object_context");
					reload = false;
				}
				else if(c == 'm')
				{
					std::cout << "Native position before: (" << go->x << ", " << go->y << ")"  << std::endl;
					sm->runOne("modify_position", reload, "object_context");
					std::cout << "Native position after: (" << go->x << ", " << go->y << ")"  << std::endl;
					reload = false;
				}
				else if(c == 'r')
				{
					reload = true;
				}
				else if(c == 'l')
				{
					std::cout << "List of current GameObjects" << std::endl;
					for(GameObject *g : GameObject::game_objects)
					{
						std::cout << "\t" << g->guid << ": " << g->x << ", " << g->y << std::endl;
					}
				}
				else if(c == 'a')
				{
					sm->runOne("random_position", reload, "object_context");
				}
				else if(c == 'o')
				{
					sm->runOne("random_object", reload, "object_context");
				}
			}
			c = getchar();
		} while(c!='q');
    }

    isolate->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();

	return 0;
}

