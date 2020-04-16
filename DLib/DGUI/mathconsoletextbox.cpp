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

void DMathConsoleTextBox::skip_spaces(char const*& input)
{
    while(std::isspace(*input))
        ++input;
}

double DMathConsoleTextBox::parse_double(char const*& input)
{
    return std::strtod(input, const_cast<char**>(&input));
}

std::string DMathConsoleTextBox::parse_identifier(char const*& input)
{
    skip_spaces(input);
    std::string identifier(1, *input++);

    while(std::isalnum(*input) || *input == '_')
        identifier += *input++;

    return identifier;
}

calculation_type DMathConsoleTextBox::parse_function_call(char const*& input)
{
    std::string const name = parse_identifier(input);

    {
        std::map<std::string,double>::iterator iter = constants.find(name);

        if( iter != constants.end() )
            return iter->second;
    }

    {
        std::map<std::string, calculation_type (*) ()>::iterator iter = nullary_functions.find(name);

        if(iter != nullary_functions.end())
            return iter->second();
    }

    {
        std::map<std::string, calculation_type (*) (calculation_type)>::iterator iter = unary_functions.find(name);

        if(iter != unary_functions.end())
        {
            calculation_type const argument = parse_primary_expression(input);

            return iter->second(argument);
        }
    }

    {
        std::map<std::string, calculation_type (*) (calculation_type, calculation_type)>::iterator iter = binary_functions.find(name);

        if(iter != binary_functions.end())
        {
            calculation_type const arg1 = parse_primary_expression(input);
            calculation_type const arg2 = parse_primary_expression(input);

            return iter->second(arg1, arg2);
        }
    }

    error = true;
    throw_error("[ERR] >> call to unknown function '" + QString::fromStdString(name) + "' ");
}

calculation_type DMathConsoleTextBox::parse_group(char const*& input)
{  
    char const delim = *input == '(' ? ')' : *input == '[' ? ']' : *input == '{' ? '}' : '|';

    calculation_type const value = parse_sum(++input);

    if(*input++ != delim){
        error = true;
        throw_error("[ERR] >> terminating '" + QString::fromStdString(std::string(1, delim)) + "' missing");
    }
    return value;
}

calculation_type DMathConsoleTextBox::parse_primary_expression(char const*& input)
{
    skip_spaces(input);

    if(*input == '+')
        return parse_primary_expression(++input);

    if(*input == '-')
        return -parse_primary_expression(++input);

    if(*input == '#')
        return std::sqrt(parse_primary_expression(++input));

    if(std::isdigit(*input) || *input == '.')
        return parse_double(input);

    if(std::isalpha(*input) || *input == '_')
        return parse_function_call(input);

    if(*input == '(' || *input == '[' || *input == '{')
        return parse_group(input);

    if(*input == '|')
    {
        calculation_type const value = parse_group(input);
        return std::abs(value);
    }

    error = true;
    throw_error("[ERR] >> expected value here: " + QString::fromUtf8(input) + " ");
}

calculation_type DMathConsoleTextBox::parse_infix_function_call(char const*& input)
{
    calculation_type result = parse_primary_expression(input);

    for(;;)
    {
        skip_spaces(input);

        if(*input != ':')
            break;

        std::string const name = parse_identifier(++input);

        skip_spaces(input);

        if(*input != ':'){
            error = true;
            throw_error("[ERR] >> missing terminating ':' at infix call to function '" + QString::fromStdString(name) + "' ");
        }

        std::map<std::string, calculation_type (*) (calculation_type, calculation_type)>::iterator iter = binary_functions.find(name);

        if(iter != binary_functions.end()){
            result = iter->second(result, parse_primary_expression(++input));
        }
        else
        {
            error = true;
            throw_error("[ERR] >> infix call to unknown function '" + QString::fromStdString(name) + "' ");
        }
    }

    return result;
}

calculation_type DMathConsoleTextBox::parse_root(char const*& input)
{
    calculation_type root = parse_infix_function_call(input);

    skip_spaces(input);

    if(*input == '#')
        root = std::pow(parse_root(++input), 1 / root);

    return root;
}

calculation_type DMathConsoleTextBox::parse_power(char const*& input)
{
    calculation_type power = parse_root(input);

    skip_spaces(input);

    if(*input == '^')
        power = std::pow(power, parse_power(++input));

    return power;
}

calculation_type DMathConsoleTextBox::parse_product(char const*& input)
{
    calculation_type product = parse_power(input);

    for(;;)
    {
        skip_spaces(input);

        if(*input == '*')
            product *= parse_power(++input);
        else if(*input == '/')
            product /= parse_power(++input);
        else if(*input == '%')
            product = std::fmod(product, parse_power(++input));
        else break;
    }

    return product;
}

calculation_type DMathConsoleTextBox::parse_sum(char const*& input)
{
    calculation_type sum = parse_product(input);

    for(;;)
    {
        skip_spaces(input);

        if(*input == '+')
            sum += parse_product(++input);
        else if(*input == '-')
            sum -= parse_product(++input);
        else break;
    }

    return sum;
}


