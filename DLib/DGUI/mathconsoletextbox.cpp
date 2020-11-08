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

#include "mathconsoletextbox.h"

// unary-operator       = '+' | '-' | '#'
// identifier           = {A-Za-z_} {A-Za-z0-9_}*
// function-call        = identifier [primary-expression] [primary-expression]
// group                = '(' sum ')' | '[' sum ']' | '{' sum '}' | '|' sum '|'
// primary-expression   = unary-operator primary-expression | double | function-call | group
// infix-function-call  = primary-expression [':' identifier ':' primary-expression]*
// root                 = infix-function-call ['#' root]
// power                = root ['^' power]
// product              = power [('*' | '/' | '%') power]*
// sum                  = product [('+' | '-') product]*

using namespace std;

std::map<std::string, calculation_type> constants;
std::map<std::string, calculation_type (*) ()> nullary_functions;
std::map<std::string, calculation_type (*) (calculation_type)> unary_functions;
std::map<std::string, calculation_type (*) (calculation_type, calculation_type)> binary_functions;

void DMathConsoleTextBox::skip_spaces(char const*& input) {
    while (std::isspace(*input))
        ++input;
}

double DMathConsoleTextBox::parse_double(char const*& input) {
    return std::strtod(input, const_cast<char**>(&input));
}

std::string DMathConsoleTextBox::parse_identifier(char const*& input) {
    skip_spaces(input);
    std::string identifier(1, *input++);

    while(std::isalnum(*input) || *input == '_')
        identifier += *input++;

    return identifier;
}

calculation_type DMathConsoleTextBox::parse_function_call(char const*& input) {
    std::string const name = parse_identifier(input);

    {
        std::map<std::string,double>::iterator iter = constants.find(name);

        if (iter != constants.end())
            return iter->second;
    }

    {
        std::map<std::string, calculation_type (*) ()>::iterator iter = nullary_functions.find(name);

        if (iter != nullary_functions.end())
            return iter->second();
    }

    {
        std::map<std::string, calculation_type (*) (calculation_type)>::iterator iter = unary_functions.find(name);

        if (iter != unary_functions.end()) {
            calculation_type const argument = parse_primary_expression(input);

            return iter->second(argument);
        }
    }

    {
        std::map<std::string, calculation_type (*) (calculation_type, calculation_type)>::iterator iter = binary_functions.find(name);

        if (iter != binary_functions.end()) {
            calculation_type const arg1 = parse_primary_expression(input);
            calculation_type const arg2 = parse_primary_expression(input);

            return iter->second(arg1, arg2);
        }
    }

    m_error = true;

    throw_error("[ERR] >> call to unknown function '" + QString::fromStdString(name) + "' ");

    Q_UNREACHABLE();
}

calculation_type DMathConsoleTextBox::parse_group(char const*& input) {
    char const delim = *input == '(' ? ')' : *input == '[' ? ']' : *input == '{' ? '}' : '|';

    calculation_type const value = parse_sum(++input);

    if (*input++ != delim) {
        m_error = true;
        throw_error("[ERR] >> terminating '" + QString::fromStdString(std::string(1, delim)) + "' missing");
    }

    return value;
}

calculation_type DMathConsoleTextBox::parse_primary_expression(char const*& input) {
    skip_spaces(input);

    if (*input == '+')
        return parse_primary_expression(++input);

    if (*input == '-')
        return -parse_primary_expression(++input);

    if (*input == '#')
        return std::sqrt(parse_primary_expression(++input));

    if (std::isdigit(*input) || *input == '.')
        return parse_double(input);

    if (std::isalpha(*input) || *input == '_')
        return parse_function_call(input);

    if (*input == '(' || *input == '[' || *input == '{')
        return parse_group(input);

    if (*input == '|') {
        calculation_type const value = parse_group(input);

        return std::abs(value);
    }

    m_error = true;

    throw_error("[ERR] >> expected value here: " + QString::fromUtf8(input) + " ");

    Q_UNREACHABLE();
}

calculation_type DMathConsoleTextBox::parse_infix_function_call(char const*& input) {
    calculation_type result = parse_primary_expression(input);

    forever {
        skip_spaces(input);

        if(*input != ':')
            break;

        std::string const name = parse_identifier(++input);

        skip_spaces(input);

        if (*input != ':') {
            m_error = true;

            throw_error("[ERR] >> missing terminating ':' at infix call to function '" + QString::fromStdString(name) + "' ");
        }

        std::map<std::string, calculation_type (*) (calculation_type, calculation_type)>::iterator iter = binary_functions.find(name);

        if (iter != binary_functions.end()) {
            result = iter->second(result, parse_primary_expression(++input));
        }
        else {
            m_error = true;

            throw_error("[ERR] >> infix call to unknown function '" + QString::fromStdString(name) + "' ");
        }
    }

    return result;
}

