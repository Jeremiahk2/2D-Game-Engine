// 
// A gentle introduction, using the print callback registered in the native
// main() function
//

//This better be the event
var death = moreArgs();
//Set it up.
death.type = "spawn";
death.time = death.time + 3000;
death.order = death.order + 1;
//Raise the new event
raise(death.guid);

death.type = "stop";
death.time = death.time;
death.order = death.order + 1;
raise(death.guid);