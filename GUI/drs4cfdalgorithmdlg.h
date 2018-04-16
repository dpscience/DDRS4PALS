#ifndef DRS4CFDALGORITHMDLG_H
#define DRS4CFDALGORITHMDLG_H

#include <QWidget>
#include <QMutex>
#include <QMutexLocker>

#include "DLib.h"

#include "drs4worker.h"
#include "drs4settingsmanager.h"

namespace Ui {
class DRS4CFDAlgorithmDlg;
}

class DRS4CFDAlgorithmDlg : public QWidget
{
    Q_OBJECT

    DRS4Worker *m_worker;

    QMutex m_mutex;

public:
    explicit DRS4CFDAlgorithmDlg(QWidget *parent = nullptr);
    virtual ~DRS4CFDAlgorithmDlg();

    void init(DRS4Worker *worker) {
        m_worker = worker;
    }

public slots:
    void load();

    void changeCFDAlgorithm(int algorithm);
    void changeInterpolationType(int type);
    void changeRenderPointsForPolynomialInterpolation(int count);
    void changeRenderPointsForSplineInterpolation(int count);

private:
    Ui::DRS4CFDAlgorithmDlg *ui;
};

#endif // DRS4CFDALGORITHMDLG_H
