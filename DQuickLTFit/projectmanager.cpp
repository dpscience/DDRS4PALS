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

#include "projectmanager.h"

static PALSProjectManager *pManagerInstance = DNULLPTR;

PALSProjectManager::PALSProjectManager()
{
    m_project = new PALSProject;
}

PALSProjectManager::~PALSProjectManager()
{
    DDELETE_SAFETY(m_project);
    DDELETE_SAFETY(pManagerInstance);
}

PALSProjectManager *PALSProjectManager::sharedInstance()
{
    if ( !pManagerInstance )
        pManagerInstance = new PALSProjectManager();

    return pManagerInstance;
}

bool PALSProjectManager::load(const QString &fileName)
{
    if ( m_project->load(fileName) )
    {
        setFileName(fileName);
        return true;
    }

    return false;
}

bool PALSProjectManager::save(const QString& fileName)
{
    if ( fileName.isEmpty() )
        return false;

    return m_project->save(fileName);
}

QString PALSProjectManager::getFileName() const
{
    return m_fileName;
}

void PALSProjectManager::setASCIIDataName(const QString &file)
{
    if (!m_project)
        return;

    m_project->setASCIIDataName(file);
}

QString PALSProjectManager::getASCIIDataName() const
{
    if (!m_project)
        return QString("unknown");

    return m_project->getASCIIDataName();
}

void PALSProjectManager::setChannelRanges(int min, int max)
{
    m_minChannel = min;
    m_maxChannel = max;
}

int PALSProjectManager::getMinChannel() const
{
    return m_minChannel;
}

int PALSProjectManager::getMaxChannel() const
{
    return m_maxChannel;
}

PALSDataStructure *PALSProjectManager::getDataStructure() const
{
    return m_project->getDataStructureAt(0);
}

PALSResultHistorie *PALSProjectManager::getResultHistorie() const
{
    return m_project->getDataStructureAt(0)->getFitSetPtr()->getResultHistoriePtr();
}

void PALSProjectManager::createEmptyProject()
{
    if ( !m_project )
        return;

    DDELETE_SAFETY(m_project);

    m_project = new PALSProject;

    PALSDataStructure *structure = new PALSDataStructure(m_project);


    structure->getFitSetPtr()->setStartChannel(0);
    structure->getFitSetPtr()->setStopChannel(1024);
    structure->getFitSetPtr()->setChannelResolution(44);
    structure->getFitSetPtr()->setMaximumIterations(200);

    const DString tauText(QString("<b>&#964;<sub>") % QVariant(1).toString() % QString("</sub></b> [ps]"));
    const DString iText(QString("<b>I<sub>") % QVariant(1).toString() % QString("</sub></b>"));
    const DString sigmaText(QString("<b>&#963;<sub>") % QVariant(1).toString() % QString("</sub></b> [ps]"));
    const DString muText(QString("<b>t0<sub>") % QVariant(1).toString() % QString("</sub></b> [ps]"));
    const DString IText(QString("<b>I<sub>") % QVariant(1).toString() % QString("</sub></b> [ps]"));

    ///2 lifetime-Params:
    PALSFitParameter *lt1 = new PALSFitParameter(structure->getFitSetPtr()->getLifeTimeParamPtr()); //lt_1
    PALSFitParameter *i1  = new PALSFitParameter(structure->getFitSetPtr()->getLifeTimeParamPtr()); //intensity_1

    lt1->setStartValue(120.0f);
    i1->setStartValue(0.1f);

    lt1->setName(DString(""));
    i1->setName(DString(""));

    lt1->setAlias(tauText);
    i1->setAlias(iText);

    ///3 device-resolution-Params:
    PALSFitParameter *sigma = new PALSFitParameter(structure->getFitSetPtr()->getDeviceResolutionParamPtr());
    PALSFitParameter *mu = new PALSFitParameter(structure->getFitSetPtr()->getDeviceResolutionParamPtr());
    PALSFitParameter *I = new PALSFitParameter(structure->getFitSetPtr()->getDeviceResolutionParamPtr());

    sigma->setStartValue(220.0f);
    mu->setStartValue(1.0f);
    I->setStartValue(1.0f);

    sigma->setName(DString(""));
    mu->setName(DString(""));
    I->setName(DString(""));

    sigma->setAlias(sigmaText);
    mu->setAlias(muText);
    I->setAlias(IText);

    ///2 source-Params:
    PALSFitParameter *source_lt = new PALSFitParameter(structure->getFitSetPtr()->getSourceParamPtr()); //lt_1
    PALSFitParameter *source_i = new PALSFitParameter(structure->getFitSetPtr()->getSourceParamPtr()); //intensity_1

    source_lt->setStartValue(120.0f);
    source_i->setStartValue(0.1f);

    source_lt->setName(DString(""));
    source_i->setName(DString(""));

    source_lt->setAlias(tauText);
    source_i->setAlias(iText);


    setFileName(""); //perhaps, its also done from outside again!
}

void PALSProjectManager::setFileName(const QString fileName)
{
    m_fileName = fileName;
    m_project->setName(fileName);
}
