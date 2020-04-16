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

#ifndef CONSTANTEXPLANATIONS_H
#define CONSTANTEXPLANATIONS_H

#include "../DTypes/types.h"

//mathematical constants:

#define PI 3.14159265359
#define E 2.718281828459045235
#define EULER_CONST 0.577215665
#define M_LOG 0.4342944819
#define CONST_OF_PLANCK_Js 6.62606957E-34;//[Js]
#define CONST_OF_PLANCK_eVs 4.135667516E-15 //[eVs]
#define REDUCED_CONST_OF_PLANCK_Js 1.054571726E-34 //[Js]
#define REDUCED_CONST_OF_PLANCK_eVs 6.58211928E-16 //[eVs]
#define GOLDEN_NUMBER ( ( 1 + sqrt(5) ) / 2)

//physical constants:

//electron mass:
#define ELECTRON_MASS 9.10938291e-31; //[kg]
#define ELECTRON_MASS_UNCERTAINTY  0.00000040e-31; //[kg]

//electron g-factor:
#define ELECTRON_GFACTOR -2.00231930436153;
#define ELECTRON_GFACTOR_UNCERTAINTY  0.00000000000053;

//electron charge:
#define ELECTRON_CHARGE  1.602176565e-19; //[C]
#define ELECTRON_CHARGE_UNCERTAINTY  0.000000035e-19; //[C]

//definition of 1 Angstrom:
#define ANGSTROM_STAR  1e-10; //[m]

//atomic mass unit:
#define ATOMIC_MASS_UNIT  1.660538921e-27; //[kg]
#define ATOMIC_MASS_UNIT_UNCERTAINTY  0.000000073e-27; //[kg]

//Avogadro constant:
#define AVOGADRO_CONSTANT  6.02214129e23; //[mol^-1]
#define AVOGADRO_CONSTANT_UNCERTAINTY  0.00000027e23; //[mol^-1]

//Bohr Magneton:
#define BOHR_MAGNETON_J  9.27400968e-24; //[J T^-1]
#define BOHR_MAGNETON_UNCERTAINTY_J  0.000020e-26; //[J T^-1]

#define BOHR_MAGNETON_eV  5.7883818066e-5; //[eV T^-1]
#define BOHR_MAGNETON_UNCERTAINTY_eV  0.0000000038e-5; //[eV T^-1]

#define BOHR_MAGNETON_Hz  13.99624555e9; //[Hz T^-1]
#define BOHR_MAGNETON_UNCERTAINTY_Hz  0.00000031e9; //[Hz T^-1]

#define BOHR_MAGNETON_K  0.67171388; //[K T^-1]
#define BOHR_MAGNETON_UNCERTAINTY_K  0.00000061; //[K T^-1]

//Bohr radius:
#define BOHR_RADIUS  0.52917721092e-10; //[m]
#define BOHR_RADIUS_UNCERTAINTY  0.00000000017e-10; //[m]

//Boltzmann constant:
#define BOLTZMANN_CONSTANT_J  1.3806488e-23; //[J K^-1]
#define BOLTZMANN_CONSTANT_UNCERTAINTY_J 0.0000013e-23; //[J K^-1]

#define BOLTZMANN_CONSTANT_eV  8.6173324e-5; //[eV K^-1]
#define BOLTZMANN_CONSTANT_UNCERTAINTY_eV  0.0000078e-5; //[eV K^-1]

#define BOLTZMANN_CONSTANT_Hz  2.0836618e10; //[Hz K^-1]
#define BOLTZMANN_CONSTANT_UNCERTAINTY_Hz  0.0000019e10; //[Hz K^-1]

//characteristic impedance of vacuum:
#define CHARACTERISTIC_IMPEDANCE_OF_VACUUM  376.730313461; //[Ohm]

//deuteron g-factor:
#define DEUTERON_G_FACTOR  0.8574382308;
#define DEUTERON_G_FACTOR_UNCERTAINTY  0.0000000072;

//speed of light:
#define SPEED_OF_LIGHT  299792458; //[m s^-1]

//acceleration of gravity:
#define ACCELERATION_OF_GRAVITY  9.80665; //[m s^-2]

//gravity constant:
#define GRAVITY_CONSTANT  6.67384e-11; //[m³*(kg*s²)^-1]

//standard atmosphere pressure:
#define ATMOSPHERE_PRESSURE  101325; //[Pa]

//Stefan-Boltzmann constant:
#define STEFAN_BOLTZMANN_CONSTANT  5.670373e-8; //[W m^-2 K^-4]
#define STEFAN_BOLTZMANN_CONSTANT_UNCERTAINTY  0.000021e-8; //[W m^-2 K^-4]

//atomic mass unit:
#define UNIFIED_ATOMIC_MASS_UNIT  1.660538921e-27; //[kg]
#define UNIFIED_ATOMIC_MASS_UNIT_UNCERTAINTY  0.000000073e-27; //[kg]

//von Klitzing constant:
#define KLITZING_CONSTANT  25812.8074434; //[Ohm]
#define KLITZING_CONSTANT_UNCERTAINTY  0.0000084; //[Ohm]

//mass of alpha particle (He-cores) by velocity = 0:
#define ALPHAPARTICLE_MASS 6.6446616e-27; //[kg]