calculation_type DMathConsoleTextBox::parse_root(char const*& input) {
    calculation_type root = parse_infix_function_call(input);

    skip_spaces(input);

    if (*input == '#')
        root = std::pow(parse_root(++input), 1 / root);

    return root;
}

calculation_type DMathConsoleTextBox::parse_power(char const*& input) {
    calculation_type power = parse_root(input);

    skip_spaces(input);

    if (*input == '^')
        power = std::pow(power, parse_power(++input));

    return power;
}

calculation_type DMathConsoleTextBox::parse_product(char const*& input) {
    calculation_type product = parse_power(input);

    forever {
        skip_spaces(input);

        if (*input == '*')
            product *= parse_power(++input);
        else if (*input == '/')
            product /= parse_power(++input);
        else if (*input == '%')
            product = std::fmod(product, parse_power(++input));
        else
            break;
    }

    return product;
}

calculation_type DMathConsoleTextBox::parse_sum(char const*& input) {
    calculation_type sum = parse_product(input);

    forever {
        skip_spaces(input);

        if (*input == '+')
            sum += parse_product(++input);
        else if (*input == '-')
            sum -= parse_product(++input);
        else
            break;
    }

    return sum;
}

QString DMathConsoleTextBox::eval_2(char const* input) {
    calculation_type result = parse_sum(input);

    skip_spaces(input);

    if (!*input)
        return QString::number(result, 'g', m_precision);
    else {
        m_error = true;

        return ("ERROR");
    }
}

calculation_type logdual(calculation_type v) {
    return log10(v)/log10(2);
}

calculation_type square(calculation_type v) {
    return v * v ;
}

calculation_type cubic(calculation_type v) {
    return v * v * v;
}

calculation_type sec(calculation_type v) {
    return 1/cos(v);
}

calculation_type csc(calculation_type v) {
    return 1/sin(v);
}

calculation_type sech(calculation_type v) {
    return 1/cosh(v);
}

calculation_type csch(calculation_type v) {
    return 1/sinh(v);
}

calculation_type arcsec(calculation_type v) {
    return acos(1/v);
}

calculation_type arccsc(calculation_type v) {
    return asin(1/v);
}

calculation_type arsinh(calculation_type v) {
    return log(v + sqrt(v*v + 1));
}

calculation_type arcosh(calculation_type v) {
    return log(v + sqrt(v*v - 1));
}

calculation_type artanh(calculation_type v) {
    return 0.5 * log((1 + v)/(1 - v));
}

calculation_type arcoth(calculation_type v) {
    return 0.5 * log((v + 1)/(v - 1));
}

calculation_type arsech(calculation_type v) {
    return log((1 + sqrt(1 - v*v ))/v);
}

calculation_type arcsch(calculation_type v) {
    return log((1/v) + sqrt( (1/(v*v)) + 1));
}

calculation_type root(calculation_type v) {
    return sqrt(v);
}

calculation_type random(calculation_type v) {
    return v*((double)rand()/RAND_MAX);
}

calculation_type sgn(calculation_type v) {
    if (v > 0)
        return 1;

    if (v < 0)
        return -1;

    if (v == 0)
        return 0;

    Q_UNREACHABLE();
}

calculation_type sinc(calculation_type v) {
    return sin(v)/v;
}

calculation_type normalized_sinc(calculation_type v) {
    return sin(v * PI)/(v * PI);
}

calculation_type factorial(calculation_type v) {
    return tgamma(v + 1);
}

calculation_type rectangular(calculation_type v) {
    if (abs(v) > 0.5)
        return 0;

    if (abs(v) == 0.5)
        return 0.5;

    if (abs(v) < 0.5)
        return 1;

    Q_UNREACHABLE();
}

calculation_type heavyside(calculation_type v) {
    if (v >= 0)
        return 1;

    if (v < 0)
        return 0;

    Q_UNREACHABLE();
}

calculation_type degree(calculation_type v) {
    return v*(180/PI);
}

calculation_type cubicroot(calculation_type v) {
    return std::pow(v,double(1/3));
}

calculation_type cot(calculation_type v) {
    return 1/tan(v);
}

calculation_type coth(calculation_type v) {
    return 1/tanh(v);
}

calculation_type arccoth(calculation_type v) {
    return 0.5 * log((v + 1)/(v - 1));
}

calculation_type arctanh(calculation_type v) {
    return 0.5 * log((v + 1)/(1 - v));
}

