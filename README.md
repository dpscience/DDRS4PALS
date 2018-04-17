# DDRS4PALS  <img src="https://github.com/dpscience/DDRS4PALS/blob/master/iconDesign/IconPNGRounded_red.png" width="25" height="25">

Copyright (c) 2016-2018 Danny Petschke (danny.petschke@uni-wuerzburg.de) All rights reserved.<br><br>
<b>DDRS4PALS</b> - A Software for the Acquisition of Positron Lifetime Spectra (PALS) using the DRS4 Evaluation Board<br>

# How to cite this Software?

You can cite all versions by using the <b>DOI 10.5281/zenodo.1219522</b>. This DOI represents all versions, and will always resolve to the latest one.<br>

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1219522.svg)](https://doi.org/10.5281/zenodo.1219522)

## v1.x
DDRS4PALS v1.02:<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1219523.svg)](https://doi.org/10.5281/zenodo.1219523)<br>

# Third Party Libraries and Licenses

<b>DDRS4PALS</b> is written in C++ using [Qt-Framework](https://www.qt.io/) and is licensed under the [GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0)

### A library for spline and polynomial interpolation fitting is provided by: 
- [ALGLIB: A cross-platform numerical analysis and data processing library](http://www.alglib.net/) License: [GNU General Public License v2 or above](https://www.gnu.org/licenses/gpl-2.0)<br>
- [Tino Kluge: A cubic spline interpolation library](http://kluge.in-chemnitz.de/opensource/spline/) License: [GNU General Public License v2 or above](https://www.gnu.org/licenses/gpl-2.0)<br>

### A least-square fitting library using the Levenberg-Marquardt algorithm is provided by:
- [MPFIT: A MINPACK-1 Least Squares Fitting Library in C](https://www.physics.wisc.edu/~craigm/idl/cmpfit.html)<br>

### A library for USB device access is provided by:
- [libusb-1.0 package](http://libusb.info/)<br>

### Source code for the communication with the DRS4 evaluation board:
- [Paul Scherrer Institut - PSI](https://www.psi.ch/drs/software-download)<br><br>
These files containing documentation, drivers, applications, C/C++ source code and the FPGA firmware in VHDL and compiled bit files.<br><br>
<b>Note:</b> Some files containing source code which is part of <b>MIDAS XML Library</b> License: [GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0)<br>

### A C++ library for the simulation of lifetime spectra based in detector pulses is provided by:
- [DLTPulseGenerator - Danny Petschke](https://github.com/dpscience/DLTPulseGenerator) License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

# Setup
1. Download [Qt-framework](https://www.qt.io/download) (at least v5.x).
2. Download and Setup MS Visual Studio Compiler 2013 (or any newer version) .
3. Compile (x86/x64) and Run the code on your Machine.
4. Start the Simulation or Acquisition Modus if your DRS4 Board is connected.
5. Finished. Press run to obtain your Lifetime Spectra.
