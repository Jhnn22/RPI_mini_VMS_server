#include "savedvideolist.h"
#include "ui_savedvideolist.h"
#define TOP_DIR QString("../../saved_video")
#include <QDir>

SavedVideoList::SavedVideoList(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SavedVideoList)
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
            QStringList files = getSavedFiles(dirName);
            ui->listWidget->clear();            // 목록을 지우고
            ui->listWidget->addItems(files);    // 파일 목록 추가

            // 파일 목록으로 전환되었으므로 버튼 활성화
            ui->back_pushButton->setEnabled(true);
            ui->play_pushButton->setEnabled(true);
        }
    });

    // "뒤로가기" 버튼 클릭 시 다시 디렉토리 목록을 보여줌
    connect(ui->back_pushButton, &QPushButton::clicked, this, [=]() {
        ui->listWidget->clear();
        ui->listWidget->addItems(dirNames);  // 다시 디렉토리 목록으로 복구

        // 다시 디렉토리 목록으로 돌아왔으므로 버튼 비활성화
        ui->back_pushButton->setEnabled(false);
        ui->play_pushButton->setEnabled(false);
    });

    // "재생" 버튼 클릭 시 파일을 재생
    connect(ui->play_pushButton, &QPushButton::clicked, this, [=]() {
        QListWidgetItem *item = ui->listWidget->currentItem();
        if (item && ui->back_pushButton->isEnabled() == true && ui->play_pushButton->isEnabled() == true) {   // 디렉토리, 파일 구분
            fileName = item->text();
            QString fullPath = TOP_DIR + "/" + dirName + "/" + fileName;
            qDebug() << "재생 파일 경로: " << fullPath;
            emit play(fullPath);
        }
    });

}

SavedVideoList::~SavedVideoList()
{
    delete ui;
}

QStringList SavedVideoList::getSavedDirs(){
    QDir dir(TOP_DIR);
    return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

QStringList SavedVideoList::getSavedFiles(const QString &dirName){
    QDir dir(TOP_DIR + "/" + dirName);
    return dir.entryList(QDir::Files);
}
