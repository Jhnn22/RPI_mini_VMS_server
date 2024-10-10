#ifndef DEVICE_H
#define DEVICE_H

#include <QString>

#define DISCONNECTED    -1
#define CAMERA_ON       1
#define CAMERA_OFF      0
#define HOST_PORT       8080

class QJsonDocument;

class Device
{
    QString name;
    QString address;
    QString rtspPort;
    QString mount;
    int status;

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
    // 테스트용
    void setName(QString name);
};

#endif // DEVICE_H
