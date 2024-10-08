#include "device.h"
#include "httplib.h"
#include <QDebug>
using namespace std;

Device::Device() {
    this->name = "";
    this->address = "";
    this->status = DISCONNECTED;
    this->rtspPort = "";
}

Device::Device(QString address) {
    this->name = "";
    this->address = address;
    this->status = DISCONNECTED;
    this->rtspPort = "";
}

void Device::registerDevice() {
    // 최초 등록
    httplib::Client client(address.toStdString(), HOST_PORT);
    httplib::Result result = client.Get("/register");
    if (result)  {
        if (result->status == httplib::StatusCode::OK_200) {
            qDebug() << result->body;
        }
    } else {
        auto error = result.error();
        qDebug() << "HTTP error:" << httplib::to_string(error);
    }
}

int  Device::getStatus() {
    return this->status;
}

void Device::updateStatus() {
    httplib::Client client(address.toStdString(), HOST_PORT);
    httplib::Result result = client.Get("/cam-status");
    if (result)  {
        if (result->status == httplib::StatusCode::OK_200) {
            qDebug() << result->body;
        }
    } else {
        auto error = result.error();
        qDebug() << "HTTP error:" << httplib::to_string(error);
    }
}

void Device::turnOn() {
    httplib::Client client(address.toStdString(), HOST_PORT);
    httplib::Params params;
    params.emplace("camera", "on");
    httplib::Result result = client.Post("/cam", params);
    if (result)  {
        if (result->status == httplib::StatusCode::OK_200) {
            qDebug() << result->body;
        }
    } else {
        auto error = result.error();
        qDebug() << "HTTP error:" << httplib::to_string(error);
    }
}

void Device::turnOff() {
    httplib::Client client(address.toStdString(), HOST_PORT);
    httplib::Params params;
    params.emplace("camera", "off");
    httplib::Result result = client.Post("/cam", params);
    if (result)  {
        if (result->status == httplib::StatusCode::OK_200) {
            qDebug() << result->body;
        }
    } else {
        auto error = result.error();
        qDebug() << "HTTP error:" << httplib::to_string(error);
    }
}

QString Device::getName() {
    return this->name;
}
