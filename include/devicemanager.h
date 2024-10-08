#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QList>

#include "device.h"

class DeviceManager
{
    QList<Device*> devices;
public:
    DeviceManager();


};

#endif // DEVICEMANAGER_H
