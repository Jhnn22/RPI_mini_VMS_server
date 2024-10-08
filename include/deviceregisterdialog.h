#ifndef DEVICEREGISTERDIALOG_H
#define DEVICEREGISTERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "device.h"

class DeviceRegisterDialog : public QDialog
{
    Q_OBJECT
public:
    DeviceRegisterDialog(QWidget *parent = nullptr);

signals:
    void dataEntered(Device* device);

private:
    QLineEdit* address;
    QPushButton* connectButton;
    QPushButton* confirmButton;
    QPushButton* cancelButton;
    Device* device;
};

#endif // DEVICEREGISTERDIALOG_H
