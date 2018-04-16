/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2018 Danny Petschke
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

#include "constantexplanations.h"

DConstantExplanations::DConstantExplanations() {}
DConstantExplanations::~DConstantExplanations() {}

QString DConstantExplanations::getExplanation(const QString &name)
{
    const QString alpha = "&alpha;";//"α";
    const QString beta = "&beta;";//"β";
    const QString gamma = "&gamma;";//"γ";
    const QString delta = "&delta;";//"δ";
    const QString epsilon = "&epsilon;";//"ε";
    const QString zeta = "&zeta;";//"ζ";
    const QString eta = "&eta;";//"η";
    const QString theta = "&theta;";//"θ";
    const QString iota = "&iota;";//"ι";
    const QString kappa = "&kappa;";//"κ";
    const QString lambda = "&lambda;";//"λ";
    const QString mu = "&mu;";//"μ";
    const QString nu = "&nu;";//"ν";
    const QString xi = "&xi;";//"ξ";
    const QString omicron = "&omicron;";//"ο";
    const QString pi = "&pi;";//"π";
    const QString rho = "&rho;";//"ρ";
    const QString sigma = "&sigma;";//"σ";
    const QString tau = "&tau;";//"τ";
    const QString upsilon = "&upsilon;";//"υ";
    const QString phi = "&phi;";//"φ";
    const QString chi = "&chi;";//"χ";
    const QString psi = "&psi;";//"ψ";
    const QString omega = "&omega;";//"ω";
    const QString omega_up = "&Omega;";//"Ω";

    const QString h_stroke = "&#295;"; //"ħ";

    QString data = name;
    QString text = "";

    //nature constants:
    //**************************************************************************************

    if ( data == "PI" ){

        text = "<b>constant: Pi [" + pi + "] = " + QString::number(PI,'f',9) + "...\n\n</b><br><br>"
               "<nobr>The number " + pi + " is a mathematical constant that is\n<br>" +
                     "the ratio of a circle's circumference to its diameter.</nobr>";

    }

    if ( data == "PLANCK_H1" ){

        text = "<b>constant: Planck Relation [h] = 4.135 667... " + expo(-15) + " [eVs]\n\n</b><br><br>" +
               "<nobr>The 'Planck Constant' is a physical constant that is the quantum of action in quantum mechanics.\n<br>" +
                     "It describes the proportionality between the photon energy and the frequency of its associated<br>" +
                     "electromagnetic wave.</nobr>";

    }

    if ( data == "PLANCK_H2" ){

        text = "<b>constant: Planck Relation [h] = 6.626 069... " + expo(-34) + " [Js]</b><br><br>" +
               "<nobr>The 'Planck Constant' is a physical constant that is the quantum of action in quantum mechanics.\n<br>" +
                     "It describes the proportionality between the photon energy and the frequency of its associated<br>" +
                     "electromagnetic wave.</nobr>";

    }

    if ( data == "E" ){

        text = "<b>constant: Euler Number [<i>e</i>] = 2.718 281 828...</b><br><br>"
               "<nobr>The Euler Number is an mathematical constant that is the base of the natural logarithm [log()].</nobr>";

    }

    if ( data == "PLANCK_RH1" ){

        text = "<b>constant: Reduced Planck Relation (or 'Dirac Constant') [&#295;] = 6.582 119... " + expo(-16) + " [eV s]\n\n</b><br><br>" +
               "<nobr>The 'Reduced Planck Constant' is equal to the 'Planck Relation' [h] and describes the proportionality\n<br>" +
                     "between the energy of a photon and the frequency of its associated electromagnetic wave.\n\n<br><br>" +
                     "Mathematical relationship to 'Planck Relation': " + "&#295;" + " = " + "h/2" + pi + "\n\n<br><br>" +
                     "<i>see also: PLANCK_H1/PLANCK_H2</i></nobr>";

    }

    if ( data == "PLANCK_RH2" ){

        text = "<b>constant: Reduced Planck Relation (or 'Dirac Constant') [&#295;] = 1.054 571... " + expo(-34) + " [J s]\n\n</b><br><br>" +
               "<nobr>The 'Reduced Planck Constant' is equal to the 'Planck Relation' [h] and describes the proportionality\n<br>" +
                     "between the energy of a photon and the frequency of its associated electromagnetic wave.\n\n<br><br>" +
                     "Mathematical relationship to 'Planck Relation': " + "&#295;" + " = " + "h/2" + pi + "\n\n<br><br>" +
                     "<i>see also: PLANCK_H1/PLANCK_H2</i></nobr>";

    }

    if ( data == "K_BOLTZMANN_1" ){

        text = "<nobr><b>constant: Boltzmann Constant [k<sub>B</sub> or k] = 8.617 332... " + expo(-5) + " [eV/K]</b><br><br></nobr>" +
                  "<nobr>The 'Boltzmann Constant' is a physical constant relating energy at the individual particle level with temperature.<br>" +
                        "It is the ratio between the 'Gas Constant' [R] <i>(see: GAS_CONST)</i> and the 'Avogadro constant' [N<sub>A</sub>] <i>(see: AVOGADRO)</i>.</nobr>" ;

    }

    if ( data == "K_BOLTZMANN_2" ){

        text = "<nobr><b>constant: Boltzmann Constant [k<sub>B</sub> or k] = 1.380 648... " + expo(-23) + " [J/K]</b><br><br></nobr>" +
                  "<nobr>The 'Boltzmann Constant' is a physical constant relating energy at the individual particle level with temperature.<br>" +
                        "It is the ratio between the 'Gas Constant' [R] <i>(see: GAS_CONST)</i> and the 'Avogadro constant' [N<sub>A</sub>] <i>(see: AVOGADRO)</i>.</nobr>" ;

    }

    if ( data == "K_BOLTZMANN_3" ){

        text = "<nobr><b>constant: Boltzmann Constant [k<sub>B</sub> or k] = 2.083 661... " + expo(10) + " [Hz/K]</b><br><br></nobr>" +
                  "<nobr>The 'Boltzmann Constant' is a physical constant relating energy at the individual particle level with temperature.<br>" +
                        "It is the ratio between the 'Gas Constant' [R] <i>(see: GAS_CONST)</i> and the 'Avogadro constant' [N<sub>A</sub>] <i>(see: AVOGADRO)</i>.</nobr>" ;

    }

    if ( data == "STEFAN_BOLTZMANN" ){

        text = "<nobr><b>constant: Stefan&ndash;Boltzmann Constant [" + sigma + "] = 5.670 373... " + expo(-8) + " [W/m<sup>-2</sup>K<sup>-4</sup>]</b><br><br></nobr>" +
                  "<nobr>The 'Stefan Boltzmann Constant' is a physical constant and describes the direct proportionality<br>" +
                        "between the total emitted energy of the black body to the 4th power of its temperature.<br><br>" +
                        "Derivatives from other constants of nature: <i>" + sigma + " = (2/15) " + pi + "<sup>5</sup>" + " k<sub>B</sub><sup>4</sup>" + " c<sup>-2</sup> h<sup>-3</sup></i>" + "<br><br>" +
                        "<i>" + pi + "</i> is Pi <i>(see: PI)</i><br>" +
                        "<i>k<sub>B</sub></i> is Boltzmann Constant <i>(see: K_BOLTZMANN_1 to K_BOLTZMANN_3)</i><br>" +
                        "<i>c</i> is Lightspeed <i>(see: LIGHTSPEED)</i><br>" +
                        "<i>h</i> is Planck Relation <i>(see: PLANCK_H1/PLANCK_H2)</i>" +
                        "</nobr>" ;

    }

    if ( data == "GOLDENRATIO" ){

        double value = ( 1 + sqrt(5) ) / 2;

        text = "<nobr><b>constant: Golden Ratio [" + phi + "] = " + QString::number(value,'f',9) + "...</b><br><br></nobr>" +
                 "<nobr>In mathematics and the arts, two quantities are in the 'Golden Ratio'<br>" +
                       "if the ratio of the sum <i>(a + b)</i> of the quantities to the larger quantity <i>(a)</i><br>" +
                       "is equal to the ratio of the larger quantity <i>(a)</i> to the smaller one <i>(b)</i>.<br><br>" +
                       "Golden Ration: <i>(a + b)/a = a/b</i> &equiv; " + phi + " &equiv; (1 + &radic;5)/2 </nobr>";

    }

    if ( data == "ELECTRONM" ){

        text = "<nobr><b>constant: Mass of Electron [m<sub>0</sub> or m<sub>e</sub>] = 9.109 382... " + expo(-31) + " [kg]</b><br><br></nobr>" +
                "<nobr>in atomic mass unit <i>(see: ATOMIC_UNIT)</i>: 5.485799... " + expo(-4) + " u<br><br>" +
                      "for 'Electron Charge' see: <i>ELECTRONC</i><br>" +
                      "for 'Electron g&ndash;Factor' see: <i>ELECTRONG</i></nobr>";

    }

    if ( data == "PROTONM" ){

        text = "<nobr><b>constant: Mass of Proton [m<sub>p</sub>] = 1.672 62... " + expo(-27) + " [kg]</b><br><br></nobr>" +
                "<nobr>in atomic mass unit <i>(see: ATOMIC_UNIT)</i>: 1.007276466812... u<br><br>" +
                      "for 'Proton Charge' see: <i>PROTONC</i><br>" +
                      "for 'Proton g&ndash;Factor' see: <i>PROTONG</i></nobr>";

    }

    if ( data == "NEUTRONM" ){

        text = "<nobr><b>constant: Mass of Neutron [m<sub>n</sub>] = 1.674 927... " + expo(-27) + " [kg]</b><br><br></nobr>" +
                "<nobr>in atomic mass unit <i>(see: ATOMIC_UNIT)</i>: 1.00866491600... u<br><br>" +
                      "for 'Neutron Charge' see: <i>NEUTRONC</i><br>" +
                      "for 'Neutron g&ndash;Factor' see: <i>NEUTRONG</i></nobr>";

    }

    if ( data == "MUONM" ){

        text = "<nobr><b>constant: Mass of Muon [m<sub>m-</sub>] = 1.883 531... " + expo(-28) + " [kg]</b><br><br></nobr>" +
                "<nobr>in atomic mass unit <i>(see: ATOMIC_UNIT)</i>: 1.00866491600... u<br><br>" +
                      "for 'Muon Charge' see: <i>MUONC</i><br>" +
                      "for 'Muon g&ndash;Factor' see: <i>MUONG</i></nobr>";

    }

    if ( data == "ANTIMUONM" ){

        text = "<nobr><b>constant: Mass of Antimuon [m<sub>m+</sub>] = 1.883 531... " + expo(-28) + " [kg]</b><br><br></nobr>" +
                "<nobr>in atomic mass unit <i>(see: ATOMIC_UNIT)</i>: 1.00866491600... u<br><br>" +
                      "for 'Antimuon Charge' see: <i>ANTIMUONC</i><br>" +
                      "for 'Antimuon g&ndash;Factor' see: <i>ANTIMUONG</i></nobr>";

    }

    if ( data == "PIONM" ){

        text = "<nobr><b>constant: Mass of Pion [m<sub>" + pi + "</sub><sup>+</sup>] = 2.488... " + expo(-28) + " [kg]</b><br><br></nobr>" +
                "<nobr>"
                      "for 'Antipion Mass' see: <i>ANTIPIONM</i><br>" +
                      "for 'Neutralpion Mass' see: <i>NEUTRALPIONM</i></nobr>";

    }

    if ( data == "ANTIPIONM" ){

        text = "<nobr><b>constant: Mass of Antipion [m<sub>" + pi + "</sub><sup>-</sup>] = 2.488... " + expo(-28) + " [kg]</b><br><br></nobr>" +
                "<nobr>"
                      "for 'Pion Mass' see: <i>PIONM</i><br>" +
                      "for 'Neutralpion Mass' see: <i>NEUTRALPIONM</i></nobr>";

    }

    if ( data == "NEUTRALPIONM" ){

        text = "<nobr><b>constant: Mass of Neutralpion [m<sub>" + pi + "</sub><sup>0</sup>] = 2.406... " + expo(-28) + " [kg]</b><br><br></nobr>" +
                "<nobr>"
                      "for 'Pion Mass' see: <i>PIONM</i><br>" +
                      "for 'Antipion Mass' see: <i>ANTIPIONM</i></nobr>";

    }

    if ( data == "ELECTRONG" ){

        text = "<nobr><b>constant: Electron g&ndash;Factor (also: Land&eacute;&ndash;Factor) [g<sub>e</sub>] = -2.002 319 304... </b><br><br></nobr>"
                "<nobr>In general the g Factor is a dimensionless quantity which characterizes the<br>"
                "magnetic moment and gyromagnetic ratio of a particle or nucleus.<br><br>"
                "for 'Proton g&ndash;Factor' see: <i>PROTONG</i><br>"
                "for 'Neutron g&ndash;Factor' see: <i>NEUTRONG</i><br>"
                "for 'Muon g&ndash;Factor' see: <iMUONG</i><br>"
                "for 'Antimuon g&ndash;Factor' see: <i>ANTIMUONG</i>"
                "</nobr>";

    }

    if ( data == "PROTONG" ){

        text = "<nobr><b>constant: Proton g&ndash;Factor (also: Land&eacute;&ndash;Factor) [g<sub>p</sub>] = 5.585 694 713... </b><br><br></nobr>"
                "<nobr>In general the g Factor is a dimensionless quantity which characterizes the<br>"
                "magnetic moment and gyromagnetic ratio of a particle or nucleus.<br><br>"
                "for 'Electron g&ndash;Factor' see: <i>ELECTRONG</i><br>"
                "for 'Neutron g&ndash;Factor' see: <i>NEUTRONG</i><br>"
                "for 'Muon g&ndash;Factor' see: <i>MUONG</i><br>"
                "for 'Antimuon g&ndash;Factor' see: <i>ANTIMUONG</i>"
                "</nobr>";

    }

    if ( data == "NEUTRONG" ){

        text = "<nobr><b>constant: Neutron g&ndash;Factor (also: Land&eacute;&ndash;Factor) [g<sub>n</sub>] = -3.826 085 45... </b><br><br></nobr>"
                "<nobr>In general the g Factor is a dimensionless quantity which characterizes the<br>"
                "magnetic moment and gyromagnetic ratio of a particle or nucleus.<br><br>"
                "for 'Electron g&ndash;Factor' see: <i>ELECTRONG</i><br>"
                "for 'Proton g&ndash;Factor' see: <i>PROTONG</i><br>"
                "for 'Muon g&ndash;Factor' see: <i>MUONG</i><br>"
                "for 'Antimuon g&ndash;Factor' see: <i>ANTIMUONG</i>"
                "</nobr>";

    }

    if ( data == "MUONG" ){

        text = "<nobr><b>constant: Muon g&ndash;Factor (also: Land&eacute;&ndash;Factor) [g<sub>m-</sub>] = -2.002 331 841... </b><br><br></nobr>"
                "<nobr>In general the g Factor is a dimensionless quantity which characterizes the<br>"
                "magnetic moment and gyromagnetic ratio of a particle or nucleus.<br><br>"
                "for 'Electron g&ndash;Factor' see: <i>ELECTRONG</i><br>"
                "for 'Proton g&ndash;Factor' see: <i>PROTONG</i><br>"
                "for 'Neutron g&ndash;Factor' see: <i>NEUTRONG</i><br>"
                "for 'Antimuon g&ndash;Factor' see: <i>ANTIMUONG</i>"
                "</nobr>";

    }

    if ( data == "ANTIMUONG" ){

        text = "<nobr><b>constant: Antimuon g&ndash;Factor (also: Land&eacute;&ndash;Factor) [g<sub>m+</sub>] = -2.002 331 841... </b><br><br></nobr>"
                "<nobr>In general the g Factor is a dimensionless quantity which characterizes the<br>"
                "magnetic moment and gyromagnetic ratio of a particle or nucleus.<br><br>"
                "for 'Electron g&ndash;Factor' see: <i>ELECTRONG</i><br>"
                "for 'Proton g&ndash;Factor' see: <i>PROTONG</i><br>"
                "for 'Neutron g&ndash;Factor' see: <i>NEUTRONG</i><br>"
                "for 'Muon g&ndash;Factor' see: <i>MUONG</i>"
                "</nobr>";

    }

    if ( data == "ELECTRONC" ){

        text = "<nobr><b>constant: Electron Charge (or 'Elementary Charge') [<i>e</i>] = 1.602 176... " + expo(-19) + " [C]</b><br><br></nobr>" +
                "<nobr>This elementary charge is a fundamental physical constant.<br><br>" +

                      "for 'Proton charge' see: <i>PROTONC</i><br>" +
                      "for 'Neutron charge' see: <i>NEUTRONC</i><br>" +
                "for 'Muon charge' see: <i>MUONC</i><br>"
                "for 'Antimuon charge' see: <i>ANTIMUONC</i><br>"
                "for 'Pion charge' see: <i>PIONC</i><br>"
                "for 'Antipion charge' see: <i>ANTIPIONC</i>"
                "</nobr>";

    }

    if ( data == "PROTONC" ){

        text = "<nobr><b>constant: Proton Charge (or 'Elementary Charge') [<i>e</i>] = 1.602 176... " + expo(-19) + " [C]</b><br><br></nobr>" +
                "<nobr>This elementary charge is a fundamental physical constant.<br><br>" +

                      "for 'Electron charge' see: <i>ELECTRONC</i><br>" +
                      "for 'Neutron charge' see: <i>NEUTRONC</i><br>" +

                "for 'Muon charge' see: <i>MUONC</i><br>"
                "for 'Antimuon charge' see: <i>ANTIMUONC</i><br>"
                "for 'Pion charge' see: <i>PIONC</i><br>"
                "for 'Antipion charge' see: <i>ANTIPIONC</i>"
                      "</nobr>";

    }

    if ( data == "MUONC" ){

        text = "<nobr><b>constant: Muon Charge (or 'Elementary Charge') [<i>e</i>] = 1.602 176... " + expo(-19) + " [C]</b><br><br></nobr>" +
                "<nobr>This elementary charge is a fundamental physical constant and is equal to the 'Electron Charge'.<br><br>" +

                      "for 'Electron charge' see: <i>ELECTRONC</i><br>" +
                      "for 'Proton charge' see: <i>PROTONC</i><br>" +
                      "for 'Neutron charge' see: <i>NEUTRONC</i><br>"
                "for 'Antimuon charge' see: <i>ANTIMUONC</i><br>"
                "for 'Pion charge' see: <i>PIONC</i><br>"
                "for 'Antipion charge' see: <i>ANTIPIONC</i>"

                "</nobr>";

    }

    if ( data == "ANTIMUONC" ){

        text = "<nobr><b>constant: Antimuon Charge (or 'Elementary Charge') [<i>e</i>] = 1.602 176... " + expo(-19) + " [C]</b><br><br></nobr>" +
                "<nobr>This elementary charge is a fundamental physical constant and is equal to the 'Electron Charge'.<br><br>" +

                      "for 'Electron charge' see: <i>ELECTRONC</i><br>" +
                      "for 'Proton charge' see: <i>PROTONC</i><br>" +
                      "for 'Neutron charge' see: <i>NEUTRONC</i><br>"
                      "for 'Muon charge' see: <i>MUONC</i><br>"
                      "for 'Pion charge' see: <i>PIONC</i><br>"
                      "for 'Antipion charge' see: <i>ANTIPIONC</i>"

                "</nobr>";

    }

    if ( data == "NEUTRONC" ){

        text = "<nobr><b>constant: Neutron Charge is exactly <i>0</i> [C]</b><br><br></nobr>"
                      "<nobr>for 'Electron charge' see: <i>ELECTRONC</i><br>"
                      "for 'Proton charge' see: <i>PROTONC</i><br>"
                "for 'Muon charge' see: <i>MUONC</i><br>"
                "for 'Antimuon charge' see: <i>ANTIMUONC</i><br>"
                "for 'Pion charge' see: <i>PIONC</i><br>"
                "for 'Antipion charge' see: <i>ANTIPIONC</i>"
                      "</nobr>";

    }

    if ( data == "PIONC" ){

        text = "<nobr><b>constant: Pion Charge (or 'Elementary Charge') [<i>e</i>] = 1.602 176... " + expo(-19) + " [C]</b><br><br></nobr>" +
                "<nobr>This elementary charge is a fundamental physical constant and is equal to the 'Electron Charge'.<br><br>" +

                      "for 'Electron charge' see: <i>ELECTRONC</i><br>" +
                      "for 'Proton charge' see: <i>PROTONC</i><br>" +
                      "for 'Neutron charge' see: <i>NEUTRONC</i><br>"
                      "for 'Muon charge' see: <i>MUONC</i><br>"
                      "for 'Antimuon charge' see: <i>ANTIMUONC</i><br>"
                      "for 'Antipion charge' see: <i>ANTIPIONC</i>"

                "</nobr>";

    }

    if ( data == "ANTIPIONC" ){

        text = "<nobr><b>constant: Antipion Charge (or 'Elementary Charge') [<i>e</i>] = 1.602 176... " + expo(-19) + " [C]</b><br><br></nobr>" +
                "<nobr>This elementary charge is a fundamental physical constant and is equal to the 'Electron Charge'.<br><br>" +

                      "for 'Electron charge' see: <i>ELECTRONC</i><br>" +
                      "for 'Proton charge' see: <i>PROTONC</i><br>" +
                      "for 'Neutron charge' see: <i>NEUTRONC</i><br>"
                      "for 'Muon charge' see: <i>MUONC</i><br>"
                      "for 'Antimuon charge' see: <i>ANTIMUONC</i><br>"
                      "for 'Pion charge' see: <i>PIONC</i>"

                "</nobr>";

    }

    if ( data == "LIGHTSPEED" ){

        text = "<nobr><b>constant: Speed of Light in vacuum [<i>c</i>] is exactly 299 792 458 [m s<sup>-1</sup>]</b></nobr>";
    }

    if ( data == "GRAVITYACC" ){

        text = "<nobr><b>constant: Earth&lsquo;s standard acceleration due to Gravity [<i>g</i>] = 9.806 65... [m s<sup>-2</sup>]</b><br><br></nobr>"
                "<nobr>see also: <i>GRAVITYCONST</i></nobr>";
    }

    if ( data == "GRAVITYCONST" ){

        text = "<nobr><b>constant: Gravitational Constant [<i>G</i>] = 6.673 84... " + expo(-11) + " [m<sup>3</sup> kg<sup>-1</sup> s<sup>-2</sup>] or [N m<sup>2</sup> s<sup>2</sup>]</b><br><br></nobr>"
                "<nobr>According to the law of universal gravitation, the attractive force (F)<br>"
                      "between two bodies is proportional to the product of their masses (m<sub>1</sub> and m<sub>2</sub>),<br>"
                      "and inversely proportional to the square of the distance r between them.<br>"
                      "The 'Gravitational Constant' is constant of proportionality.<br><br>"
                      "mathematical definition: <i>F = G m<sub>1</sub> m<sub>2</sub> r<sup>-2</sup></i><br><br>"
                      "see also: <i>GRAVITYACC</i></nobr>";
    }

    if ( data == "ATOMIC_UNIT" ){

        text = "<nobr><b>constant: Unified Atomic Mass Unit [<i>u</i>] or Dalton [<i>Da</i>] = 1.660 538... " + expo(-27) + " [kg]</b><br><br></nobr>"
                "<nobr>Defines a standard unit that is used for indicating mass on an atomic or molecular scale.</nobr>";
    }

    if ( data == "KLITZING" ){

        text = "<nobr><b>constant: von Klitzing Constant [R<sub>K</sub>] = 25812.807 443 4... [" + omega_up + "]</b><br><br></nobr>"
                "<nobr>Derivatives from other constants of nature: <i>R<sub>K</sub> = h e<sup>-2<sup></i><br><br>"
                "<i>h</i> is Planck Relation <i>(see: PLANCK_H1/PLANCK_H2)</i><br>"
                "<i>e</i> is Elementary Charge <i>(see: ELECTRONC)</i></nobr>";
    }

    if ( data == "ATM_PRESSURE" ){

        text = "<nobr><b>constant: Atmospheric Pressure [p<sub>0</sub>] = 101 325 [Pa]</b><br><br></nobr>"
                "<nobr>101325 <i>Pascal</i> [Pa] is equal to 1 <i>Atmosphere</i> [atm].</nobr>";
    }

    if ( data == "COMPTON_0" ){

        text = "<nobr><b>constant: Compton Wavelength [<i>" + lambda + "<sub>0</sub></i>] = 2.426 310... " + expo(-12) + " [m]</b><br><br></nobr>"
                "<nobr>The Compton wavelength of a particle is equivalent to the wavelength of a photon<br>"
                      "whose energy is the same as the rest&ndash;mass energy of the particle.<br><br>"
                      "Derivatives from other constants of nature: <i>" + lambda + "<sub>0</sub> = h/m<sub>e</sub> c</i><br><br>"
                      "<i>h</i> is Planck Relation <i>(see: PLANCK_H1/PLANCK_H2)</i><br>"
                      "<i>m<sub>e</sub></i> is mass of an electron <i>(see: ELECTRONM)</i><br>"
                      "<i>c</i> is speed of light <i>(see: LIGHTSPEED)</i></nobr>";
    }

    if ( data == "BOHR" ){

        text = "<nobr><b>constant: Bohr Radius [<i>a<sub>0</sub></i>] = 0.529 177... " + expo(-10) + " [m]</b><br><br></nobr>"
                "<nobr>The 'Bohr Radius' is approximately equal to the most probable distance<br>"
                      "between the proton and electron in a hydrogen atom in its ground state."
                      "<br><br>"
                      "Derivatives from other constants of nature: <i>a<sub>0</sub> = 4 " + pi + " " + epsilon + "<sub>0</sub> " + "&#295;" + "<sup>2</sup> m<sub>e</sub><sup>-1</sup> e<sup>-2</sup></i><br><br>"
                      "<i>" + pi + "</i> is Pi <i>(see: PI)</i><br>"
                      "<i>" + epsilon + "<sub>0</sub></i> is Permittivity of free space <i>(see: PERMVAC1)</i><br>"
                      "<i>" + "&#295;" + "</i> reduced Planck Relation <i>(see: PLANCK_RH1/PLANCK_RH2)</i><br>"
                      "<i>m<sub>e</sub></i> is mass of an electron <i>(see: ELECTRONM)</i><br>"
                      "<i>e</i> is Elementary Charge <i>(see: ELECTRONC)</i>"
                      "</nobr>";
    }

    if ( data == "AVOGADRO" ){

        text = "<nobr><b>constant: Avogadro Constant [<i>N<sub>A</sub></i>] = 6.022 141... " + expo(23) + " [mol<sup>-1</sup>]</b><br><br></nobr>"

                      "<nobr>Derivatives from other constants of nature: <i>N<sub>A</sub> = R / k<sub>B</sub> = F / e</i><br><br>"

                      "<i>k<sub>B</sub></i> is Boltzmann Constant <i>(see: K_BOLTZMANN_1</i> to <i>K_BOLTZMANN_3)</i><br>"
                      "<i>R</i> is Gas Constant <i>(see: GAS_CONST)</i><br>"
                      "<i>e</i> is Elementary Charge <i>(see: ELECTRONC)</i><br>"
                      "<i>F</i> is Faraday Constant <i>(see: FARADAY)</i>"
                      "</nobr>";
    }

    if ( data == "ANGSTROM" ){

        text = "<nobr><b>constant: Angstr&ouml;m [&#8491;] = 1 " + expo(-10) + " [m]</b></nobr>";

    }

    if ( data == "GAS_CONST" ){

        text = "<nobr><b>constant: Ideal Gas Constant [<i>R</i>] = 8.314 462 1... [J mol<sup>-1</sup> K<sup>-1</sup>]</b><br><br></nobr>"

                      "<nobr>Derivatives from other constants of nature: <i>R = k<sub>B</sub> N<sub>A</sub></i><br><br>"

                      "<i>k<sub>B</sub></i> is Boltzmann Constant <i>(see: K_BOLTZMANN_1</i> to <i>K_BOLTZMANN_3)</i><br>"
                      "<i>N<sub>A</sub></i> is Avogadro Constant <i>(see: AVOGADRO)</i>"
                      "</nobr>";
    }

    if ( data == "FARADAY" ){

        text = "<nobr><b>constant: Faraday Constant [<i>F</i>] = 9.648 533... " + expo(4) + " [C mol<sup>-1</sup>]</b><br><br></nobr>"
                      "<nobr>It is the magnitude of electric charge per mole of electrons.<br><br>"
                      "Derivatives from other constants of nature: <i>F = e N<sub>A</sub></i><br><br>"

                      "<i>e</i> is Elementary Charge <i>(see: ELECTRONC)</i><br>"
                      "<i>N<sub>A</sub></i> is Avogadro Constant <i>(see: AVOGADRO)</i>"
                      "</nobr>";
    }

    if ( data == "HUBBLE" ){

        text = "<nobr><b>constant: Hubble Constant [<i>H<sub>0</sub></i>] = 74.3 [km s<sup>-1</sup> Mpc<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>The 'Hubble Constant' describes the rate of the expansion of the universe up to date (<u>state:</u> year 2012)."
                      "</nobr>";
    }

    if ( data == "PERMVAC1" ){

        text = "<nobr><b>constant: Permittivity of Free Space or Electric Constant [<i>" + epsilon + "<sub>0</sub></i>] = 8.854 187... " + expo(-12) + " [A s V<sup>-1</sup> m<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>It describes the value of the absolute dielectric permittivity of classical vacuum.<br><br>"
                "Derivatives from other constants of nature: <i>" + epsilon + "<sub>0</sub> = " + mu + "<sub>0</sub><sup>-1</sup> c<sup>-2</sup></i><br><br>"
                "<i>" + mu + "<sub>0</sub></i> is Permeability of free space <i>(see: PERMVAC2)</i><br>"
                "<i>c</sub></i> is Lightspeed <i>(see: LIGHTSPEED)</i>"
                "</nobr>";
    }

    if ( data == "PERMVAC2" ){

        text = "<nobr><b>constant: Permeability of Free Space or Magnetic Constant [<i>" + mu + "<sub>0</sub></i>] = 4 " + pi + " " + expo(-7) +  " [N A<sup>-2</sup>]</b><br><br></nobr>"
                "<nobr>It describes the value of the absolute magnetic permeability of classical vacuum.<br><br>"
                "Derivatives from other constants of nature: <i>" + mu + "<sub>0</sub> = " + epsilon + "<sub>0</sub><sup>-1</sup> c<sup>-2</sup></i><br><br>"
                "<i>" + epsilon + "<sub>0</sub></i> is Permittivity of free space <i>(see: PERMVAC1)</i><br>"
                "<i>c</sub></i> is Lightspeed <i>(see: LIGHTSPEED)</i>"
                "</nobr>";
    }

    if ( data == "ALPHAM" ){

        text = "<nobr><b>constant: Alpha Particle (Helium Nucleus) Mass [m<sub>" + alpha + "</sub>] = 6.644 661... " + expo(-27) + " [kg]</b><br><br></nobr>" +
                "<nobr>in atomic mass unit <i>(see: ATOMIC_UNIT)</i>: 4.001 506 179... u<br><br>" +
                      "for 'Alpha Particle Charge' see: <i>ALPHAC</i>";

    }

    if ( data == "ALPHAC" ){

        text = "<nobr><b>constant: Alpha Particle (Helium Nucleus) Charge = 2 <i>e</i> [C]</b><br><br></nobr>"
                "<i>e</i> is Elementary Charge <i>(see: ELECTRONC)</i>";
    }

    if ( data == "BOHRM_J" ){

        text = "<nobr><b>constant: Bohr Magneton [<i>" + mu + "<sub>B</sub></i>] = 9.274 009... " + expo(-24) + " [J T<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>The 'Bohr Magneton' expresses an electron magnetic dipole moment.<br><br>"
                "Derivatives from other constants of nature: <i>" + mu + "<sub>B</sub> = &frac12; e &#295; m<sub>e</sub><sup>-1</sup></i><br><br>"
                "<i>" + "&#295;" + "</i> is Reduced Planck Relation <i>(see: PLANCK_RH1/PLANCK_RH2)</i><br>"
                "<i>e</i> is Elementary Charge <i>(see: ELECTRONC)</i><br>"
                "<i>m<sub>e</sub></i> is Electron Mass <i>(see: ELECTRONM)</i>"
                "</nobr>";
    }

    if ( data == "BOHRM_eV" ){

        text = "<nobr><b>constant: Bohr Magneton [<i>" + mu + "<sub>B</sub></i>] = 5.788 381... " + expo(-5) + " [eV T<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>The 'Bohr Magneton' expresses an electron magnetic dipole moment.<br><br>"
                "Derivatives from other constants of nature: <i>" + mu + "<sub>B</sub> = &frac12; e " + "&#295;" + " m<sub>e</sub><sup>-1</sup></i><br><br>"
                "<i>" + "&#295;" + "</i> is Reduced Planck Relation <i>(see: PLANCK_RH1/PLANCK_RH2)</i><br>"
                "<i>e</i> is Elementary Charge <i>(see: ELECTRONC)</i><br>"
                "<i>m<sub>e</sub></i> is Electron Mass <i>(see: ELECTRONM)</i>"
                "</nobr>";
    }

    if ( data == "BOHRM_Hz" ){

        text = "<nobr><b>constant: Bohr Magneton [<i>" + mu + "<sub>B</sub></i>] = 13.996 2455... " + expo(9) + " [Hz T<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>The 'Bohr Magneton' expresses an electron magnetic dipole moment.<br><br>"
                "Derivatives from other constants of nature: <i>" + mu + "<sub>B</sub> = &frac12; e " + "&#295;" + " m<sub>e</sub><sup>-1</sup></i><br><br>"
                "<i>" + "&#295;" + "</i> is Reduced Planck Relation <i>(see: PLANCK_RH1/PLANCK_RH2)</i><br>"
                "<i>e</i> is Elementary Charge <i>(see: ELECTRONC)</i><br>"
                "<i>m<sub>e</sub></i> is Electron Mass <i>(see: ELECTRONM)</i>"
                "</nobr>";
    }

    if ( data == "IMPEDANCE" ){

        text = "<nobr><b>constant: Impedance of Free Space [<i>Z<sub>0</sub></i>] = 376.730 313 461... [" + omega_up + "]</b><br><br></nobr>"
                "<nobr>The 'Impedance of Free Space' relating the magnitudes of the electric and magnetic fields<br>"
                "of electromagnetic radiation travelling trough free space.<br><br>"
                "Derivatives from other constants of nature: <i>Z<sub>0</sub> = &radic;(" + mu + "<sub>0</sub> /" + epsilon + "<sub>0</sub> )</i><br><br>"
                "<i>" + mu + "<sub>0</sub></i> is Permeability of free space <i>(see: PERMVAC2)</i><br>"
                "<i>" + epsilon + "<sub>0</sub></i> is Permittivity of free space <i>(see: PERMVAC1)</i>"
                "</nobr>";
    }

    if ( data == "COSMICV_1" ){

        text = "<nobr><b>constant: First Cosmic Velocity [<i>v<sub>1</sub></i>] = 7.91 [km s<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>The 'First Cosmic Velocity' is the circular path speed.<br>"
                "It is needed in order to achieve a near earth orbit.<br><br>"
                "see also: <i>COSMICV_2</i> to <i>COSMICV_4</i></nobr>";
    }

    if ( data == "COSMICV_2" ){

        text = "<nobr><b>constant: Second Cosmic Velocity or Escape Velocity [<i>v<sub>e</sub></i> or <i>v<sub>2</sub></i>] = 11.2 [km s<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>The 'Second Cosmic Velocity' describes the earth&acute;s escape velocity<br><br>"

                "see also: <i>COSMICV_1/COSMICV_3/COSMICV_4</i></nobr>";
    }

    if ( data == "COSMICV_3" ){

        text = "<nobr><b>constant: Third Cosmic Velocity (heliocentric) [<i>v<sub>3</sub></i>] = 42.1 [km s<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>'Third Cosmic Velocity' (geocentric) is 16.5 km s<sup>-1</sup><br><br>"
                "The 'Third Cosmic Velocity' is necessary to leave the sun<br><br>."
                "see also: <i>COSMICV_1/COSMICV_2/COSMICV_4</i></nobr>";
    }

    if ( data == "COSMICV_4" ){

        text = "<nobr><b>constant: Fourth Cosmic Velocity (geocentric) [<i>v<sub>4</sub></i>] is approximatly 320 [km s<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>"
                "The 'Fourth Cosmic Velocity' is necessary to leave our galaxy.<br><br>"

                "see also: <i>COSMICV_1</i> to <i>COSMICV_3</i></nobr>";
    }

    if ( data == "RYDBERG" ){

        text = "<nobr><b>constant: Rydberg Constant [<i>R<sub>H</sub></i> or <i>R<sub>&infin;</sub></i>] = 1.097 373... " + expo(7) + " [m<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>Derivatives from other constants of nature: <i>R<sub>&infin;</sub> = &frac14; " +  alpha + " (" + pi + " a<sub>0</sub>)<sup>-1</sup></i><br><br>"
                "<i>" + alpha + "</i> is Fine &ndash; Structure Constant <i>(see: FINESTRUCT)</i><br>"
                "<i>" + pi + "</i> is Pi <i>(see: PI)</i><br>"
                "<i>a<sub>0</sub></i> is Bohr Radius <i>(see: BOHR)</i>"
                "</nobr>";
    }

    if ( data == "SOLAR" ){

        text = "<nobr><b>constant: Solar Constant [<i>E<sub>0</sub></i>] = 1.36 [kW m<sup>-2</sup>]</b><br><br></nobr>"
                "<nobr>The 'Solar Constant' includes all types of solar radiation and is<br>"
                "the amount of incoming solar electromagnetic radiation per unit.</nobr>";
    }

    if ( data == "WIENSHIFT" ){

        text = "<nobr><b>constant: Wien Displacement Constant [<i>b</i>] = 2.897 768... " + expo(-3) + " [m K]</b><br><br></nobr>"
                "<nobr>The 'Wien Displacement Constant' is the constant of proportionality between<br>"
                "the peak wavelength and the absolute temperature of the black body."
                "</nobr>";
    }

    if ( data == "FINESTRUCT" ){

        text = "<nobr><b>constant: Fine &ndash; Structure Constant [<i>" + alpha + "</i>] = 7.297 552... " + expo(-3) + "</b><br><br></nobr>"
                "<nobr>The 'Fine &ndash; Structure Constant' is approximatly <i>1/137</i>.<br><br></nobr>"

                "<nobr>Derivatives from other constants of nature: <i>" + alpha + " = e<sup>2</sup> (" + "&#295;" + " c)<sup>-1</sup></i><br><br>"
                "<i>" + "&#295;" + "</i> is Reduced Planck Constant <i>(see: PLANCK_RH1/PLANCK_RH2)</i><br>"
                "<i>c</i> is Lightspeed <i>(see: LIGHTSPEED)</i><br>"
                "<i>e</i> is Elementary Charge <i>(see: ELECTRONC)</i>"
                "</nobr>";
    }

    if ( data == "MOLARVOL" ){

        text = "<nobr><b>constant: Molar Volume of Ideal Gas [<i>V<sub>m</sub></i>] = 0.022 414 10... [m<sup>3</sup> mol<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>Describes the 'Molar Volume' of an Ideal Gas for<i> T = 273.16 K (see: KELVIN)</i><br>"
                       "and <i>p = 101325 Pa (see: ATM_PRESSURE).</i>"
                "</nobr>";
    }

    if ( data == "LOSCHMIDT" ){

        text = "<nobr><b>constant: Loschmidt Constant or Loschmidt Number [<i>n<sub>0</sub></i>] = 2.686 763..." + expo(25) + " [m<sup>-3</sup>]</b><br><br></nobr>"
                "<nobr>Describes the number of particles, atoms or molecules of an Ideal Gas in a <br>"
                      "given volume for<i> T = 273.16 K (see: KELVIN)</i> and <i>p = 101325 Pa (see: ATM_PRESSURE).</i>"
                "</nobr>";
    }

    if ( data == "THOMSON" ){

        text = "<nobr><b>constant: Thomson Cross Section for an Electron [<i>" + sigma + "<sub>t</sub></i>] = 0.665 246 16..." + expo(-28) + " [m<sup>2</sup>]</b><br><br></nobr>"
                "<nobr>Derivatives from other constants of nature: <i>" + sigma + "<sub>t</sub> = (8/3) " + pi + " (" + alpha + " " + h_stroke + " m<sub>e</sub><sup>-1</sup> c<sup>-1</sup>)<sup>2</sup></i><br><br>"
                "<i>" + alpha + "</i> is Fine &ndash; Structure Constant <i>(see: FINESTRUCT)</i><br>"
                "<i>" + "&#295;" + "</i> is Reduced Planck Constant <i>(see: PLANCK_RH1/PLANCK_RH2)</i><br>"
                "<i>" + pi + "</i> is Pi <i>(see: PI)</i><br>"
                "<i>m<sub>e</sub></i> is Electron Mass <i>(see: ELECTRONM)</i><br>"
                "<i>c</sub></i> is Lightspeed <i>(see: LIGHTSPEED)</i>"
                "</nobr>";
    }

    if ( data == "COREM_J" ){

        text = "<nobr><b>constant: Core Magneton or Nuclear Magneton [<i>" + mu + "<sub>N</sub></i>] = 5.050 786..." + expo(-27) + " [J T<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>The 'Core Magneton' is the absolute value of the magnetic moment<br>"
                "of a Dirac particle with the charge and mass of a proton.<br><br>"
                "Derivatives from other constants of nature: <i>" + mu + "<sub>N</sub> =  &frac12; e " + h_stroke + " m<sub>p</sub><sup>-1</sup></i><br><br>"
                "<i>e</i> is Elementary Charge <i>(see: ELECTRONC)</i><br>"
                "<i>" + "&#295;" + "</i> is Reduced Planck Constant <i>(see: PLANCK_RH1/PLANCK_RH2)</i><br>"
                "<i>m<sub>p</sub></i> is Proton Mass <i>(see: PROTONM)</i>"
                "</nobr>";
    }

    if ( data == "COREM_eV" ){

        text = "<nobr><b>constant: Core Magneton or Nuclear Magneton [<i>" + mu + "<sub>N</sub></i>] = 3.152 451..." + expo(-8) + " [eV T<sup>-1</sup>]</b><br><br></nobr>"
                "<nobr>The 'Core Magneton' is the absolute value of the magnetic moment<br>"
                "of a Dirac particle with the charge and mass of a proton.<br><br>"
                "Derivatives from other constants of nature: <i>" + mu + "<sub>N</sub> =  &frac12; e " + h_stroke + " m<sub>p</sub><sup>-1</sup></i><br><br>"
                "<i>e</i> is Elementary Charge <i>(see: ELECTRONC)</i><br>"
                "<i>" + "&#295;" + "</i> is Reduced Planck Constant <i>(see: PLANCK_RH1/PLANCK_RH2)</i><br>"
                "<i>m<sub>p</sub></i> is Proton Mass <i>(see: PROTONM)</i>"
                "</nobr>";
    }

    if ( data == "MAGNMOMENT_E" ){

        text = "<nobr><b>constant: Magnetic Moment of an Electron [<i>" + mu + "<sub>e</sub></i>] = -928.477 01..." + expo(-26) + " [J T<sup>-1</sup>]</b></nobr>";
    }

    if ( data == "MAGNMOMENT_P" ){

        text = "<nobr><b>constant: Magnetic Moment of a Proton [<i>" + mu + "<sub>p</sub></i>] = 1.410 607..." + expo(-26) + " [J T<sup>-1</sup>]</b></nobr>";
    }

    if ( data == "MAGNMOMENT_N" ){

        text = "<nobr><b>constant: Magnetic Moment of a Neutron [<i>" + mu + "<sub>n</sub></i>] = -0.966 237..." + expo(-26) + " [J T<sup>-1</sup>]</b></nobr>";
    }

    if ( data == "MAGNMOMENT_M" ){

        text = "<nobr><b>constant: Magnetic Moment of a Muon or Antimuon [<i>" + mu + "<sub>" + mu + "</sub></i>] = -449.044 78..." + expo(-26) + " [J T<sup>-1</sup>]</b></nobr>";
    }

    if ( data == "KELVIN" ){

        text = "<nobr><b>constant: Null Degree Celsius (0 °C) = 273.16 [K]</b></nobr>";
    }

    if ( data == "EULER_CONST" ){

        text = "<nobr><b>constant: Euler Constant or Euler Mascheroni Constant [<i>" + gamma + "</i>] = 0.577 215 665...</b></nobr>";
    }

    if ( data == "RYDBERG_ENERGY" ){

        text = "<nobr><b>constant: Rydberg Energy [<i>E<sub>0<(sub></i>] = 13.605 698 1... [eV]</b></nobr>";
    }

    if ( data == "OUT" ){

        text = "<nobr><b>'OUT' returns the last done calculated value.</b></nobr>";
    }

    //    constants["COMPTON_E"] = COMPTON_WAVELENGTH_ELECTRON;
    //    constants["COMPTON_N"] = COMPTON_WAVELENGTH_NEUTRON;
    //    constants["COMPTON_P"] = COMPTON_WAVELENGTH_PROTON;



    //functions for calculations:
    //*********************************************************************************

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
