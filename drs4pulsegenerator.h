/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2020 Danny Petschke
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
**  @author: Danny Petschke
**  @contact: danny.petschke@uni-wuerzburg.de
**
*****************************************************************************/

#ifndef DDRS4PULSEGENERATOR_H
#define DDRS4PULSEGENERATOR_H

#include <QMutex>
#include <QMutexLocker>

#include <iostream>
#include <random>
#include <array>

#include "DRS/DRS.h"

#include "DLib.h"

#include "dversion.h"

#include "drs4simulationsettingsmanager.h"
#include "drs4settingsmanager.h"

#include "include/DLTPulseGenerator/DLTPulseGenerator.h"

typedef enum {
    LogNormalPulse = 0,
    ElectronicComponents = 1
} DPulseSimulationType;

class DRS4PulseGenerator : public DLifeTime::DLTCallback
{
    DLifeTime::DLTPulseGenerator *m_generator;

    DLifeTime::DLTPHS m_phsDistribution;
    DLifeTime::DLTSetup m_deviceInfo;
    DLifeTime::DLTPulse m_pulseInfo;

    DLifeTime::DLTSimulationInput m_simulationInput;

    DLifeTime::DLTError m_error;

    DPulseSimulationType m_type;

    mutable QMutex m_mutex;

public:
    static DRS4PulseGenerator *sharedInstance();

    DPulseSimulationType type() const;
    QString getErrorString() const;

    void update();

    bool receiveGeneratedPulsePair(float *pulseATime, float *pulseAVoltage, float *pulseBTime, float *pulseBVoltage);

    virtual void onEvent(DLifeTime::DLTError error) override {
        m_error = error;

        DLifeTime::DLTCallback::onEvent(error);
    }

private:
    DRS4PulseGenerator();
    virtual ~DRS4PulseGenerator();
};

#endif // DDRS4PULSEGENERATOR_H
