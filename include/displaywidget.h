#ifndef DISPLAYWIDGET_H
#define DISPLAYWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QDebug>

class DisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DisplayWidget(QWidget *parent = nullptr);

signals:
    void focusChanged(DisplayWidget* widget, bool hasFocus);

protected:
    void paintEvent(QPaintEvent *event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    bool m_hasFocus;
};

#endif // DISPLAYWIDGET_H
