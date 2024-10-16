#ifndef DEVICE_H
#define DEVICE_H

#include <QString>
#include <QObject>
#include <QFuture>
#include <QtConcurrent>

#define DISCONNECTED    -1
#define CAMERA_ON       1
#define CAMERA_OFF      0
#define HOST_PORT       8080

class QJsonDocument;

class Device : public QObject
{
    Q_OBJECT
public:
    Device();
    Device(QString address);
    void registerDevice();
    int getStatus();
    void updateStatus();
    void turnOn();
    void turnOff();
    void setAddress(QString address);
    QString getName();
    QString getAddress();
    QString getRtspPort();
    QString getMount();
    QJsonDocument stringToJsonDoc(std::string& content);
    void setName(QString name);

    void startPeriodicUpdate(int interval = 3000);
    void stopPeriodicUpdate();

signals:
    void statusChanged(int status);

private:
    QString name;
    QString address;
    QString rtspPort;
    QString mount;
    int status;

    QFuture<void> updateFuture;
    bool isUpdateRunning;

    void periodicUpdate(int interval);
};

#endif // DEVICE_H
