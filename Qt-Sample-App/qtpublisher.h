#ifndef QTPUBLISHER_H
#define QTPUBLISHER_H

#include "opentok.h"
#include "participant.h"

#include <QPainter>
#include <QtMultimedia>
#include <QtDebug>

#include <set>

namespace {
otc_video_frame_format qtframe_to_otcframe_format(QVideoFrameFormat::PixelFormat qvideoformat)
{
    switch (qvideoformat) {
    case QVideoFrameFormat::Format_YUV420P:
        return OTC_VIDEO_FRAME_FORMAT_YUV420P;
    case QVideoFrameFormat::Format_NV12:
        return OTC_VIDEO_FRAME_FORMAT_NV12;
    case QVideoFrameFormat::Format_NV21:
        return OTC_VIDEO_FRAME_FORMAT_NV21;
    case QVideoFrameFormat::Format_YUYV:
        return OTC_VIDEO_FRAME_FORMAT_YUY2;
    case QVideoFrameFormat::Format_UYVY:
        return OTC_VIDEO_FRAME_FORMAT_UYVY;
    case QVideoFrameFormat::Format_ARGB8888:
        return OTC_VIDEO_FRAME_FORMAT_ARGB32;
    case QVideoFrameFormat::Format_BGRA8888:
        return OTC_VIDEO_FRAME_FORMAT_BGRA32;
    case QVideoFrameFormat::Format_ABGR8888:
        return OTC_VIDEO_FRAME_FORMAT_ABGR32;
    case QVideoFrameFormat::Format_Jpeg:
        return OTC_VIDEO_FRAME_FORMAT_MJPEG;
    case QVideoFrameFormat::Format_RGBA8888:
        return OTC_VIDEO_FRAME_FORMAT_RGBA32;
    default:
        return OTC_VIDEO_FRAME_FORMAT_UNKNOWN;
    }
}
} // namespace

namespace opentok_qt {
class QCameraCapturer : public QObject
{
    // Allways add the Q_OBJECT macro
    Q_OBJECT
public:
    QCameraCapturer(QObject *parent) : QObject(parent)
    {
        connect(&sink, &QVideoSink::videoFrameChanged, this, &QCameraCapturer::processVideoFrame);
    }
    static otc_bool init(const otc_video_capturer *capturer, void *user_data)
    {
        const auto self = static_cast<QCameraCapturer *>(user_data);
        self->otc_catpurer = capturer;

        const auto width = 1760;
        const auto height = 1328;
        const auto comparator = [width, height](const QCameraFormat &a, const QCameraFormat &b) {
            return std::abs(a.resolution().width() * a.resolution().height() - width * height)
                   < std::abs(b.resolution().width() * b.resolution().height() - width * height);
        };

        std::set<QCameraFormat, decltype(comparator)> formats(comparator);

        const auto cameraDevice = self->camera.cameraDevice();
        qInfo() << "Supported camera resolutions:";
        for (auto &&f : cameraDevice.videoFormats()) {
            qInfo() << f.resolution();
            formats.insert(f);
        }

        qInfo() << "Setting camera resolution to:" << formats.begin()->resolution();

        self->camera.setCameraFormat(*formats.begin());
        self->session.setCamera(&self->camera);
        self->session.setVideoSink(&self->sink);
        return OTC_TRUE;
    }
    static otc_bool destroy(const otc_video_capturer *capturer, void *user_data)
    {
        const auto self = static_cast<QCameraCapturer *>(user_data);
        return OTC_TRUE;
    }
    static otc_bool start(const otc_video_capturer *capturer, void *user_data)
    {
        const auto self = static_cast<QCameraCapturer *>(user_data);
        self->camera.start();
        return OTC_TRUE;
    }
    static otc_bool stop(const otc_video_capturer *capturer, void *user_data)
    {
        const auto self = static_cast<QCameraCapturer *>(user_data);
        self->camera.stop();
        return OTC_TRUE;
    }
    static otc_bool getCaptureSettings(const otc_video_capturer *capturer,
                                       void *user_data,
                                       struct otc_video_capturer_settings *settings)
    {
        const auto self = static_cast<QCameraCapturer *>(user_data);
        const auto format = self->camera.cameraFormat();
        settings->height = format.resolution().height();
        settings->width = format.resolution().width();

