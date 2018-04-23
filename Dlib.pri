#-------------------------------------------------
#
# DLib - Version: 17-05-2016 (Danny Petschke)
#
#-------------------------------------------------
DEFINES += DLIB_VERSION_1_0_2


QT       += core gui xml svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += c++11

HEADERS  +=  DLib.h\
                        DLib/DTypes/defines.h\
                        DLib/DTypes/types.h\
                        DLib/DXML/simplexml.h\
                        DLib/DGUI/svgbutton.h\
                        DLib/DGUI/slider.h\
                        DLib/DGUI/verticalrangedoubleslider.h\
                        DLib/DGUI/horizontalrangedoubleslider.h\
                        DLib/DGUI/mathconsoletextbox.h\
                        DLib/DPlot/plot2DXWidget.h\
                        DLib/DPlot/plot2DXCurve.h\
                        DLib/DPlot/plot2DXAxis.h\
                        DLib/DPlot/plot2DXCanvas.h\
                        DLib/DThirdParty/Crypt/tinyaes.h\
                        DLib/DThirdParty/MedianFilter/medflt.h\
                        DLib/DMath/dspline.h\
                        DLib/DMath/constantexplanations.h\
                        DLib/DMath/dmedianfilter.h\


SOURCES  +=  DLib/DTypes/defines.cpp\
                        DLib/DTypes/types.cpp\
                        DLib/DXML/simplexml.cpp\
                        DLib/DGUI/svgbutton.cpp\
                        DLib/DGUI/slider.cpp\
                        DLib/DGUI/verticalrangedoubleslider.cpp\
                        DLib/DGUI/horizontalrangedoubleslider.cpp\
                        DLib/DGUI/mathconsoletextbox.cpp\
                        DLib/DPlot/plot2DXWidget.cpp\
                        DLib/DPlot/plot2DXCurve.cpp\
                        DLib/DPlot/plot2DXAxis.cpp\
                        DLib/DPlot/plot2DXCanvas.cpp\
                        DLib/DThirdParty/Crypt/tinyaes.cpp\
                        DLib/DMath/constantexplanations.cpp\
                        DLib/DMath/dmedianfilter.cpp\

FORMS    += DLib/DGUI/horizontalrangedoubleslider.ui\
                    DLib/DGUI/verticalrangedoubleslider.ui\

RESOURCES += DLib/DResources/res.qrc