calculation_type arccot(calculation_type v) {
    return atan(1/v);
}

calculation_type arcsinh(calculation_type v) {
    return 0.5 * log( v + sqrt(v*v + 1));
}

double fibo(double z) {

    if (z == 0)
        return 0;

    if (z==1
            || z==2)
        return 1;
    else
        return fibo(z - 1) + fibo(z - 2);
}

calculation_type fibonacci(calculation_type v) {
    if (v >= 0)
        return fibo(v);
    else {
        double exponent = -1;
        double value = fibo(abs(v));

        for (int i = 1 ; i < abs(v) + 1 ; i ++) {

            exponent *= (-1);
        }

        return exponent * value;
    }
}

calculation_type bernoulliNumber(calculation_type v) {
    return v / (exp(v) - 1);
}

calculation_type sigmoide1(calculation_type v) {
    return 1 / (exp(-v) + 1);
}

calculation_type sigmoide2(calculation_type v) {
    return v / sqrt(v*v + 1);
}

calculation_type radiant(calculation_type v) {
    return v * PI / 180;
}

void DMathConsoleTextBox::init_constants() {
    constants["PI"] = PI;
    constants["E"]  = E;

    constants["OUT"] = 0;

    for (std::map<std::string, calculation_type>::iterator iter = constants.begin() ; iter != constants.end() ; iter ++)
        m_systConstList.append(QString::fromStdString(iter->first));
}

void DMathConsoleTextBox::showToolTip(const QString& data) {
    QToolTip::hideText();

    return; // we do not want to have the tooltip

    const QPoint p(m_completer->popup()->geometry().x() + m_completer->popup()->geometry().width() - 2 , m_completer->popup()->frameGeometry().topRight().y() - m_completer->popup()->horizontalScrollBar()->height() + 9 );
    const QString text = DConstantExplanations::getExplanation(data);

    if (!text.isEmpty()
         || !text.isNull())
        QToolTip::showText(p, text);
}

void DMathConsoleTextBox::setTextFont(const QFont &font) {
    m_nf = font;
    setFont(m_nf);

    m_completer->popup()->setFont(m_nf);

    update();
}

void DMathConsoleTextBox::clearOutput() {
    selectAll();

    QKeyEvent * eventClear = new QKeyEvent(QEvent::KeyPress, Qt::Key_CapsLock, Qt::NoModifier, "");

    keyPressEvent(eventClear);

    //DDELETE_SAFETY(eventClear);
}

