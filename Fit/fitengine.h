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

#ifndef FITENGINE_H
#define FITENGINE_H

#include <stdlib.h>

#include "DLib.h"

#include "drs4boardmanager.h"

#include <cmath>
#include "mpfit.h"

using namespace std;

class PALSFitErrorCodeStringBuilder;

typedef enum
{
    no_Weighting ,
    yerror_Weighting ,
    yvariance_Weighting
}residualWeighting;

typedef struct
{
  double *x;
  double *y;
  double *ey;

  int dataCnt;

  int weighting;
}values;

double weightedResidual(double y , double yi , double erryi , int weightingCode);
bool linearRegression(const QVector<QPointF> &data, double *slope, double *intercept);
int gaussian(int dataCnt, int paramCnt, double *fitParamArray, double *dy, double **dvec, void *vars);
int gaussianWithBkgrd(int dataCnt, int paramCnt, double *fitParamArray, double *dy, double **dvec, void *vars);

class FitEngine
{
public:
    FitEngine();
    virtual ~FitEngine();

public:
    static FitEngine *sharedInstance();

    bool runLinear(const QVector<QPointF> &dataVec, double *slope, double *intercept);
    bool runGaussian(const QVector<int>& dataVec, int numberOfIter, int numberOfSplinePoints, double *startSigma, double *startMean, double *amplitude, QVector<QPointF>* visData, QString *returnValue);
    bool runGaussianWithBkgrd(const QVector<int>& dataVec, int numberOfIter, int numberOfSplinePoints, double *startSigma, double *startMean, double *amplitude, double *bkgrd, QVector<QPointF> *visData, QString *returnValue);

private:
    void getOutputData(QVector<QPointF> *visData, values *v, double *params, int numberOfSplinePoints);
    void getOutputDataWithBkgrd(QVector<QPointF> *visData, values *v, double *params, int numberOfSplinePoints);
};

class PALSFitErrorCodeStringBuilder
{
public:
    inline static QString errorString(int errorCode)
    {
        switch ( errorCode )
        {
        case 0:
            return QString("General Input Parameter Error.");
            break;

        case 1:
            return QString("OK. Convergence in &#967;<sup>2</sup>.");
            break;

        case 2:
            return QString("OK. Convergence in Parameter Value.");
            break;

        case 3:
            return QString("OK. Convergence in &#967;<sup>2</sup> & Parameter Value.");
            break;

        case 4:
            return QString("OK. Convergence in Orthogonality.");
            break;

        case 5:
            return QString("OK. Maximum Number of Iterations reached.");
            break;

        case 6:
            return QString("OK. No further Imrovements: Relative &#967;<sup>2</sup>-Convergence Criterium.");
            break;

        case 7:
            return QString("OK. No further Imrovements: Relative Parameter-Convergence Criterium.");
            break;

        case 8:
            return QString("OK. No further Imrovements: Orthogonality-Convergence Criterium.");
            break;

        case -16:
            return QString("Error. User-Function produced non-finite Values.");
            break;

        case -17:
            return QString("Error. No User Function was supplied.");
            break;

        case -18:
            return QString("Error. No User Data-Points were supplied.");
            break;

        case -19:
            return QString("Error. No free Parameters.");
            break;

        case -20:
            return QString("Error. Memory Allocation Error.");
            break;

        case -21:
            return QString("Error. Initial Values inconsistent with Constraints.");
            break;

        case -22:
            return QString("Error. Initial Constraints inconsistent.");
            break;

        case -23:
            return QString("Error. General Input Parameter Error.");
            break;

        case -24:
            return QString("Error. Not enough degrees of freedom.");
            break;

        case -60:
            return QString("Error: Internal Nullptr.");
            break;

        case -61:
            return QString("Error: Internal Nullptr.");
            break;

        case -62:
            return QString("Error: Internal Nullptr.");
            break;

        default:
            return QString("");
            break;
        }

        Q_UNREACHABLE();
    }
};

#endif // FITENGINE_H
