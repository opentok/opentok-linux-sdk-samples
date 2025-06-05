#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "qtsession.h"

constexpr const char *kApiKey = "";
constexpr const char *kSession = "";
constexpr const char *kToken = "";
constexpr const char *kConnecting = "Connecting";
constexpr const char *kConnect = "Connect";
constexpr const char *kDisconnect = "Disconnect";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->setText(kApiKey);
    ui->lineEdit_2->setText(kSession);
    ui->lineEdit_3->setText(kToken);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::connectSession);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::newSessionWindow);
}

MainWindow::~MainWindow()
{
    const auto session = findChild<opentok_qt::Session *>();
    if (session != nullptr)
        delete session;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    const auto session = findChild<opentok_qt::Session *>();
    if (session != nullptr)
        delete session;
    QMainWindow::closeEvent(event);
}

void MainWindow::connectSession()
{
    if (findChild<opentok_qt::Session *>() == nullptr) {
        new opentok_qt::Session(this, ui->lineEdit->text(), ui->lineEdit_2->text(), ui->lineEdit_3->text());
        ui->pushButton->setDisabled(true);
        ui->pushButton->setText(kConnecting);
    } else {
        destroySession();
    }
}

void MainWindow::destroySession()
{
    const auto session = findChild<opentok_qt::Session *>();
    session->setParent(nullptr);
    session->deleteLater();
}

void MainWindow::sessionConnected()
{
    ui->pushButton->setText(kDisconnect);
    ui->pushButton->setEnabled(true);
}

void MainWindow::sessionError()
{
    if (ui->pushButton->text() == kConnecting) {
        ui->pushButton->setText(kConnect);
        ui->pushButton->setEnabled(true);
        destroySession();
    }
}

void MainWindow::sessionDisconnected()
{
    destroySession();
    ui->pushButton->setText(kConnect);
    ui->pushButton->setEnabled(true);
}

void MainWindow::newSessionWindow()
{
    auto newWindow = new MainWindow(nullptr);
    newWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    newWindow->show();
}

void MainWindow::addMessage(QString message)
{
    ui->textEdit->append(message);
}