//energy of alpha particle (He-cores) by velocity = 0:
#define ALPHAPARTICLE_ENERGY 3727.1e6; //[eV]

//charge of alpha particle (He-cores) by velocity = 0:
#define ALPHAPARTICLE_CHARGE 2*1.602176565e-19; //[C]

//vacuum permittivities
#define ELECTRIC_PERMITTIVITY_VACUUM 8.85418781762e-12; //[C (V*m)^-1]
#define MAGNETIC_PERMITTIVITY_VACUUM (4 * 3.14159265358979323846f * 1e-7); //[V s (A*m)^-1]

//general gas constant/Rydbergconstant
#define GAS_CONST 8.3144621; //[J (K*mol)^-1]

//Faraday constant
#define FARADAY_CONST 9.64853399e4; //[C mol^-1]

//Hubble constant
#define HUBBLE_CONST 74.3; //[km (s*Mpc)^-1]

//first cosmic velocity
#define COSMIC_VELOCITY_1 7.91; //[km/s]

//second cosmic velocity
#define COSMIC_VELOCITY_2 11.2; //[km/s]

//third cosmic velocity
#define COSMIC_VELOCITY_3 42.1; //[km/s]

//fourth cosmic velocity
#define COSMIC_VELOCITY_4 320; //[km/s]

//proton mass by velocity = 0
#define PROTON_MASS 1.67262e-27; //[kg]

//proton energy by velocity = 0
#define PROTON_ENERGY 938.27e6; //[eV]

//proton g-factor
#define PROTON_GFACTOR 5.585694713; //[eV]

//proton specific charge
#define PROTON_SPECIFIC_CHARGE 9.5788e7; //[C/kg]
#define PROTON_CHARGE 1.602176565e-19; //[C]

//neutron mass:
#define NEUTRON_MASS  1.674927351e-27; //[kg]
#define NEUTRON_GFACTOR -3.82608545;

#define NEUTRON_MASS_UNCERTAINTY  0.000000074e-27; //[kg]
#define MUON_GFACTOR -2.0023318414;
#define MUON_MASS 1.88353130e-28; //[kg]

#define MUON_CHARGE 1.602176565e-19; //[C]

#define ANTIMUON_GFACTOR -2.0023318414;
#define ANTIMUON_MASS 1.88353130e-28; //[kg]
#define ANTIMUON_CHARGE 1.602176565e-19; //[C]

#define PION_MASS 2.488e-28; //[kg]
#define ANTIPION_MASS 2.488e-28; //[kg]
#define NEUTRALPION_MASS 2.406e-28; //[kg]

#define PION_CHARGE 1.602176565e-19; //[C]
#define ANTIPION_CHARGE 1.602176565e-19; //[C]
#define NEUTRALPION_CHARGE 0; //[C]

//compton wavelength lambda0:
#define COMPTON_WAVELENGTH  2.4263102389e-12; //[m]

//compton wavelength lambdae,p,n:
#define COMPTON_WAVELENGTH_ELECTRON  3.86159323e-13; //[m]
#define COMPTON_WAVELENGTH_NEUTRON  2.10019445e-16; //[m]
#define COMPTON_WAVELENGTH_PROTON  2.10308937e-16; //[m]

//core magneton:
#define CORE_MAGNETON_eV  3.15245166e-8; //[eV T^-1]
#define CORE_MAGNETON_J  5.0507866e-27; //[J T^-1]

//magnetic moment:
#define MAGNETIC_MOMENT_ELECTRON  -928.47701e-26; //[J T^-1]
#define MAGNETIC_MOMENT_PROTON  1.41060761e-26; //[J T^-1]
#define MAGNETIC_MOMENT_NEUTRON  -0.96623707e-26; //[J T^-1]
#define MAGNETIC_MOMENT_MUON  -449.04478e-26; //[J T^-1]

//Rydberg energy:
#define RYDBERG_ENERGY  13.6056981; //[eV]

//Thomson cross-section:
#define THOMSON_CROSS_SECTION  0.66524616e-28; //[m²]

//Loschmidt constant:
#define LOSCHMIDT_CONSTANT 2.686763e25; //[m-3]

//Molar volume::
#define MOLAR_VOLUME 0.02241410; //[m³/mol]

//0° C:
#define KELVIN 273.15; //[K]

//lattice space of silicon in {220} direction:
#define LATTICE_SPACING_OF_SILICON_220  192.0155714e-12;  //[m]
#define LATTICE_SPACING_OF_SILICON_220_UNCERTAINTY  0.0000032e-12; //[m]

//Rydberg constant (coremass -> infinity)
#define RYDBERG_INFINITY 1.0973731568539e7; //[1/m]

//Rydberg constant (liquid,H)
#define RYDBERG_H 1.0967758e7; //[1/m]

//solar constant
#define SOLAR_CONST 1.36; //[kW/m²]

//Wien shift constant
#define WIEN_SHIFT_CONST 2.8977685e-3; //[m K]

//Fine Structure constant
#define FINESTRUCT_CONST 7.2973525698e-3;


class DConstantExplanations
{
public:
    DConstantExplanations();
    virtual ~DConstantExplanations();

public:
    static QString getExplanation(const QString& name);

private:
    static QString expo(int exponent);
};

#endif // CONSTANTEXPLANATIONS_H
