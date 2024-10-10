#include "deviceregisterdialog.h"
#include "ui_deviceregisterdialog.h"
#include "device.h"

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

DeviceRegisterDialog::DeviceRegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DeviceRegisterDialog)
{
    ui->setupUi(this);
    device = new Device();

    ui->confirmButton->setDisabled(true);

    connect(ui->connectButton, &QPushButton::clicked, this, [=]{
        device->setAddress(ui->ipAddress->text());
        device->registerDevice();
        if (device->getStatus() != DISCONNECTED) {
            ui->connectButton->setDisabled(true);
            ui->confirmButton->setDisabled(false);
            ui->statusLabel->setText(tr("연결 완료. 추가 버튼을 눌러 장치를 추가해주세요."));
        }
        else {
            ui->statusLabel->setText(tr("연결 실패. 주소를 다시 확인해주세요."));
        }
    });

    connect(ui->confirmButton, &QPushButton::clicked, this, [=]{
        emit dataEntered(device);
        accept();
    });

    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

DeviceRegisterDialog::~DeviceRegisterDialog()
{
    delete ui;
}
