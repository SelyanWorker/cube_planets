#pragma once

#include <android/log.h>
#include <sstream>

namespace cp {

    extern std::ostream out;

    class LogBuffer: public std::stringbuf {
    public:
        explicit LogBuffer(const std::string& logTag)
        : logTag(logTag){}

    protected:
        virtual int sync() override {
            __android_log_print(ANDROID_LOG_DEBUG, logTag.c_str(), "%s", str().c_str());
            str("");
            return 0;
        }

    private:
        std::string logTag;
    };

}