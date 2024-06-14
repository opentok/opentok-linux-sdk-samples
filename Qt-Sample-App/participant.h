#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include "opentok.h"
#include <memory>
#include <QObject>
#include <QString>

namespace opentok_qt {

class Participant : public QObject
{
    Q_OBJECT
public:
    Participant(QObject *parent) : QObject(parent) {}
    virtual QString getId() = 0;
signals:
    void frame(std::shared_ptr<const otc_video_frame>);
    void connected(opentok_qt::Participant *);
};
}
#endif // PARTICIPANT_H
