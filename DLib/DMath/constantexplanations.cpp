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

#include "constantexplanations.h"

DConstantExplanations::DConstantExplanations() {}
DConstantExplanations::~DConstantExplanations() {}

QString DConstantExplanations::getExplanation(const QString &name) {
    const QString pi = "&pi;";//"π";

    QString data = name;
    QString text = "";

    if ( data == "PI" ){

        text = "<b>constant: Pi [" + pi + "] = " + QString::number(PI,'f',9) + "...</b><br><br>";

    }

    if ( data == "E" ){

        text = "<b>constant: Euler Number [<i>e</i>] = 2.718 281 828...</b>";

    }

    if ( data == "OUT" ){

        text = "<nobr><b>'OUT' returns the recent calculated value.</b></nobr>";
    }

    if ( data == "sin()" ){

        text = "<nobr>Trigonometric function: <i><b>Sine</b></i><br><br></nobr>"
                "<nobr>Computes the 'Sine' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "cos()" ){

        text = "<nobr>Trigonometric function: <i><b>Cosine</b></i><br><br></nobr>"
                "<nobr>Computes the 'Cosine' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "tan()" ){

        text = "<nobr>Trigonometric function: <i><b>Tangent</b></i><br><br></nobr>"
                "<nobr>Computes the 'Tangent' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "cot()" ){

        text = "<nobr>Trigonometric function: <i><b>Cotangent</b></i><br><br></nobr>"
                "<nobr>Computes the 'Cotangent' of an angle in Radian [<i>rad</i>].</nobr>";
    }


    if ( data == "sinh()" ){

        text = "<nobr>Hyperbolic trigonometric function: <i><b>Hyperbolic Sine</b></i><br><br></nobr>"
                "<nobr>Computes the 'Hyperbolic Sine' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "cosh()" ){

        text = "<nobr>Hyperbolic trigonometric function: <i><b>Hyperbolic Cosine</b></i><br><br></nobr>"
                "<nobr>Computes the 'Hyperbolic Cosine' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "tanh()" ){

        text = "<nobr>Hyperbolic trigonometric function: <i><b>Hyperbolic Tangent</b></i><br><br></nobr>"
                "<nobr>Computes the 'Hyperbolic Tangent' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "coth()" ){

        text = "<nobr>Hyperbolic trigonometric function: <i><b>Hyperbolic Cotangent</b></i><br><br></nobr>"
                "<nobr>Computes the 'Hyperbolic Cotangent' of an angle in Radian [<i>rad</i>].</nobr>";
    }


    if ( data == "arcsin()" ){

        text = "<nobr>Inverse trigonometric function: <i><b>Arc Sine or inverse Sine</b></i><br><br></nobr>"
                "<nobr>Computes the 'Arc Sine' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arccos()" ){

        text = "<nobr>Inverse trigonometric function: <i><b>Arc Cosine or inverse Cosine</b></i><br><br></nobr>"
                "<nobr>Computes the 'Arc Cosine' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arctan()" ){

        text = "<nobr>Inverse trigonometric function: <i><b>Arc Tangent or inverse Tangent</b></i><br><br></nobr>"
                "<nobr>Computes the 'Arc Tangent' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arccot()" ){

        text = "<nobr>Inverse trigonometric function: <i><b>Arc Cotangent or inverse Cotangent</b></i><br><br></nobr>"
                "<nobr>Computes the 'Arc Cotangent' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "sec()" ){

        text = "<nobr>Trigonometric function: <i><b>Secant</b></i><br><br></nobr>"
                "<nobr>Computes the 'Secant' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "csc()" ){

        text = "<nobr>Trigonometric function: <i><b>Cosecant</b></i><br><br></nobr>"
                "<nobr>Computes the 'Cosecant' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "sech()" ){

        text = "<nobr>Hyperbolic trigonometric function: <i><b>Hyperbolic Secant</b></i><br><br></nobr>"
                "<nobr>Computes the 'Hyperbolic Secant' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "csch()" ){

        text = "<nobr>Hyperbolic trigonometric function: <i><b>Hyperbolic Cosecant</b></i><br><br></nobr>"
                "<nobr>Computes the 'Hyperbolic Cosecant' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arcsinh()" ){

        text = "<nobr>Inverse hyperbolic trigonometric function: <i><b>Arc Hyperbolic Sine or Inverse Hyperbolic Sine</b></i><br><br></nobr>"
                "<nobr>Computes the 'Arc Hyperbolic Sine' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arccosh()" ){

        text = "<nobr>Inverse hyperbolic trigonometric function: <i><b>Arc Hyperbolic Cosine or Inverse Hyperbolic Cosine</b></i><br><br></nobr>"
                "<nobr>Computes the 'Arc Hyperbolic Cosine' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arctanh()" ){

        text = "<nobr>Inverse hyperbolic trigonometric function: <i><b>Arc Hyperbolic Tangent or Inverse Hyperbolic Tangent</b></i><br><br></nobr>"
                "<nobr>Computes the 'Arc Hyperbolic Tangent' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arccoth()" ){

        text = "<nobr>Inverse hyperbolic trigonometric function: <i><b>Arc Hyperbolic Cotangent or Inverse Hyperbolic Cotangent</b></i><br><br></nobr>"
                "<nobr>Computes the 'Arc Hyperbolic Cotangent' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arcsech()" ){

        text = "<nobr>Inverse hyperbolic trigonometric function: <i><b>Arc Hyperbolic Secant or Inverse Hyperbolic Secant</b></i><br><br></nobr>"
                "<nobr>Computes the 'Arc Hyperbolic Secant' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arccsch()" ){

        text = "<nobr>Inverse hyperbolic trigonometric function: <i><b>Arc Hyperbolic Cosecant or Inverse Hyperbolic Cosecant</b></i><br><br></nobr>"
                "<nobr>Computes the 'Arc Hyperbolic Cosecant' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arsinh()" ){

        text = "<nobr>Inverse hyperbolic trigonometric function: <i><b>Area Hyperbolic Sine</b></i><br><br></nobr>"
                "<nobr>Computes the 'Area Hyperbolic Sine' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arcosh()" ){

        text = "<nobr>Inverse hyperbolic trigonometric function: <i><b>Area Hyperbolic Cosine</b></i><br><br></nobr>"
                "<nobr>Computes the 'Area Hyperbolic Cosine' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "artanh()" ){

        text = "<nobr>Inverse hyperbolic trigonometric function: <i><b>Area Hyperbolic Tangent</b></i><br><br></nobr>"
                "<nobr>Computes the 'Area Hyperbolic Tangent' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arcoth()" ){

        text = "<nobr>Inverse hyperbolic trigonometric function: <i><b>Area Hyperbolic Cotangent</b></i><br><br></nobr>"
                "<nobr>Computes the 'Area Hyperbolic Cotangent' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arsech()" ){

        text = "<nobr>Inverse hyperbolic trigonometric function: <i><b>Area Hyperbolic Secant</b></i><br><br></nobr>"
                "<nobr>Computes the 'Area Hyperbolic Secant' of an angle in Radian [<i>rad</i>].</nobr>";
    }

    if ( data == "arcsch()" ){

        text = "<nobr>Inverse hyperbolic trigonometric function: <i><b>Area Hyperbolic Cosecant</b></i><br><br></nobr>"
                "<nobr>Computes the 'Area Hyperbolic Cosecant' of an angle in Radian [<i>rad</i>].</nobr>";
    }


    if ( data == "log()" ){

        text = "<nobr>Logarithmic function: <i><b>Natural Logarithm log<sub>e</sub>(x)</b></i><br><br></nobr>"
                "<nobr>Computes the Logarithm to base <i>e (see: EULER)</i>.<br><br>"
                "see also: <i>ln()</i></nobr>";
    }

    if ( data == "ln()" ){

        text = "<nobr>Logarithmic function: <i><b>Natural Logarithm log<sub>e</sub>(x) &equiv; ln(x)</b></i><br><br></nobr>"
                "<nobr>Computes the Logarithm to base <i>e (see: EULER)</i>.<br><br>"
                "see also: <i>log()</i></nobr>";
    }

    if ( data == "ld()" ){

        text = "<nobr>Logarithmic function: <i><b>Binary Logarithm log<sub>2</sub>(x) &equiv; ld(x)</b></i><br><br></nobr>"
                "<nobr>Computes the Logarithm to base <i>2</i>.</nobr>";

    }

    if ( data == "lg()" ){

        text = "<nobr>Logarithmic function: <i><b>Common Logarithm log<sub>10</sub>(x) &equiv; lg(x)</b></i><br><br></nobr>"
                "<nobr>Computes the Logarithm to base <i>10</i>.</nobr>";

    }

    if ( data == "exp()" ){

        text = "<nobr>Exponential function: <i><b>e<sup>x</sup></b></i><br><br></nobr>"
                "<nobr>Computes the inverse function of the 'Natural Logarithm' <i>(see: log()/ln())</i>.</nobr>";;
    }

    if ( data == "abs()" ){

        text = "<nobr>Mathematical function: <i><b>Absolute Value or Modulus</b></i><br><br></nobr>"
                "<nobr>The absolute value of a real number is non negative.</nobr>";
    }

    if ( data == "sgn()" ){

        text = "<nobr>Odd mathematical function: <i><b>Sign Function or Signum Function</i> sgn(x)</b></nobr>";
    }

    if ( data == "floor()" ){

        text = "<nobr>Computational function: <i><b>Floor Function or Greatest Integer Function floor(x)</b></i><br><br></nobr>"
                "<nobr>Maps a real number to the largest previous integer.</nobr>";
    }

    if ( data == "ceil()" ){

        text = "<nobr>Computational function: <i><b>Ceiling Function ceil(x)</b></i><br><br></nobr>"
                "<nobr>Maps a real number to the smallest following integer.</nobr>";
    }

    if ( data == "trunc()" ){

        text = "<nobr>Computational function: <i><b>Truncation cuts off the digits of a real number right of the decimal point.</b></i></nobr>";
    }

    if ( data == "toDegree()" ){

        text = "<nobr>Converting function: <b><i>Converts an angle from Radian</i> [rad] <i>to Degree</i> [&deg;].</b><br><br></nobr>"
                "<nobr>see also: <i>toRadian()</nobr>";
    }

    if ( data == "toRadian()" ){

        text = "<nobr>Converting function: <b><i>Converts an angle from Degree</i> [&deg;] <i>to Radian</i> [rad].</b><br><br></nobr>"
                "<nobr>see also: <i>toDegree()</nobr>";
    }

    if ( data == "square()" ){

        text = "<nobr>Mathematical function: <b><i>Calculates the real number n to the power of 2</i> ( n<sup>2</sup> )</b><br><br></nobr>"
                "<nobr>see also: <i>sqrt()</nobr>";
    }

    if ( data == "cubic()" ){

        text = "<nobr>Mathematical function: <b><i>Calculates the real number n to the power of 3</i> ( n<sup>3</sup> )</b><br><br></nobr>"
                "<nobr>see also: <i>cbrt()</nobr>";
    }

    if ( data == "sqrt()" ){

        text = "<nobr>Mathematical function: <b><i>Calculates the squareroot of a real number n</i> ( &radic;n )</b><br><br></nobr>"
                "<nobr><u>note:</u> you can also use the symbol '&#35;' instead of 'sqrt()'<br><br>"
                "see also: <i>square()</nobr>";
    }

    if ( data == "cbrt()" ){

        text = "<nobr>Mathematical function: <b><i>Calculates the cubicroot of a real number n</i> ( n<sup>1/3</sup> )</b><br><br></nobr>"
                "see also: <i>cubic()</nobr>";
    }

    if ( data == "fac()" ){

        text = "<nobr>Statistic function: <b><i>Calculates the factorial</i> (!) <i>of a real number n</i></b><br><br></nobr>"
                "<nobr><u>note:</u> !n &equiv; &Gamma;(n + 1)<br><br>"
                "see also: <i>gamma()</nobr>";
    }

    if ( data == "rnd()" ){

        text = "<nobr>Computational function: <b><i>Computes a random number between 0 and n </i> rnd[n]</b></nobr>";
    }

    if ( data == "sinc()" ){

        text = "<nobr>Signal processing function: <b><i>Sinc function</i> sinc(x)</b><br><br></nobr>"
                "<nobr>mathematical definition: <i>sin(x)/x</i><br><br>"
                "see also: <i>nsinc()</i></nobr>";
    }

    if ( data == "nsinc()" ){

        text = "<nobr>Signal processing function: <b><i>Normalized Sinc function</i> sinc(x)</b><br><br></nobr>"
                "<nobr>mathematical definition: <i>sin(" + pi + " x)/" + pi + " x</i><br><br>"
                "see also: <i>sinc()</i></nobr>";
    }

    if ( data == "rect()" ){

        text = "<nobr>Signal processing function: <b><i>Rectangular function or Normalized Boxcar function</i> &prod;(x)</b><br><br></nobr>"
                "see also: <i>heavyside()</i></nobr>";
    }

    if ( data == "heavyside()" ){

        text = "<nobr>Signal processing function: <b><i>Heavyside function or Unit Step function</i> &theta;(x)</b><br><br></nobr>"
                "see also: <i>rect()</i></nobr>";
    }

    if ( data == "gamma()" ){

        text = "<nobr>Factorial function: <b><i>Gamma function</i> &Gamma;(x)</b><br><br></nobr>"
                "see also: <i>lngamma()</i> and <i>fac()</i></nobr>";
    }

    if ( data == "lngamma()" ){

        text = "<nobr>Computational factorial function: <b><i>Natural Logarithm Gamma function</i> ln(&Gamma;(x))</b><br><br></nobr>"
                "see also: <i>gamma()</i> and <i>fac()</i></nobr>";
    }

    if ( data == "erf()" ){

        text = "<nobr>Statistic/Sigmoid function: <b><i>Error function or Gauss Error function</i> erf(x)</b><br><br></nobr>"
                "see also: <i>erfc(),sig1()</i> and <i>sig2()</i></nobr>";
    }

    if ( data == "erfc()" ){

        text = "<nobr>Statistic/Sigmoid function: <b><i>Complementary Error function or Complementary Gauss Error function</i> erfc(x)</b><br><br></nobr>"
                "see also: <i>erf(),sig1()</i> and <i>sig2()</i></nobr>";
    }

    if ( data == "sig1()" ){

        text = "<nobr>Sigmoid function: <b><i>Logistic function</i> S(x)</b><br><br></nobr>"
                "<nobr>mathematical definition: <i>S(x) = (e<sup>-x</sup> + 1)<sup>-1</sup></i><br><br>"
                "see also: <i>erf(),tanh()</i> and <i>sig2()</i></nobr>";
    }

    if ( data == "sig2()" ){

        text = "<nobr>Sigmoid function: <b><i>Algebraic function</i></b><br><br></nobr>"
                "<nobr>mathematical definition: <i>f(x) = x (&radic;(x<sup>2</sup> + 1))<sup>-1</sup></i><br><br>"
                "see also: <i>erf(),tanh()</i> and <i>sig1()</i></nobr>";
    }


    return (text);
}

QString DConstantExplanations::expo(int exponent)
{
    return "10<sup>" + QString::number(exponent,'f',0) + "</sup>";
}
