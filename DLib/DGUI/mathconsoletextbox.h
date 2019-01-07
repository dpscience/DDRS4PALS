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

#ifndef MATHCONSOLETEXTBOX_H
#define MATHCONSOLETEXTBOX_H

#include "../DTypes/types.h"
#include "../DMath/constantexplanations.h"


typedef double calculation_type; //change here the format type of calculation e.g. double, float, int, etc.

class DMathConsoleTextBox : public QTextEdit
{
    Q_OBJECT
public:
    DMathConsoleTextBox(QWidget *parent = nullptr);
    virtual ~DMathConsoleTextBox();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void keyReleaseEvent(QKeyEvent *e);

public slots:
    void insertCompletion(const QString &completion);
    void showToolTip(const QString& data);

    void setTextFont(const QFont& font);

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
    void clearFromConstantList(const QString& value);
    void call_outletFunction(const QString& Outlet_Function);
    bool in_ConstantList(const QString &value);
    bool in_UserList(const QString& value);
    void throw_error(const QString& message);

private:
    bool error;
    int fw;
    QColor tc;
    QFont nf;
    QList<QString> outlet_functions;
    double ANS;
    int ParamCount;
    int precision;
    QList<QString> systConstList;
    QList<QString> systFuncList;
    QList<QString> nonCalcFuncList;
    QCompleter * m_completer;
    bool c_wasShown;
    bool cleared;
};

#endif // MATHCONSOLETEXTBOX_H
