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

#ifndef MATHCONSOLETEXTBOX_H
#define MATHCONSOLETEXTBOX_H

#include "../DTypes/types.h"
#include "../DMath/constantexplanations.h"


typedef double calculation_type; //change here the format type of calculation e.g. double, float, int, etc.

class DMathConsoleTextBox : public QTextEdit
{
    Q_OBJECT
public:
    DMathConsoleTextBox(QWidget *parent = DNULLPTR);
    virtual ~DMathConsoleTextBox();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);

public slots:
    void insertCompletion(const QString &completion);
    void showToolTip(const QString& data);

    void setTextFont(const QFont& font);

    void clearOutput();

private:
    QString textUnderCursor() const;

    void skip_spaces(char const*& input);
    double parse_double(char const*& input);
    std::string parse_identifier(char const*& input);
    calculation_type parse_function_call(char const*& input);
    calculation_type parse_group(char const*& input);
    calculation_type parse_primary_expression(char const*& input);
    calculation_type parse_infix_function_call(char const*& input);
    calculation_type parse_root(char const*& input);
    calculation_type parse_power(char const*& input);
    calculation_type parse_product(char const*& input);
    calculation_type parse_sum(char const*& input);
    QString eval_2(char const* input);
    void init_constants();
    void throw_error(const QString& message);

private:
    bool m_error;
    int m_fw;
    QColor m_tc;
    QFont m_nf;
    double m_ans;
    int m_paramCount;
    int m_precision;
    QList<QString> m_outlet_functions;
    QList<QString> m_systConstList;
    QList<QString> m_systFuncList;
    QList<QString> m_nonCalcFuncList;
    QCompleter *m_completer;
    bool m_completerWasShown;
    bool m_cleared;
};

#endif // MATHCONSOLETEXTBOX_H
