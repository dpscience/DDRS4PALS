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

#include "fitengine.h"

double weightedResidual(double y, double yi, double erryi, int weightingCode)
{

    double residual = 0.0f;
    double weight = 0.0f;

    switch ( weightingCode )
    {
    case 0: //no weighting:
        weight = 1;

        residual = weight*(yi - y);
        break;

    case 1: //y-error weighting
        weight = (1/erryi);

        if ( erryi == 0 )
            residual = (yi - y);
        else
            residual = weight*(yi - y);

        break;

    case 2: //y-variance weighting
        if ( yi != 0 )
        {
            weight = (1/yi);
            residual = weight*(yi - y);
        }
        else
            residual = ( yi - y );

        break;

    default:
        residual = 0;
        break;
    }

    return residual;
}

int gaussian(int dataCnt, int paramCnt, double *fitParamArray, double *dy, double **dvec, void *vars)
{
        DUNUSED_PARAM(dvec);

        values *v = (values*) vars;

        double *x = v->x;
        double *y = v->y;
        double *ey = v->ey;

        const double sigma       = fitParamArray[paramCnt-3];
        const double e0             = fitParamArray[paramCnt-2];
        const double amplitude = fitParamArray[paramCnt-1];

        double f = 0;

        for ( int i = 0 ; i < dataCnt ; ++ i )
        {
            const double u = 1.0f/(sqrtf(2.0*PI)*sigma);

            f = amplitude*u*exp(-0.5*((x[i]-e0)/sigma)*((x[i]-e0)/sigma));

            dy[i] = weightedResidual(f, y[i], ey[i], v->weighting);
        }

        return 1;
}

int gaussianWithBkgrd(int dataCnt, int paramCnt, double *fitParamArray, double *dy, double **dvec, void *vars)
{
    DUNUSED_PARAM(dvec);

    values *v = (values*) vars;

    double *x = v->x;
    double *y = v->y;
    double *ey = v->ey;

    const double sigma       = fitParamArray[paramCnt-4];
    const double e0             = fitParamArray[paramCnt-3];
    const double amplitude = fitParamArray[paramCnt-2];
    const double bkgrd        = fitParamArray[paramCnt-1];

    double f = 0;

    for ( int i = 0 ; i < dataCnt ; ++ i )
    {
        const double u = 1.0f/(sqrtf(2.0*PI)*sigma);

        f = bkgrd + amplitude*u*exp(-0.5*((x[i]-e0)/sigma)*((x[i]-e0)/sigma));

        dy[i] = weightedResidual(f, y[i], ey[i], v->weighting);
    }

    return 1;
}

bool linearRegression(const QVector<QPointF>& data, double *slope, double *intercept)
{
    if (!slope || !intercept)
        return false;

    double xSum = 0, ySum = 0, xxSum = 0, xySum = 0;

    for (int i = 0 ; i < data.size() ; ++ i )
    {
        xSum += data.at(i).x();
        ySum += data.at(i).y();
        xxSum += data.at(i).x()*data.at(i).x();
        xySum += data.at(i).x()*data.at(i).y();
    }

    *slope = (data.size() * xySum - xSum * ySum) / (data.size() * xxSum - xSum * xSum);
    *intercept = (ySum - *slope * xSum) / data.size();

    return true;
}

FitEngine *__sharedInstanceFitEngine = nullptr;

FitEngine::FitEngine()
{
    //nothing to do!
}

FitEngine::~FitEngine()
{
    DDELETE_SAFETY(__sharedInstanceFitEngine);
}

FitEngine *FitEngine::sharedInstance()
{
    if ( !__sharedInstanceFitEngine )
        __sharedInstanceFitEngine = new FitEngine();

    return __sharedInstanceFitEngine;
}

bool FitEngine::runLinear(const QVector<QPointF> &dataVec, double *slope, double *intercept)
{
    if ( dataVec.isEmpty() )
        return false;

    return linearRegression(dataVec, slope, intercept);
}

bool FitEngine::runGaussian(const QVector<int>& dataVec, int numberOfIter, int numberOfSplinePoints, double *startSigma, double *startMean, double *amplitude, QVector<QPointF> *visData, QString* returnValue)
{
    if ( !startSigma || !startMean || !amplitude || !returnValue )
        return false;

    if ( dataVec.isEmpty() )
        return false;


    //initialize the data-set:
    const int paramCnt = 3;

     double *x = new double[(const int)dataVec.size()];
     double *y = new double[(const int)dataVec.size()];
     double *ey = new double[(const int)dataVec.size()];

     //copy to array:
     int i = 0;
     for ( int yVal : dataVec )
     {
          x[i] = i;
          y[i] = (double)yVal;
          ey[i] = sqrtf((double)yVal);

          i ++;
     }

     values v;

     v.dataCnt = dataVec.size();
     v.x = x;
     v.y = y;
     v.ey = ey;
     v.weighting = residualWeighting::no_Weighting;

    //reset param-contraints:
    mp_par paramContraints[paramCnt];
    memset(&paramContraints[0], 0, sizeof(paramContraints));

    //params initial conditions:
    double params[paramCnt];

    //sigma
    paramContraints[0].fixed = 0;
    paramContraints[0].limited[0] = 0;

    params[0] = *startSigma;

    //e0
    paramContraints[1].fixed = 0;
    paramContraints[1].limited[0] = 0;

    params[1] = *startMean;

    //amplitude
    paramContraints[2].fixed = 0;
    paramContraints[2].limited[0] = 0;

    params[2] = *amplitude;

    //returned parameter errors:
    double paramErrors[paramCnt];
    double *finalResiduals = new double[(const int)dataVec.size()];

    //reset result struct:
    mp_result result;
    memset(&result,0,sizeof(result));

    result.xerror = paramErrors;
    result.resid  = finalResiduals;

    //reset config struct:
    mp_config config;
    memset(&config,0,sizeof(config));

    config.maxiter = numberOfIter;

    //run the fit:
    int stat = mpfit(gaussian,
                             dataVec.size(),
                             paramCnt,
                             params,
                             paramContraints,
                             &config,
                             (void*) &v,
                             &result);

    DUNUSED_PARAM(stat);

    getOutputData(visData, &v, params, numberOfSplinePoints);

    delete [] x;
    delete [] y;
    delete [] ey;
    delete [] finalResiduals;


    *returnValue = PALSFitErrorCodeStringBuilder::errorString(stat);

    if ( stat >= 1 && stat <= 8 ) //error!
        return true;
    else
        return false;
}