QString DMathConsoleTextBox::eval_2(char const* input)
{
    calculation_type result = parse_sum(input);

    skip_spaces(input);

    if(!*input){

        return QString::number(result,'g',precision);

    }else{

        error = true;
        return ("ERROR");
    }
}

calculation_type logdual(calculation_type v)
{
    return ( log10(v)/log10(2) );
}

calculation_type square(calculation_type v)
{
    return ( v * v );
}

calculation_type cubic(calculation_type v)
{
    return ( v * v * v );
}

calculation_type sec(calculation_type v)
{
    return ( 1/cos(v) );
}

calculation_type csc(calculation_type v)
{
    return ( 1/sin(v) );
}

calculation_type sech(calculation_type v)
{
    return ( 1/cosh(v) );
}

calculation_type csch(calculation_type v)
{
    return ( 1/sinh(v) );
}

calculation_type arcsec(calculation_type v)
{
    return ( acos(1/v) );
}

calculation_type arccsc(calculation_type v)
{
    return ( asin(1/v) );
}

calculation_type arsinh(calculation_type v)
{
    return ( log(v + sqrt(v*v + 1)) );
}

calculation_type arcosh(calculation_type v)
{
    return ( log(v + sqrt(v*v - 1)) );
}

calculation_type artanh(calculation_type v)
{
    return ( 0.5 * log( ( 1 + v )/( 1 - v ) ) );
}

calculation_type arcoth(calculation_type v)
{
    return ( 0.5 * log( ( v + 1 )/( v - 1 ) ) );
}

calculation_type arsech(calculation_type v)
{
    return log( (1 + sqrt( 1 - v*v ))/v );
}

calculation_type arcsch(calculation_type v)
{
    return log( (1/v) + sqrt( (1/(v*v)) + 1 ) );
}

calculation_type root(calculation_type v)
{
    return sqrt(v);
}

calculation_type random(calculation_type v)
{
    return v*((double)rand() / RAND_MAX);
}

calculation_type sgn(calculation_type v)
{
    if ( v > 0 ){ return 1;}
    if ( v < 0 ){ return -1;}
    if ( v == 0 ){ return 0;}
}

calculation_type sinc(calculation_type v)
{
    return (sin(v)/v);
}

calculation_type normalized_sinc(calculation_type v)
{
    return sin(v * 3.14159265358979323846f )/(v * 3.14159265358979323846f);
}

calculation_type factorial(calculation_type v)
{
    return tgamma(v + 1);
}

calculation_type rectangular(calculation_type v)
{
    if ( abs(v) > 0.5 ){return 0;}
    if ( abs(v) == 0.5 ){return 0.5;}
    if ( abs(v) < 0.5 ){return 1;}
}

calculation_type heavyside(calculation_type v)
{
    if ( v >= 0 ){return 1;}
    if ( v < 0 ){return 0;}
}

calculation_type degree(calculation_type v)
{
    return ( v * ( 180 / PI ) );
}

calculation_type cubicroot(calculation_type v)
{
    return ( std::pow(v,0.3333333333333333333333333333333333333333333333333333333333333333333333333333333333) );
}

calculation_type cot(calculation_type v)
{
    return ( 1/tan(v) );
}

calculation_type coth(calculation_type v)
{
    return ( 1/tanh(v) );
}

calculation_type arccoth(calculation_type v)
{
    return ( 0.5 * log( (v + 1) / (v - 1) ) );
}

calculation_type arctanh(calculation_type v)
{
    return ( 0.5 * log( (1 + v) / (1 - v) ) );
}

calculation_type arccot(calculation_type v)
{
    return atan(1/v);
}

calculation_type arcsinh(calculation_type v)
{
    return (0.5 * log( v + sqrt(v*v + 1) ));
}

double fibo( double z ){

    if( z == 0 )
        return 0;
    if( z==1 || z==2 )
        return 1;
    else
        return fibo( z-1 ) + fibo( z-2 );
}

calculation_type fibonacci(calculation_type v)
{
    if ( v >= 0 ){
        return fibo(v);
    }else{

        double exponent = -1;
        double value = fibo(abs(v));

        for ( int i = 1 ; i < abs(v) + 1 ; i ++ ){

            exponent *= (-1);
        }

        return exponent * value;
    }
}

calculation_type bernoulliNumber(calculation_type v)
{
    return ( v / (exp(v) - 1) );
}

calculation_type sigmoide1(calculation_type v)
{
    return ( 1 / ( exp(-v) + 1 ) );
}

calculation_type sigmoide2(calculation_type v)
{
    return ( v / sqrt( v*v + 1 ) );
}

calculation_type radiant(calculation_type v)
{
    return ( v * PI / 180 );
}

