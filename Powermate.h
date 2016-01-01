#ifndef __POWERMATE__DOT_H__SENTRY__
#define __POWERMATE__DOT_H__SENTRY__

namespace PowermateScroll {

    class Powermate {

    public: 
        Powermate(int mode);
        ~Powermate();
        int Read(struct input_event* eventBuffer, long unsigned int size);
        
    private: 
        // File descriptor of the powermate device node
        int fd;
        
        int TryDevice(const char *devName, int mode);
    };
}

#endif
