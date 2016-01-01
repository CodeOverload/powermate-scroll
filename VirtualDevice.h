#ifndef __VIRTUALDEVICE__DOT_H__SENTRY__
#define __VIRTUALDEVICE__DOT_H__SENTRY__

namespace PowermateScroll {

    class VirtualDevice {

    public: 
        VirtualDevice();
        ~VirtualDevice();
        
        void Scroll(int amount);
        void ChangeVolume(bool up);
        
    private: 
        // uinput file descriptor
        int fd;
        
        void Create();
        void SetParams();
        void SetParam(unsigned long request, int bit);
        
        // Sends a report event, which indicates that we won't be writing any more
        // events for the current command
        void SendReportEvent();
    };
}

#endif
