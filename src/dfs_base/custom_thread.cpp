#include "custom_thread.h"

void CustomThread::start()
{
    quit = false;
    fw.setFuture(QtConcurrent::run(this, &CustomThread::process));
    qDebug() << "CustomThread::stop();";
}

void CustomThread::stop()
{
    quit = true;
    fw.waitForFinished();
    qDebug() << "CustomThread::stop();";
}
