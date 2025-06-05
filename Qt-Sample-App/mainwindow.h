#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <QMainWindow>
#include <QString>

#include "./ui_mainwindow.h"
#include "glvideowidget.h"
#include "participant.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Ui::MainWindow *ui;

public slots:
    void addVideoWidget(opentok_qt::Participant *participant)
    {
        auto openGLWidget = new GLVideoWidget(ui->widget);
        openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));

        // Connect this widget to the new participant frames
        connect(participant, &opentok_qt::Participant::frame, openGLWidget, &GLVideoWidget::frame);
        participants[participant->getId()] = openGLWidget;

        resetGrid();
    }

    void removeVideoWidget(QString id)
    {
        const auto it = participants.find(id);
        if (it != participants.end()) {
            it->second->deleteLater();
            participants.erase(it);
        }
        resetGrid();
    }

    void connectSession();

    void destroySession();

    void sessionConnected();

    void sessionDisconnected();

    void sessionError();

    void newSessionWindow();

    void addMessage(QString message);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    std::unordered_map<QString, GLVideoWidget *> participants;

    void resetGrid()
    {
        delete ui->gridLayout_3;
        ui->gridLayout_3 = new QGridLayout(ui->widget);
        ui->gridLayout_3->setObjectName(QString::fromUtf8("gridLayout"));
        ui->gridLayout_3->setContentsMargins(0, 0, 0, 0);

        const auto size = static_cast<int>(std::ceil(std::sqrt(participants.size())));

        std::size_t index = 0;
        for (const auto &p : participants) {
            ui->gridLayout_3->addWidget(p.second, index / size, index % size, 1, 1);
            ++index;
        }
    }
};
#endif // MAINWINDOW_H