bool FitEngine::runGaussianWithBkgrd(const QVector<int> &dataVec, int numberOfIter, int numberOfSplinePoints, double *startSigma, double *startMean, double *amplitude, double *bkgrd, QVector<QPointF> *visData, QString *returnValue)
{
    if ( !startSigma || !startMean || !amplitude || !bkgrd || !returnValue )
        return false;

    if ( dataVec.isEmpty() )
        return false;


    //initialize the data-set:
    const int paramCnt = 4;

     double *x = new double[(const int)dataVec.size()];
     double *y = new double[(const int)dataVec.size()];
     double *ey = new double[(const int)dataVec.size()];

     //copy to array:
     int i = 0;
     for ( int yVal : dataVec )
     {
          x[i] = i;
          y[i] = (double)yVal;
          ey[i] = sqrtf((double)yVal);

          i ++;
     }

     values v;

     v.dataCnt = dataVec.size();
     v.x = x;
     v.y = y;
     v.ey = ey;
     v.weighting = residualWeighting::no_Weighting;

    //reset param-contraints:
    mp_par paramContraints[paramCnt];
    memset(&paramContraints[0], 0, sizeof(paramContraints));

    //params initial conditions:
    double params[paramCnt];

    //sigma
    paramContraints[0].fixed = 0;
    paramContraints[0].limited[0] = 0;

    params[0] = *startSigma;

    //e0
    paramContraints[1].fixed = 0;
    paramContraints[1].limited[0] = 0;

    params[1] = *startMean;

    //amplitude
    paramContraints[2].fixed = 0;
    paramContraints[2].limited[0] = 0;

    params[2] = *amplitude;

    //bkgrd
    paramContraints[3].fixed = 1;
    paramContraints[3].limited[0] = 0;

    params[3] = *bkgrd;

    //returned parameter errors:
    double paramErrors[paramCnt];
    double *finalResiduals = new double[(const int)dataVec.size()];

    //reset result struct:
    mp_result result;
    memset(&result,0,sizeof(result));

    result.xerror = paramErrors;
    result.resid  = finalResiduals;

    //reset config struct:
    mp_config config;
    memset(&config,0,sizeof(config));

    config.maxiter = numberOfIter;

    //run the fit:
    int stat = mpfit(gaussianWithBkgrd,
                             dataVec.size(),
                             paramCnt,
                             params,
                             paramContraints,
                             &config,
                             (void*) &v,
                             &result);

    DUNUSED_PARAM(stat);

    getOutputDataWithBkgrd(visData, &v, params, numberOfSplinePoints);

    delete [] x;
    delete [] y;
    delete [] ey;
    delete [] finalResiduals;


    *returnValue = PALSFitErrorCodeStringBuilder::errorString(stat);

    if ( stat >= 1 && stat <= 8 ) //error!
        return true;
    else
        return false;
}

void FitEngine::getOutputData(QVector<QPointF> *visData, values *v, double *params, int numberOfSplinePoints)
{
    visData->clear();

    int stepsInBetween = numberOfSplinePoints;
    const double step = (v->x[1]-v->x[0])/stepsInBetween;

    for ( int i = 0 ; i < (v->dataCnt*stepsInBetween)-1 ; ++ i )
    {
            const double xVal = v->x[0] + (double)i*(double)step;

            const double u = 1.0f/(sqrtf(2.0*PI)*params[0]);
            double yVal = params[2]*u*exp(-0.5*((xVal-params[1])/params[0])*((xVal-params[1])/params[0]));

            visData->append(QPointF(xVal, yVal));
    }
}

void FitEngine::getOutputDataWithBkgrd(QVector<QPointF> *visData, values *v, double *params, int numberOfSplinePoints)
{
    visData->clear();

    int stepsInBetween = numberOfSplinePoints;
    const double step = (v->x[1]-v->x[0])/stepsInBetween;

    for ( int i = 0 ; i < (v->dataCnt*stepsInBetween)-1 ; ++ i )
    {
            const double xVal = v->x[0] + (double)i*(double)step;

            const double u = 1.0f/(sqrtf(2.0*PI)*params[0]);
            double yVal = params[3] + params[2]*u*exp(-0.5*((xVal-params[1])/params[0])*((xVal-params[1])/params[0]));

            visData->append(QPointF(xVal, yVal));
    }
}

