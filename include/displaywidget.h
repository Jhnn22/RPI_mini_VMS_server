#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include "gstthread.h"

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QLabel>

class DisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DisplayWidget(QWidget *parent = nullptr);
    void playVideo(QString rtspURI);
    void stopVideo();

signals:
    void focusChanged(DisplayWidget* widget, bool hasFocus);

protected:
    // 포커스가 되었다는 표시를 하기 위함
    void paintEvent(QPaintEvent *event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    bool m_hasFocus;
    GstThread* thread;
    QLabel* window;

    void initWindow();
};

#endif // DISPLAYWIDGET_H