void DMathConsoleTextBox::init_constants()
{
    constants["PI"] = PI;
    constants["E"]  = E;
    constants["GOLDENRATIO"] = GOLDEN_NUMBER;
    constants["EULER_CONST"] = EULER_CONST;
    constants["PLANCK_H1"] = CONST_OF_PLANCK_eVs;
    constants["PLANCK_H2"] = CONST_OF_PLANCK_Js;
    constants["PLANCK_RH1"] = REDUCED_CONST_OF_PLANCK_eVs;
    constants["PLANCK_RH2"] = REDUCED_CONST_OF_PLANCK_Js;
    constants["K_BOLTZMANN_1"] = BOLTZMANN_CONSTANT_eV;
    constants["K_BOLTZMANN_2"] = BOLTZMANN_CONSTANT_J;
    constants["K_BOLTZMANN_3"] = BOLTZMANN_CONSTANT_Hz;
    constants["STEFAN_BOLTZMANN"] = STEFAN_BOLTZMANN_CONSTANT;
    constants["ELECTRONM"] = ELECTRON_MASS;
    constants["NEUTRONM"] = NEUTRON_MASS;
    constants["MUONM"] = MUON_MASS;
    constants["PROTONM"] = PROTON_MASS;
    constants["ELECTRONG"] = ELECTRON_GFACTOR;       
    constants["NEUTRONG"] = NEUTRON_GFACTOR;
    constants["MUONG"] = MUON_GFACTOR;
    constants["PROTONG"] = PROTON_GFACTOR;
    constants["ELECTRONC"] = ELECTRON_CHARGE;
    constants["NEUTRONC"] = 0;
    constants["PROTONC"] = PROTON_CHARGE;
    constants["MUONC"] = MUON_CHARGE;
    constants["LIGHTSPEED"] = SPEED_OF_LIGHT;
    constants["GRAVITYACC"] = ACCELERATION_OF_GRAVITY;
    constants["GRAVITYCONST"] = GRAVITY_CONSTANT;
    constants["ATOMIC_UNIT"] = UNIFIED_ATOMIC_MASS_UNIT;
    constants["KLITZING"] = KLITZING_CONSTANT;
    constants["ATM_PRESSURE"] = ATMOSPHERE_PRESSURE;
    constants["COMPTON_0"] = COMPTON_WAVELENGTH;
    constants["BOHR"] = BOHR_RADIUS;
    constants["AVOGADRO"] = AVOGADRO_CONSTANT;
    constants["ANGSTROM"] = ANGSTROM_STAR;
    constants["GAS_CONST"] = GAS_CONST;
    constants["FARADAY"] = FARADAY_CONST;
    constants["HUBBLE"] = HUBBLE_CONST;
    constants["PERMVAC1"] = ELECTRIC_PERMITTIVITY_VACUUM;
    constants["PERMVAC2"] = MAGNETIC_PERMITTIVITY_VACUUM;
    constants["BOHRM_J"] = BOHR_MAGNETON_J;
    constants["BOHRM_eV"] = BOHR_MAGNETON_eV;
    constants["BOHRM_Hz"] = BOHR_MAGNETON_Hz;
    constants["IMPEDANCE"] = CHARACTERISTIC_IMPEDANCE_OF_VACUUM;
    constants["ALPHAM"] = ALPHAPARTICLE_MASS;
    constants["ALPHAC"] = ALPHAPARTICLE_CHARGE;
    constants["COSMICV_1"] = COSMIC_VELOCITY_1;
    constants["COSMICV_2"] = COSMIC_VELOCITY_2;
    constants["COSMICV_3"] = COSMIC_VELOCITY_3;
    constants["COSMICV_4"] = COSMIC_VELOCITY_4;
    constants["RYDBERG"] = RYDBERG_INFINITY;
    constants["SOLAR"] = SOLAR_CONST;
    constants["WIENSHIFT"] = WIEN_SHIFT_CONST;
    constants["FINESTRUCT"] = FINESTRUCT_CONST;
    constants["ANTIMUONC"] = ANTIMUON_CHARGE;
    constants["PIONC"] = PION_CHARGE;
    constants["ANTIPIONC"] = ANTIPION_CHARGE;
    constants["NEUTRALPIONC"] = NEUTRALPION_CHARGE;
    constants["PIONM"] = PION_MASS;
    constants["ANTIPIONM"] = ANTIPION_MASS;
    constants["NEUTRALPIONM"] = NEUTRALPION_MASS;
    constants["ANTIMUONG"] = ANTIMUON_GFACTOR;
    constants["COREM_J"] = CORE_MAGNETON_J;
    constants["COREM_eV"] = CORE_MAGNETON_eV;
    constants["MAGNMOMENT_E"] = MAGNETIC_MOMENT_ELECTRON;
    constants["MAGNMOMENT_N"] = MAGNETIC_MOMENT_NEUTRON;
    constants["MAGNMOMENT_P"] = MAGNETIC_MOMENT_PROTON;
    constants["MAGNMOMENT_M"] = MAGNETIC_MOMENT_MUON;
    constants["RYDEBERG_ENERGY"] = RYDBERG_ENERGY;
    constants["THOMSON"] = THOMSON_CROSS_SECTION;
    constants["LOSCHMIDT"] = LOSCHMIDT_CONSTANT;
    constants["MOLARVOL"] = MOLAR_VOLUME;
    constants["KELVIN"] =  KELVIN;

    //TODO:
    constants["COMPTON_E"] = COMPTON_WAVELENGTH_ELECTRON;
    constants["COMPTON_N"] = COMPTON_WAVELENGTH_NEUTRON;
    constants["COMPTON_P"] = COMPTON_WAVELENGTH_PROTON;
    //END TODO:

    constants["OUT"] = 0;

    for ( std::map<std::string, calculation_type>::iterator iter = constants.begin() ; iter != constants.end() ; iter ++ ){

        systConstList.append( QString::fromStdString(iter->first) );
    }
}

