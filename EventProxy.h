#ifndef __EVENTPROXY__DOT_H__SENTRY__
#define __EVENTPROXY__DOT_H__SENTRY__

#include "VirtualDevice.h"
#include "Powermate.h"

namespace PowermateScroll {

    class EventProxy {

    public: 
        EventProxy();
        ~EventProxy();
        
        void Start();
        
    private: 
        // Virtual (uinput) device to send events to
        VirtualDevice* virtDevice = NULL;
        
        // Powermate device to listen on
        Powermate* powermate = NULL;
        
        // Is the powermate button currently held down?
        int isButtonDown;
        
        void ProcessEvent(struct input_event *ev);
    };
}

#endif
