#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "displaywidget.h"
#include "devicemanager.h"
#include "deviceregisterdialog.h"

#include <string>

#include <QMessageBox>
#include <QListWidgetItem>
#include <QTimer>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    makePage1();
    makePage2();

    ui->stackedWidget->setCurrentIndex(0);

    initDeviceList();

    // 확대 버튼
    connect(ui->zoom_in_pushButton, &QPushButton::clicked, this, [&](){
        page2->layout()->addWidget(focusedDisplay);
        ui->stackedWidget->setCurrentIndex(1);
    });
    // 축소 버튼
    connect(ui->zoom_out_pushButton, &QPushButton::clicked, this, [&](){
        QGridLayout* page1_layout = qobject_cast<QGridLayout*>(page1->layout());
        if (page1_layout) {
            page1_layout->addWidget(focusedDisplay, displays[focusedDisplay].first, displays[focusedDisplay].second);
        }
        ui->stackedWidget->setCurrentIndex(0);
    });
    // 연결 버튼
    connect(ui->connectButton, &QPushButton::clicked, this, [=]() {
        // TODO : url 체크
        QListWidgetItem* item = ui->listWidget->currentItem();
        if (!item) {
            qDebug() << "There's no item";
            return;
        }
        int deviceStatus = deviceManager->getStatus(item->text());
        if (deviceStatus == DISCONNECTED) {
            qDebug() << "disconnected";
            return;
        }
        else if (deviceStatus == CAMERA_OFF) {
            qDebug() << "camera is not running";
            return;
        }

        QString address = deviceManager->getAddress(item->text());
        if (address != "" && focusedDisplay) {
            qDebug() << "start video at" << focusedDisplay->objectName() << address;
            focusedDisplay->playVideo(address);
        }
    });
    // 연결 해제 버튼
    connect(ui->disconnectButton, &QPushButton::clicked, this, [=]() {
        focusedDisplay->stopVideo();
    });
    // 장치 추가 버튼
    connect(ui->addDeviceButton, &QPushButton::clicked, this, [=]() {
        DeviceRegisterDialog* dialog = new DeviceRegisterDialog();
        connect(dialog, &DeviceRegisterDialog::dataEntered, this, [=](Device* device) {
            // 장치 매니저 추가
            deviceManager->addDevice(device);
            // listWidget 추가
            QListWidgetItem* item = new QListWidgetItem(device->getName(), ui->listWidget);
            paintItem(item, device->getStatus());
            item->setTextAlignment(Qt::AlignCenter);
        });
        dialog->exec();
        dialog->deleteLater();
    });
    // 장치 삭제 버튼
    connect(ui->removeDeviceButton, &QPushButton::clicked, this, [=]() {
        QListWidgetItem* selectedItem = ui->listWidget->currentItem();
        if (selectedItem) {
            QMessageBox::StandardButton reply = QMessageBox::question(this,
                "삭제 확인",
                selectedItem->text() + tr("를 삭제합니다."),
                QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::Yes) {
                // 장치 관리자에서 제거
                deviceManager->removeDevice(selectedItem->text());

                // 리스트 위젯에서 제거
                int row = ui->listWidget->row(selectedItem);
                ui->listWidget->takeItem(row);
                delete selectedItem;
            }
        }
    });
    // 카메라 켜기 버튼
    connect(ui->cameraOnButton, &QPushButton::clicked, this, [=]() {
        QListWidgetItem* selectedItem = ui->listWidget->currentItem();
        if (selectedItem) {
            deviceManager->turnOnCamera(selectedItem->text());
        }
    });
    // 카메라 끄기 버튼
    connect(ui->cameraOffButton, &QPushButton::clicked, this, [=]() {
        QListWidgetItem* selectedItem = ui->listWidget->currentItem();
        if (selectedItem) {
            deviceManager->turnOffCamera(selectedItem->text());
        }
    });
    // 주기적으로 장치 상태 업데이트
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() {
        deviceManager->updateAll();
        QList<int> status = this->deviceManager->getAllStatus();
        for (int i = 0; i < status.size(); i++) {
            auto item = ui->listWidget->item(i);
            paintItem(item, status[i]);
        }
    });
    timer->start(10000);
}

void MainWidget::makePage1() {
    /* 아홉게로 구성된 화면을 보여주는 페이지1 */
    page1 = new QWidget();

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(1);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            DisplayWidget* display = new DisplayWidget(page1);
            displays[display] = qMakePair(i, j);
            display->setObjectName(QString("display_%1").arg(i*3 + j));
            connect(display, &DisplayWidget::focusChanged, this, [=](DisplayWidget* widget, bool hasFocus) {
                if (hasFocus) {
                    focusedDisplay = widget;
                    qDebug() << "focussed display changed" << focusedDisplay->objectName();
                }
            });
            display->setContentsMargins(2,2,2,2);
            gridLayout->addWidget(display, i, j);
        }
    }
    page1->setLayout(gridLayout);
    ui->stackedWidget->addWidget(page1);
}

void MainWidget::makePage2() {
    /* 집중하고 싶은 화면을 보여주는 페이지2 */
    page2 = new QWidget();
    page2->setLayout(new QHBoxLayout());
    ui->stackedWidget->addWidget(page2);
}

void MainWidget::initDeviceList() {
    deviceManager = new DeviceManager();
    // deviceManager->load();
    // -----------------------test --------------
    Device* d1 = new Device(tr("192.168.0.1"));
    Device* d2 = new Device(tr("192.168.0.2"));
    Device* d3 = new Device(tr("192.168.0.3"));
    d1->setName("test1");
    d2->setName("test2");
    d3->setName("test3");
    deviceManager->addDevice(d1);
    deviceManager->addDevice(d2);
    deviceManager->addDevice(d3);
    for (int i = 0; i < 10; i++) {
        Device* d = new Device(tr("222.222.222.222"));
        d->setName(QString::fromStdString(std::to_string(i)));
        deviceManager->addDevice(d);
    }
    //------------------------------------------

    QList<QString> names = this->deviceManager->getAllName();
    QList<int> status = this->deviceManager->getAllStatus();
    for (int i = 0; i < names.size(); i++) {
        qDebug() << names[i];
        QListWidgetItem* item = new QListWidgetItem(names[i], ui->listWidget);
        paintItem(item, status[i]);
        item->setTextAlignment(Qt::AlignCenter);
    }
}

void MainWidget::paintItem(QListWidgetItem* item, int status) {
    switch (status) {
    case DISCONNECTED:
        item->setForeground(QBrush(Qt::gray));
        break;
    case CAMERA_OFF:
        item->setForeground(QBrush(Qt::red));
        break;
    case CAMERA_ON:
        item->setForeground(QBrush(Qt::green));
        break;
    default:
        break;
    }
}

MainWidget::~MainWidget()
{
    delete page1;
    delete page2;
    delete ui;
    delete deviceManager;
}