void DMathConsoleTextBox::clearFromConstantList(const QString &value)
{
    for ( std::map<std::string, calculation_type>::iterator iter = constants.begin() ; iter != constants.end() ; iter++){

        if ( QString::fromStdString(iter->first) == value ){

            constants.erase(iter);
            break;
        }
    }
}

void DMathConsoleTextBox::call_outletFunction(const QString& Outlet_Function)
{
    if ( Outlet_Function == "_clear:" ){
        this->selectAll();
        cleared = true;
    }

    if ( Outlet_Function == "_systConst:" ){

        int i = 1;

        for ( std::map<std::string, calculation_type>::iterator iter = constants.begin() ; iter != constants.end() ; iter ++ ){

            if ( iter == constants.begin() ){
                this->insertPlainText( "\n");
            }

            if ( systConstList.contains( QString::fromStdString(iter->first) ) ){//i <= ParamCount ){

                this->setTextColor( QColor( Qt::black ) );
                this->insertPlainText( "\n                " + QString::fromStdString(iter->first) + " (defines:) " + QString::number(iter->second,'e',6) + QString(" ...      \n") );
            }
            i++;
        }
    }

    if ( Outlet_Function == "_userDefs:" ){

        int i = 1;

        for ( std::map<std::string, calculation_type>::iterator iter = constants.begin() ; iter != constants.end() ; iter++){

            if ( iter == constants.begin() ){
                this->insertPlainText( "\n");
            }

            if ( !systConstList.contains( QString::fromStdString(iter->first) ) ){//i > ParamCount ){

                this->setTextColor( QColor( Qt::black ) );
                this->insertPlainText( "\n                " + QString::fromStdString(iter->first) + " (defines:) " + QString::number(iter->second,'e',6) + QString(" ...      \n") );

            }
            i++;
        }

        if ( i == ParamCount + 1 ){

            this->setTextColor( QColor( Qt::black ) );
            this->insertPlainText( "\n                " + QString("definition list is empty!") + "\n" );

        }
    }

    if ( Outlet_Function == "_allConstDefs:" ){

        for ( std::map<std::string, calculation_type>::iterator iter = constants.begin() ; iter != constants.end() ; iter++){

            if ( iter == constants.begin() ){
                this->insertPlainText( "\n");
            }

            this->setTextColor( QColor( Qt::black ) );
            this->insertPlainText( "\n                " + QString::fromStdString(iter->first) + " (defines:) " + QString::number(iter->second,'e',6) + QString(" ...      \n") );
        }
    }

    if ( Outlet_Function == "_floatPrec:" ){

        this->setTextColor( QColor( Qt::darkBlue ) );
        this->insertPlainText( "\n\n[PRC] >> maximum precision: " +  QString::number(precision,'f',0)  + "\n" );
    }

    if ( Outlet_Function == "_OUTtoExp:" ){

        this->setTextColor( QColor( Qt::darkGreen ) );
        ANS = QString::number(ANS,'e',precision).toDouble();
        constants["OUT"] = ANS;
        this->insertPlainText( "\n\n[OUT] >> " +  QString::number(ANS,'e',precision)  + "\n" );
    }

    if ( Outlet_Function == "_OUTtoFloat:" ){

        this->setTextColor( QColor( Qt::darkGreen ) );
        ANS = QString::number(ANS,'f',precision).toDouble();
        constants["OUT"] = ANS;
        this->insertPlainText( "\n\n[OUT] >> " +  QString::number(ANS,'f',precision)  + "\n" );
    }
}

bool DMathConsoleTextBox::in_ConstantList(const QString& value)
{
    bool returnVal = false;

    for ( std::map<std::string, calculation_type>::iterator iter = constants.begin() ; iter != constants.end() ; iter++){

        if ( QString::fromStdString(iter->first) == value ){

            returnVal = true;
            break;
        }
    }

    return returnVal;
}

bool DMathConsoleTextBox::in_UserList(const QString& value)
{
    bool returnVal = false;

    for ( std::map<std::string, calculation_type>::iterator iter = constants.begin() ; iter != constants.end() ; iter++){

        if ( QString::fromStdString(iter->first) == value && !systConstList.contains(value) ){

            returnVal = true;
            break;
        }
    }

    return returnVal;
}

