/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2022 Dr. Danny Petschke
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see http://www.gnu.org/licenses/.
**
*****************************************************************************
**
**  @author: Dr. Danny Petschke
**  @contact: danny.petschke@uni-wuerzburg.de
**
*****************************************************************************
**
** related publications:
**
** when using DDRS4PALS for your research purposes please cite:
**
** DDRS4PALS: A software for the acquisition and simulation of lifetime spectra using the DRS4 evaluation board:
** https://www.sciencedirect.com/science/article/pii/S2352711019300676
**
** and
**
** Data on pure tin by Positron Annihilation Lifetime Spectroscopy (PALS) acquired with a semi-analog/digital setup using DDRS4PALS
** https://www.sciencedirect.com/science/article/pii/S2352340918315142?via%3Dihub
**
** when using the integrated simulation tool /DLTPulseGenerator/ of DDRS4PALS for your research purposes please cite:
**
** DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses
** https://www.sciencedirect.com/science/article/pii/S2352711018300530
**
** Update (v1.1) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses
** https://www.sciencedirect.com/science/article/pii/S2352711018300694
**
** Update (v1.2) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses
** https://www.sciencedirect.com/science/article/pii/S2352711018301092
**
** Update (v1.3) to DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses
** https://www.sciencedirect.com/science/article/pii/S235271101930038X
**/

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
    explicit DRS4CFDAlgorithmDlg(QWidget *parent = DNULLPTR);
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
