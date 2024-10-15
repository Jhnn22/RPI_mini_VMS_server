#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>

class DisplayWidget;
class DeviceManager;
class QListWidgetItem;
class SavedVideoList;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    Ui::MainWidget *ui;
    QList<QString> ips;
    QMap<DisplayWidget*, QPair<int, int>> displays;
    QWidget* page1;
    QWidget* page2;

    DisplayWidget* focusedDisplay;

    DeviceManager* deviceManager;

    void makePage1();
    void makePage2();
    void initDeviceList();
    void paintItem(QListWidgetItem* item, int status);

    SavedVideoList *savedVideoList;

    QMediaPlayer *mediaPlayer;
    QVideoWidget *videoWidget;
    void playSavedVideo(QString fullPath);
};
#endif // MAINWIDGET_H
