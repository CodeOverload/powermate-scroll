#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include "IOException.h"
#include "VirtualDevice.h"

namespace PowermateScroll {

    VirtualDevice::VirtualDevice() {
        Create();
    }

    VirtualDevice::~VirtualDevice() {
        if (ioctl(fd, UI_DEV_DESTROY) < 0) {
            fprintf(stderr, "Warning: error destroying virtual device");
        }
        
        close(fd);
    }

    void VirtualDevice::Scroll(int amount) {

        struct input_event ev;
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_REL;
        ev.code = REL_WHEEL;
        ev.value = amount;
        
        if (write(fd, &ev, sizeof(struct input_event)) < 0) {
            throw IOException("Error writing scroll event");
        } 

        SendReportEvent();
    }

    void VirtualDevice::ChangeVolume(bool up) {    
        struct input_event ev;
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_KEY;
        ev.code = up ? KEY_VOLUMEUP : KEY_VOLUMEDOWN;
        ev.value = 1;
        
        if (write(fd, &ev, sizeof(struct input_event)) < 0) {
            throw IOException("Error writing volume key down event");
        } 
        
        ev.value = 0;
        if (write(fd, &ev, sizeof(struct input_event)) < 0) {
            throw IOException("Error writing volume key release event");
        }
        
        SendReportEvent();
    }

    void VirtualDevice::Create() {

        fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
        if (fd < 0) {
            throw IOException("Couldn't open /dev/uinput");
        }
        
        try {
            SetParams();
            
            struct uinput_user_dev uidev;
            memset(&uidev, 0, sizeof(uidev));
            snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-powermate");
            uidev.id.bustype = BUS_USB;
            uidev.id.vendor  = 0x1;
            uidev.id.product = 0x1;
            uidev.id.version = 1;
            
            if (write(fd, &uidev, sizeof(uidev)) < 0) {
                throw IOException("Error writing virtual device info to uinput");
            }

            if (ioctl(fd, UI_DEV_CREATE) < 0) {
                throw IOException("Error creating uinput virtual device");
            }
        }
        catch (IOException& ex) {
            close(fd);
            throw ex;
        }
    }

    void VirtualDevice::SetParams() {
	    SetParam(UI_SET_EVBIT, EV_KEY);
	    SetParam(UI_SET_EVBIT, EV_REL);
	    SetParam(UI_SET_EVBIT, EV_SYN);

	    SetParam(UI_SET_RELBIT, REL_X);
	    SetParam(UI_SET_RELBIT, REL_Y);
	    SetParam(UI_SET_RELBIT, REL_WHEEL);
	
	    SetParam(UI_SET_KEYBIT, BTN_LEFT);
	    SetParam(UI_SET_KEYBIT, BTN_RIGHT);
	    SetParam(UI_SET_KEYBIT, BTN_MIDDLE);
	
	    SetParam(UI_SET_KEYBIT, KEY_VOLUMEUP);
	    SetParam(UI_SET_KEYBIT, KEY_VOLUMEDOWN);
    }

    void VirtualDevice::SetParam(unsigned long request, int bit) {
	    if (ioctl(fd, request, bit)) {
            throw IOException("Error setting uinput device parameter");
        }
    }

    void VirtualDevice::SendReportEvent() {
        struct input_event ev;
        memset(&ev, 0, sizeof(struct input_event));
        ev.type = EV_SYN;
        ev.code = SYN_REPORT;
        ev.value = 0;
        
        if (write(fd, &ev, sizeof(struct input_event)) < 0) {
            throw IOException("Error writing report event");
        }
    }

}

