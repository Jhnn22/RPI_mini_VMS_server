#include "mainwidget.h"
#include <QApplication>

#include <gst/gst.h>
#include <gst/video/videooverlay.h>

int main(int argc, char *argv[])
{
    gst_init(&argc, &argv);
    QApplication a(argc, argv);
    MainWidget w;
    w.show();

    return a.exec();
}
