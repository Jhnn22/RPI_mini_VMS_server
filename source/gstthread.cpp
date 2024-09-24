#include "gstthread.h"

#include <glib.h>
#include <gst/gst.h>
#include <gst/video/videooverlay.h>

using namespace std;

GstThread::GstThread(WId windowId, QString rtspURI) : m_windowId(windowId), m_rtspURI(rtspURI) {
}


static void on_pad_added(GstElement *element, GstPad *pad, gpointer data) {
    GstPad *sinkpad;
    GstElement *depay = (GstElement *)data;

    sinkpad = gst_element_get_static_pad(depay, "sink");

    if (!gst_pad_is_linked(sinkpad)) {
        if (gst_pad_link(pad, sinkpad) != GST_PAD_LINK_OK) {
            g_printerr("Failed to link pads!\n");
        }
    }

    gst_object_unref(sinkpad);
}

static void pad_added_handler(GstElement *src, GstPad *new_pad, GstElement *depay) {
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

void GstThread::run() {
    gst_debug_set_default_threshold(GST_LEVEL_WARNING);
    GstElement *pipeline, *source, *depay, *parse, *decode, *convert, *sink;
    GstBus *bus;
    GstMessage *msg;

    // // GStreamer 파이프라인을 RTSP 스트림에 맞게 설정
    // string pipelineStr = "rtspsrc location=rtsp://192.168.0.16:8554/test ! rtph264depay ! h264parse ! openh264dec ! videoconvert ! glimagesink";
    // pipeline = gst_parse_launch(pipelineStr.c_str(), NULL);


    // GstElement* videoSink = gst_bin_get_by_interface(GST_BIN(pipeline), GST_TYPE_VIDEO_OVERLAY);
    // // QWidget에 비디오를 렌더링하기 위한 핸들 설정
    // gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(videoSink), m_windowId);

    // 파이프라인 생성
    pipeline = gst_pipeline_new("rtsp-player");

    // 요소 생성
    source = gst_element_factory_make("rtspsrc", "source");
    depay = gst_element_factory_make("rtph264depay", "depay");
    parse = gst_element_factory_make("h264parse", "parse");
    decode = gst_element_factory_make("openh264dec", "decode");
    convert = gst_element_factory_make("videoconvert", "convert");
    sink = gst_element_factory_make("glimagesink", "sink");

    // 모든 요소가 생성되었는지 확인
    if (!pipeline) {
        g_printerr("Failed to create pipeline element.\n");
        return;
    } else if (!source) {
        g_printerr("Failed to create source element.\n");
        return;
    } else if (!depay) {
        g_printerr("Failed to create depay element.\n");
        return;
    } else if (!parse) {
        g_printerr("Failed to create parse element.\n");
        return;
    } else if (!decode) {
        g_printerr("Failed to create decode element.\n");
        return;
    } else if (!convert) {
        g_printerr("Failed to create convert element.\n");
        return;
    } else if (!sink) {
        g_printerr("Failed to create sink element.\n");
        return;
    }

    g_print("All elements created successfully.\n");


    // 파이프라인에 요소 추가
    gst_bin_add_many(GST_BIN(pipeline), source, depay, parse, decode, convert, sink, nullptr);

    // pad-added signal 처리
    g_signal_connect(source, "pad-added", G_CALLBACK(pad_added_handler), depay);

    // RTSP 소스 속성 설정
    string rtsp_location = "rtsp://"+ m_rtspURI.toStdString() +":8554/test";
    qDebug() << m_rtspURI;
    g_object_set(G_OBJECT(source), "location", rtsp_location.c_str(), nullptr);

    // // rtspsrc의 특정 패드를 사용하도록 설정
    // g_object_set(G_OBJECT(source), "latency", 0, nullptr);
    // g_object_set(G_OBJECT(source), "protocols", 4, nullptr); // TCP를 사용하도록 설정

    // 요소 연결
    if (!gst_element_link_many(depay, parse, decode, convert, sink, nullptr)) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return;
    }

    // // rtspsrc와 rtph264depay를 연결
    // g_object_set(G_OBJECT(source), "drop-on-latency", TRUE, nullptr);
    // g_object_set(G_OBJECT(source), "do-rtsp-keep-alive", TRUE, nullptr);
    // g_object_set(G_OBJECT(source), "do-rtcp", TRUE, nullptr);

    // source의 'pad-added' 시그널에 콜백 연결 (동적 패드 처리)
    // g_signal_connect(source, "pad-added", G_CALLBACK(on_pad_added), depay);

    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(sink), m_windowId);

    // --------------------------------------------------------------------
    // 파이프라인을 재생 상태로 설정
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // 버스를 통해 메시지 대기
    bus = gst_element_get_bus(pipeline);
    do {
        msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
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
                break;
            case GST_MESSAGE_EOS:
                qDebug()<< "End-Of-Stream reached.";
                break;
            case GST_MESSAGE_STATE_CHANGED:
                // 파이프라인의 상태 변경만 관심 있음
                if (GST_MESSAGE_SRC(msg) == GST_OBJECT(pipeline)) {
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
    } while(msg != NULL);
    // 파이프라인 정지 및 리소스 해제
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    gst_object_unref(bus);
}
