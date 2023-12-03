#include "ScriptManager.h"

/** Definition of static container */
std::map<std::string, ContextContainer> ScriptManager::context_containers;

std::queue<GameObject*> ScriptManager::scriptArgs;

/** Note: function signature is very important */
ScriptManager::ScriptManager(v8::Isolate* isolate, v8::Local<v8::Context>& context)
{
	if (context_containers.empty())
	{
		std::map<std::string, ScriptMetaData> scripts;
		ContextContainer context_container{ "default", isolate, context, scripts };
		context_containers.emplace(std::make_pair("default", context_container));
	}
}

ContextContainer ScriptManager::getContextContainer(std::string context_name)
{
	return context_containers[context_name];
}

// TODO: Free scripts
ScriptManager::~ScriptManager()
{
	/*
	this->isolate->Dispose();
	delete this->create_params.array_buffer_allocator;
	*/
}

std::string read_script(std::filesystem::path path)
{
	std::ifstream in_file(path, std::ios::in);
	const auto script_size = std::filesystem::file_size(path);

	std::string script(script_size, '\0');
	in_file.read(script.data(), script_size);

	return script;
}

void ScriptManager::addContext(v8::Isolate* isolate, v8::Local<v8::Context>& context, std::string context_name)
{
	std::map<std::string, ScriptMetaData> scripts;
	ContextContainer context_container{ context_name, isolate, context, scripts };
	context_containers.emplace(std::make_pair(context_name, context_container));
}

void ScriptManager::addScript(std::string script_id, std::string file_name, std::string context_name)
{
	ScriptMetaData smd;
	smd.script_id = script_id;
	smd.file_name = file_name;
	smd.script = compile(read_script(file_name), context_name);
	context_containers[context_name].scripts.emplace(std::make_pair(script_id, smd));
}

void ScriptManager::runOne(std::string script_id, bool reload, std::string context_name)
{
	// Note: You must enter the context prior to compiling the script
	ContextContainer container = context_containers[context_name];
	v8::Context::Scope context_scope(container.context);

	ScriptMetaData smd = container.scripts[script_id];

	v8::TryCatch try_catch(container.isolate);

	if (reload)
	{
		smd.script = compile(read_script(smd.file_name), context_name);
	}

	/**
	 * This is how you do error checking in a script.
	 */
	v8::Local<v8::Value> result;
	if (!smd.script->Run(container.context).ToLocal(&result))
	{
		v8::String::Utf8Value error(container.isolate, try_catch.Exception());
		std::cerr << "**********************************************************" << std::endl;
		std::cerr << "Error while executing script: " << script_id << std::endl;
		std::cerr << "Exception: " << *error << std::endl;
		std::cerr << "**********************************************************" << std::endl;
		return;
	}

	v8::String::Utf8Value utf8(container.isolate, result);
	if (strcmp(*utf8, "undefined") != 0)
		printf("%s\n", *utf8);
}

void ScriptManager::runAll(bool reload, std::string context_name)
{
	ContextContainer container = context_containers[context_name];
	for (std::pair<std::string, ScriptMetaData> i : container.scripts)
	{
		runOne(i.second.script_id, reload, context_name);
	}
}

void ScriptManager::reloadAll(std::string context_name)
{
	ContextContainer container = context_containers[context_name];
	for (std::pair<std::string, ScriptMetaData> p : container.scripts)
	{
		p.second.script = compile(p.second.file_name, context_name);
	}
}

v8::Local<v8::Script> ScriptManager::compile(std::string script_src_string, std::string context_name)
{
	ContextContainer container = context_containers[context_name];
	v8::Context::Scope context_scope(container.context);
	v8::Local<v8::String> source = v8::String::NewFromUtf8(container.isolate, script_src_string.c_str(), v8::NewStringType::kNormal).ToLocalChecked();
	v8::Local<v8::Script> script = v8::Script::Compile(container.context, source).ToLocalChecked();
	return script;
}

/**
 * This function demonstrates how you can get an object by using the string
 * associated with the object when added to the Global proxy. Note, despite the
 * return type of Global()->Get(...) being v8::Local<v8::Value>, this is indeed
 * a v8::Local<v8::Object>.
 */
void ScriptManager::getHandleFromScript(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::Local<v8::Value> object = context->Global()->Get(context, args[0]->ToString(args.GetIsolate()->GetCurrentContext()).ToLocalChecked()).ToLocalChecked();
	args.GetReturnValue().Set(object);
}

void ScriptManager::addArgs(GameObject *test)
{
	scriptArgs.push(test);
}



void ScriptManager::getNextArg(const v8::FunctionCallbackInfo<v8::Value>& args)
{
	v8::Isolate* isolate = args.GetIsolate();
	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::EscapableHandleScope handle_scope(args.GetIsolate());
	v8::Context::Scope context_scope(context);

	GameObject* new_object = scriptArgs.front();
	v8::Local<v8::Object> v8_obj = new_object->exposeToV8(isolate, context);
	args.GetReturnValue().Set(handle_scope.Escape(v8_obj));
}

