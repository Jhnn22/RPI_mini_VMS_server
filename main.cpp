#include "mainwidget.h"
#include "gstthread.h"
#include <QApplication>

#include <gst/gst.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gst_init(&argc, &argv);
    MainWidget w;
    w.show();

    // GstThread gstThread(w.winId());
    // gstThread.start();

    return a.exec();
}
