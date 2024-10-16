#include "videolistdialog.h"
#include "ui_videolistdialog.h"
#define TOP_DIR QString("../../saved_video")
#include <QDir>

VideoListDialog::VideoListDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::VideoListDialog)
{
    ui->setupUi(this);

    // 첫 화면은 비활성화
    ui->back_pushButton->setEnabled(false);
    ui->play_pushButton->setEnabled(false);

    // 디렉토리 목록을 가져와 추가
    QStringList dirNames = getSavedDirs();
    ui->listWidget->addItems(dirNames);

    // 디렉토리 클릭 시 파일 목록을 보여줌
    connect(ui->listWidget, &QListWidget::itemClicked, this, [=](QListWidgetItem *item) {
        if (item && ui->back_pushButton->isEnabled() == false && ui->play_pushButton->isEnabled() == false) { // 디렉토리, 파일 구분
            dirName = item->text();
            QStringList fileNames = getSavedFiles(dirName);
            listManage(fileNames, true);
        }
    });

    // "뒤로가기" 버튼 클릭 시 다시 디렉토리 목록을 보여줌
    connect(ui->back_pushButton, &QPushButton::clicked, this, [=]() {
        listManage(dirNames, false);
    });

    // "재생" 버튼 클릭 시 파일을 재생
    connect(ui->play_pushButton, &QPushButton::clicked, this, [=]() {
        QListWidgetItem *item = ui->listWidget->currentItem();
        if (item && ui->back_pushButton->isEnabled() == true && ui->play_pushButton->isEnabled() == true) {   // 디렉토리, 파일 구분
            fileName = item->text();
            QString fullPath = TOP_DIR + "/" + dirName + "/" + fileName;
            qDebug() << "재생 파일 경로: " << fullPath;
            emit play(fullPath);    //파일 재생 시그널 전송
            listManage(dirNames, false);
        }
    });

}

VideoListDialog::~VideoListDialog()
{
    delete ui;
}

QStringList VideoListDialog::getSavedDirs(){
    QDir dir(TOP_DIR);
    return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

QStringList VideoListDialog::getSavedFiles(const QString &dirName){
    QDir dir(TOP_DIR + "/" + dirName);
    return dir.entryList(QDir::Files);
}

void VideoListDialog::listManage(const QStringList &names, bool flag){
    ui->listWidget->clear();
    ui->listWidget->addItems(names);

    ui->back_pushButton->setEnabled(flag);
    ui->play_pushButton->setEnabled(flag);
}
