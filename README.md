# powermate-scroll

An app that allows the Griffin PowerMate to be used to scroll and change the volume in linux. It's written in C++, although it's been > 10 years since I last wrote C++ so it's probably not perfect. 

It works by listening on events from the PowerMate driver (which has been part of the linux kernel for a while now) and proxying them via a uinput device as mouse scroll / volume up/down events. 

