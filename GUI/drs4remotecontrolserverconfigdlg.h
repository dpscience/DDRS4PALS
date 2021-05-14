#ifndef DRS4REMOTECONTROLSERVERCONFIGDLG_H
#define DRS4REMOTECONTROLSERVERCONFIGDLG_H

#include "dversion.h"
#include "RemoteControlServer/DRS4RemoteControlServer.h"
#include "drs4programsettingsmanager.h"
#include "drs4worker.h"

#include "DLib.h"

#include <QMainWindow>

namespace Ui {
class DRS4RemoteControlServerConfigDlg;
}

class DRS4RemoteControlServerConfigDlg : public QMainWindow {
    Q_OBJECT

public:
    explicit DRS4RemoteControlServerConfigDlg(DRS4Worker *worker, QWidget *parent = DNULLPTR);
    virtual ~DRS4RemoteControlServerConfigDlg();

protected:
    virtual void showEvent(QShowEvent *event);

private slots:
    void updateServer();
    void changeAutostart(bool on);
    void changePort(int port);
    void startStopServer();

private:
    Ui::DRS4RemoteControlServerConfigDlg *ui;

    DRS4Worker *m_worker;
};

#endif // DRS4REMOTECONTROLSERVERCONFIGDLG_H
