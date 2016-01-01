#ifndef __IOEXCEPTION__DOT_H__SENTRY__
#define __IOEXCEPTION__DOT_H__SENTRY__

namespace PowermateScroll {

    class IOException {

    public: 
        IOException(const char* reason);
        const char* GetMessage();
        
    private: 
        const char* message;
    };
}

#endif
