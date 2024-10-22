#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QList>
#include <QObject>

#include "device.h"

class DeviceManager : public QObject
{
    Q_OBJECT
private:
    QList<Device*> devices;

    int findByName(QString name);
public:
    DeviceManager();

    void addDevice(Device* device);
    void removeDevice(QString name);
    void turnOnCamera(QString name);
    void turnOffCamera(QString name);
    QList<QString> getAllName();
    QList<int> getAllStatus();
    QString getAddress(QString name);
    int getStatus(QString name);
    // TODO: 파일 입출력

signals:
    void deviceStatusChanged(QString name, int status);
};

#endif // DEVICEMANAGER_H
