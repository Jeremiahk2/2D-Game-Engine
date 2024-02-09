# Example of Integrating Javascript with a C++ Game Engine

This repository contains example code and helper functions to assist in
integrating javascript scripts in the C++ game engine created for CSC 481/581.
The examples demonstrate:

1. How to initialize v8 ([ScriptDemo.cpp](ScriptDemo.cpp))
2. How to add functions to the global proxy that are callable from javascript ([ScriptDemo.cpp](ScriptDemo.cpp))
3. How to execute scripts from C++ ([ScriptDemo.cpp](ScriptDemo.cpp) and [ScriptManager.h](ScriptManager.h)/[ScriptManager.cpp](ScriptManager.cpp))
4. How to expose objects to javascript ([GameObject.h](GameObject.h)/[GameObject.cpp](GameObject.cpp) and [v8helpers.h](v8helpers.h)/[v8helpers.cpp](v8helpers.cpp))
5. How to create new objects from scripts using factory functions ([GameObject.h](GameObject.h)/[GameObject.cpp](GameObject.cpp))

All of this functionality is further demonstrated with the sample scripts in the [scripts](scripts/) directory.

## Installation:

This repository has been tested on Ubuntu 20.04 with build-essential and
libnode-dev installed. 
```bash
> sudo apt update && sudo apt -y install build-essential libnode-dev
```

__Note: Ubuntu's version of v8 uses node 10__, which is comparatively ancient.
Thus, be careful when looking for v8's API documentation. The documentation
corresponding to this version of v8 can be found
[here](https://v8docs.nodesource.com/node-10.15/index.html).
 
## Running the Samples:

Using the included `Makefile` you can compile the code by simply executing
`make` in the top-level directory. Run the complied binary with `./main`. The
following usage message will appear.
```bash
JavaScript <--> C++ Integration Example
---------------------------------------

Commands:
	h: run hello_world.js
	p: run perform_function.js
	c: run create_object.js
	m: run modify_position.js
	r: reload from file only the next script called
	l: list all game objects
	a: create new object and assign random position
	o: run random_object.js
	?: to print this message
	q: quit
```
You can procede by entering a single character from the list, followed by
pressing enter. 

## Tips on Integrating

Much of the code is documented with comments that highlight some important
"gotchas" of v8's syntax. Please pay attention to function signatures and
return types, as these will often be the source of issues. In particular, most
functions that are passed as parameters to v8 functions must be static (e.g.,
getters and setters). 

If you'd like to take advantage of the helper function declared in
[v8helpers.h](v8helpers.h) for exposing objects to javascript, please pay
careful attention to the example implementation of
`GameObject::expostToV8(v8::Isolate *isolate, v8::Local<v8::Context>
&context)`. The `std::vector` of `ParamContainer` structs is required to take
advantage of the helper function. 

