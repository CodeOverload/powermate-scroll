#include <linux/input.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <signal.h>

#include "EventProxy.h"
#include "VirtualDevice.h"
#include "Powermate.h"
#include "IOException.h"

#define BUFFER_SIZE 32

namespace PowermateScroll {

    EventProxy::EventProxy() {
        isButtonDown = 0;
        
        powermate = new Powermate(O_RDONLY);
        
        try {
            virtDevice = new VirtualDevice();
        }
        catch (IOException& ex) {
            delete powermate;
            throw ex;
        }
    }

    EventProxy::~EventProxy() {
        if (virtDevice != NULL) {
            delete virtDevice;
        }
        
        if (powermate != NULL) {
            delete powermate;
        }
    }


    void EventProxy::ProcessEvent(struct input_event *ev) {

        switch (ev->type) {
        
            case EV_REL: {
                if (ev->code != REL_DIAL) {
                  return;
                }
                int val = (int) ev->value;
                if (isButtonDown) {
                    virtDevice->ChangeVolume(val > 0);
                }
                else {
                    virtDevice->Scroll(val);
                }
                break;
            }
            
            case EV_KEY: {
                if (ev->code != BTN_0) {
                  return;
                }
                isButtonDown = ev->value;
                break;
            }
        }
    }

    void EventProxy::Start() {
        struct input_event eventBuffer[BUFFER_SIZE];

        while (1) {
            int eventsRead = powermate->Read(eventBuffer, sizeof(struct input_event) * BUFFER_SIZE);
            for (int i = 0; i < eventsRead; ++i) {
                ProcessEvent(&eventBuffer[i]);
            }
        }
    }
}


PowermateScroll::EventProxy* proxy = NULL;

void sigintHandler(int s) {
    if (proxy != NULL) {
        delete proxy;
    }
    exit(10); 
}


int main(int argc, char *argv[]) {

    // Register Ctrl+C / SIGINT handler
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = sigintHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);
   
    // Initialise and start the event proxy
    try {
        proxy = new PowermateScroll::EventProxy();
    }
    catch (PowermateScroll::IOException& ex) {
        fprintf(stderr, "Error initialising event proxy: %s\n", ex.GetMessage());
        delete proxy;
        return 2;
    }

    try {
        proxy->Start();
    }
    catch (PowermateScroll::IOException& ex) {
        fprintf(stderr, "Error processing events: %s\n", ex.GetMessage());
        delete proxy;
        return 3;
    }
    return 0;
}
