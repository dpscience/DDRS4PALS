#include "drs4remotecontrolserverconfigdlg.h"
#include "ui_drs4remotecontrolserverconfigdlg.h"

DRS4RemoteControlServerConfigDlg::DRS4RemoteControlServerConfigDlg(DRS4Worker *worker, QWidget *parent) :
    QMainWindow(parent),
    m_worker(worker),
    ui(new Ui::DRS4RemoteControlServerConfigDlg) {
    ui->setupUi(this);

    setWindowTitle(PROGRAM_NAME);

    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExp ipRegex ("^" + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange
                     + "\\." + ipRange + "$");
    QRegExpValidator *ipValidator = new QRegExpValidator(ipRegex, this);

    connect(ui->checkBox_autoLaunch, SIGNAL(clicked(bool)), this, SLOT(changeAutostart(bool)));
    connect(ui->spinBox_port, SIGNAL(valueChanged(int)), this, SLOT(changePort(int)));
    connect(ui->pushButton_start, SIGNAL(clicked()), this, SLOT(startStopServer()));
}

DRS4RemoteControlServerConfigDlg::~DRS4RemoteControlServerConfigDlg() {
    DDELETE_SAFETY(ui);
}

void DRS4RemoteControlServerConfigDlg::showEvent(QShowEvent *event) {
    ui->checkBox_autoLaunch->setChecked(DRS4ProgramSettingsManager::sharedInstance()->remoteControlServerAutostart());
    ui->spinBox_port->setValue(DRS4ProgramSettingsManager::sharedInstance()->remoteControlServerPort());

    updateServer();

    event->accept();

    QMainWindow::showEvent(event);
}

void DRS4RemoteControlServerConfigDlg::updateServer() {
    if (DRS4RemoteControlServer::sharedInstance()->isListening()) {
        ui->pushButton_start->setText("stop listening ...");

        ui->spinBox_port->setEnabled(false);
    }
    else {
        ui->pushButton_start->setText("start listening ...");

        ui->spinBox_port->setEnabled(true);
    }
}

void DRS4RemoteControlServerConfigDlg::changeAutostart(bool on) {
    DRS4ProgramSettingsManager::sharedInstance()->setRemoteControlServerAutostart(on);
}

void DRS4RemoteControlServerConfigDlg::changePort(int port) {
    DRS4ProgramSettingsManager::sharedInstance()->setRemoteControlServerPort(port);

    updateServer();
}

void DRS4RemoteControlServerConfigDlg::changeIP(const QString &ip) {
    DRS4ProgramSettingsManager::sharedInstance()->setRemoteControlServerIP(ip);

    updateServer();
}

void DRS4RemoteControlServerConfigDlg::startStopServer() {
    if (!m_worker)
        return;

    if (DRS4RemoteControlServer::sharedInstance()->isListening()) {
        DRS4RemoteControlServer::sharedInstance()->stop();
    }
    else {
        DRS4RemoteControlServer::sharedInstance()->start(m_worker);
    }

    updateServer();
}