void DMathConsoleTextBox::showToolTip(const QString& data)
{
    QToolTip::hideText();

    QPoint p( m_completer->popup()->geometry().x() + m_completer->popup()->geometry().width() - 2 , m_completer->popup()->frameGeometry().topRight().y() - m_completer->popup()->horizontalScrollBar()->height() + 9 );
    QString text = DConstantExplanations::getExplanation(data);


    if ( data == "_clear:" ){

        text = "<nobr><b>Clears the screen. It does not delete the done defines.</b></nobr>";
    }

    if ( data == "_systConst:" ){

        text = "<nobr><b>Lists all given constants with its value.</b></nobr>";
    }

    if ( data == "_userDefs:" ){

        text = "<nobr><b>Lists all your done defines with its value.</b></nobr>";
    }

    if ( data == "_allConstDefs:" ){

        text = "<nobr><b>Lists all constants and your done defines with its value.</b></nobr>";
    }

    if ( data == "_OUTtoExp:" ){

        text = "<nobr><b>Sets the last result <i>(see: OUT)</i> to the exponential number format.</b></nobr>";
    }

    if ( data == "_OUTtoFloat:" ){

        text = "<nobr><b>Sets the last result <i>(see: OUT)</i> to the float number format.</b></nobr>";
    }

    if ( data == "_def:" ){

        text = "<nobr><b>Defines a new constant which you can use in following calculations.</b><br><br></nobr>"
                "<nobr>input expection: <i>(name as string)_def:(value as number)</i></nobr>";
    }

    if ( data == "_unDef:" ){

        text = "<nobr><b>Undo the definition of a constant.</b><br><br></nobr>"
                "<nobr>input expection: <i>_unDef:(name as string)</i><br><br>"
                "<u>note:</u> for redefine a constant use _<i>def:</i>. The value will be overwritten.</nobr>";
    }

    if ( data == "_floatPrec:" ){

        text = "<nobr><b>Returns the current number precision.</b><br><br></nobr>"
                "<nobr>The precision is the number of digits used to perform a given computation.<br><br>"
                "<i>Default precision:</i> 10</nobr>";
    }

    if ( data == "_setfloatPrec:" ){

        text = "<nobr><b>Sets the number precision.</b><br><br></nobr>"
                "<nobr>The precision is the number of digits used to perform a given computation.<br><br>"
                "<nobr>input expection: <i>_setfloatPrec:(number as positiv integer)</i><br><br>"
                "<i>Default precision:</i> 10</nobr>";
    }


    if ( !text.isEmpty() || !text.isNull() ){
        QToolTip::showText(p , text);
    }
}

void DMathConsoleTextBox::setTextFont(const QFont &font)
{
    nf = font;
    setFont(nf);
    m_completer->popup()->setFont(nf);

    update();
}

