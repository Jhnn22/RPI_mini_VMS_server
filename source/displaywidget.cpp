#include "displaywidget.h"

DisplayWidget::DisplayWidget(QWidget *parent) : QWidget(parent), m_hasFocus(false)
{
}

void DisplayWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_hasFocus) {
        painter.setPen(QPen(Qt::blue, 2));
    } else {
        painter.setPen(QPen(Qt::black, 1));
    }

    painter.drawRect(rect().adjusted(2, 2, -2, -2));
}

void DisplayWidget::focusInEvent(QFocusEvent* event)
{
    QWidget::focusInEvent(event);
    m_hasFocus = true;
    update();

    emit focusChanged(this, m_hasFocus);
}

void DisplayWidget::focusOutEvent(QFocusEvent* event)
{
    QWidget::focusOutEvent(event);
    m_hasFocus = false;
    update();

    emit focusChanged(this, m_hasFocus);
}

void DisplayWidget::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    qDebug() << "focus" << objectName();
    setFocus();
}
