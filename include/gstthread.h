#ifndef GSTTHREAD_H
#define GSTTHREAD_H

#include <QWidget>
#include <QThread>

class GstThread : public QThread
{
    Q_OBJECT
public:
    GstThread(WId windowId, QString rtspURI);

protected:
    void run() override;

private:
    WId m_windowId;
    QString m_rtspURI;
};

#endif // GSTTHREAD_H
