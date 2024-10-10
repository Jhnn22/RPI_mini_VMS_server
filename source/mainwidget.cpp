#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "displaywidget.h"
#include <QDir>
#include <QListWidget>
#include <QDialog>

#define TOP_DIR QString("../../saved_video")


MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    makePage1();
    makePage2();

    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget_2->setCurrentIndex(0);

    // stackedWidget_2 버튼 설정
    connect(ui->zoom_in_pushButton, &QPushButton::clicked, this, [&](){
        page2->layout()->addWidget(focusedDisplay);
        ui->stackedWidget_2->setCurrentIndex(1);
    });
    connect(ui->zoom_out_pushButton, &QPushButton::clicked, this, [&](){
        QGridLayout* page1_layout = qobject_cast<QGridLayout*>(page1->layout());
        if (page1_layout) {
            page1_layout->addWidget(focusedDisplay, displays[focusedDisplay].first, displays[focusedDisplay].second);
        }
        ui->stackedWidget_2->setCurrentIndex(0);
    });

    connect(ui->change_pushButton, &QPushButton::clicked, this, [=]() {
        // TODO : url 체크
        QString url = ui->ip_lineEdit->text();
        if (url != "" && focusedDisplay) {
            qDebug() << "start video" << focusedDisplay->objectName() << url;
            focusedDisplay->playVideo(url);
        }
    });

    connect(ui->delete_pushButton, &QPushButton::clicked, this, [=]() {
        focusedDisplay->stopVideo();
    });

    connect(ui->load_pushButton, &QPushButton::clicked, this, [=](){
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle("저장 목록");
        dialog->setFixedSize(300, 300);

        QVBoxLayout *layout = new QVBoxLayout(dialog);

        // 초기 상태는 디렉토리 목록
        QListWidget *listWidget = new QListWidget(dialog);
        layout->addWidget(listWidget);

        // "뒤로가기" 버튼과 "재생" 버튼을 추가하고 비활성화
        QPushButton *backButton = new QPushButton("뒤로가기", dialog);
        QPushButton *playButton = new QPushButton("재생", dialog);
        backButton->setEnabled(false);  // 처음에는 비활성화
        playButton->setEnabled(false);  // 처음에는 비활성화
        layout->addWidget(backButton);
        layout->addWidget(playButton);

        // 디렉토리 목록을 가져와 추가
        QStringList dirNames = getSavedDirs();
        listWidget->addItems(dirNames);

        // 디렉토리 클릭 시 파일 목록을 보여줌
        connect(listWidget, &QListWidget::itemClicked, this, [=](QListWidgetItem *item) {
            if (item && backButton->isEnabled() == false && playButton->isEnabled() == false) { // 디렉토리, 파일 구분
                QString dirName = item->text();
                QStringList files = getSavedFiles(dirName);
                listWidget->clear();            // 목록을 지우고
                listWidget->addItems(files);    // 파일 목록 추가
                dialog->setWindowTitle(dirName + " 폴더의 파일 목록");

                // 파일 목록으로 전환되었으므로 버튼 활성화
                backButton->setEnabled(true);
                playButton->setEnabled(true);
            }
        });

        // "뒤로가기" 버튼 클릭 시 다시 디렉토리 목록을 보여줌
        connect(backButton, &QPushButton::clicked, this, [=]() {
            listWidget->clear();
            listWidget->addItems(dirNames);  // 다시 디렉토리 목록으로 복구
            dialog->setWindowTitle("저장 목록");

            // 다시 디렉토리 목록으로 돌아왔으므로 버튼 비활성화
            backButton->setEnabled(false);
            playButton->setEnabled(false);
        });

        // "재생" 버튼 클릭 시 파일을 재생
        connect(playButton, &QPushButton::clicked, this, [=]() {
            QListWidgetItem *item = listWidget->currentItem();
            if (item && backButton->isEnabled() == true && playButton->isEnabled() == true) {   // 디렉토리, 파일 구분
                QString fileName = item->text();
                QString dirName = dialog->windowTitle().replace(" 폴더의 파일 목록", "");
                QString fullPath = TOP_DIR + "/" + dirName + "/" + fileName;
                qDebug() << "재생 파일 경로: " << fullPath;
                // 재생 관련 로직 추가
                playSavedVideo(fullPath);
                dialog->accept();  // 재생 후 다이얼로그 닫기
            }
        });

        dialog->exec();

    });
}

QStringList MainWidget::getSavedDirs(){
    QDir dir(TOP_DIR);
    return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

QStringList MainWidget::getSavedFiles(const QString &dirName){
    QDir dir(TOP_DIR + "/" + dirName);
    return dir.entryList(QDir::Files);
}

void MainWidget::playSavedVideo(QString fullPath){
    // 화면 전환
    ui->stackedWidget->setCurrentIndex(1);

    // VideoWidget 설정
    videoWidget = new QVideoWidget(this);
    ui->layout->addWidget(videoWidget);

    // MediaPlayer 설정
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setVideoOutput(videoWidget);
    mediaPlayer->setSource(QUrl::fromLocalFile(fullPath));

    // 버튼 연결
    connect(ui->back_pushButton, &QPushButton::clicked, this, [=](){
        mediaPlayer->stop();    // mediaPlayer 정지
        // mediaPlayer 삭제
        mediaPlayer->deleteLater();
        // videoWidget 삭제
        ui->layout->removeWidget(videoWidget);
        videoWidget->deleteLater();
        videoWidget = nullptr;
        // 초기 화면으로 이동
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->play_pushButton, &QPushButton::clicked, mediaPlayer, &QMediaPlayer::play);
    connect(ui->pause_pushButton, &QPushButton::clicked, mediaPlayer, &QMediaPlayer::pause);
    connect(ui->stop_pushButton, &QPushButton::clicked, mediaPlayer, &QMediaPlayer::stop);
    connect(ui->rewind_pushButton, &QPushButton::clicked, this, [this](){
        mediaPlayer->setPosition(std::max<qint64>(0, mediaPlayer->position() - 5000));
    });
    connect(ui->forward_pushButton, &QPushButton::clicked, this, [this](){
        mediaPlayer->setPosition(mediaPlayer->position() + 5000);
    });



    // 비디오 재생 시작
    mediaPlayer->play();
}

void MainWidget::makePage1() {
    /* 아홉게로 구성된 화면을 보여주는 페이지1 */
    page1 = new QWidget();
    focusedDisplay = nullptr;   // 포커스 초기화

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
    ui->stackedWidget_2->addWidget(page1);
}

void MainWidget::makePage2() {
    /* 집중하고 싶은 화면을 보여주는 페이지2 */
    page2 = new QWidget();
    page2->setLayout(new QHBoxLayout());
    ui->stackedWidget_2->addWidget(page2);
}

MainWidget::~MainWidget()
{
    delete page1;
    delete page2;
    delete ui;
}
