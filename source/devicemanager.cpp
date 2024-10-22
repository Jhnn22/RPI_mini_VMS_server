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
    connect(device, &Device::statusChanged, this, [=](int status) {
        emit deviceStatusChanged(device->getName(), status);
    });
    device->startPeriodicUpdate();
    this->devices.append(device);
    qDebug() << "Device Added";
}

void DeviceManager::removeDevice(QString name) {
    int found = findByName(name);
    if (found == -1) {
        qDebug() << "There's no device";
        return;
    }
    this->devices[found]->stopPeriodicUpdate();
    this->devices.removeAt(found);
    qDebug() << "Device removed";
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

QString DeviceManager::getAddress(QString name) {
    int found = findByName(name);
    if (found == -1) {
        qDebug() << "There's no device";
        return "";
    }
    Device* device = this->devices[found];
    QString address = device->getAddress() + ":" + device->getRtspPort() + device->getMount();
    return address;
}

int DeviceManager::getStatus(QString name) {
    int found = findByName(name);
    if (found == -1) {
        qDebug() << "There's no device";
        return -2;
    }
    return this->devices[found]->getStatus();
}