DMathConsoleTextBox::DMathConsoleTextBox(QWidget *parent) :
    QTextEdit(parent) {
    init_constants();

    unary_functions["sin"] = sin;
    unary_functions["cos"] = cos;
    unary_functions["tan"] = tan;
    unary_functions["cot"] = cot;

    unary_functions["sinh"] = sinh;
    unary_functions["cosh"] = cosh;
    unary_functions["tanh"] = tanh;
    unary_functions["coth"] = coth;

    unary_functions["arcsin"] = asin;
    unary_functions["arccos"] = acos;
    unary_functions["arctan"] = atan;
    unary_functions["arccot"] = arccot;

    unary_functions["ln"] = log;
    unary_functions["ld"] = logdual;
    unary_functions["lg"] = log10;
    unary_functions["log"] = log;

    unary_functions["exp"] = exp;

    unary_functions["abs"] = abs;
    unary_functions["sgn"] = sgn;
    unary_functions["floor"] = floor;
    unary_functions["ceil"] = ceil;
    unary_functions["trunc"] = trunc;

    unary_functions["toDegree"] = degree;
    unary_functions["toRadian"] = radiant;

    unary_functions["square"] = square;
    unary_functions["cubic"] = cubic;
    unary_functions["sqrt"] = root;
    unary_functions["cbrt"] = cbrt;
    unary_functions["fac"] = factorial;

    unary_functions["rnd"] = random;

    unary_functions["sec"] = sec;
    unary_functions["csc"] = csc;
    unary_functions["sech"] = sech;
    unary_functions["csch"] = csch;

    unary_functions["arcsec"] = arcsec;
    unary_functions["arccsc"] = arccsc;

    unary_functions["arcsinh"] = asinh;
    unary_functions["arccosh"] = acosh;
    unary_functions["arctanh"] = atanh;
    unary_functions["arccoth"] = arccoth;

    unary_functions["arsinh"] = arsinh;
    unary_functions["arcosh"] = arcosh;
    unary_functions["artanh"] = artanh;
    unary_functions["arcoth"] = arcoth;
    unary_functions["arsech"] = arsech;
    unary_functions["arcsch"] = arcsch;

    unary_functions["sinc"] = sinc;
    unary_functions["nsinc"] = normalized_sinc;
    unary_functions["rect"] = rectangular;
    unary_functions["heavyside"] = heavyside;

    unary_functions["sig1"] = sigmoide1;
    unary_functions["sig2"] = sigmoide2;

    unary_functions["gamma"] = tgamma;
    unary_functions["lngamma"] = lgamma;

    unary_functions["erf"] = erf;
    unary_functions["erfc"] = erfc;

    setCursorWidth(5);
    setTextInteractionFlags( Qt::TextEditable |
                             Qt::TextSelectableByKeyboard |
                             Qt::LinksAccessibleByKeyboard |
                             Qt::TextSelectableByMouse |
                             Qt::TextEditorInteraction
                             );

    setFontWeight( QFont::StyleItalic );
    setTextColor(Qt::blue);

    m_outlet_functions.clear(); // not used

    m_tc = textColor();
    m_nf = font();

    m_error = false;
    m_cleared = false;

    insertPlainText("[CAL] << ");

    m_ans = 0;
    constants["OUT"] = m_ans;

    m_paramCount = constants.size();
    m_precision = 10;

    setLineWrapMode(QTextEdit::WidgetWidth);
    setOverwriteMode(false);
    setUndoRedoEnabled(false);
    setAcceptRichText(false);
    setShortcutEnabled(false);
    setAcceptDrops(false);

    setContextMenuPolicy(Qt::NoContextMenu);

    m_completerWasShown = false;

    m_completer = new QCompleter(this);
    m_completer->setWidget(this);

    QStringList list;

    for (std::map<std::string, calculation_type>::iterator iter = constants.begin() ; iter != constants.end() ; iter ++ )
        list << QString::fromStdString(iter->first) ;

    for (std::map<std::string, calculation_type (*) (calculation_type)>::iterator iter = unary_functions.begin() ; iter != unary_functions.end() ; iter ++)
        list << QString::fromStdString(iter->first) + "()" ;

    QStringListModel *model = new QStringListModel(list, m_completer);

    m_completer->setModel(model);
    m_completer->setCaseSensitivity(Qt::CaseSensitive);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCompletionRole(Qt::DisplayRole);
    m_completer->popup()->setFont(font());

    for (std::map<std::string, calculation_type (*) (calculation_type)>::iterator iter = unary_functions.begin() ; iter != unary_functions.end() ; iter ++)
        m_systFuncList.append(QString::fromStdString(iter->first));

    for (int i = 0 ; i < list.size() ; i ++)
        m_nonCalcFuncList.append(list.at(i));

    connect(m_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
    connect(m_completer, SIGNAL(highlighted(QString)), this, SLOT(showToolTip(QString)));
}

DMathConsoleTextBox::~DMathConsoleTextBox() {
    DDELETE_SAFETY(m_completer);
}

void DMathConsoleTextBox::keyPressEvent(QKeyEvent *e) {
    bool comma = false;

    static QString notAllowed("°!§$&=?`´²³ß~';µ@€<>|üÜäÄöÖ\\");

    if (!notAllowed.contains(e->text().right(1))
            || e->text().isEmpty()) {
        if (e->key() == Qt::Key_Comma) {

            insertPlainText(".");
            comma = true;

            e->ignore();
        }

        if (!comma) {
            if (!m_completer->popup()->isVisible()) {
                m_completerWasShown = false;

                QString result;

                if (e->key() == Qt::Key_Space)
                    e->setAccepted(false);

                if (e->key() ==  Qt::Key_Return) {
                    moveCursor(QTextCursor::MoveOperation::End, QTextCursor::MoveMode::MoveAnchor);

                    QTextBlock block = textCursor().block();
                    QStringList list = block.text().split("<<");

                    if (list.size() == 2) {
                        result = eval_2(list[1].toStdString().c_str());

                        if (!m_error) {
                            if (result.isEmpty()
                                    || result.isNull()
                                    || result.contains("ERROR")) {
                                throw_error("[ERR] >> noncalculating input done!\n");
                            }
                            else {
                                m_ans = result.toDouble();
                                constants["OUT"] = m_ans;

                                setTextColor(QColor(Qt::darkGreen));
                                setFont(m_nf);

                                insertPlainText("\n\n[OUT] >> " + result + "\n");
                            }
                        }
                        else {
                            if (result.contains("ERROR")) {
                                throw_error( "[ERR] >> noncalculating input done!\n");
                            }
                            else {
                                insertPlainText("\n");
                            }
                        }
                    }

                    if (list.size() == 1) {
                        result = eval_2(list[0].toStdString().c_str());

                        if (!m_error) {
                            if (result.isEmpty()
                                    || result.isNull()
                                    || result.contains("ERROR")) {
                                throw_error( "[ERR] >> noncalculating input done!\n");
                            }
                            else {
                                setTextColor(QColor(Qt::darkGreen));
                                setFont(m_nf);

                                insertPlainText("\n\n[OUT] >> " + result + "\n");
                            }
                        }
                        else {
                            if (result.contains("ERROR")) {
                                throw_error( "[ERR] >> noncalculating input done!\n");
                            }
                            else {
                                insertPlainText("\n");
                            }
                        }
                    }

                    QTextEdit::keyPressEvent(e);
                }

                if (e->key() == Qt::Key_Backspace) {
                    if (textCursor().columnNumber() < 10
                            || textCursor().blockNumber() + 1 != document()->blockCount()) {
                            /* nothing here */
                    }
                    else {
                        QTextEdit::keyPressEvent(e);
                    }
                }

                if (e->key() == Qt::Key_Left) {
                    if (textCursor().columnNumber() < 10) {
                        /* nothing here */
                    }
                    else {
                        QTextEdit::keyPressEvent(e);
                    }
                }

                if (e->key() == Qt::Key_Right) {
                    QTextEdit::keyPressEvent(e);
                }

                if (e->key() != Qt::Key_Space
                        && e->key() != Qt::Key_Return
                        && e->key() != Qt::Key_Backspace
                        && e->key() != Qt::Key_Left
                        && e->key() != Qt::Key_Up
                        && e->key() != Qt::Key_Right ) {
                    if (textCursor().columnNumber() < 9
                            || textCursor().blockNumber() + 1 != document()->blockCount() ) {
                            /* nothing here */
                    }
                    else {
                        QTextEdit::keyPressEvent(e);
                    }
                }
            }

            if ( m_completer->popup()->isVisible() ) {
                m_completerWasShown = true;

                switch(e->key()) {
                case Qt::Key_Return:
                case Qt::Key_Escape:
                case Qt::Key_Tab:
                case Qt::Key_Backtab:
                    e->ignore();
                    return;

                default:
                    break;
                }

                QTextEdit::keyPressEvent(e);
            }

            const QString completionPrefix = textUnderCursor();

            if (completionPrefix != m_completer->completionPrefix()) {
                m_completer->setCompletionPrefix(completionPrefix);
                m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));

                showToolTip(m_completer->currentCompletion());
            }

            if (!e->text().isEmpty()
                    && completionPrefix.length() >= 1) {
                QRect cr = cursorRect();

                cr.setWidth(m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());
                m_completer->complete(cr);

                showToolTip(m_completer->currentCompletion());
            }
            else {
                m_completer->popup()->setVisible(false);
            }
        }
    }
}

