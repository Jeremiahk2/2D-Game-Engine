INSTRUCTIONS FOR SETTING UP ENVIRONMENT

Step One:
Extract the zip files wherever, or in your repos folder in visual studio.
The zip files contain homework two as well, because I keep it all in one github. You can compare with these if you want, or just ignore them.
I think homework three is much better implemented than homework two ever was.

Step Two: 
Go to Visual Studio, File > Open > Project or Solution 
Then go into the VS 481 folder, click on the VS 481.sin file and then open

Step Three
Once open you should see seven projects. We're only using the two at the end that start with "hw3".
hw2p2
hw2p3Client
hw2p3Server
hw2Client
hw2Server
hw3Server
hw3Client

Step Four
Right click on hw3Client> Properties > C/C++ > General > Additional Include Directories
Add the path to your copy of the "hw3Common" folder that you extracted.
You may need to delete the path that was already there (Users/Jerry...) first

Step Five
Right click on hw3Server > Properties > C/C++ > General > Additional Include Directories
Add the path to your copy of the "hw3Common" folder that you extracted.
You may need to delete the path that was already there (Users/Jerry...) first

Step Six
Build both projects. Simply right click on them and click build.

Step Seven
Run server, then client. To do this quickly, go to Project -> Configure startup projects -> current selection.
Then, simply click on the server (It will turn bold white) then click the run arrow.
Then repeat with the client n times.


EXTRA NOTES PLEASE READ BEFORE TESTING
1. Resizing bug has been fixed! You can move the window or resize it without falling through the platforms.
In fact, visuals continue to update while doing so.

2. To change the tic rate, there is a preprocessor constant to do so. It's not required for this homework, but it's there if you need it.
For hw3Client it is on line 20 of main.cpp.
For hw3Serverit is on line 40 of main.cpp.

3. I made a video where I demo building/running the assignment. I show off most of the features and show how to link correctly.
Here it is: https://drive.google.com/file/d/1_FegTqL7ck0ysBjV62BKhL-615lY6Ssk/view?usp=sharing

4. There are some extra files in my folders. They shouldn't be displayed in VS studio if you used the .sin file.
They will not be there in my hw4 repo. Most of them are old threads I don't need anymore. One of them is a 
Game Object Serializer I attempted to make, which completely failed. You can disregard these.

KEYBINDS:
A: Move left
D: Move right
Space: Jump
Left Arrow: Change scaling by .5 (2 -> 1 -> .5)
Right Arrow: Change scaling by 2.0 (.5 -> 1 -> 2)
Q: Toggle resize mode (Default proportional). This is not supported with SideBounds.
They still do their function, but it's rather pointless if you can just resize your screen.


INSTRUCTIONS FOR GRADING EACH PART

Part 1 & 2: Everything is in one spot. You should be able to test both parts with just the two projects listed.
For part one, the side boundary is located on the far right side of the map, where the one platform is above the other.
There are two, and change your view accordingly. Death bounds are everywhere that you can see. So if you jump off screen to the left
you won't get hit by a death boundary.
However, if you jump off as far right as you can go, you will hit a death boundary. This death boundary will respawn
you back at the starting point, where yet another SideBound is placed, so that your view is updated on respawn.
Part 3: Did not attempt.