DMathConsoleTextBox::DMathConsoleTextBox(QWidget *parent) :
    QTextEdit(parent)
{
    //initialize the constants with its name
    init_constants();


    //trigonometric functions:
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

    //Logarithm & Exponential functions:
    unary_functions["ln"] = log;
    unary_functions["ld"] = logdual;
    unary_functions["lg"] = log10;
    unary_functions["log"] = log;

    unary_functions["exp"] = exp;

    //number converting function:
    unary_functions["abs"] = abs;
    unary_functions["sgn"] = sgn;
    unary_functions["floor"] = floor;
    unary_functions["ceil"] = ceil;
    //unary_functions["round"] = round;
    unary_functions["trunc"] = trunc;

    unary_functions["toDegree"] = degree;
    unary_functions["toRadian"] = radiant;


    //special defined calculations:
    unary_functions["square"] = square;
    unary_functions["cubic"] = cubic;
    unary_functions["sqrt"] = root;
    unary_functions["cbrt"] = cbrt;
    unary_functions["fac"] = factorial;

    //special numbers
    unary_functions["rnd"] = random;
    //unary_functions["fibonacci"] = fibonacci;
    //unary_functions["bernoulli"] = bernoulliNumber;
    //unary_functions["euler"] = sech;

    //secans/cosecans (hyperbolicus)
    unary_functions["sec"] = sec;
    unary_functions["csc"] = csc;
    unary_functions["sech"] = sech;
    unary_functions["csch"] = csch;

    //arcussecans/arcuscosecans
    unary_functions["arcsec"] = arcsec;
    unary_functions["arccsc"] = arccsc;

    //arcus..hyperbolicus
    unary_functions["arcsinh"] = asinh;
    unary_functions["arccosh"] = acosh;
    unary_functions["arctanh"] = atanh;
    unary_functions["arccoth"] = arccoth;
    //unary_functions["arcsech"] = arcsech;//TODO
    //unary_functions["arccsch"] = arccsch;//TODO

    //area trigonometric functions
    unary_functions["arsinh"] = arsinh;
    unary_functions["arcosh"] = arcosh;
    unary_functions["artanh"] = artanh;
    unary_functions["arcoth"] = arcoth;
    unary_functions["arsech"] = arsech;
    unary_functions["arcsch"] = arcsch;

    //signal processing functions
    unary_functions["sinc"] = sinc;
    unary_functions["nsinc"] = normalized_sinc;
    unary_functions["rect"] = rectangular;
    unary_functions["heavyside"] = heavyside;

    //special functions
    unary_functions["sig1"] = sigmoide1;
    unary_functions["sig2"] = sigmoide2;

    unary_functions["gamma"] = tgamma;
    unary_functions["lngamma"] = lgamma;

    unary_functions["erf"] = erf;
    unary_functions["erfc"] = erfc;


    //Outlet functions:
    outlet_functions.append( "_clear:" );
    outlet_functions.append( "_systConst:" );
    outlet_functions.append( "_userDefs:" );
    outlet_functions.append( "_allConstDefs:" );
    outlet_functions.append( "_floatPrec:" );
    outlet_functions.append( "_OUTtoExp:" );
    outlet_functions.append( "_OUTtoFloat:" );


    setCursorWidth(5);
    setTextInteractionFlags( Qt::TextEditable |
                             Qt::TextSelectableByKeyboard |
                             Qt::LinksAccessibleByKeyboard |
                             Qt::TextSelectableByMouse |
                             Qt::TextEditorInteraction
                             );

    setFontWeight( QFont::StyleItalic );
    setTextColor(Qt::blue);

    tc = this->textColor();
    nf = this->font();

    error = false;
    cleared = false;

    this->insertPlainText("[CAL] << ");

    ANS = 0;
    constants["OUT"] = ANS;

    ParamCount = constants.size();
    precision = 10;

    this->setLineWrapMode(QTextEdit::WidgetWidth);
    this->setOverwriteMode(false);
    this->setUndoRedoEnabled(false);
    this->setAcceptRichText(false);
    this->setShortcutEnabled(false);
    this->setAcceptDrops(false);

    this->setContextMenuPolicy(Qt::NoContextMenu);


    c_wasShown = false;

    m_completer = new QCompleter(this);
    m_completer->setWidget(this);

    QStringList m_list;

    m_list << "_allConstDefs:"
           << "_clear:"
           << "_def:"
           << "_floatPrec:"
           << "_OUTtoExp:"
           << "_OUTtoFloat:"
           << "_setfloatPrec:"
           << "_systConst:"
           << "_unDef:"
           << "_userDefs:";


      for ( std::map<std::string, calculation_type>::iterator iter = constants.begin() ; iter != constants.end() ; iter ++ ){
          m_list << QString::fromStdString(iter->first) ;
      }

      for ( std::map<std::string, calculation_type (*) (calculation_type)>::iterator iter = unary_functions.begin() ; iter != unary_functions.end() ; iter ++ ){
          m_list << QString::fromStdString(iter->first) + "()" ;
      }


    QStringListModel * model = new QStringListModel(m_list,m_completer);

    m_completer->setModel(model);
    m_completer->setCaseSensitivity( Qt::CaseSensitive );
    m_completer->setCompletionMode( QCompleter::PopupCompletion );
    m_completer->setCompletionRole( Qt::DisplayRole );
    m_completer->popup()->setFont(font());


    for ( std::map<std::string, calculation_type (*) (calculation_type)>::iterator iter = unary_functions.begin() ; iter != unary_functions.end() ; iter ++ ){
        systFuncList.append( QString::fromStdString(iter->first) );
    }

    for ( int i = 0 ; i < m_list.size() ; i ++ ){
        nonCalcFuncList.append( m_list.at(i) );
    }

    connect( m_completer , SIGNAL(activated(QString)) , this , SLOT(insertCompletion(QString)) );
    connect( m_completer , SIGNAL(highlighted(QString)) , this , SLOT(showToolTip(QString)) );
}

DMathConsoleTextBox::~DMathConsoleTextBox() {}