void DMathConsoleTextBox::keyReleaseEvent(QKeyEvent *e) {
    if (!m_completer->popup()->isVisible()
         && !m_completerWasShown) {
        if (e->key() == Qt::Key_Space) {
            e->setAccepted(false);
        }

        if (e->key() ==  Qt::Key_Return) {
            if (!m_error) {
                setTextColor(m_tc);
                setFont(m_nf);

                insertPlainText("[CAL] << ");
            }
            else {
                setTextColor(m_tc);
                setFont(m_nf);

                insertPlainText("[CAL] << ");

                m_error = false;
            }

            QTextEdit::keyReleaseEvent(e);
        }

        if (e->key() != Qt::Key_Space
             && e->key() != Qt::Key_Return
             && e->key() != Qt::Key_Backspace
             && e->key() != Qt::Key_Left) {
            QTextEdit::keyReleaseEvent(e);
        }
    }
}

void DMathConsoleTextBox::insertCompletion(const QString &completion) {
    QTextCursor cursor = textCursor();

    const int extra = completion.length() - m_completer->completionPrefix().length();

    cursor.movePosition(QTextCursor::Left);
    cursor.movePosition(QTextCursor::EndOfWord);

    cursor.insertText(completion.right(extra));

    if (completion.right(1) == ")")
        cursor.setPosition(cursor.position()-1);

    setTextCursor( cursor );
}

QString DMathConsoleTextBox::textUnderCursor() const {
    QTextCursor cursor = textCursor();

    cursor.select(QTextCursor::WordUnderCursor);

    return cursor.selectedText();
}

void DMathConsoleTextBox::throw_error(const QString &message) {
    setTextColor(QColor(Qt::red));

    insertPlainText("\n" + message);

    m_error = true;
}
