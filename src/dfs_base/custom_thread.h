#ifndef CUSTOM_THREAD_H
#define CUSTOM_THREAD_H

#include <QtConcurrent/QtConcurrent>

class CustomThread
{
  public:
    void start();
    void stop();
    virtual void process() = 0;
  protected:
    std::atomic_bool quit;
  private:
    QFutureWatcher<void> fw;
};

#endif // CUSTOM_THREAD_H
