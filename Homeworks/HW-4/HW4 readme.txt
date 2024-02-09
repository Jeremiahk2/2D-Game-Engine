INSTRUCTIONS FOR SETTING UP ENVIRONMENT

Step One:
Extract the zip files wherever, or in your repos folder in visual studio.
The zip files contain homework two as well, because I keep it all in one github. You can compare with these if you want, or just ignore them.
I think homework three is much better implemented than homework two ever was.

Step Two: 
Go to Visual Studio, File > Open > Project or Solution 
Then go into the VS 481 folder, click on the VS 481.sin file and then open

Step Three
Once open you should see seven projects. We're only using the two at the end that start with "hw4".
hw2p2
hw2p3Client
hw2p3Server
hw2Client
hw2Server
hw3Server
hw3Client
hw4Server
hw4Client

Step Four
Right click on hw4Client> Properties > C/C++ > General > Additional Include Directories
Add the path to your copy of the "hw4Common" folder that you extracted.
You may need to delete the path that was already there (Users/Jerry...) first

Step Five
Right click on hw3Server > Properties > C/C++ > General > Additional Include Directories
Add the path to your copy of the "hw4Common" folder that you extracted.
You may need to delete the path that was already there (Users/Jerry...) first

Step Six
Build both projects. Simply right click on them and click build.

Step Seven
Run server, then client. To do this quickly, go to Project -> Configure startup projects -> current selection.
Then, simply click on the server (It will turn bold white) then click the run arrow.
Then repeat with the client n times.

Also, if you want to use the executables directly, go to VS 481 -> x64 -> Release -> hw4Client or hw4Server applications

EXTRA NOTES PLEASE READ BEFORE TESTING
1. To change the tic rate, there is a preprocessor constant to do so. It's not required for this homework, but it's there if you need it.
For hw4Client it is near line 20 of main.cpp.
For hw4Server it is near line 40 of main.cpp.

2. I made a video where I demo the new features of this assignment. Sorry about the timer half way through, couldn't hear it on my end.
Here it is: https://drive.google.com/file/d/1_FegTqL7ck0ysBjV62BKhL-615lY6Ssk/view?usp=sharing

KEYBINDS:
A: Move left
D: Move right
Space: Jump
Left Arrow: Change scaling by .5 (2 -> 1 -> .5) (Only affects character, intended)
Right Arrow: Change scaling by 2.0 (.5 -> 1 -> 2)
Q: Toggle resize mode (Default proportional). This is not supported with SideBounds.
They still do their function, but it's rather pointless if you can just resize your screen.


INSTRUCTIONS FOR GRADING EACH PART

Part 1 & 2: Both of these parts are together. Collisions, gravity, movement of the character (Except jumping), dying, and spawning are all handled using events on the client
The only things that you can see that aren't handled by events are the updating of the moving platforms, which wasn't required.
In addition, the closing of the server and the client are handled through passing an event over the network to each client as the client connects.
That satisifes part two's requirement, and demonstrates using the timeline priority queue from part one.
Part 3: Did not attempt.

