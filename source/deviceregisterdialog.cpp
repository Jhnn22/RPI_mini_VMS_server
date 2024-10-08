#include "deviceregisterdialog.h"
#include <QWidget>
#include <QVBoxLayout>

DeviceRegisterDialog::DeviceRegisterDialog(QWidget *parent)
    :QDialog(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    address = new QLineEdit(this);
    connectButton = new QPushButton("연결", this);
    confirmButton = new QPushButton("추가", this);
    cancelButton = new QPushButton("취소", this);
    confirmButton->setDisabled(true);

    layout->addWidget(address);
    layout->addWidget(connectButton);
    layout->addWidget(confirmButton);
    layout->addWidget(cancelButton);

    device = new Device();

    connect(connectButton, &QPushButton::clicked, this, [=]{
        device->setAddress(address->text());
        device->registerDevice();
        if (device->getStatus() != DISCONNECTED) {
            connectButton->setDisabled(true);
            confirmButton->setDisabled(false);
        }
    });

    connect(confirmButton, &QPushButton::clicked, this, [=]{
        emit dataEntered(device);
        accept();
    });

    connect(cancelButton, &QPushButton::clicked, this, [=]{
        accept();
    });
}
