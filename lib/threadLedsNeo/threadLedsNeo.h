#ifndef __THREADLEDSNEO_H
#define __THREADLEDSNEO_H

#include "mbed.h"

class ThreadLedsNeo
{
protected:
    DigitalOut m_leds;
    Mutex mutex;
    bool m_data[720];
    void runThread();
    static void run(ThreadLedsNeo *p)
    {
        p->runThread();
    }
    Thread m_mainThread;
    static bool refreshEnabled;
    static int refreshTime;
    void setRefreshEnable(bool enabled = true) { refreshEnabled = enabled; }
public:
    ThreadLedsNeo(PinName pin, int refreshTimeInit = 50);
    ~ThreadLedsNeo();
    void setLed(int n, uint8_t r, uint8_t g, uint8_t b);
    void lock() { mutex.lock(); }
    void unlock() { mutex.unlock(); }
};

#endif
