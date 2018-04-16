#---------------------------------------------------------
#
# AlgLib - Version: 3.13.0 (C++)
#
# distributed under GPL 2+ license, which is not 
# suited for commercial distribution
#
#----------------------------------------------------------
DEFINES += ALGLIB_VERSION_3_13_0

QT  += core

CONFIG   += c++11

HEADERS  +=  alglib.h\                        
                        AlgLib/alglibinternal.h\
                        AlgLib/alglibmisc.h\
                        AlgLib/ap.h\
                        AlgLib/dataanalysis.h\
                        AlgLib/diffequations.h\
                        AlgLib/fasttransforms.h\
                        AlgLib/integration.h\
                        AlgLib/interpolation.h\
                        AlgLib/linalg.h\
                        AlgLib/optimization.h\
                        AlgLib/solvers.h\
                        AlgLib/specialfunctions.h\
                        AlgLib/statistics.h\

SOURCES  +=  AlgLib/alglibinternal.cpp\
                        AlgLib/alglibmisc.cpp\
                        AlgLib/ap.cpp\
                        AlgLib/dataanalysis.cpp\
                        AlgLib/diffequations.cpp\
                        AlgLib/fasttransforms.cpp\
                        AlgLib/integration.cpp\
                        AlgLib/interpolation.cpp\
                        AlgLib/linalg.cpp\
                        AlgLib/optimization.cpp\
                        AlgLib/solvers.cpp\
                        AlgLib/specialfunctions.cpp\
                        AlgLib/statistics.cpp\

