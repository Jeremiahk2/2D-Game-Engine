function update(obj) {
	obj.x = Math.floor((Math.random()*13)+1)-3;
	obj.y = Math.floor((Math.random()*13)+1)-3;
}

function selectobject(num) {
	s = Math.random();
	pick = Math.floor(num*s);
	handle_string = "gameobject" + pick;
	return handle_string;
}

var handle_string = selectobject(3);
var handle = gethandle(handle_string);
print("Picking " + handle_string + ": (" + handle.x + ", " + handle.y + ")");
handle.x = Math.floor((Math.random()*13)+1)-3;
handle.y = Math.floor((Math.random()*13)+1)-3;
print("Updated to " + handle_string + ": (" + handle.x + ", " + handle.y + ")");
