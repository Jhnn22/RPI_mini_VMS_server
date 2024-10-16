#include "displaywidget.h"
#include <QBoxLayout>
DisplayWidget::DisplayWidget(QWidget *parent) : QWidget(parent), m_hasFocus(false) {
    thread = NULL;
    initWindow();
}

void DisplayWidget::playVideo(QString rtspURI) {
    stopVideo();
    QString displayName = this->objectName();
    thread = new GstThread(window->winId(), rtspURI, displayName);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void DisplayWidget::stopVideo() {
    if (thread) {
        thread->stop();
        thread->quit();
        thread->wait();
        delete thread;
        thread = NULL;
    }
}

void DisplayWidget::startRecording(){
    if(thread){
        thread->startRecording();
    }
}

void DisplayWidget::endRecording(){
    if(thread){
        thread->endRecording();
    }
}


void DisplayWidget::initWindow() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(0);

    window = new QLabel(this);
    window->setScaledContents(true);
    window->setPixmap(QPixmap(":/image/standby_screen.png"));
    window->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    layout->addWidget(window);
    setLayout(layout);
}

void DisplayWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_hasFocus) {
        painter.setPen(QPen(Qt::red, 2));
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