void DMathConsoleTextBox::keyPressEvent(QKeyEvent *e)
{
    bool comma = false;

    static QString notAllowed("°!§$&=?`´²³ß~';µ@€<>|üÜäÄöÖ\\");


    if ( !notAllowed.contains( e->text().right(1) ) || e->text().isEmpty() ){

        if( e->key() == Qt::Key_Comma ){

            this->insertPlainText(".");
            comma = true;

            e->ignore();
        }

        if ( !comma ){

            //if popup for autocompletion is NOT visible:
            if ( !m_completer->popup()->isVisible() ){

                c_wasShown = false;

                QString result;

                if ( e->key() == Qt::Key_Space ){
                    e->setAccepted(false);
                }

                if ( e->key() ==  Qt::Key_Return ) {

                    this->moveCursor(QTextCursor::MoveOperation::End,QTextCursor::MoveMode::MoveAnchor);


                    QTextBlock block = textCursor().block();

                    QStringList list = block.text().split("<<");

                    QString Outlet_Function;

                    //case for: _def: , _clear: , _userDefs: , systConst: , _allConstDefs:
                    if ( list.size() == 2 ){

                        Outlet_Function = QString(list.at(1)).trimmed();


                        //case for: _clear , _systConst: , userDefs: , allConstDefs:
                        if ( outlet_functions.contains( Outlet_Function ) ){

                            call_outletFunction( Outlet_Function );

                            if ( cleared ){

                                cleared = false;

                                e->ignore();
                            }


                        }

                        //case for: _def: , _setfloatPrec: , _unDef:
                        if ( !outlet_functions.contains( Outlet_Function ) ){


                            QStringList undefSplit = Outlet_Function.split(QString("_unDef:"));
                            QStringList precSplit = Outlet_Function.split( QString("_setfloatPrec:") );
                            QStringList defSplit = Outlet_Function.split(QString("_def:"));



                            if ( defSplit.size() == 2 || precSplit.size() == 2 || undefSplit.size() == 2 ){


                                if ( defSplit.size() == 2 ){

                                    if ( defSplit.at(0).isEmpty() || defSplit.at(0).isNull() || defSplit.at(1).isEmpty() || defSplit.at(1).isNull() ){

                                        throw_error( "[ERR] >> definition failed! \n[ERR] >> expected input is: name(as string)_def:value(as number)\n");

                                    }else{

                                        //hier testen: 1. soll systVariable umbenannt werden ??
                                        if ( systConstList.contains( defSplit.at(0).trimmed() ) || systFuncList.contains( defSplit.at(0).trimmed() ) || nonCalcFuncList.contains( defSplit.at(0).trimmed() ) ){

                                            if ( nonCalcFuncList.contains( defSplit.at(0).trimmed() ) ){
                                                throw_error( "[ERR] >> FORBIDDEN to redefine system function: '" + defSplit.at(0).trimmed() + "'!\n");
                                            }

                                            if ( systConstList.contains( defSplit.at(0).trimmed() ) ){
                                                throw_error( "[ERR] >> FORBIDDEN to redefine system constant: '" + defSplit.at(0).trimmed() + "'!\n");
                                            }

                                            if ( systFuncList.contains( defSplit.at(0).trimmed() ) ){
                                                throw_error( "[ERR] >> FORBIDDEN to redefine mathematical function: '" + defSplit.at(0).trimmed() + "()'!\n");
                                            }


                                        }else{

                                            result = eval_2(defSplit.at(1).trimmed().toStdString().c_str());

                                            bool ok0,ok1;

                                            double check1 = result.toDouble(&ok1);
                                            double check0 = defSplit.at(0).trimmed().toDouble(&ok0);

                                            if ( ok1 == true && ok0 == false && QString::number(check1) != "nan" && QString::number(check1) != "inf" ){

                                                this->setTextColor( QColor(Qt::darkMagenta) );
                                                this->insertPlainText("\n\n[DEF] << " + defSplit.at(0).trimmed() + " = " + result + "\n" );

                                                constants[defSplit.at(0).trimmed().toStdString().c_str()] = result.toDouble();

                                            }else{

                                                throw_error( "[ERR] >> definition failed! \n[ERR] >> expected input is: name(as string)_def:value(as number)\n");
                                            }
                                        }
                                    }
                                }


                                if ( precSplit.size() == 2 ){

                                    result = eval_2(precSplit.at(1).trimmed().toStdString().c_str());

                                    if ( !precSplit.at(1).trimmed().isEmpty() || !precSplit.at(1).trimmed().isNull() ){

                                        bool ok;

                                        precision = abs(precSplit.at(1).trimmed().toInt(&ok));

                                        if ( ok ){

                                            this->setTextColor( QColor(Qt::darkBlue) );
                                            this->insertPlainText("\n\n[PRC] << maximum precision set to: " + QString::number(abs(precSplit.at(1).trimmed().toDouble()),'f',0) + "\n" );

                                        }else{

                                            throw_error( "[ERR] >> setting maximum precision failed!\n[ERR] >> expected input is: _setfloatPrec:value(as positiv integer)\n");
                                        }

                                    }else{

                                        //hier testen ob der wert auch integer ist ?
                                        throw_error( "[ERR] >> setting maximum precision failed!\n[ERR] >> expected input is: _setfloatPrec:value(as positiv integer)\n");
                                    }
                                }


                                if ( undefSplit.size() == 2 ){

                                    if ( (!undefSplit.at(1).trimmed().isEmpty() || !undefSplit.at(1).trimmed().isNull()) && (undefSplit.at(0).trimmed().isNull() || undefSplit.at(0).trimmed().isEmpty()) ){

                                        //testen ob in constants???
                                        if ( in_UserList( undefSplit.at(1).trimmed() ) ){

                                            clearFromConstantList(undefSplit.at(1).trimmed());

                                            this->setTextColor( QColor(Qt::darkGray) );
                                            this->insertPlainText("\n\n[DEF] >> '" + undefSplit.at(1).trimmed() + "' successfully deleted from definition list!\n");

                                        }else{

                                            throw_error( "[ERR] >> no value: '" + undefSplit.at(1).trimmed() + "' in definition list\n");
                                        }

                                    }else{

                                        throw_error( "[ERR] >> undo definition failed!\n[ERR] >> expected input is: _unDef:name(as string)\n");
                                    }
                                }


                                //case for: normal calculation
                            }else{

                                result = eval_2( list[list.size()-1].toStdString().c_str() );

                                if ( error == false ){

                                    if ( result.isEmpty() || result.isNull() || result.contains("ERROR") ){

                                        throw_error( "[ERR] >> noncalculating input done!\n");

                                    }else{

                                        ANS = result.toDouble();
                                        constants["OUT"] = ANS;

                                        this->setTextColor( QColor(Qt::darkGreen) );
                                        this->setFont( nf );

                                        this->insertPlainText("\n\n[OUT] >> " + result + "\n");
                                    }

                                }else{

                                    if ( result.contains("ERROR") ){

                                        throw_error( "[ERR] >> noncalculating input done!\n");

                                    }else{

                                        this->insertPlainText("\n");
                                    }
                                }

                            }
                        }

                    }

                    //case for: problem or error in splitted string
                    if ( list.size() == 1 ){

                        result = eval_2(list[list.size()-1].toStdString().c_str());

                        if ( error == false ){


                            if ( result.isEmpty() || result.isNull() || result.contains("ERROR") ){

                                throw_error( "[ERR] >> noncalculating input done!\n");

                            }else{

                                this->setTextColor( QColor(Qt::darkGreen) );
                                this->setFont( nf );

                                this->insertPlainText("\n\n[OUT] >> " + result + "\n");
                            }

                        }else{

                            if ( result.contains("ERROR") ){

                                throw_error( "[ERR] >> noncalculating input done!\n");

                            }else{

                                this->insertPlainText("\n");
                            }
                        }
                    }
                    QTextEdit::keyPressEvent(e);
                }


                if ( e->key() == Qt::Key_Backspace ){

                    if ( this->textCursor().columnNumber() < 10 || this->textCursor().blockNumber() + 1 != this->document()->blockCount() ){

                    }else{
                        QTextEdit::keyPressEvent(e);
                    }
                }


                if ( e->key() == Qt::Key_Left ){

                    if ( this->textCursor().columnNumber() < 10 ){

                    }else{
                        QTextEdit::keyPressEvent(e);
                    }

                }

                if ( e->key() == Qt::Key_Right ){

                    QTextEdit::keyPressEvent(e);
                }

                if ( e->key() != Qt::Key_Space && e->key() != Qt::Key_Return && e->key() != Qt::Key_Backspace && e->key() != Qt::Key_Left && e->key() != Qt::Key_Up && e->key() != Qt::Key_Right ){

                    if ( this->textCursor().columnNumber() < 9 || this->textCursor().blockNumber() + 1 != this->document()->blockCount() ){

                    }else{
                        QTextEdit::keyPressEvent(e);
                    }

                }
            }//end: is popup NOT visible:


            if ( m_completer->popup()->isVisible() ){

                c_wasShown = true;

                switch( e->key() )
                {

                case Qt::Key_Return:
                case Qt::Key_Escape:
                case Qt::Key_Tab:
                case Qt::Key_Backtab:

                    e->ignore();
                    return;

                default:
                    break;
                }

                QTextEdit::keyPressEvent( e );


            }//end: is visible:


            const QString completionPrefix = textUnderCursor();


            if ( completionPrefix != m_completer->completionPrefix() ){

                m_completer->setCompletionPrefix( completionPrefix );
                m_completer->popup()->setCurrentIndex( m_completer->completionModel()->index(0,0));

                this->showToolTip( m_completer->currentCompletion() );
            }

            if ( !e->text().isEmpty() && completionPrefix.length() >= 1 ){

                QRect cr = cursorRect();

                cr.setWidth(m_completer->popup()->sizeHintForColumn(0)
                            + m_completer->popup()->verticalScrollBar()->sizeHint().width());

                m_completer->complete(cr);

                this->showToolTip( m_completer->currentCompletion() );

            }else{

                m_completer->popup()->setVisible(false);
            }
        }

    }
}