        settings->expected_delay = 0;
        settings->fps = format.maxFrameRate();
        settings->format = 0;
        settings->mirror_on_local_render = OTC_FALSE;
        return OTC_TRUE;
    }

private:
    QCamera camera;
    QMediaCaptureSession session;
    QVideoSink sink;
    const otc_video_capturer *otc_catpurer;

private slots:
    void processVideoFrame()
    {
        const uint8_t *planes[3];
        int strides[3];

        QVideoFrame videoframe = sink.videoFrame();
        if (videoframe.map(QVideoFrame::ReadOnly)) {
            for (std::size_t i = 0; i < videoframe.planeCount(); i++) {
                planes[i] = videoframe.bits(i);
                strides[i] = videoframe.bytesPerLine(i);
            }
            otc_video_frame *otc_frame
                = otc_video_frame_new_from_planes(qtframe_to_otcframe_format(
                                                      videoframe.pixelFormat()),
                                                  videoframe.width(),
                                                  videoframe.height(),
                                                  planes,
                                                  strides);

            const auto ret = otc_video_capturer_provide_frame(otc_catpurer, 0, otc_frame);
            otc_video_frame_delete(otc_frame);
        }
    }
};

class Publisher : public Participant
{
    Q_OBJECT
    static void onPublisherStreamCreated(otc_publisher *publisher,
                                         void *user_data,
                                         const otc_stream *stream)
    {
        const auto self = static_cast<Publisher *>(user_data);
        emit self->connected(self);
    }

    static void onPublisherRenderFrame(otc_publisher *publisher,
                                       void *user_data,
                                       const otc_video_frame *frame)
    {
        const auto self = static_cast<Publisher *>(user_data);
        std::shared_ptr<otc_video_frame>
            converted_frame(otc_video_frame_convert(OTC_VIDEO_FRAME_FORMAT_ARGB32, frame),
                            otc_video_frame_delete);

        otc_video_frame_set_timestamp(converted_frame.get(), otc_video_frame_get_timestamp(frame));
        emit self->frame(converted_frame);
    }

    static void onPublisherStreamDestroyed(otc_publisher *publisher,
                                           void *user_data,
                                           const otc_stream *stream)
    {
        qDebug() << __FUNCTION__ << "callback function";
    }

    static void onPublisherError(otc_publisher *publisher,
                                 void *user_data,
                                 const char *error_string,
                                 enum otc_publisher_error_code error_code)
    {
        qDebug() << "Publisher error. Error code:" << error_string;
    }

    static void on_otc_log_message(const char *message)
    {
        qDebug() << __FUNCTION__ << ":" << message;
    }

public:
    Publisher(otc_session *s, QObject *parent)
        : Participant(parent), publisher(nullptr, nullptr), session(s)
    {
        otc_publisher_callbacks publisher_callbacks = {0};
        publisher_callbacks.user_data = this;
        publisher_callbacks.on_stream_created = onPublisherStreamCreated;
        publisher_callbacks.on_render_frame = onPublisherRenderFrame;
        publisher_callbacks.on_stream_destroyed = onPublisherStreamDestroyed;
        publisher_callbacks.on_error = onPublisherError;

        QCameraCapturer *capturer = new QCameraCapturer(this);

        otc_video_capturer_callbacks video_capturer_callbacks;
        video_capturer_callbacks.destroy = QCameraCapturer::destroy;
        video_capturer_callbacks.init = QCameraCapturer::init;
        video_capturer_callbacks.start = QCameraCapturer::start;
        video_capturer_callbacks.stop = QCameraCapturer::stop;
        video_capturer_callbacks.get_capture_settings = QCameraCapturer::getCaptureSettings;
        video_capturer_callbacks.user_data = capturer;
        video_capturer_callbacks.reserved = nullptr;

        publisher = std::unique_ptr<otc_publisher, otc_status (*)(otc_publisher *)>(
            otc_publisher_new("opentok-qt", &video_capturer_callbacks, &publisher_callbacks),
            otc_publisher_delete);
        otc_publisher_set_publish_audio (publisher.get(), OTC_FALSE);

        otc_session_publish(session, publisher.get());
    }

    QString getId() override { return otc_publisher_get_publisher_id(publisher.get()); }

    ~Publisher() { otc_session_unpublish(session, publisher.get()); }

private:
    std::unique_ptr<otc_publisher, otc_status (*)(otc_publisher *)> publisher;
    otc_session *session = nullptr;
};
}
#endif // QTPUBLISHER_H
