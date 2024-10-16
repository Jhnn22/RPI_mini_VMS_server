#include "device.h"
#include "httplib.h"
#include <QDebug>
#include <QJsonDocument>
using namespace std;

Device::Device() {
    this->name = "";
    this->address = "";
    this->status = DISCONNECTED;
    this->mount = "";
    this->rtspPort = "";
}

Device::Device(QString address) {
    this->name = "";
    this->address = address;
    this->status = DISCONNECTED;
    this->mount = "";
    this->rtspPort = "";
}

void Device::registerDevice() {
    qDebug() << "register";
    // 최초 등록
    httplib::Client client(address.toStdString(), HOST_PORT);
    // client.set_connection_timeout(0, 10000);
    httplib::Result result = client.Post("/register");
    if (result)  {
        if (result->status == httplib::StatusCode::OK_200) {
            qDebug() << result->body;
            QJsonDocument res = stringToJsonDoc(result->body);
            if (res["message"] == "error") {
                qDebug() << res["error"];
                return;
            }
            this->name = res["name"].toString();
            this->mount = res["mount"].toString();
            this->rtspPort = res["port"].toString();
            updateStatus();
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
    client.set_connection_timeout(0, 10000);
    httplib::Result result = client.Get("/cam-status");
    if (result)  {
        if (result->status == httplib::StatusCode::OK_200) {
            qDebug() << result->body;
            QJsonDocument res = stringToJsonDoc(result->body);
            if (res["message"] == "running") {
                this->status = CAMERA_ON;
            }
            else if (res["message"] == "stopped") {
                this->status = CAMERA_OFF;
            }
        }
    } else {
        auto error = result.error();
        qDebug() << "HTTP error:" << httplib::to_string(error);
        this->status = DISCONNECTED;
    }
}

void Device::turnOn() {
    httplib::Client client(address.toStdString(), HOST_PORT);
    QJsonValue jsonBody("{ \"camera\" : \"on\" }");
    QString body = jsonBody.toString();
    httplib::Result result = client.Post("/cam", body.toStdString(), "application/json");
    if (result)  {
        if (result->status == httplib::StatusCode::OK_200) {
            qDebug() << result->body;
            QJsonDocument res = stringToJsonDoc(result->body);
            if (res["message"] == "failed") {
                qDebug() << "camera on reqeust failed";
                return;
            }
            else if (res["message"] == "success") {
                this->status = CAMERA_ON;
            }
        }
    } else {
        auto error = result.error();
        qDebug() << "HTTP error:" << httplib::to_string(error);
    }
}

void Device::turnOff() {
    httplib::Client client(address.toStdString(), HOST_PORT);
    QJsonValue jsonBody("{ \"camera\" : \"off\" }");
    QString body = jsonBody.toString();
    httplib::Result result = client.Post("/cam", body.toStdString(), "application/json");
    if (result)  {
        if (result->status == httplib::StatusCode::OK_200) {
            qDebug() << result->body;
            QJsonDocument res = stringToJsonDoc(result->body);
            if (res["message"] == "failed") {
                qDebug() << "camera on reqeust failed";
                return;
            }
            else if (res["message"] == "success") {
                this->status = CAMERA_OFF;
            }
        }
    } else {
        auto error = result.error();
        qDebug() << "HTTP error:" << httplib::to_string(error);
    }
}

void Device::setAddress(QString address) {
    this->address = address;
}

QString Device::getName() {
    return this->name;
}

QString Device::getAddress() {
    return this->address;
}
QString Device::getRtspPort() {
    return this->rtspPort;
}

QString Device::getMount() {
    return this->mount;
}

QJsonDocument Device::stringToJsonDoc(std::string& content) {
    return QJsonDocument::fromJson(QString::fromStdString(content).toUtf8());
}
// test 용
void Device::setName(QString name) {
    this->name = name;
}