void DMathConsoleTextBox::keyReleaseEvent(QKeyEvent *e)
{

    if ( !m_completer->popup()->isVisible() && c_wasShown == false ){

        if ( e->key() == Qt::Key_Space ){
            e->setAccepted(false);
        }

        if ( e->key() ==  Qt::Key_Return ) {

            if ( error == false ){

                this->setTextColor( tc );
                this->setFont( nf );

                this->insertPlainText("[CAL] << ");

            }else{

                this->setTextColor( tc );
                this->setFont( nf );

                this->insertPlainText("[CAL] << ");

                error = false;
            }

            QTextEdit::keyReleaseEvent(e);
        }

        if ( e->key() != Qt::Key_Space
             && e->key() != Qt::Key_Return
             && e->key() != Qt::Key_Backspace
             && e->key() != Qt::Key_Left ){

            QTextEdit::keyReleaseEvent(e);
        }
    }
}

void DMathConsoleTextBox::insertCompletion(const QString &completion)
{
    QTextCursor cursor = textCursor();


    int extra = completion.length() - m_completer->completionPrefix().length();

    cursor.movePosition( QTextCursor::Left );
    cursor.movePosition( QTextCursor::EndOfWord );

    cursor.insertText( completion.right( extra ) );

    if ( completion.right(1) == ")" ){
        cursor.setPosition(cursor.position()-1);
    }

    setTextCursor( cursor );    
}

QString DMathConsoleTextBox::textUnderCursor() const
{
    QTextCursor cursor = textCursor();

    cursor.select( QTextCursor::WordUnderCursor );

    return cursor.selectedText();
}

void DMathConsoleTextBox::throw_error(const QString &message)
{
    this->setTextColor( QColor( Qt::red ) );

    this->insertPlainText("\n" + message);

    error = true;
}
