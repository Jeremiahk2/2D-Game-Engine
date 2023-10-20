

INSTRUCTIONS FOR SETTING UP ENVIRONMENT

Step One:
Extract the zip files wherever, or in your repos folder in visual studio

Step Two: 
Go to Visual Studio, File > Open > Project or Solution 
Then go into the VS 481 folder, click on the VS 481.sin file and then open

Step Four:
Once open you should see five projects:
hw2p2
hw2p3Client
hw2p3Server
hw2Client
hw2Server

Step Five:
Right click on hw2Client > Properties > C/C++ > General > Additional Include Directories
Add the path to your copy of the "hw2Common" folder that you extracted.
You may need to delete the path that was already there (Users/Jerry...) first

Step Six:
Right click on hw2Server > Properties > C/C++ > General > Additional Include Directories
Add the path to your copy of the "hw2Common" folder that you extracted.
You may need to delete the path that was already there (Users/Jerry...) first


EXTRA NOTES PLEASE READ BEFORE TESTING
1. There is a slight problem with part four involving moving/resizing a window.
You need to do your movements/resizing in short bursts so your characters do not
fall through the platforms. A video is provided. See timestamp 4:02 for demo.

2. Very occasionally, closing a sfml window will crash the server. 
This was very rare for me, but if it does happen, simply close the terminals and restart.
Though disconnecting isn't a requirement, I thought I should give a heads up.

3. There is a hard cap of 10 clients that can connect to a server over it's lifetime for part four. Part three is unbounded.
For example if you add 10 clients and then try to add an eleventh, it will not let you.
Or, if you add 7 clients, disconnect 4, then add another 3, so you are at 6 clients, you won't be able to add any more (because you've added ten total).
Adding too many clients should not crash the server nor any of the other clients. The process you tried to open will simply close the window and exit with a status of 1.

4. To change the tic rate, there is a preprocessor constant to do so.
For hw2p2 it is on line 47 of main.cpp.
For hw2Client it is on line 56.
For hw2Server it is on line 40.

5. I made a video where I demo building/running and testing each part of the assignment, if you need it.
Here it is:
https://drive.google.com/file/d/1f34xQYOLxA7nDGasVli-Z8aicxlbddnC/view?usp=sharing
It also contains most of the info I mention in the readme (May have to turn volume up).

6. If additional parts of the project are required, let me know. I have been routinely updating github and should have whatever is needed.
Though I don't have a section that is solely part one, as I started part two and part one at the same time. But if I'm understanding correctly, 
part four should meet part one's requirements (Part four server has two threads, main- handles client updates - and mthread. Part four client has two threads, main - handles player input - and cthread.


KEYBINDS:
A: Move left
D: Move right
Space: Jump
Left Arrow: Change scaling by .5 (2 -> 1 -> .5)
Right Arrow: Change scaling by 2.0 (.5 -> 1 -> 2)
Q: Toggle resize mode
P: Pause/unpause (Only available in hw2p2)


INSTRUCTIONS FOR GRADING EACH PART

Part One:
For grading part one, see part four. 
I implemented threading much better by the time I got to part four, and part four is how I will use it going forwards.
It still meets the requirement, as both the server and the client use two sets of two different threads (including mains).

Part Two:
For grading part two, use the hw2p2 project.
As a tip, if you want to run a specific project, do the following:
Right click the solution (VS 481), go to Properties > Common Properties > Startup Project
Select "Current selection".
Then when you want to run a project, simply click on the project (It should turn bold white), and press the green start button.
It should automatically build and run.

Part Three:
For grading part three, use hw2p3Client and hw2p3Server OR part four
Build both (Right click project > build)
Run the server, and THEN the client. To add more clients simply select the client project, and keep pressing run.

Part Four:
For grading part four, use hw2Client and hw2Server. These are the files I will be taking to hw3
Build both (Right click project > build)
Run the server, and THEN the client.
To add more clients, simply highlight the client project and repeatedly run.

Part Five:
Part five wasn't really implemented. Some things work as you'd expect them to, and others don't.

For both parts three and four, DO NOT run two servers at the same time.

