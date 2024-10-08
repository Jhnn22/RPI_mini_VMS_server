#include "devicemanager.h"

#include <QDebug>

DeviceManager::DeviceManager() {}

int DeviceManager::findByName(QString name) {
    for (int i = 0; i < devices.size(); i++) {
        if (devices[i]->getName() == name) {
            return i;
        }
    }
    return -1;
}

void DeviceManager::addDevice(Device* device) {
    this->devices.append(device);
    qDebug() << "Device Added";
}

void DeviceManager::removeDevice(QString name) {
    int found = findByName(name);
    if (found == -1) {
        qDebug() << "There's no device";
        return;
    }
    this->devices.removeAt(found);
    qDebug() << "Device removed";
}

void DeviceManager::updateAll() {
    for (auto device : this->devices) {
        device->updateStatus();
    }
}

void DeviceManager::turnOnCamera(QString name) {
    int found = findByName(name);
    if (found == -1) {
        qDebug() << "There's no device";
        return;
    }
    this->devices[found]->turnOn();
}

void DeviceManager::turnOffCamera(QString name) {
    int found = findByName(name);
    if (found == -1) {
        qDebug() << "There's no device";
        return;
    }
    this->devices[found]->turnOff();
}

QList<QString> DeviceManager::getAllName() {
    QList<QString> names;
    for (auto device : this->devices) {
        names.append(device->getName());
    }
    return names;
}

QList<int> DeviceManager::getAllStatus() {
    QList<int> status;
    for (auto device : this->devices) {
        status.append(device->getStatus());
    }
    return status;
}
