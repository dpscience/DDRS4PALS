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
