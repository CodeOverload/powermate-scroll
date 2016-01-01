#include "IOException.h"

namespace PowermateScroll {

    IOException::IOException(const char* reason) {
        message = reason;
    }

    const char* IOException::GetMessage() {
        return message;
    }
}

