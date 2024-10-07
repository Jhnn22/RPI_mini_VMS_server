#include "gstthread.h"
#include <glib.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <opencv2/highgui.hpp>
using namespace std;

typedef struct _CustomData{
    GstElement *pipeline, *source, *depay, *tee;
    GstElement *video_queue, *parse, *decode, *convert, *video_sink;
    GstElement *app_queue, *app_sink;

} CustomData;

static void pad_added_handler(GstElement *src, GstPad *new_pad, GstElement *depay) {
    /*
     * 동적 패드 연결을 위한 핸들러
     * rtsp로 받는 source는 여상 데이터일 수도 있고, 음성 데이터일 수도 있기 때문에
     * 동적으로 패드를 연결해야 함.
     */

    GstPad *sink_pad = gst_element_get_static_pad(depay, "sink");
    if (gst_pad_is_linked(sink_pad)) {
        g_object_unref(sink_pad);
        return;
    }

    GstPadLinkReturn ret;
    GstCaps *new_pad_caps = gst_pad_query_caps(new_pad, NULL);
    GstStructure *new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
    const gchar *new_pad_type = gst_structure_get_name(new_pad_struct);

    // Check if the new pad type is video
    if (g_str_has_prefix(new_pad_type, "application/x-rtp")) {
        ret = gst_pad_link(new_pad, sink_pad);
        if (GST_PAD_LINK_FAILED(ret)) {
            g_printerr("Type is '%s' but link failed.\n", new_pad_type);
        } else {
            g_print("Link succeeded with pad type '%s'.\n", new_pad_type);
        }
    }

    gst_caps_unref(new_pad_caps);
    g_object_unref(sink_pad);
}

static GstFlowReturn new_sample (GstElement *sink, CustomData *data) {
    GstSample *sample;

    g_signal_emit_by_name (sink, "pull-sample", &sample);
    if (sample) {
        /* The only thing we do in this example is print a * to indicate a received buffer */
        g_print ("*");
        gst_sample_unref (sample);
        return GST_FLOW_OK;
    }

    return GST_FLOW_ERROR;
}

GstThread::GstThread(WId windowId, QString rtspURL) : windowId(windowId), rtspURL(rtspURL) {
    stopped = false;
}

void GstThread::stop() {
    stopped = true;
}

