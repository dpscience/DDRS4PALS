![badge-OS](https://img.shields.io/badge/OS-tested%20under%20Windows%2010-brightgreen)
![badge-OS](https://img.shields.io/badge/OS-tested%20under%20Windows%2011-brightgreen)

Support this project and keep always updated about recent software releases, bug fixes and major improvements by [following on researchgate](https://www.researchgate.net/project/DDRS4PALS-a-software-for-the-acquisition-and-simulation-of-positron-annihilation-lifetime-spectra-PALS-using-the-DRS4-evaluation-board) or [github](https://github.com/dpscience?tab=followers).

[![badge-researchGate](https://img.shields.io/badge/project-researchGate-brightgreen)](https://www.researchgate.net/project/DDRS4PALS-a-software-for-the-acquisition-and-simulation-of-positron-annihilation-lifetime-spectra-PALS-using-the-DRS4-evaluation-board)

![badge-followers](https://img.shields.io/github/followers/dpscience?style=social)
![badge-stars](https://img.shields.io/github/stars/dpscience/DDRS4PALS?style=social)
![badge-forks](https://img.shields.io/github/forks/dpscience/DDRS4PALS?style=social)

# DDRS4PALS <img src="/iconDesign/IconPNGRounded_red.png" width="25" height="25">

![badge-license](https://img.shields.io/badge/OS-Windows-blue)
![badge-language](https://img.shields.io/badge/language-C++-blue)
![badge-license](https://img.shields.io/badge/license-GPL-blue)

Copyright (c) 2016-2021 Danny Petschke (danny.petschke@uni-wuerzburg.de) All rights reserved.<br>

<b>DDRS4PALS</b> - A software for the acquisition and simulation of positron lifetime spectra (PALS) using the DRS4 evaluation board<br>

<b>pyRemoteDDRS4PALS: </b><b>DDRS4PALS</b> can be remotely controlled via [pyRemoteDDRS4PALS](https://github.com/dpscience/pyRemoteDDRS4PALS) provided by a TCP/IP and xml based API enabling the integration of the DRS4 evaluation board into any customized measurement environment related to PALS, i.e. synchronizing peripherical instrumentation such as temperature (heating/cooling) stages, cycle-loading machines or a vacuum control unit with the DRS4 evaluation board using DDRS4PALS software. Therefore, DDRS4PALS has integrated a server to communicate with.

### ``preview of the user interface of DDRS4PALS``
<br>![DDRS4PALS](/images/ddrs4pals.png)

# Quickstart Guide on Windows OS <img src="https://github.com/dpscience/DDRS4PALS/blob/master/iconDesign/IconPNGRounded_red.png" width="25" height="25">

## ``Option 1: via installer``
* Download the latest installer (<b>installer_DDRS4PALS-v1.18.exe</b>): https://github.com/dpscience/DDRS4PALS/releases
* Run the installer.
* <b>Windows 10|11:</b> install the <b>Zadig USB driver</b>: https://zadig.akeo.ie/ 
* Plug in the <b>DRS4 evaluation board</b> and run the <b>Zadig</b> program. 
* Select the <b>DRS4 evaluation board</b> from the list of devices and click on "Install Driver".
* Run the <b>DDRS4PALS</b> executable.

<b>The repository of the DDRS4PALS-installer can be found</b> [here.](https://github.com/dpscience/DDRS4PALS-installer)

## ``Option 2: manual installation``
* Download the latest software release (<b>DDRS4PALS_v118.rar</b>): https://github.com/dpscience/DDRS4PALS/releases
* Unzip <b>DDRS4PALS_v118.rar</b>.
* Download and install the <b>Visual C++ Redistributable Package (x64)</b> if requested by the OS: https://www.microsoft.com/de-de/download/details.aspx?id=48145
* <b>Windows 10|11:</b> install the <b>Zadig USB driver</b>: https://zadig.akeo.ie/ 
* Plug in the <b>DRS4 evaluation board</b> and run the <b>Zadig</b> program. 
* Select the <b>DRS4 evaluation board</b> from the list of devices and click on "Install Driver".
* Run the <b>DDRS4PALS</b> executable.

# Related Publications <img src="https://github.com/dpscience/DDRS4PALS/blob/master/iconDesign/IconPNGRounded_red.png" width="25" height="25">

### ``Publication in SoftwareX (Jun. 2019)``

![DDRS4PALS](/images/softxPub_software.png)

[DDRS4PALS: A software for the acquisition and simulation of lifetime spectra using the DRS4 evaluation board (SoftwareX, Elsevier)](https://www.sciencedirect.com/science/article/pii/S2352711019300676)<br><br>

### ``Publication in DIB (Feb. 2019)``

![DIB](/images/dib.png)

Experimentally obtained and simulated Positron Annihilation Lifetime Spectra (PALS) acquired on pure Tin (4N) using DDRS4PALS software were published in the following [Data in Brief (Elsevier)](https://www.sciencedirect.com/science/article/pii/S2352340918315142) article.<br><br>  

# How to cite this Software? <img src="https://github.com/dpscience/DDRS4PALS/blob/master/iconDesign/IconPNGRounded_red.png" width="25" height="25">

* <b>When running this software for your research purposes you should at least cite the following publication.</b><br>

[![DOI](https://img.shields.io/badge/DOI-10.1016%2Fj.softx.2019.100261-yellowgreen)](https://doi.org/10.1016/j.softx.2019.100261)

[DDRS4PALS: A software for the acquisition and simulation of lifetime spectra using the DRS4 evaluation board](https://www.sciencedirect.com/science/article/pii/S2352711019300676)<br>

* <b>Additionally, you must cite the applied version of this software in your study.</b><br>

You can cite all released software versions by using the <b>DOI 10.5281/zenodo.1219522</b>. This DOI represents all versions, and will always resolve to the latest one.<br>

[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1219522.svg)](https://doi.org/10.5281/zenodo.1219522)

## ``v1.x``

DDRS4PALS <b>v1.18</b> (<b>DLTPulseGenerator v1.4</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7032030.svg)](https://doi.org/10.5281/zenodo.7032030)<br>

This version <b>1.18</b> has integrated the new version 1.4 of DLTPulseGenerator library for simulation purposes. This update enables you to incorporate pulse height spectra (PHS) from real data or generated via Geant4, so that effects on the background contributing to the lifetime spectra with regard to the PHS window settings can be studied more realistically.

For more information, see the attached example files demonstrating the integration of a real BC422-Q plastic scintillator PHS spectrum considered as simulation input. The related section/tag in the simulation input file is: tag 'pulse-height-spectrum-PHS'. 

DDRS4PALS <b>v1.17</b> (<b>DLTPulseGenerator v1.3</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4736899.svg)](https://doi.org/10.5281/zenodo.4736899)<br>

This version <b>1.17</b> can be remotely controlled via [pyRemoteDDRS4PALS](https://github.com/dpscience/pyRemoteDDRS4PALS) enabling the integration of the DRS4 evaluation board into any customized measurement environment related to PALS, i.e. synchronizing peripherical instrumentation such as temperature (heating/cooling) stages, cycle-loading machines or a vacuum control unit with the DRS4 evaluation board using DDRS4PALS software.

This version is compatible with [pyRemoteDDRS4PALS v1.0](https://github.com/dpscience/pyRemoteDDRS4PALS/releases/tag/1.0)

DDRS4PALS <b>v1.16</b> (<b>DLTPulseGenerator v1.3</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4736899.svg)](https://doi.org/10.5281/zenodo.4736899)<br>

Please, upgrade to <b>v1.16</b> since v.1.15 is skipping the pulse-shape filter data section during loading from a settings file.

DDRS4PALS <b>v1.15</b> (<b>DLTPulseGenerator v1.3</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4730967.svg)](https://doi.org/10.5281/zenodo.4730967)<br>

Since this release [(v1.15)](https://doi.org/10.5281/zenodo.4730967), DDRS4PALS has implemented a freely configurable HTTP server enabling access to all measurement related relevant data and settings via browser, i.e. url requests.    

DDRS4PALS <b>v1.14</b> (<b>DLTPulseGenerator v1.3</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4460129.svg)](https://doi.org/10.5281/zenodo.4460129)<br>

DDRS4PALS <b>v1.13</b> (<b>DLTPulseGenerator v1.3</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4308386.svg)](https://doi.org/10.5281/zenodo.4308386)<br>

DDRS4PALS <b>v1.12</b> (<b>DLTPulseGenerator v1.3</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4264425.svg)](https://doi.org/10.5281/zenodo.4264425)<br>

DDRS4PALS <b>v1.11</b> (<b>DLTPulseGenerator v1.3</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3754770.svg)](https://doi.org/10.5281/zenodo.3754770)<br>

DDRS4PALS <b>v1.10</b> (<b>DLTPulseGenerator v1.3</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3356791.svg)](https://doi.org/10.5281/zenodo.3356791)<br>

This release [(v1.10)](https://doi.org/10.5281/zenodo.3356791) contains the analysis tool [DQuickLTFit v4.1](https://doi.org/10.5281/zenodo.3356830), which enables lifetime spectra to be directly opened and analysed from DDRS4PALS software. Moreover, lifetime spectra can be directly exported as (*.dquicklt) file providing an easy and convenient way for spectra analysis. 

DDRS4PALS <b>v1.09</b> (<b>DLTPulseGenerator v1.3</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3275603.svg)](https://doi.org/10.5281/zenodo.3275603)<br>

DDRS4PALS <b>v1.08</b> (<b>DLTPulseGenerator v1.3</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2540851.svg)](https://doi.org/10.5281/zenodo.2540851)<br>

DDRS4PALS <b>v1.07</b> (<b>DLTPulseGenerator v1.2</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1495099.svg)](https://doi.org/10.5281/zenodo.1495099)<br>

DDRS4PALS <b>v1.06</b> (<b>DLTPulseGenerator v1.2</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1434714.svg)](https://doi.org/10.5281/zenodo.1434714)<br>

DDRS4PALS <b>v1.05</b> (<b>DLTPulseGenerator v1.2</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1302998.svg)](https://doi.org/10.5281/zenodo.1302998)<br>

DDRS4PALS <b>v1.04</b> (<b>DLTPulseGenerator v1.2</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1285836.svg)](https://doi.org/10.5281/zenodo.1285836)<br>

DDRS4PALS <b>v1.03</b> (<b>DLTPulseGenerator v1.1</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1227278.svg)](https://doi.org/10.5281/zenodo.1227278)<br>

DDRS4PALS <b>v1.02</b> (<b>DLTPulseGenerator v1.1</b>):<br>[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1219523.svg)](https://doi.org/10.5281/zenodo.1219523)<br>

# Insights ... <img src="https://github.com/dpscience/DDRS4PALS/blob/master/iconDesign/IconPNGRounded_red.png" width="25" height="25">

### ``signal-scope: monitor the acquired or simulated detector-output pulses``
<br>![Detector-Pulse Monitor](/images/scope.png)

### ``energy spectra (PHS): select the energy windows assigning the start & stop quanta``
<br>![PHS](/images/phs.png)

### ``online tracking of the resulting spectra``
<br>![Spectrum](/images/spec.png)<br>

### ``timing & voltage/gain calibration without the need of running DRSOsc software``
<br>![calibration](/images/calibration.png)<br>

### ``accessing DDRS4PALS remotely via internet browser``
<br>![DDRS4PALS-http](/images/server-integration-DDRS4PALS.png)
<br>![DDRS4PALS-http-url](/images/main.png)

### ``remote control of DDRS4PALS via Python-API (pyRemoteDDRS4PALS)``
<br>![DDRS4PALS-rc](/images/rc.png)
<br>![DDRS4PALS-rc-py](/images/pyRemote.png)

## producing high-quality lifetime spectra exploiting a set of freely configurable physical filters:

### ``1D median filter for spike-removal and noise-reduction``
<br>![Median-Filter](/images/spikeRemoval.png)

### ``pile-up rejection by area-filtering``
<br>![Area-Filter](/images/areaFilter.png)

### ``detection of misshaped or artifact-afflicted pulses using shape-sensitive filtering``
<br>![Shape-Filter](/images/shapeFilter.png)

# License of DDRS4PALS (GNU General Public License) <img src="https://github.com/dpscience/DDRS4PALS/blob/master/iconDesign/IconPNGRounded_red.png" width="25" height="25">

Copyright (c) 2016-2021 Danny Petschke (danny.petschke@uni-wuerzburg.de) All rights reserved.<br>

<p align="justify">This program is free software: you can redistribute it and/or modify<br>
it under the terms of the GNU General Public License as published by<br>
the Free Software Foundation, either version 3 of the License, or<br>
(at your option) any later version.<br><br>

This program is distributed in the hope that it will be useful,<br>
but WITHOUT ANY WARRANTY; without even the implied warranty of<br>
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.<br><br></p>

For more details see [GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0)

# Used Third Party Libraries and Licenses <img src="https://github.com/dpscience/DDRS4PALS/blob/master/iconDesign/IconPNGRounded_red.png" width="25" height="25">

<b>DDRS4PALS</b> is written in C++ using the [Qt-Framework](https://www.qt.io/) licensed under the [GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0)

The following 3rd party libraries are used by <b>DDRS4PALS</b> software.<br>

### ``ALGLIB``
* [ALGLIB: A cross-platform numerical analysis and data processing library](http://www.alglib.net/)<br><br>License: [GNU General Public License v2 or above](https://www.gnu.org/licenses/gpl-2.0)<br>

### ``Tino Kluge``
* [Tino Kluge: A cubic spline interpolation library](http://kluge.in-chemnitz.de/opensource/spline/)<br><br>License: [GNU General Public License v2 or above](https://www.gnu.org/licenses/gpl-2.0)<br>

### ``Sergejus Zabinski``
* [Sergejus Zabinski: FAST MEDIAN FILTERING](http://www.sergejusz.com/engineering_tips/median_filter.htm)<br><br>License: [Apache License v2](http://www.apache.org/licenses/LICENSE-2.0)<br>

### ``Danny Petschke``
* [DLTPulseGenerator: A library for the simulation of lifetime spectra based on detector-output pulses](https://github.com/dpscience/DLTPulseGenerator)<br><br>License: [BSD-3-Clause](https://opensource.org/licenses/BSD-3-Clause)

### ``MPFIT (MINPACK-1)``
* [MPFIT: A MINPACK-1 Least Squares Fitting Library in C](https://www.physics.wisc.edu/~craigm/idl/cmpfit.html)<br>

### ``libusb-1.0``
* [libusb-1.0 package: USB device access](http://libusb.info/)<br>

### ``DRS4 evaluation board software and source code package``
* [Paul Scherrer Institut - PSI](https://www.psi.ch/drs/software-download)<br><br>
These files containing documentation, drivers, applications, C/C++ source code and the FPGA firmware in VHDL and compiled bit files.<br><br>
<b>Note:</b> Some files containing source code which is part of <b>MIDAS XML Library</b><br><br>License: [GNU General Public License v3](https://www.gnu.org/licenses/gpl-3.0)<br>

# Deploy DDRS4PALS on Windows OS from Source Files using QtCreator <img src="https://github.com/dpscience/DDRS4PALS/blob/master/iconDesign/IconPNGRounded_red.png" width="25" height="25">
* Download the QtCreator and the [Qt-framework](https://www.qt.io/download) (at least v5.x).
* Download and Setup the MS Visual Studio compiler (at least version 2013). It should also work with any other compiler e.g. MinGW but I recommend using VS compiler.
* Open the .pro file in QtCreator. 
* Deploy DDRS4PALS. It should finish without any errors.
* Connect your DRS4-EB (if available) and start the simulation or acquisition mode.
* Finished.
