#ifndef DEVICEREGISTERDIALOG_H
#define DEVICEREGISTERDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;
class Device;

namespace Ui {
class DeviceRegisterDialog;
}

class DeviceRegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceRegisterDialog(QWidget *parent = nullptr);
    ~DeviceRegisterDialog();

signals:
    void dataEntered(Device* device);

private:
    Ui::DeviceRegisterDialog *ui;
    QLineEdit* address;
    QPushButton* connectButton;
    QPushButton* confirmButton;
    QPushButton* cancelButton;
    Device* device;
};

#endif // DEVICEREGISTERDIALOG_H
