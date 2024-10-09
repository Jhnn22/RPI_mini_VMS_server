#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QList>

#include "device.h"

class DeviceManager
{
private:
    QList<Device*> devices;

    int findByName(QString name);
public:
    DeviceManager();

    void addDevice(Device* device);
    void removeDevice(QString name);
    void updateAll();
    void turnOnCamera(QString name);
    void turnOffCamera(QString name);
    QList<QString> getAllName();
    QList<int> getAllStatus();
    QString getAddress(QString name);
    int getStatus(QString name);
    // TODO: 파일 입출력
};

#endif // DEVICEMANAGER_H
