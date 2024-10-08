#ifndef GSTTHREAD_H
#define GSTTHREAD_H

#include <QWidget>
#include <QThread>

class GstThread : public QThread
{
    Q_OBJECT
public:
    GstThread(WId windowId, QString rtspURI);
    void stop();

protected:
    void run() override;

private:
    WId windowId;
    QString rtspURL;
    bool stopped;
};

#endif // GSTTHREAD_H
