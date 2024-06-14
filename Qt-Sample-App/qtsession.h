#ifndef QTSESSION_H
#define QTSESSION_H

#include "mainwindow.h"
#include "opentok.h"
#include "participant.h"
#include "qtpublisher.h"
#include "qtsubscriber.h"

#include <QtDebug>

#include <map>

namespace opentok_qt {
class Session : public QObject
{
    // Allways add the Q_OBJECT macro
    Q_OBJECT
public:
    Session(MainWindow *parent,
            const QString &apikey,
            const QString &sessionid,
            const QString &token)
        : QObject(parent), session_(nullptr, nullptr)
    {
        // Notify main window about new participants
        connect(this, &Session::newPartipant, parent, &MainWindow::addVideoWidget);

        // Notify main window about destroyed participants
        connect(this, &Session::removeParticipant, parent, &MainWindow::removeVideoWidget);

        // Notify main window when session is connected
        connect(this, &Session::sessionConnected, parent, &MainWindow::sessionConnected);

        // Notify main window when session is disconnected
        connect(this, &Session::sessionDisconnected, parent, &MainWindow::sessionDisconnected);

        // Notify main window when there is an error
        connect(this, &Session::sessionError, parent, &MainWindow::sessionError);

        // Logging messages
        connect(this, &Session::message, parent, &MainWindow::addMessage);

        init_session(apikey, sessionid, token);
    }
    static void onSessionConnected(otc_session *session, void *user_data)
    {
        const auto self = static_cast<Session *>(user_data);
        emit self->message(QString("Session connected ") + otc_session_get_id(session));
        emit self->sessionConnected();

        QMetaObject::invokeMethod(self, [self]() mutable { self->createPublisher(); });
    }

    static void onSessionConnectionCreated(otc_session *session,
                                           void *user_data,
                                           const otc_connection *connection)
    {
        const auto self = static_cast<Session *>(user_data);
        emit self->message(QString("Connection created ") + otc_connection_get_id(connection));
    }

    static void onSessionConnectionDropped(otc_session *session,
                                           void *user_data,
                                           const otc_connection *connection)
    {
        const auto self = static_cast<Session *>(user_data);
        emit self->message(QString("Connection dropped ") + otc_connection_get_id(connection));
    }

    static void onSessionStreamReceived(otc_session *session,
                                        void *user_data,
                                        const otc_stream *stream)
    {
        const auto self = static_cast<Session *>(user_data);
        emit self->message(QString("Stream received ") + otc_stream_get_id(stream));

        std::shared_ptr<const otc_stream> stream_copy(otc_stream_copy(stream), otc_stream_delete);

        QMetaObject::invokeMethod(self, [self, stream_copy]() mutable {
            self->streamReceived(stream_copy);
        });
    }

    static void onSessionStreamDropped(otc_session *session,
                                       void *user_data,
                                       const otc_stream *stream)
    {
        const auto self = static_cast<Session *>(user_data);
        emit self->message(QString("Stream dropped ") + otc_stream_get_id(stream));

        std::shared_ptr<const otc_stream> stream_copy(otc_stream_copy(stream), otc_stream_delete);

        QMetaObject::invokeMethod(self, [self, stream_copy]() mutable {
            self->streamDropped(stream_copy);
        });
    }

    static void onSessionDisconnected(otc_session *session, void *user_data)
    {
        const auto self = static_cast<Session *>(user_data);
        emit self->message(QString("Session disconnected ") + otc_session_get_id(session));
        emit self->sessionDisconnected();
    }

    static void onSessionError(otc_session *session,
                               void *user_data,
                               const char *error_string,
                               enum otc_session_error_code error)
    {
        const auto self = static_cast<Session *>(user_data);
        emit self->message(QString("Session error ") + error_string);
        emit self->sessionError();
    }

    void init_session(const QString &apikey, const QString &sessionid, const QString &token)
    {
        otc_session_callbacks session_callbacks = {0};
        session_callbacks.on_connected = onSessionConnected;
        session_callbacks.on_connection_created = onSessionConnectionCreated;
        session_callbacks.on_connection_dropped = onSessionConnectionDropped;
        session_callbacks.on_stream_received = onSessionStreamReceived;
        session_callbacks.on_stream_dropped = onSessionStreamDropped;
        session_callbacks.on_disconnected = onSessionDisconnected;
        session_callbacks.on_error = onSessionError;
        session_callbacks.user_data = this;

        session_ = std::unique_ptr<otc_session, otc_status (*)(otc_session *)>(
            otc_session_new(apikey.toStdString().c_str(),
                            sessionid.toStdString().c_str(),
                            &session_callbacks),
            otc_session_delete);

        if (session_ == nullptr) {
            qCritical() << "Could not create OpenTok session successfully";
        }

        otc_session_connect(session_.get(), token.toStdString().c_str());
    }

private:
    std::unique_ptr<otc_session, otc_status (*)(otc_session *)> session_;
    std::multimap<std::string, std::unique_ptr<Participant>> participants;

signals:
    void newPartipant(opentok_qt::Participant *);
    void removeParticipant(QString id);
    void sessionConnected();
    void sessionDisconnected();
    void message(QString message);
    void sessionError();

private slots:
    void participantConnected(opentok_qt::Participant *part)
    {
        emit newPartipant(static_cast<Participant *>(part));
    }

    void streamReceived(std::shared_ptr<const otc_stream> stream)
    {
        auto newsubscriber = std::make_unique<Subscriber>(session_.get(), stream.get(), nullptr);
        // Wait until participant is connected
        connect(newsubscriber.get(), &Participant::connected, this, &Session::participantConnected);
        participants.insert({otc_stream_get_id(stream.get()),
                             std::unique_ptr<Participant>(std::move(newsubscriber))});
    }

    void createPublisher()
    {
        auto newpublisher = std::make_unique<Publisher>(session_.get(), nullptr);
        // Wait until participant is connected
        connect(newpublisher.get(), &Participant::connected, this, &Session::participantConnected);
        participants.insert({"PRIVATEID", std::unique_ptr<Participant>(std::move(newpublisher))});
    }

    void streamDropped(std::shared_ptr<const otc_stream> stream)
    {
        for (auto its = participants.equal_range(otc_stream_get_id(stream.get()));
             its.first != its.second;) {
            const auto id = its.first->second->getId();
            its.first->second.reset();
            emit removeParticipant(id);
            its.first = participants.erase(its.first);
        }
    }
};
}
#endif // QTSESSION_H
