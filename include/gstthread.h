#ifndef GSTTHREAD_H
#define GSTTHREAD_H

#include <QWidget>
#include <QThread>
#include <gst/gst.h>
#include <opencv2/opencv.hpp>

class GstThread : public QThread
{
    Q_OBJECT
public:
    GstThread(WId windowId, QString rtspURI, QString displayName);

    typedef struct _CustomData{
        GstElement *pipeline, *source, *depay, *parse, *decode, *convert, *tee;
        GstElement *video_queue, *video_sink;
        GstElement *app_queue, *app_sink;
        cv::VideoWriter *videoWriter = nullptr;
        cv::Mat before, after;
        QString path;
        bool isRecording = false;
    } CustomData;
    CustomData *data;

    void stop();
    void startRecording();
    void endRecording();

protected:
    void run() override;

private:
    WId windowId;
    QString rtspURL;
    QString displayName;
    QString str;
    bool stopped;
};

#endif // GSTTHREAD_H
