#include "drs4cfdalgorithmdlg.h"
#include "ui_drs4cfdalgorithmdlg.h"

DRS4CFDAlgorithmDlg::DRS4CFDAlgorithmDlg(QWidget *parent) :
    QWidget(parent),
    m_worker(nullptr),
    ui(new Ui::DRS4CFDAlgorithmDlg)
{
    ui->setupUi(this);

    ui->comboBox_cfdAlgorithm->addItems(DRS4InterpolationType::typeList());
    ui->comboBox_interpolationType->addItems(DRS4SplineInterpolationType::typeList());

    ui->spinBox_renderPoints_polynomial->setRange(2, 10000);
    ui->spinBox_renderPoints_spline->setRange(2, 10000);

    connect(ui->comboBox_cfdAlgorithm, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCFDAlgorithm(int)));
    connect(ui->comboBox_interpolationType, SIGNAL(currentIndexChanged(int)), this, SLOT(changeInterpolationType(int)));
    connect(ui->spinBox_renderPoints_polynomial, SIGNAL(valueChanged(int)), this, SLOT(changeRenderPointsForPolynomialInterpolation(int)));
    connect(ui->spinBox_renderPoints_spline, SIGNAL(valueChanged(int)), this, SLOT(changeRenderPointsForSplineInterpolation(int)));

    ui->comboBox_cfdAlgorithm->setCurrentIndex(0);
    ui->comboBox_interpolationType->setCurrentIndex(0);

    ui->spinBox_renderPoints_polynomial->setValue(10);
    ui->spinBox_renderPoints_spline->setValue(10);
}

DRS4CFDAlgorithmDlg::~DRS4CFDAlgorithmDlg()
{
    DDELETE_SAFETY(ui);
}

void DRS4CFDAlgorithmDlg::load()
{
    disconnect(ui->spinBox_renderPoints_polynomial, SIGNAL(valueChanged(int)), this, SLOT(changeRenderPointsForPolynomialInterpolation(int)));
    disconnect(ui->spinBox_renderPoints_spline, SIGNAL(valueChanged(int)), this, SLOT(changeRenderPointsForSplineInterpolation(int)));

    ui->spinBox_renderPoints_polynomial->setValue(DRS4SettingsManager::sharedInstance()->polynomialSamplingCounts());
    ui->spinBox_renderPoints_spline->setValue(DRS4SettingsManager::sharedInstance()->splineIntraSamplingCounts());

    connect(ui->spinBox_renderPoints_polynomial, SIGNAL(valueChanged(int)), this, SLOT(changeRenderPointsForPolynomialInterpolation(int)));
    connect(ui->spinBox_renderPoints_spline, SIGNAL(valueChanged(int)), this, SLOT(changeRenderPointsForSplineInterpolation(int)));

    ui->comboBox_interpolationType->setCurrentIndex(((int)DRS4SettingsManager::sharedInstance()->splineInterpolationType())-1);
    ui->comboBox_cfdAlgorithm->setCurrentIndex(((int)DRS4SettingsManager::sharedInstance()->interpolationType())-1);
}

void DRS4CFDAlgorithmDlg::changeCFDAlgorithm(int algorithm)
{
    if (!m_worker)
        return;

    QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    ui->stackedWidget->setCurrentIndex(algorithm);
    DRS4SettingsManager::sharedInstance()->setInterpolationType((DRS4InterpolationType::type)(algorithm+1));

    if ((ui->comboBox_interpolationType->currentIndex()+1) == DRS4SplineInterpolationType::type::linear)
        ui->spinBox_renderPoints_spline->setEnabled(false);
    else
        ui->spinBox_renderPoints_spline->setEnabled(true);

    m_worker->setBusy(false);
}

void DRS4CFDAlgorithmDlg::changeInterpolationType(int type)
{
    if (!m_worker)
        return;

    QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setSplineInterpolationType((DRS4SplineInterpolationType::type)(type+1));

    if ((type+1) == DRS4SplineInterpolationType::type::linear)
        ui->spinBox_renderPoints_spline->setEnabled(false);
    else
        ui->spinBox_renderPoints_spline->setEnabled(true);

    m_worker->setBusy(false);
}

void DRS4CFDAlgorithmDlg::changeRenderPointsForPolynomialInterpolation(int count)
{
    if (!m_worker)
        return;

    QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setPolynomialSamplingCounts(count);

    m_worker->setBusy(false);
}

void DRS4CFDAlgorithmDlg::changeRenderPointsForSplineInterpolation(int count)
{
    if (!m_worker)
        return;

    QMutexLocker locker(&m_mutex);

    m_worker->setBusy(true);

    while(!m_worker->isBlocking()) {}

    DRS4SettingsManager::sharedInstance()->setSplineIntraSamplingCounts(count);

    m_worker->setBusy(false);
}
