//
// This cript calls gameobjectfactory() to create a new game object, and
// recievs it's handle. 
//
// NOTE: gameobjectfactory is a static function added to the global context in
// the native main() function
//
var new_obj = gameobjectfactory();
print("Received new object: " +  new_obj.guid)
