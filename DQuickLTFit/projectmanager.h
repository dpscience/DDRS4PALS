/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2019 Danny Petschke
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

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "dversion.h"
#include "settings.h"

class PALSProjectManager
{
    PALSProject*  m_project;
    QString m_fileName;
    int m_minChannel;
    int m_maxChannel;

    PALSProjectManager();
    virtual ~PALSProjectManager();

public:
    static PALSProjectManager *sharedInstance();

    bool load(const QString& fileName);
    bool save(const QString& fileName);

    void setFileName(const QString fileName);
    QString getFileName() const;

    void setASCIIDataName(const QString& file);
    QString getASCIIDataName() const;

    void setChannelRanges(int min, int max);

    int getMinChannel() const;
    int getMaxChannel() const;

    PALSDataStructure *getDataStructure() const;
    PALSResultHistorie *getResultHistorie() const;

    ///Creates a default Project:
    void createEmptyProject();
};

#endif // PROJECTMANAGER_H
