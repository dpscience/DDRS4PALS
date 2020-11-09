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

#ifndef DLIB_H
#define DLIB_H

#include "DLib/DTypes/defines.h"
#include "DLib/DTypes/types.h"

#include "DLib/DXML/simplexml.h"

#include "DLib/DCompression/compressionwrapper.h"

#include "DLib/DThirdParty/Crypt/tinyaes.h"
#include "DLib/DThirdParty/Miniz/miniz.h"
#include "DLib/DThirdParty/MedianFilter/medflt.h"

#include "DLib/DGUI/slider.h"
#include "DLib/DGUI/svgbutton.h"
#include "DLib/DGUI/horizontalrangedoubleslider.h"
#include "DLib/DGUI/verticalrangedoubleslider.h"
#include "DLib/DGUI/mathconsoletextbox.h"

#include "DLib/DPlot/plot2DXWidget.h"
#include "DLib/DPlot/plot2DXCurve.h"
#include "DLib/DPlot/plot2DXCanvas.h"
#include "DLib/DPlot/plot2DXAxis.h"

#include "DLib/DMath/constantexplanations.h"
#include "DLib/DMath/dmedianfilter.h"

#endif // DLIB_H
