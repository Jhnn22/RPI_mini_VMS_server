#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "displaywidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    makePage1();
    makePage2();

    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->zoom_in_pushButton, &QPushButton::clicked, this, [&](){
        page2->layout()->addWidget(focusedDisplay);
        ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->zoom_out_pushButton, &QPushButton::clicked, this, [&](){
        QGridLayout* page1_layout = qobject_cast<QGridLayout*>(page1->layout());
        if (page1_layout) {
            page1_layout->addWidget(focusedDisplay, displays[focusedDisplay].first, displays[focusedDisplay].second);
        }
        ui->stackedWidget->setCurrentIndex(0);
    });

    connect(ui->change_pushButton, &QPushButton::clicked, this, [=]() {
        // TODO : url 체크
        // 만약 focus된 화면이면 안바뀌게 - 저장할 수 없으니까
        // QString url = ui->ip_lineEdit->text();
        // if (url != "" && focusedDisplay) {
        //     qDebug() << "start video" << focusedDisplay->objectName() << url;
        //     focusedDisplay->playVideo(url);
        // }
    });

    connect(ui->delete_pushButton, &QPushButton::clicked, this, [=]() {
        focusedDisplay->stopVideo();
    });
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

MainWidget::~MainWidget()
{
    delete page1;
    delete page2;
    delete ui;
}
