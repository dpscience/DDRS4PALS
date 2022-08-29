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

#ifndef VERTICALRANGEDOUBLESLIDER_H
#define VERTICALRANGEDOUBLESLIDER_H

#include "../DTypes/types.h"

#define DVerticalRangeDoubleSlider_WIDGET_MIN   0
#define DVerticalRangeDoubleSlider_WIDGET_MAX   100


namespace Ui {
class DVerticalRangeDoubleSlider;
}

class DVerticalRangeDoubleSlider : public QWidget
{
    Q_OBJECT
public:
    typedef enum
    {
        INT,
        DOUBLE
    } VALUE_TYPE;

public:
    explicit DVerticalRangeDoubleSlider(QWidget *parent = 0);
    virtual ~DVerticalRangeDoubleSlider();

    double upperLimit() const;
    double lowerLimit() const;

    double upperValue() const;
    double lowerValue() const;

    VALUE_TYPE valueType() const;

public slots:
    void setLimits(double min, double max);

    void setLowerLevel(double value);
    void setUpperLevel(double value);

    void setValueType(VALUE_TYPE type);

private slots:
    void updateLowerLevel(double value);
    void updateUpperLevel(double value);

    void updateLowerLevel();
    void updateUpperLevel();

private slots:
    void rangeHasChanged(double lowerLimit, double upperLimit);

signals:
    void rangeChanged(double, double);

private:
    Ui::DVerticalRangeDoubleSlider *ui;

    double m_upperLimit, m_lowerLimit;
    VALUE_TYPE m_type;
    QValidator *m_numberValidator;
};

#endif // VERTICALRANGEDOUBLESLIDER_H
