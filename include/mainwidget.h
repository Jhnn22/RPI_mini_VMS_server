#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class DisplayWidget;

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

    void makePage1();
    void makePage2();

};
#endif // MAINWIDGET_H
