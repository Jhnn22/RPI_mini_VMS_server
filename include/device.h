#ifndef DEVICE_H
#define DEVICE_H

#include <QString>

#define DISCONNECTED    -1
#define CAMERA_ON       1
#define CAMERA_OFF      0
#define HOST_PORT       5000


class Device
{
    QString name;
    QString address;
    QString rtspPort;
    int status;

public:
    Device();
    Device(QString address);
    void registerDevice();
    int getStatus();
    void updateStatus();
    void turnOn();
    void turnOff();
    QString getName();
};

#endif // DEVICE_H
