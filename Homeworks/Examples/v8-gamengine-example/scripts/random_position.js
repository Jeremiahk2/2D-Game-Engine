// 
// This cript combines multiple functionalities demonstrated in the other scripts
// 1. Calling a function
// 2. Using javascript built in functions
// 3. Creating a new object instance
// 4. Using the handle for the new object instance
//
function update(go) {
	x = Math.floor((Math.random()*13)+1)-3;
	y = Math.floor((Math.random()*13)+1)-3;
	go.x = x;
	go.y = y;
}

var new_object = gameobjectfactory();
print("Created new object: " + new_object.guid + " with position (" + new_object.x + ", " + new_object.y + ")");
print("Setting random position");
update(new_object);
print("Updated position randomly to " + new_object.guid + ": (" + new_object.x + ", " + new_object.y + ")");
