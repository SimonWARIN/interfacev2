#include "threadLedsNeo.h"

bool ThreadLedsNeo::refreshEnabled = true;
int ThreadLedsNeo::refreshTime = 50;

ThreadLedsNeo::ThreadLedsNeo(PinName pin, int refreshTimeInit) : m_leds(pin, 0) {
    refreshTime = refreshTimeInit;
    m_mainThread.start(callback(ThreadLedsNeo::run, this));
    for (int i = 0; i < 720; i++) {
        m_data[i] = false;
    }
}

ThreadLedsNeo::~ThreadLedsNeo() {
    m_mainThread.join();
}

void ThreadLedsNeo::runThread() {
    while (1) {
        if (refreshEnabled) {
            mutex.lock();
            bool *p = &m_data[0];
            CriticalSectionLock::enable();
            for (int i = 0; i < 720; i++) {
                if (!(*p)) {
                m_leds = 1;
                    wait_ns(1);
                    m_leds = 0;
                    wait_ns(500);
                } else {
                m_leds = 1;
                    wait_ns(400);
                    m_leds = 0;
                    wait_ns(400);
                }
                p++;
            }
            CriticalSectionLock::disable();
            mutex.unlock();
        }
        ThisThread::sleep_for(chrono::milliseconds(refreshTime));
    }
}

void ThreadLedsNeo::setLed(int n, uint8_t r, uint8_t g, uint8_t b)
{
    if ((n<0)||(n>=30)) return;
    n = n * 24;
    uint8_t masque = 0x80;
    for (int i=0; i<8; i++) {
        m_data[n++] = (g & masque) != 0;
        masque >>= 1;
    }
    masque = 0x80;
    for (int i=0; i<8; i++) {
        m_data[n++] = (r & masque) != 0;
        masque >>= 1;
    }
    masque = 0x80;
    for (int i=0; i<8; i++) {
        m_data[n++] = (b & masque) != 0;
        masque >>= 1;
    }
}
