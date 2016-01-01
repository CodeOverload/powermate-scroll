#include <linux/input.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "IOException.h"
#include "Powermate.h"

#define NUM_VALID_PREFIXES 2
#define NUM_EVENT_DEVICES 16

namespace PowermateScroll {

    static const char *VALID_PREFIXES[NUM_VALID_PREFIXES] = {
      "Griffin PowerMate",
      "Griffin SoundKnob"
    };

    Powermate::Powermate(int mode) {
        char devname[256];
        fd = 0;

        for (int i = 0; i < NUM_EVENT_DEVICES; ++i) {
            sprintf(devname, "/dev/input/event%d", i);
            int r = TryDevice(devname, mode);
            if (r >= 0) {
                fd = r;
                return;
            }
        }

        throw IOException("No Powermate device node found under /dev/input");
    }

    Powermate::~Powermate() {
        if (fd >= 0) {
            close(fd);
        }
    }

    int Powermate::Read(struct input_event* eventBuffer, long unsigned int size) {
        int bytesRead = read(fd, eventBuffer, size);
        if (!bytesRead) {
            throw IOException("read() failed on Powermate device node");
        }
        return bytesRead / sizeof(struct input_event);
    }

    int Powermate::TryDevice(const char *devName, int mode) {
        int fdCurrent = open(devName, mode);
        if (fdCurrent < 0) {
            fprintf(stderr, "Unable to open \"%s\": %s\n", devName, strerror(errno));
            return -1;
        }

        char name[255];
        if (ioctl(fdCurrent, EVIOCGNAME(sizeof(name)), name) < 0) {
            fprintf(stderr, "\"%s\": EVIOCGNAME failed: %s\n", devName, strerror(errno));
            close(fdCurrent);
            return -1;
        }

        for (int i = 0; i < NUM_VALID_PREFIXES; ++i) {
        if (!strncasecmp(name, VALID_PREFIXES[i], strlen(VALID_PREFIXES[i])))
            return fdCurrent;
        }

        close(fdCurrent);
        return -1;
    }
}

