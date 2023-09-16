// TestLog.hpp
#pragma once

#include <iostream>
#include <sstream>

/// Crude surrogate for std::cout so that I can enable/disable print statements
/// from a single place for performance testing of state machine eval code.

class TestLog
{
public:
    template <typename T>
    TestLog& operator << (const T &x) {
        if(mEnabled) { std::cout << x; }
        if(mCapturing) { mCaptureSS << x; }
        return *this;
    }

    TestLog& operator<< (std::ostream& (*f)(std::ostream &)) {
        if(mEnabled) { f(std::cout); }
        return *this;
    }

    TestLog& operator<< (std::ostream& (*f)(std::ios &)) {
        if(mEnabled) { f(std::cout); }
        return *this;
    }

    TestLog& operator<< (std::ostream& (*f)(std::ios_base &)) {
        if(mEnabled) { f(std::cout); }
        return *this;
    }

    void enable()
    {
        std::cout << "*** Enabling TestLog Output ***" << std::endl;
        mEnabled = true;
    }
    void disable()
    {
        std::cout << "*** Disabling TestLog Output ***" << std::endl;
        mEnabled = false;
    }

    void startCapture()
    {
        mCaptureSS.clear();
        mCaptureSS.str(std::string());
        mCapturing = true;
    }
    void stopCapture() { mCapturing = false; }
    std::string getCaptured() { return mCaptureSS.str(); }

    static TestLog& instance()
    {
        static TestLog sInstance;
        return sInstance;
    }
private:
    TestLog() { mEnabled = true; }
    bool mEnabled;

    // Capture log traffic to an internal strinstream for retrieval later.
    bool mCapturing;
    std::stringstream mCaptureSS;
};

