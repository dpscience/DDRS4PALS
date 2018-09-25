# DDRS4PALS  <img src="https://github.com/dpscience/DDRS4PALS/blob/master/iconDesign/IconPNGRounded_red.png" width="25" height="25">

Copyright (c) 2016-2018 Danny Petschke (danny.petschke@uni-wuerzburg.de) All rights reserved.<br><br>
<b>DDRS4PALS</b> - A Software for the Acquisition and Simulation of Positron Lifetime Spectra (PALS) using the DRS4 Evaluation Board<br>

# How to cite this Software?

You can cite all versions by using the <b>DOI 10.5281/zenodo.1219522</b>. This DOI represents all versions, and will always resolve to the latest one.<br>

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1219522.svg)](https://doi.org/10.5281/zenodo.1219522)

## v1.x
DDRS4PALS <b>v1.05</b> (<b>DLTPulseGenerator v1.2</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1302998.svg)](https://doi.org/10.5281/zenodo.1302998)<br>

DDRS4PALS <b>v1.04</b> (<b>DLTPulseGenerator v1.2</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1285836.svg)](https://doi.org/10.5281/zenodo.1285836)<br>

DDRS4PALS <b>v1.03</b> (<b>DLTPulseGenerator v1.1</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1227278.svg)](https://doi.org/10.5281/zenodo.1227278)<br>

DDRS4PALS <b>v1.02</b> (<b>DLTPulseGenerator v1.1</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1219523.svg)](https://doi.org/10.5281/zenodo.1219523)<br>

# Third Party Libraries and Licenses

<b>DDRS4PALS</b> is written in C++ using [Qt-Framework](https://www.qt.io/) (licensed under the [GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0))

The following 3rd party libraries used by <b>DDRS4PALS</b> software.<br>

### ALGLIB
- [ALGLIB: A cross-platform numerical analysis and data processing library](http://www.alglib.net/)<br><br>License: [GNU General Public License v2 or above](https://www.gnu.org/licenses/gpl-2.0)<br>

### Tino Kluge
- [Tino Kluge: A cubic spline interpolation library](http://kluge.in-chemnitz.de/opensource/spline/)<br><br>License: [GNU General Public License v2 or above](https://www.gnu.org/licenses/gpl-2.0)<br>

### Sergejus Zabinski
- [Sergejus Zabinski: FAST MEDIAN FILTERING](http://www.sergejusz.com/engineering_tips/median_filter.htm)<br><br>License: [Apache License v2](http://www.apache.org/licenses/LICENSE-2.0)<br>

### Danny Petschke
- [DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses](https://github.com/dpscience/DLTPulseGenerator)<br><br>License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

### MPFIT (MINPACK-1)
- [MPFIT: A MINPACK-1 Least Squares Fitting Library in C](https://www.physics.wisc.edu/~craigm/idl/cmpfit.html)<br>

### libusb-1.0
- [libusb-1.0 package: USB device access](http://libusb.info/)<br>

### DRS4 evaluation board software and source code package
- [Paul Scherrer Institut - PSI](https://www.psi.ch/drs/software-download)<br><br>
These files containing documentation, drivers, applications, C/C++ source code and the FPGA firmware in VHDL and compiled bit files.<br><br>
<b>Note:</b> Some files containing source code which is part of <b>MIDAS XML Library</b><br><br>License: [GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0)<br>

# Setup
1. Download [Qt-framework](https://www.qt.io/download) (at least v5.x).
2. Download and Setup MS Visual Studio Compiler 2013 (or any newer version) .
3. Compile (x86/x64) and Run the code on your Machine.
4. Start the Simulation or Acquisition Modus if your DRS4 Board is connected.
5. Finished. Press run to obtain your Lifetime Spectra.

# Manual
- The full documentation can be found [here](https://github.com/dpscience/DDRS4PALS/wiki).