void GstThread::run() {
    // gst_debug_set_default_threshold(GST_LEVEL_WARNING);  // gst 디버그 범위 설정 코드
    CustomData data;
    GstPad *tee_video_pad, *tee_app_pad;
    GstPad *queue_video_pad, *queue_app_pad;
    GstBus *bus;
    GstMessage *msg;

    // 파이프라인 생성
    data.pipeline = gst_pipeline_new("rtsp-player");

    // 요소 생성
    data.source = gst_element_factory_make("rtspsrc", "source");
    data.depay = gst_element_factory_make("rtph264depay", "depay");
    data.tee = gst_element_factory_make("tee", "tee");

    data.video_queue = gst_element_factory_make("queue", "video_queue");
    data.parse = gst_element_factory_make("h264parse", "parse");
    data.decode = gst_element_factory_make("openh264dec", "decode");
    data.convert = gst_element_factory_make("videoconvert", "convert");
    data.video_sink = gst_element_factory_make("glimagesink", "video_sink");

    data.app_queue = gst_element_factory_make("queue", "app_queue");
    data.app_sink = gst_element_factory_make("appsink", "app_sink");

    // 모든 요소가 생성되었는지 확인
    if (!data.pipeline) {
        g_printerr("Failed to create pipeline element.\n");
        return;
    } else if (!data.source) {
        g_printerr("Failed to create source element.\n");
        return;
    } else if (!data.depay) {
        g_printerr("Failed to create depay element.\n");
        return;
    } else if (!data.tee) {
        g_printerr("Failed to create tee element.\n");
        return;
    } else if (!data.video_queue) {
        g_printerr("Failed to create video_queue element.\n");
        return;
    } else if (!data.parse) {
        g_printerr("Failed to create parse element.\n");
        return;
    } else if (!data.decode) {
        g_printerr("Failed to create decode element.\n");
        return;
    } else if (!data.convert) {
        g_printerr("Failed to create convert element.\n");
        return;
    } else if (!data.video_sink) {
        g_printerr("Failed to create video_sink element.\n");
        return;
    } else if (!data.app_queue) {
        g_printerr("Failed to create app_queue element.\n");
        return;
    } else if (!data.app_sink) {
        g_printerr("Failed to create app_sink element.\n");
        return;
    }
    g_print("All elements created successfully.\n");


    // 파이프라인에 요소 추가
    gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.depay, data.tee
                     , data.video_queue, data.parse, data.decode, data.convert, data.video_sink
                     , data.app_queue, data.app_sink, nullptr);

    // source와 depay 연결 간 동적 패드 처리
    // pad-added signal 처리
    g_signal_connect(data.source, "pad-added", G_CALLBACK(pad_added_handler), data.depay);

    // app_sink 설정
    g_object_set(data.app_sink, "emit-signals", TRUE, nullptr);
    g_signal_connect(data.app_sink, "new-sample", G_CALLBACK(new_sample), &data);

    // RTSP 소스 속성 설정
    string rtsp_location = "rtsp://"+ rtspURL.toStdString() +":8554/test";
    g_object_set(G_OBJECT(data.source), "location", rtsp_location.c_str(), nullptr);


    // 요소 연결
    if (!gst_element_link_many(data.depay, data.tee, nullptr)) {
        g_printerr("depay | tee could not be linked.\n");
        gst_object_unref(data.pipeline);
        return;
    }
    if(!gst_element_link_many(data.video_queue, data.parse, data.decode, data.convert, data.video_sink, nullptr)){
        g_printerr("video_queue | parse | decode | convert | video_sink tee could not be linked.\n");
        gst_object_unref(data.pipeline);
        return;
    }
    if (!gst_element_link_many(data.app_queue, data.app_sink, nullptr)) {
        g_printerr("app_queue | app_sink could not be linked.\n");
        gst_object_unref(data.pipeline);
        return;
    }

    // tee 요소 pad 설정
    tee_video_pad = gst_element_request_pad_simple (data.tee, "src_%u");
    g_print ("Obtained request pad %s for video branch.\n", gst_pad_get_name (tee_video_pad));
    queue_video_pad = gst_element_get_static_pad (data.video_queue, "sink");
    tee_app_pad = gst_element_request_pad_simple (data.tee, "src_%u");
    g_print ("Obtained request pad %s for app branch.\n", gst_pad_get_name (tee_app_pad));
    queue_app_pad = gst_element_get_static_pad (data.app_queue, "sink");
    if(gst_pad_link(tee_video_pad, queue_video_pad) != GST_PAD_LINK_OK){
        g_printerr ("Tee with video branch could not be linked\n");
        gst_object_unref (data.pipeline);
        return;
    }
    if(gst_pad_link(tee_app_pad, queue_app_pad) != GST_PAD_LINK_OK){
        g_printerr ("Tee with app branch could not be linked\n");
        gst_object_unref (data.pipeline);
        return;
    }
    gst_object_unref (queue_video_pad);
    gst_object_unref (queue_app_pad);

    // video를 QWidget에 overlay
    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(data.video_sink), windowId);

    // 파이프라인을 재생 상태로 설정
    gst_element_set_state(data.pipeline, GST_STATE_PLAYING);

    // 버스를 통해 메시지 대기
    bus = gst_element_get_bus(data.pipeline);
    while (!stopped){
        msg = gst_bus_timed_pop_filtered(bus, 100*GST_MSECOND,
                (GstMessageType)(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
        if (msg != NULL) {
            GError *err;
            gchar *debug_info;

            switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                qDebug() << "Error received from element " << GST_OBJECT_NAME(msg->src) << ": " << err->message;
                qDebug() << "Debugging information: " << (debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                stop();
                break;
            case GST_MESSAGE_EOS:
                qDebug()<< "End-Of-Stream reached.";
                stop();
                break;
            case GST_MESSAGE_STATE_CHANGED:
                if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data.pipeline)) {
                    GstState old_state, new_state, pending_state;
                    gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
                    qDebug() << "Pipeline state changed from " << gst_element_state_get_name(old_state) << " to " << gst_element_state_get_name(new_state);
                }
                break;
            default:
                qDebug() << "Unexpected message received.";
                break;
            }
            gst_message_unref(msg);
        }
    }
    // 파이프라인 정지 및 리소스 해제
    gst_element_release_request_pad (data.tee, tee_video_pad);
    gst_element_release_request_pad (data.tee, tee_app_pad);
    gst_object_unref (tee_video_pad);
    gst_object_unref (tee_app_pad);
    gst_element_set_state(data.pipeline, GST_STATE_NULL);
    gst_object_unref(data.pipeline);
    gst_object_unref(bus);
}
