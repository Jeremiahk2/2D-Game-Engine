// This script moves the GameObject with handle "gameobject0" (it's guid)
function move(x,y) {
	var tx = gameobject0.x;
	var ty = gameobject0.y;
	gameobject0.x = tx + x;
	gameobject0.y = ty + y;
}

print("Starting position for " << gameobject0.guid << ": " + gameobject0.x + ", " + gameobject0.y + "\nUpdating...");
move(-2,-5);
print("Ending position for " << gameobject0.guid << ": " + gameobject0.x + ", " + gameobject0.y + "\nDone...");


