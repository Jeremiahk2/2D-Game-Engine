# 2D Game Engine (SFML)

This is a game engine created by me. It includes various features including multithreading, networked multiplayer, JavaScript integrations, and an event management system, among other things. This project was completed entirely individually by me. It started out as a Linux-based project, but I found SFML's Linux support to be lacking, and moved to this repo for development on windows through Visual Studio 2019.

## Basic Project Structure
The current project contains an example game (Snake) along with the engine that utilizes all of the features that the engine offers, including a limited multiplayer. You'll fine a Snake folder with GameClient, Engine, and GameServer. It's what it sounds like. GameClient (And all thread classes in that folder), are what runs on the client side. GameServer (And all threads within) are what runs on the server side. Engine code is shared between both. I used relative paths for linking, so it should work fine when downloaded.

## Instructions for running and developing.

1. Download the project or clone the repository.
2. Install VCPKG (https://github.com/microsoft/vcpkg) and set it up (./bootstrap)
3. Install the following VCPKG packages and versions: sfml (2.6.1), cppzmq(4.10.0), and zeromq(2023_06_20). SFML version 3.0.0 and above are not supported. The other two have had minimal changes as far as I'm aware, and will probably work with the latest version.
4. Run integrate install in VCPKG
5. Open the .sin file in Visual Studio 2019. This project will not run in 2022 and beyond.
6. Use NuGet Restore to get V8 version v142 added as a "packages" folder. The linking for this is already set up. The project will run without this, but certain things that rely on scripts will not (Like collecting apples in the snake game).
7. Run the GameServer first. There are many ways to do this. I usually just set it as my startup and then run it that way.
8. Repeat with GameClient.
9. You can repeat with as many game clients as you want. The idea here is that multiple people can play at once, and the "High score" will be updated for eachother in real time for competition.
10. Do not open multiple servers. Nothing terrible will happen, but it won't work.

## Future plans

After this project was complete, I continued to develop game-related tools like AI in another repo (https://github.com/Jeremiahk2/GameAI), so you can check that out there.

I actually have a branch up where I was converting the project to a CMake project to avoid the hassle that native Visual Studio projects come with. I managed to succeed at this for the most part, but realized that I didn't really want to use SFML anymore, because I have the knowledge and capability now to use lower-level libraries that I can make have much better performance and scalability. Also, I found better ways to program, and better ways to use C++, so I'll be making a new repo using a different library in the future, if and when I decide to make a new game engine. The new game engine will support 3D games and will not use V8.
