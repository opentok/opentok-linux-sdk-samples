#ifndef QTSUBSCRIBER_H
#define QTSUBSCRIBER_H

#include "opentok.h"
#include "participant.h"

#include <QtDebug>

namespace opentok_qt {
class Subscriber : public Participant
{
    Q_OBJECT
    static void onConnected(otc_subscriber *subscriber, void *user_data, const otc_stream *stream)
    {
        const auto self = static_cast<Subscriber *>(user_data);
        emit self->connected(self);
    }

    static void onDisconnected(otc_subscriber *subscriber, void *user_data) {}

    static void onRenderFrame(otc_subscriber *subscriber,
                              void *user_data,
                              const otc_video_frame *frame)
    {
        const auto self = static_cast<Subscriber *>(user_data);
        std::shared_ptr<const otc_video_frame>
            converted_frame(otc_video_frame_convert(OTC_VIDEO_FRAME_FORMAT_ARGB32, frame),
                            otc_video_frame_delete);
        emit self->frame(converted_frame);
    }

    static void onError(otc_subscriber *subscriber,
                        void *user_data,
                        const char *error_string,
                        enum otc_subscriber_error_code error)
    {}

public:
    Subscriber(otc_session *s, const otc_stream *stream, QObject *parent)
        : Participant(parent), subscriber_(nullptr, nullptr), session(s)
    {
        otc_subscriber_callbacks subscriber_callbacks = {0};
        subscriber_callbacks.user_data = this;
        subscriber_callbacks.on_connected = Subscriber::onConnected;
        subscriber_callbacks.on_render_frame = Subscriber::onRenderFrame;
        subscriber_callbacks.on_error = Subscriber::onError;

        subscriber_ = std::unique_ptr<otc_subscriber, otc_status (*)(otc_subscriber *)>(
            otc_subscriber_new(stream, &subscriber_callbacks), otc_subscriber_delete);

        if (subscriber_ == nullptr) {
            qDebug() << "Could not create OpenTok subscriber successfully";
            return;
        }
        otc_session_subscribe(session, subscriber_.get());

        // We will not reuse any subscriber, let's copy the initial ID and asume or subscriber
        // wrapper has a constant ID (instead of changing after a stream dropped event)
        id = otc_subscriber_get_subscriber_id(subscriber_.get());
    }

    QString getId() override { return id; }

    ~Subscriber() { otc_session_unsubscribe(session, subscriber_.get()); }

private:
    std::unique_ptr<otc_subscriber, otc_status (*)(otc_subscriber *)> subscriber_;
    QString id;
    otc_session *session;
};
}
#endif // QTSUBSCRIBER_H
