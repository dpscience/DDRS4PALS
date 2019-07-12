/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2019 Danny Petschke
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

#include "settings.h"

PALSProject::PALSProject()
{
    m_rootNode = new DSimpleXMLNode("project");
    m_parentNode = new DSimpleXMLNode("data-structure");

    m_lastSaveTimeNode = new DSimpleXMLNode("last-save-date_time");
    m_projectNameNode = new DSimpleXMLNode("title");
    m_asciiDataNameNode = new DSimpleXMLNode("ascii-data");

    m_lastSaveTimeNode->setValue(QDateTime::currentDateTime());
    m_projectNameNode-> setValue("undefined project-title");
    m_asciiDataNameNode->setValue("unknown");

    *m_rootNode << m_lastSaveTimeNode << m_projectNameNode << m_asciiDataNameNode << m_parentNode;
}

PALSProject::~PALSProject()
{
    //clear all old project settings:
    while ( m_dataStructureList.size() > 0 )
    {
        PALSDataStructure* stru = m_dataStructureList.takeFirst();
        DDELETE_SAFETY(stru);
    }

    m_dataStructureList.clear();

    DDELETE_SAFETY(m_lastSaveTimeNode);
    DDELETE_SAFETY(m_projectNameNode);
    DDELETE_SAFETY(m_asciiDataNameNode);
    DDELETE_SAFETY(m_parentNode);
    DDELETE_SAFETY(m_rootNode);
}

bool PALSProject::save(const DString &projectPath)
{
    setLastSaveTime(QDateTime::currentDateTime());

    DSimpleXMLWriter xmlWriter(projectPath);

#ifdef QT_DEBUG
    //m_rootNode->XMLMessageBox();
#endif

    return xmlWriter.writeToFile(m_rootNode, false);
}

bool PALSProject::load(const DString &projectPath)
{
    DSimpleXMLReader xmlReader(projectPath);
    DSimpleXMLTag projectContent;

    if ( xmlReader.readFromFile(&projectContent) )
    {
        clear();

#ifdef QT_DEBUG
    //projectContent.XMLMessageBox();
#endif

        bool ok = false;
        DSimpleXMLTag safeTag = projectContent.getTag("project").getTag(m_lastSaveTimeNode, &ok);

        if ( ok ) setLastSaveTime(safeTag.getValue().toDateTime());
        else      setLastSaveTime(QDateTime::currentDateTime());

        safeTag = projectContent.getTag("project").getTag(m_projectNameNode, &ok);

        if ( ok ) setName(safeTag.getValue().toString());
        else      setName(DString("undefined project-title"));

        safeTag = projectContent.getTag("project").getTag(m_asciiDataNameNode, &ok);

        if ( ok ) setASCIIDataName(safeTag.getValue().toString());
        else      setASCIIDataName(DString("unknown"));

        ok = true;
        int dataStructCnt = 0;

        do
        {
            const DString name = DString("PALS_ID_" + QVariant(dataStructCnt).toString());
            const DSimpleXMLTag tag = projectContent.getTag("project").getTag("data-structure").getTag(name, &ok);
            DUNUSED_PARAM(tag);

            if ( ok ){
                PALSDataStructure *dStr = new PALSDataStructure(this, projectContent.getTag("project").getTag("data-structure"), name);
                DUNUSED_PARAM(dStr);
            }

            dataStructCnt ++;
        } while ( ok );
    }
    else
        return false;


    return true;
}

unsigned int PALSProject::getSize() const
{
    return (unsigned int)m_dataStructureList.size();
}

DSimpleXMLNode *PALSProject::getParent() const
{
    return m_parentNode;
}

void PALSProject::setLastSaveTime(const QDateTime &dateTime)
{
    m_lastSaveTimeNode->setValue(dateTime);
}

void PALSProject::setName(const DString &name)
{
    m_projectNameNode->setValue(name);
}

void PALSProject::setASCIIDataName(const DString &name)
{
    m_asciiDataNameNode->setValue(name);
}

void PALSProject::addDataStructure(PALSDataStructure *dataStructure)
{
    if ( dataStructure )
        m_dataStructureList.append(dataStructure);
}

void PALSProject::removeDataStructure(unsigned int index)
{
    if ( index >= getSize() )
        return;


    PALSDataStructure* stru = m_dataStructureList.takeAt(index);
    DDELETE_SAFETY(stru);

    for ( unsigned int i = 0 ; i < getSize() ; ++ i ){
        m_dataStructureList[i]->getParent()->setNodeName("PALS_ID_" + QVariant(i).toString());
    }
}

void PALSProject::removeDataStructure(PALSDataStructure *dataStructure)
{
    if ( !dataStructure )
        return;


    const int index = m_dataStructureList.indexOf(dataStructure);

    if ( index > -1 )
        removeDataStructure(index);
}

PALSDataStructure *PALSProject::operator[](unsigned int index) const
{
    if ( index >= getSize() )
        return nullptr;
    else
        return m_dataStructureList[index];
}

PALSDataStructure *PALSProject::getDataStructureAt(unsigned int index) const
{
    if ( index >= getSize() )
        return nullptr;
    else
        return m_dataStructureList[index];
}

QDateTime PALSProject::getLastSaveTime() const
{
    return m_lastSaveTimeNode->getValue().toDateTime();
}

DString PALSProject::getName() const
{
    return (DString)m_projectNameNode->getValue().toString();
}

DString PALSProject::getASCIIDataName() const
{
    return (DString)m_asciiDataNameNode->getValue().toString();
}

void PALSProject::clear()
{
    //clear all old project settings:
    while ( m_dataStructureList.size() > 0 ) {
        //qDebug() << "hallo";
        PALSDataStructure* stru = m_dataStructureList.takeFirst();
        DDELETE_SAFETY(stru);
    }

    m_dataStructureList.clear();

    DDELETE_SAFETY(m_lastSaveTimeNode);
    DDELETE_SAFETY(m_projectNameNode);
    DDELETE_SAFETY(m_asciiDataNameNode);
    DDELETE_SAFETY(m_parentNode);
    DDELETE_SAFETY(m_rootNode);

    m_rootNode  = new DSimpleXMLNode("project");
    m_parentNode = new DSimpleXMLNode("data-structure");
    m_lastSaveTimeNode = new DSimpleXMLNode("last-save-date_time");
    m_projectNameNode = new DSimpleXMLNode("title");
    m_asciiDataNameNode = new DSimpleXMLNode("ascii-data");

    m_lastSaveTimeNode->setValue(QDateTime::currentDateTime());
    m_projectNameNode ->setValue("undefined project-title");
    m_asciiDataNameNode->setValue("unknown");

    *m_rootNode << m_lastSaveTimeNode << m_projectNameNode << m_asciiDataNameNode << m_parentNode;
}


PALSDataStructure::PALSDataStructure(PALSProject *parent)
{
    m_parentNode = new DSimpleXMLNode("PALS_ID_" + QVariant(parent->getSize()).toString());

    m_nameNode = new DSimpleXMLNode("name");

    m_dataSet = new PALSDataSet(this);
    m_fitSet = new PALSFitSet (this);

    m_nameNode->setValue("new project");

    *m_parentNode << m_nameNode;
    *(parent->getParent()) << m_parentNode;

    parent->addDataStructure(this);
}

PALSDataStructure::PALSDataStructure(PALSProject *parent, const DSimpleXMLTag &tag, const DString &name)
{
    m_parentNode = new DSimpleXMLNode(name);

    m_nameNode = new DSimpleXMLNode("name");

    m_dataSet = new PALSDataSet(this, tag.getTag(name));
    m_fitSet = new PALSFitSet (this, tag.getTag(name));

    bool ok = false;

    DSimpleXMLTag safeTag = tag.getTag(m_parentNode).getTag("name", &ok);

    if ( ok ) m_nameNode->setValue(safeTag.getValue());
    else m_nameNode->setValue("new project");

    *m_parentNode << m_nameNode;
    *(parent->getParent()) << m_parentNode;

    parent->addDataStructure(this);
}

PALSDataStructure::~PALSDataStructure()
{
    DDELETE_SAFETY(m_dataSet);
    DDELETE_SAFETY(m_fitSet);
    DDELETE_SAFETY(m_nameNode);
    DDELETE_SAFETY(m_parentNode);
}

DSimpleXMLNode *PALSDataStructure::getParent() const
{
    return m_parentNode;
}

void PALSFitSet::setChannelResolution(double resolution)
{
    m_channelResolutionNode->setValue(resolution);
}

void PALSFitSet::setStartChannel(int startChannel)
{
    m_startChannelNode->setValue(startChannel);
}

void PALSFitSet::setStopChannel(int stopChannel)
{
    m_stopChannelNode->setValue(stopChannel);
}

void PALSFitSet::setAverageLifeTime(double avgLifeTime)
{
    m_averageLifeTimeNode->setValue(avgLifeTime);
}

void PALSFitSet::setAverageLifeTimeError(double avgLtError)
{
    m_averageLifeTimeErrorNode->setValue(avgLtError);
}

void PALSFitSet::setCountsInRange(int countsInRange)
{
    m_countsInRangeNode->setValue(countsInRange);
}

void PALSFitSet::setFitFinishCode(const QString &finishCode)
{
    m_fitFinishCodeNode->setValue(finishCode);
}

void PALSFitSet::setFitFinishCodeValue(int value)
{
    m_fitFinishCodeValueNode->setValue(value);
}

void PALSFitSet::setTimeStampOfLastFitResult(const QString &timeStamp)
{
    m_dateTimeOfLastFitResultsNode->setValue(timeStamp);
}

void PALSFitSet::setPeakToBackgroundRatio(double ratio)
{
    m_peakToBackgroundRatioNode->setValue(ratio);
}

void PALSFitSet::setSumOfIntensities(double sum)
{
    m_sumOfIntensitiesNode->setValue(sum);
}

void PALSFitSet::setErrorSumOfIntensities(double sum)
{
    m_sumErrorOfIntensitiesNode->setValue(sum);
}

void PALSFitSet::setDataPlotImage(const QImage &image)
{
    QByteArray arr;
    QBuffer buffer(&arr);

    if ( buffer.open(QIODevice::WriteOnly) )
    {
        image.save(&buffer, "PNG");

        m_dataPlotImageNode->setValue(arr);
    }
    else
        m_dataPlotImageNode->setValue("");
}

void PALSFitSet::setResidualPlotImage(const QImage &image)
{
    QByteArray arr;
    QBuffer buffer(&arr);

    if ( buffer.open(QIODevice::WriteOnly) )
    {
        image.save(&buffer, "PNG");

        m_residualPlotImageNode->setValue(arr);
    }
    else
        m_residualPlotImageNode->setValue("");
}

void PALSFitSet::setSpectralCentroid(double center)
{
    m_spectralCentroidNode->setValue(center);
}

void PALSFitSet::setTZeroSpectralCentroid(double center)
{
    m_t0spectralCentroidNode->setValue(center);
}

double PALSFitSet::getChannelResolution() const
{
   return m_channelResolutionNode->getValue().toDouble();
}

int PALSFitSet::getStartChannel() const
{
    return m_startChannelNode->getValue().toInt();
}

int PALSFitSet::getStopChannel() const
{
    return m_stopChannelNode->getValue().toInt();
}

double PALSFitSet::getAverageLifeTime() const
{
    return m_averageLifeTimeNode->getValue().toDouble();
}

double PALSFitSet::getAverageLifeTimeError() const
{
    return m_averageLifeTimeErrorNode->getValue().toDouble();
}

int PALSFitSet::getCountsInRange() const
{
    return m_countsInRangeNode->getValue().toInt();
}

QString PALSFitSet::getFitFinishCode() const
{
    return m_fitFinishCodeNode->getValue().toString();
}

int PALSFitSet::getFitFinishCodeValue() const
{
    return m_fitFinishCodeValueNode->getValue().toInt();
}

QString PALSFitSet::getTimeStampOfLastFitResult() const
{
    return m_dateTimeOfLastFitResultsNode->getValue().toString();
}

double PALSFitSet::getPeakToBackgroundRation() const
{
    return m_peakToBackgroundRatioNode->getValue().toDouble();
}

double PALSFitSet::getSumOfIntensities() const
{
    return m_sumOfIntensitiesNode->getValue().toDouble();
}

double PALSFitSet::getErrorSumOfIntensities() const
{
    return m_sumErrorOfIntensitiesNode->getValue().toDouble();
}

QImage PALSFitSet::getDataPlotImage() const
{
    const QByteArray img = m_dataPlotImageNode->getValue().toByteArray();

    return QImage::fromData(img, "PNG");
}

QImage PALSFitSet::getResidualPlotImage() const
{
    const QByteArray img = m_residualPlotImageNode->getValue().toByteArray();

    return QImage::fromData(img, "PNG");
}

double PALSFitSet::getSpectralCentroid() const
{
    bool ok = false;
    double center =  m_spectralCentroidNode->getValue().toDouble(&ok);
    if (!ok)
        center = 0;

    return center;
}

double PALSFitSet::getT0SpectralCentroid() const
{
    bool ok = false;
    double center =  m_t0spectralCentroidNode->getValue().toDouble(&ok);
    if (!ok)
        center = 0;

    return center;
}

PALSDataSet *PALSDataStructure::getDataSetPtr() const
{
    return m_dataSet;
}

PALSFitSet *PALSDataStructure::getFitSetPtr() const
{
    return m_fitSet;
}

void PALSDataStructure::setName(const DString &name)
{
    m_nameNode->setValue(name);
}

DString PALSDataStructure::getName() const
{
    return (DString)m_nameNode->getValue().toString();
}

PALSDataSet::PALSDataSet(PALSDataStructure *parent)
{
    m_parentNode = new DSimpleXMLNode("data");
    m_xyDataNode = new DSimpleXMLNode("lt-data");
    m_xyRawDataNode = new DSimpleXMLNode("lt-data-raw");
    m_fitDataNode = new DSimpleXMLNode("lt-fit-data");
    m_residualNode = new DSimpleXMLNode("lt-residuals");
    m_colorResidualsNode = new DSimpleXMLNode("residuals-color");
    m_colorDataNode = new DSimpleXMLNode("lt-color");
    m_xyDataBinFac = new DSimpleXMLNode("bin-factor");

    m_colorResidualsNode->setValue(QColor(Qt::red) .colorNames().at(0));
    m_colorDataNode->setValue(QColor(Qt::blue).colorNames().at(0));
    m_xyDataBinFac->setValue(1);

    *m_parentNode << m_xyRawDataNode << m_xyDataNode << m_xyDataBinFac << m_fitDataNode << m_residualNode << m_colorDataNode << m_colorResidualsNode;
    *(parent->getParent()) << m_parentNode;
}

PALSDataSet::PALSDataSet(PALSDataStructure *parent, const DSimpleXMLTag &tag)
{
    m_parentNode = new DSimpleXMLNode("data");
    m_xyDataNode = new DSimpleXMLNode("lt-data");
    m_xyRawDataNode = new DSimpleXMLNode("lt-data-raw");
    m_fitDataNode = new DSimpleXMLNode("lt-fit-data");
    m_residualNode = new DSimpleXMLNode("lt-residuals");
    m_colorResidualsNode = new DSimpleXMLNode("residuals-color");
    m_colorDataNode = new DSimpleXMLNode("lt-color");
    m_xyDataBinFac = new DSimpleXMLNode("bin-factor");

    bool ok = false;

    DSimpleXMLTag safeTag = tag.getTag("data").getTag("lt-data", &ok);
    if ( ok ) m_xyDataNode->setValue(safeTag.getValue());

    safeTag = tag.getTag("data").getTag("lt-data-raw", &ok);
    if ( ok ) m_xyRawDataNode->setValue(safeTag.getValue());

    safeTag = tag.getTag("data").getTag("bin-factor", &ok);
    if ( ok ) m_xyDataBinFac->setValue(safeTag.getValue());
    else m_xyDataBinFac->setValue(1);

    safeTag = tag.getTag("data").getTag("lt-fit-data", &ok);
    if ( ok ) m_fitDataNode->setValue(safeTag.getValue());

    safeTag = tag.getTag("data").getTag("lt-residuals", &ok);
    if ( ok ) m_residualNode->setValue(safeTag.getValue());

    safeTag = tag.getTag("data").getTag("residuals-color", &ok);
    if ( ok ) m_colorResidualsNode->setValue(safeTag.getValue());
    else m_colorResidualsNode->setValue(QColor(Qt::red).colorNames().at(0));

    safeTag = tag.getTag("data").getTag("lt-color", &ok);
    if ( ok ) m_colorDataNode->setValue(safeTag.getValue());
    else m_colorDataNode->setValue(QColor(Qt::blue).colorNames().at(0));


    const QStringList xyDataStringList = DString(m_xyDataNode->getValue().toString()).parseBetween2("{", "}");
    const QStringList xyRawDataStringList = DString(m_xyRawDataNode->getValue().toString()).parseBetween2("{", "}");
    const QStringList xyFitDataStringList = DString(m_fitDataNode->getValue().toString()).parseBetween2("{", "}");
    const QStringList residualDataStringList = DString(m_residualNode->getValue().toString()).parseBetween2("{", "}");

    m_xyData.clear();
    m_xyRawData.clear();
    m_fitData.clear();
    m_residualData.clear();

    for ( int i = 0 ; i < xyDataStringList.size() ; ++ i )
    {
        const QStringList list = xyDataStringList.at(i).split("|");

        if ( list.size() == 2 )
        {
            const double x = list.at(0).toDouble();
            const double y = list.at(1).toDouble();

            m_xyData.append(QPointF(x, y));
        }
    }

    for ( int i = 0 ; i < xyRawDataStringList.size() ; ++ i )
    {
        const QStringList list = xyRawDataStringList.at(i).split("|");

        if ( list.size() == 2 )
        {
            const double x = list.at(0).toDouble();
            const double y = list.at(1).toDouble();

            m_xyRawData.append(QPointF(x, y));
        }
    }

    for ( int i = 0 ; i < xyFitDataStringList.size() ; ++ i )
    {
        const QStringList list = xyFitDataStringList.at(i).split("|");

        if ( list.size() == 2 )
        {
            const double x = list.at(0).toDouble();
            const double y = list.at(1).toDouble();

            m_fitData.append(QPointF(x, y));
        }
    }

    for ( int i = 0 ; i < residualDataStringList.size() ; ++ i )
    {
        const QStringList list = residualDataStringList.at(i).split("|");

        if ( list.size() == 2 )
        {
            const double x = list.at(0).toDouble();
            const double y = list.at(1).toDouble();

            m_residualData.append(QPointF(x, y));
        }
    }

    *m_parentNode << m_xyRawDataNode << m_xyDataNode << m_xyDataBinFac << m_fitDataNode << m_residualNode << m_colorDataNode << m_colorResidualsNode;
    (*parent->getParent()) << m_parentNode;
}

PALSDataSet::~PALSDataSet()
{
    DDELETE_SAFETY(m_xyDataNode);
    DDELETE_SAFETY(m_xyRawDataNode);
    DDELETE_SAFETY(m_xyDataBinFac);
    DDELETE_SAFETY(m_fitDataNode);
    DDELETE_SAFETY(m_residualNode);
    DDELETE_SAFETY(m_colorResidualsNode);
    DDELETE_SAFETY(m_colorDataNode);
    DDELETE_SAFETY(m_parentNode);
}

DSimpleXMLNode *PALSDataSet::getParent() const
{
    return m_parentNode;
}

void PALSDataSet::clearFitData()
{
    m_xyData.clear();
    m_xyDataNode->setValue("");

    m_xyRawData.clear();
    m_xyRawDataNode->setValue("");
}

void PALSDataSet::clearResidualData()
{
    m_residualData.clear();
    m_residualNode->setValue("");
}

void PALSDataSet::setLifeTimeData(const QList<QPointF> &dataSet)
{
    m_xyData = dataSet;

    DString dataString;
    for ( int i = 0 ; i < m_xyData.size() ; ++ i )
        dataString += "{" + QVariant(m_xyData[i].x()).toString() + "|" + QVariant(m_xyData[i].y()).toString() + "}";

    m_xyDataNode->setValue(dataString);
}

void PALSDataSet::setLifeTimeRawData(const QList<QPointF> &rawDataSet)
{
    m_xyRawData = rawDataSet;

    DString dataString;
    for ( int i = 0 ; i < m_xyRawData.size() ; ++ i )
        dataString += "{" + QVariant(m_xyRawData[i].x()).toString() + "|" + QVariant(m_xyRawData[i].y()).toString() + "}";

    m_xyRawDataNode->setValue(dataString);
}

void PALSDataSet::setFitData(const QList<QPointF> &dataSet)
{
    m_fitData = dataSet;

    DString dataString;
    for ( int i = 0 ; i < m_fitData.size() ; ++ i )
        dataString += "{" + QVariant(m_fitData[i].x()).toString() + "|" + QVariant(m_fitData[i].y()).toString() + "}";

    m_fitDataNode->setValue(dataString);
}

void PALSDataSet::setResiduals(const QList<QPointF> &residuals)
{
    m_residualData = residuals;

    DString dataString;
    for ( int i = 0 ; i < m_residualData.size() ; ++ i )
        dataString += "{" + QVariant(m_residualData[i].x()).toString() + "|" + QVariant(m_residualData[i].y()).toString() + "}";

    m_residualNode->setValue(dataString);
}

void PALSDataSet::setLifeTimeDataColor(const DColor &color)
{
    m_colorDataNode->setValue(color.colorNames().at(0));
}

void PALSDataSet::setResidualsColor(const DColor &color)
{
    m_colorResidualsNode->setValue(color.colorNames().at(0));
}

void PALSDataSet::setBinFactor(unsigned int binFac)
{
    m_xyDataBinFac->setValue(binFac);
}

QList<QPointF> PALSDataSet::getLifeTimeData() const
{
    return m_xyData;
}

QList<QPointF> PALSDataSet::getLifeTimeRawData() const
{
    return m_xyRawData;
}

QList<QPointF> PALSDataSet::getFitData() const
{
    return m_fitData;
}

QList<QPointF> PALSDataSet::getResiduals() const
{
    return m_residualData;
}

DColor PALSDataSet::getLifeTimeDataColor() const
{
    return (DColor)QColor(m_colorDataNode->getValue().toString());
}

DColor PALSDataSet::getResidualsColor() const
{
    return (DColor)QColor(m_colorResidualsNode->getValue().toString());
}

unsigned int PALSDataSet::getBinFactor() const
{
    bool ok = false;
    int fac = m_xyDataBinFac->getValue().toInt(&ok);
    if (!ok)
        fac = 1;

    return fac;
}


PALSFitSet::PALSFitSet(PALSDataStructure *parent)
{
    m_parentNode = new DSimpleXMLNode("fit");

    m_maxIterationsNode = new DSimpleXMLNode("max-iterations");
    m_neededIterationsNode = new DSimpleXMLNode("needed-iterations");
    m_chiSquareOnStart = new DSimpleXMLNode("reduced-chi-square-start");
    m_chiSquareAfterFit = new DSimpleXMLNode("reduced-chi-square-fit");
    m_channelResolutionNode = new DSimpleXMLNode("channel-resolution");
    m_startChannelNode = new DSimpleXMLNode("start-channel");
    m_stopChannelNode = new DSimpleXMLNode("stop-channel");
    m_averageLifeTimeNode = new DSimpleXMLNode("average-lt");
    m_averageLifeTimeErrorNode = new DSimpleXMLNode("average-lt-error");
    m_countsInRangeNode = new DSimpleXMLNode("counts-in-range");
    m_dateTimeOfLastFitResultsNode = new DSimpleXMLNode("last-date-time-of-fit-result");
    m_fitFinishCodeNode = new DSimpleXMLNode("last-fit-finish-code");
    m_fitFinishCodeValueNode = new DSimpleXMLNode("last-fit-finish-code-value");
    m_peakToBackgroundRatioNode = new DSimpleXMLNode("peak-to-background-ratio");
    m_sumOfIntensitiesNode = new DSimpleXMLNode("sum-of-intensities");
    m_sumErrorOfIntensitiesNode = new DSimpleXMLNode("sum-error-of-intensities");
    m_dataPlotImageNode = new DSimpleXMLNode("data-plot-raw-image");
    m_residualPlotImageNode = new DSimpleXMLNode("residual-plot-raw-image");
    m_spectralCentroidNode = new DSimpleXMLNode("spectral-centroid");
    m_t0spectralCentroidNode = new DSimpleXMLNode("t0-spectral-centroid");

    m_sourceParams = new PALSSourceParameter(this);
    m_deviceResolutionParams = new PALSDeviceResolutionParameter(this);
    m_bgParam = new PALSBackgroundParameter(this);
    m_lifeTimeParams = new PALSLifeTimeParameter(this);
    m_resultHistorie = new PALSResultHistorie(this);

    m_maxIterationsNode->setValue(200);
    m_neededIterationsNode->setValue(0);
    m_chiSquareOnStart->setValue(0);
    m_chiSquareAfterFit->setValue(0);
    m_channelResolutionNode->setValue(0.0f);
    m_startChannelNode->setValue(0);
    m_stopChannelNode->setValue(1024);
    m_averageLifeTimeNode->setValue(0.0f);
    m_averageLifeTimeErrorNode->setValue(0.0f);
    m_countsInRangeNode->setValue(0);
    m_dateTimeOfLastFitResultsNode->setValue("");
    m_fitFinishCodeNode->setValue("");
    m_fitFinishCodeValueNode->setValue(-1);
    m_peakToBackgroundRatioNode->setValue(0.0f);
    m_sumOfIntensitiesNode->setValue(0.0f);
    m_sumErrorOfIntensitiesNode->setValue(0.0f);
    m_dataPlotImageNode->setValue("");
    m_residualPlotImageNode->setValue("");
    m_spectralCentroidNode->setValue(0.0f);
    m_t0spectralCentroidNode->setValue(0.0f);


    *m_parentNode << m_maxIterationsNode  << m_neededIterationsNode << m_t0spectralCentroidNode << m_spectralCentroidNode << m_chiSquareOnStart << m_chiSquareAfterFit << m_channelResolutionNode << m_startChannelNode << m_stopChannelNode << m_averageLifeTimeNode << m_averageLifeTimeErrorNode << m_countsInRangeNode << m_dateTimeOfLastFitResultsNode << m_fitFinishCodeNode << m_fitFinishCodeValueNode << m_peakToBackgroundRatioNode << m_sumOfIntensitiesNode << m_sumErrorOfIntensitiesNode << m_dataPlotImageNode << m_residualPlotImageNode;
    *(parent->getParent()) << m_parentNode;
}

PALSFitSet::PALSFitSet(PALSDataStructure *parent, const DSimpleXMLTag &tag)
{
    m_parentNode = new DSimpleXMLNode("fit");

    m_maxIterationsNode = new DSimpleXMLNode("max-iterations");
    m_neededIterationsNode = new DSimpleXMLNode("needed-iterations");
    m_chiSquareOnStart = new DSimpleXMLNode("reduced-chi-square-start");
    m_chiSquareAfterFit = new DSimpleXMLNode("reduced-chi-square-fit");
    m_channelResolutionNode = new DSimpleXMLNode("channel-resolution");
    m_startChannelNode = new DSimpleXMLNode("start-channel");
    m_stopChannelNode = new DSimpleXMLNode("stop-channel");
    m_averageLifeTimeNode = new DSimpleXMLNode("average-lt");
    m_averageLifeTimeErrorNode = new DSimpleXMLNode("average-lt-error");
    m_countsInRangeNode = new DSimpleXMLNode("counts-in-range");
    m_dateTimeOfLastFitResultsNode = new DSimpleXMLNode("last-date-time-of-fit-result");
    m_fitFinishCodeNode = new DSimpleXMLNode("last-fit-finish-code");
    m_fitFinishCodeValueNode = new DSimpleXMLNode("last-fit-finish-code-value");
    m_peakToBackgroundRatioNode = new DSimpleXMLNode("peak-to-background-ratio");
    m_sumOfIntensitiesNode = new DSimpleXMLNode("sum-of-intensities");
    m_sumErrorOfIntensitiesNode = new DSimpleXMLNode("sum-error-of-intensities");
    m_dataPlotImageNode = new DSimpleXMLNode("data-plot-raw-image");
    m_residualPlotImageNode = new DSimpleXMLNode("residual-plot-raw-image");
    m_spectralCentroidNode = new DSimpleXMLNode("spectral-centroid");
    m_t0spectralCentroidNode = new DSimpleXMLNode("t0-spectral-centroid");

    m_sourceParams = new PALSSourceParameter(this, tag.getTag("fit"));
    m_deviceResolutionParams = new PALSDeviceResolutionParameter(this, tag.getTag("fit"));
    m_bgParam = new PALSBackgroundParameter(this, tag.getTag("fit"));
    m_lifeTimeParams = new PALSLifeTimeParameter(this, tag.getTag("fit"));
    m_resultHistorie = new PALSResultHistorie(this, tag.getTag("fit"));

    bool ok = false;

    DSimpleXMLTag safeTag = tag.getTag("fit").getTag("max-iterations", &ok);

    if ( ok ) m_maxIterationsNode->setValue(safeTag.getValue());
    else      m_maxIterationsNode->setValue(200);

    safeTag = tag.getTag("fit").getTag("needed-iterations", &ok);

    if ( ok ) m_neededIterationsNode->setValue(safeTag.getValue());
    else      m_neededIterationsNode->setValue(0);

    safeTag = tag.getTag("fit").getTag("spectral-centroid", &ok);

    if ( ok ) m_spectralCentroidNode->setValue(safeTag.getValue());
    else      m_spectralCentroidNode->setValue(0.0f);

    safeTag = tag.getTag("fit").getTag("t0-spectral-centroid", &ok);

    if ( ok ) m_t0spectralCentroidNode->setValue(safeTag.getValue());
    else      m_t0spectralCentroidNode->setValue(0.0f);

    safeTag = tag.getTag("fit").getTag("chi-square-start", &ok);

    if ( ok ) m_chiSquareOnStart->setValue(safeTag.getValue());
    else      m_chiSquareOnStart->setValue(0);

    safeTag = tag.getTag("fit").getTag("chi-square-fit", &ok);

    if ( ok ) m_chiSquareAfterFit->setValue(safeTag.getValue());
    else      m_chiSquareAfterFit->setValue(0);

    safeTag = tag.getTag(m_parentNode).getTag("channel-resolution", &ok);

    if ( ok )  m_channelResolutionNode->setValue(safeTag.getValue());
    else       m_channelResolutionNode->setValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("start-channel", &ok);

    if ( ok )  m_startChannelNode->setValue(safeTag.getValue());
    else       m_startChannelNode->setValue(0);

    safeTag = tag.getTag(m_parentNode).getTag("stop-channel", &ok);

    if ( ok )  m_stopChannelNode->setValue(safeTag.getValue());
    else       m_stopChannelNode->setValue(1024);

    safeTag = tag.getTag(m_parentNode).getTag("average-lt", &ok);

    if ( ok )  m_averageLifeTimeNode->setValue(safeTag.getValue());
    else       m_averageLifeTimeNode->setValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("average-lt-error", &ok);

    if ( ok )  m_averageLifeTimeErrorNode->setValue(safeTag.getValue());
    else       m_averageLifeTimeErrorNode->setValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("counts-in-range", &ok);

    if ( ok )  m_countsInRangeNode->setValue(safeTag.getValue());
    else       m_countsInRangeNode->setValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("last-date-time-of-fit-result", &ok);

    if ( ok )  m_dateTimeOfLastFitResultsNode->setValue(safeTag.getValue());
    else       m_dateTimeOfLastFitResultsNode->setValue("");

    safeTag = tag.getTag(m_parentNode).getTag("last-fit-finish-code", &ok);

    if ( ok )  m_fitFinishCodeNode->setValue(safeTag.getValue());
    else       m_fitFinishCodeNode->setValue("");

    safeTag = tag.getTag(m_parentNode).getTag("last-fit-finish-code-value", &ok);

    if ( ok )  m_fitFinishCodeValueNode->setValue(safeTag.getValue());
    else       m_fitFinishCodeValueNode->setValue("");

    safeTag = tag.getTag(m_parentNode).getTag("peak-to-background-ratio", &ok);

    if ( ok )  m_peakToBackgroundRatioNode->setValue(safeTag.getValue());
    else       m_peakToBackgroundRatioNode->setValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("sum-of-intensities", &ok);

    if ( ok )  m_sumOfIntensitiesNode->setValue(safeTag.getValue());
    else       m_sumOfIntensitiesNode->setValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("sum-error-of-intensities", &ok);

    if ( ok )  m_sumErrorOfIntensitiesNode->setValue(safeTag.getValue());
    else       m_sumErrorOfIntensitiesNode->setValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("data-plot-raw-image", &ok);

    if ( ok )  m_dataPlotImageNode->setValue(safeTag.getValue());
    else       m_dataPlotImageNode->setValue("");

    safeTag = tag.getTag(m_parentNode).getTag("residual-plot-raw-image", &ok);

    if ( ok )  m_residualPlotImageNode->setValue(safeTag.getValue());
    else       m_residualPlotImageNode->setValue("");


    *m_parentNode << m_maxIterationsNode << m_neededIterationsNode << m_t0spectralCentroidNode << m_spectralCentroidNode << m_chiSquareOnStart << m_chiSquareAfterFit << m_channelResolutionNode << m_startChannelNode << m_stopChannelNode << m_averageLifeTimeNode << m_averageLifeTimeErrorNode << m_countsInRangeNode << m_dateTimeOfLastFitResultsNode << m_fitFinishCodeNode << m_fitFinishCodeValueNode << m_peakToBackgroundRatioNode << m_sumOfIntensitiesNode << m_sumErrorOfIntensitiesNode << m_dataPlotImageNode << m_residualPlotImageNode;
    *(parent->getParent()) << m_parentNode;
}

PALSFitSet::~PALSFitSet()
{
    DDELETE_SAFETY(m_sourceParams);
    DDELETE_SAFETY(m_deviceResolutionParams);
    DDELETE_SAFETY(m_lifeTimeParams);
    DDELETE_SAFETY(m_resultHistorie);
    DDELETE_SAFETY(m_bgParam);
    DDELETE_SAFETY(m_maxIterationsNode);
    DDELETE_SAFETY(m_neededIterationsNode);
    DDELETE_SAFETY(m_t0spectralCentroidNode);
    DDELETE_SAFETY(m_spectralCentroidNode);
    DDELETE_SAFETY(m_chiSquareOnStart);
    DDELETE_SAFETY(m_chiSquareAfterFit);
    DDELETE_SAFETY(m_channelResolutionNode);
    DDELETE_SAFETY(m_startChannelNode);
    DDELETE_SAFETY(m_stopChannelNode);
    DDELETE_SAFETY(m_averageLifeTimeNode);
    DDELETE_SAFETY(m_averageLifeTimeErrorNode);
    DDELETE_SAFETY(m_countsInRangeNode);
    DDELETE_SAFETY(m_dateTimeOfLastFitResultsNode);
    DDELETE_SAFETY(m_fitFinishCodeNode);
    DDELETE_SAFETY(m_fitFinishCodeValueNode);
    DDELETE_SAFETY(m_peakToBackgroundRatioNode);
    DDELETE_SAFETY(m_sumOfIntensitiesNode);
    DDELETE_SAFETY(m_sumErrorOfIntensitiesNode);
    DDELETE_SAFETY(m_dataPlotImageNode);
    DDELETE_SAFETY(m_residualPlotImageNode);
    DDELETE_SAFETY(m_parentNode);
}

DSimpleXMLNode *PALSFitSet::getParent() const
{
    return m_parentNode;
}

PALSSourceParameter *PALSFitSet::getSourceParamPtr() const
{
    return m_sourceParams;
}

PALSDeviceResolutionParameter *PALSFitSet::getDeviceResolutionParamPtr() const
{
    return m_deviceResolutionParams;
}

PALSLifeTimeParameter *PALSFitSet::getLifeTimeParamPtr() const
{
    return m_lifeTimeParams;
}

PALSBackgroundParameter *PALSFitSet::getBackgroundParamPtr() const
{
    return m_bgParam;
}

PALSResultHistorie *PALSFitSet::getResultHistoriePtr() const
{
    return m_resultHistorie;
}

int PALSFitSet::getComponentsCount() const
{
    return getLifeTimeParamPtr()->getSize() + getSourceParamPtr()->getSize();
}

void PALSFitSet::setMaximumIterations(unsigned int iterations)
{
    m_maxIterationsNode->setValue(iterations);
}

void PALSFitSet::setNeededIterations(unsigned int iterations)
{
    m_neededIterationsNode->setValue(iterations);
}

void PALSFitSet::setChiSquareOnStart(double value)
{
    m_chiSquareOnStart->setValue(value);
}

void PALSFitSet::setChiSquareAfterFit(double value)
{
    m_chiSquareAfterFit->setValue(value);
}

unsigned int PALSFitSet::getMaximumIterations() const
{
    return (unsigned int)m_maxIterationsNode->getValue().toInt();
}

unsigned int PALSFitSet::getNeededIterations() const
{
    return (unsigned int)m_neededIterationsNode->getValue().toInt();
}

double PALSFitSet::getChiSquareOnStart() const
{
    return m_chiSquareOnStart->getValue().toDouble();
}

double PALSFitSet::getChiSquareAfterFit() const
{
    return m_chiSquareAfterFit->getValue().toDouble();
}

PALSSourceParameter::PALSSourceParameter(PALSFitSet *parent)
{
    m_parentNode = new DSimpleXMLNode("source-params");
    *(parent->getParent()) << m_parentNode;
}

PALSSourceParameter::PALSSourceParameter(PALSFitSet *parent, const DSimpleXMLTag &tag)
{
    m_parentNode = new DSimpleXMLNode("source-params");

    bool ok = true;
    int  paramCnt = 0;
    do {
        const DSimpleXMLTag safeTag = tag.getTag("source-params").getTag("param_" + QVariant(paramCnt).toString(), &ok);
        DUNUSED_PARAM(safeTag);

        if ( ok )
        {
            PALSFitParameter *param = new PALSFitParameter(this, tag.getTag("source-params"), "param_" + QVariant(paramCnt).toString());
            DUNUSED_PARAM(param);
        }

        paramCnt ++;
    }while ( ok );

    *(parent->getParent()) << m_parentNode;
}

PALSSourceParameter::~PALSSourceParameter()
{
    //clear all old project settings:
    while ( m_fitParamSettings.size() > 0 )
    {
        PALSFitParameter* param = m_fitParamSettings.takeFirst();
        DDELETE_SAFETY(param);
    }

    m_fitParamSettings.clear();

    DDELETE_SAFETY(m_parentNode);
}

DSimpleXMLNode *PALSSourceParameter::getParent() const
{
    return m_parentNode;
}

unsigned int PALSSourceParameter::getSize() const
{
    return (unsigned int)m_fitParamSettings.size();
}

void PALSSourceParameter::addParameter(PALSFitParameter *param)
{
    if ( param )
        m_fitParamSettings.append(param);
}

void PALSSourceParameter::removeParameter(unsigned int index)
{
    if ( index >= getSize() )
        return;


    PALSFitParameter* param = m_fitParamSettings.takeAt(index);
    DDELETE_SAFETY(param);

    for ( unsigned int i = 0 ; i < getSize() ; ++ i )
    {
        m_fitParamSettings[i]->getParent()->setNodeName("param_" + QVariant(i).toString());
    }
}

void PALSSourceParameter::removeParameter(PALSFitParameter *param)
{
    if ( !param )
        return;


    const int index = m_fitParamSettings.indexOf(param);

    if ( index > -1 )
        removeParameter(index);
}

PALSFitParameter *PALSSourceParameter::operator[](unsigned int index) const
{
    if ( index >= getSize() )
        return nullptr;
    else
        return m_fitParamSettings[index];
}

PALSFitParameter *PALSSourceParameter::getParameterAt(unsigned int index) const
{
    if ( index >= getSize() )
        return nullptr;
    else
        return m_fitParamSettings[index];
}


PALSFitParameter::PALSFitParameter(PALSSourceParameter *parent)
{
    m_parentNode = new DSimpleXMLNode("param_" + QVariant(parent->getSize()).toString());

    m_active = new DSimpleXMLNode("active");
    m_name = new DSimpleXMLNode("parameter-name");
    m_alias = new DSimpleXMLNode("parameter-alias");
    m_startValue = new DSimpleXMLNode("start-value");
    m_upperBounding = new DSimpleXMLNode("upper-bounding-value");
    m_lowerBounding = new DSimpleXMLNode("lower-bounding-value");
    m_bupperBounding = new DSimpleXMLNode("upper-bounding-value-enabled");
    m_blowerBounding = new DSimpleXMLNode("lower-bounding-value-enabled");
    m_fixed = new DSimpleXMLNode("fixed-value");

    m_fitValue = new DSimpleXMLNode("fitted-value");
    m_fitValueError = new DSimpleXMLNode("fitted-value-error");

    setActive(true);
    setName(DString(""));
    setAlias(DString("undefined"));
    setStartValue(0.0f);
    setUpperBoundingValue(0.0f);
    setUpperBoundingEnabled(false);
    setLowerBoundingValue(0.0f);
    setLowerBoundingEnabled(false);
    setAsFixed(false);
    setFitValue(0.0f);
    setFitValueError(0.0f);

    *m_parentNode << m_name << m_alias << m_startValue << m_upperBounding << m_lowerBounding << m_fixed << m_fitValue << m_fitValueError << m_bupperBounding << m_blowerBounding << m_active;
    *(parent->getParent()) << m_parentNode;

    parent->addParameter(this);
}

PALSFitParameter::PALSFitParameter(PALSDeviceResolutionParameter *parent)
{
    m_parentNode = new DSimpleXMLNode("param_" + QVariant(parent->getSize()).toString());

    m_active = new DSimpleXMLNode("active");
    m_name = new DSimpleXMLNode("parameter-name");
    m_alias = new DSimpleXMLNode("parameter-alias");
    m_startValue = new DSimpleXMLNode("start-value");
    m_upperBounding = new DSimpleXMLNode("upper-bounding-value");
    m_lowerBounding = new DSimpleXMLNode("lower-bounding-value");
    m_bupperBounding = new DSimpleXMLNode("upper-bounding-value-enabled");
    m_blowerBounding = new DSimpleXMLNode("lower-bounding-value-enabled");
    m_fixed = new DSimpleXMLNode("fixed-value");

    m_fitValue = new DSimpleXMLNode("fitted-value");
    m_fitValueError = new DSimpleXMLNode("fitted-value-error");

    setActive(true);
    setName(DString(""));
    setAlias(DString("undefined"));
    setStartValue(0.0f);
    setUpperBoundingValue(0.0f);
    setUpperBoundingEnabled(false);
    setLowerBoundingValue(0.0f);
    setLowerBoundingEnabled(false);
    setAsFixed(false);
    setFitValue(0.0f);
    setFitValueError(0.0f);

    *m_parentNode << m_name << m_alias << m_startValue << m_upperBounding << m_lowerBounding << m_fixed << m_fitValue << m_fitValueError << m_bupperBounding << m_blowerBounding << m_active;
    *(parent->getParent()) << m_parentNode;

    parent->addParameter(this);
}

PALSFitParameter::PALSFitParameter(PALSBackgroundParameter *parent)
{
    m_parentNode = new DSimpleXMLNode("param_bg");

    m_active = new DSimpleXMLNode("active");
    m_name = new DSimpleXMLNode("parameter-name");
    m_alias = new DSimpleXMLNode("parameter-alias");
    m_startValue = new DSimpleXMLNode("start-value");
    m_upperBounding = new DSimpleXMLNode("upper-bounding-value");
    m_lowerBounding = new DSimpleXMLNode("lower-bounding-value");
    m_bupperBounding = new DSimpleXMLNode("upper-bounding-value-enabled");
    m_blowerBounding = new DSimpleXMLNode("lower-bounding-value-enabled");
    m_fixed = new DSimpleXMLNode("fixed-value");

    m_fitValue = new DSimpleXMLNode("fitted-value");
    m_fitValueError  = new DSimpleXMLNode("fitted-value-error");

    setActive(true);
    setName(DString(""));
    setAlias(DString("undefined"));
    setStartValue(0.0f);
    setUpperBoundingValue(0.0f);
    setUpperBoundingEnabled(false);
    setLowerBoundingValue(0.0f);
    setLowerBoundingEnabled(false);
    setAsFixed(false);
    setFitValue(0.0f);
    setFitValueError(0.0f);

    *m_parentNode << m_name << m_alias << m_startValue << m_upperBounding << m_lowerBounding << m_fixed << m_fitValue << m_fitValueError << m_bupperBounding << m_blowerBounding << m_active;
    *(parent->getParent()) << m_parentNode;

    parent->addParameter(this);
}

PALSFitParameter::PALSFitParameter(PALSLifeTimeParameter *parent)
{
    m_parentNode = new DSimpleXMLNode("param_" + QVariant(parent->getSize()).toString());

    m_active = new DSimpleXMLNode("active");
    m_name = new DSimpleXMLNode("parameter-name");
    m_alias = new DSimpleXMLNode("parameter-alias");
    m_startValue = new DSimpleXMLNode("start-value");
    m_upperBounding = new DSimpleXMLNode("upper-bounding-value");
    m_lowerBounding = new DSimpleXMLNode("lower-bounding-value");
    m_bupperBounding = new DSimpleXMLNode("upper-bounding-value-enabled");
    m_blowerBounding = new DSimpleXMLNode("lower-bounding-value-enabled");
    m_fixed = new DSimpleXMLNode("fixed-value");

    m_fitValue = new DSimpleXMLNode("fitted-value");
    m_fitValueError  = new DSimpleXMLNode("fitted-value-error");

    setActive(true);
    setName(DString(""));
    setAlias(DString("undefined"));
    setStartValue(0.0f);
    setUpperBoundingValue(0.0f);
    setUpperBoundingEnabled(false);
    setLowerBoundingValue(0.0f);
    setLowerBoundingEnabled(false);
    setAsFixed(false);
    setFitValue(0.0f);
    setFitValueError(0.0f);

    *m_parentNode << m_name << m_alias << m_startValue << m_upperBounding << m_lowerBounding << m_fixed << m_fitValue << m_fitValueError << m_bupperBounding << m_blowerBounding << m_active;
    *(parent->getParent()) << m_parentNode;

    parent->addParameter(this);
}

PALSFitParameter::PALSFitParameter(PALSSourceParameter *parent, const DSimpleXMLTag& tag, const DString& name)
{
    m_parentNode = new DSimpleXMLNode(name);

    m_active = new DSimpleXMLNode("active");
    m_name = new DSimpleXMLNode("parameter-name");
    m_alias = new DSimpleXMLNode("parameter-alias");
    m_startValue = new DSimpleXMLNode("start-value");
    m_upperBounding = new DSimpleXMLNode("upper-bounding-value");
    m_lowerBounding = new DSimpleXMLNode("lower-bounding-value");
    m_bupperBounding = new DSimpleXMLNode("upper-bounding-value-enabled");
    m_blowerBounding = new DSimpleXMLNode("lower-bounding-value-enabled");
    m_fixed = new DSimpleXMLNode("fixed-value");

    m_fitValue = new DSimpleXMLNode("fitted-value");
    m_fitValueError = new DSimpleXMLNode("fitted-value-error");

    bool ok = false;
    DSimpleXMLTag safeTag = tag.getTag(m_parentNode).getTag("parameter-name", &ok);

    if ( ok ) setName(DString(safeTag.getValue().toString()));
    else setName(DString(""));

    safeTag = tag.getTag(m_parentNode).getTag("parameter-alias", &ok);
    if ( ok ) setAlias(DString(safeTag.getValue().toString()));
    else setAlias(DString("undefined"));

    safeTag = tag.getTag(m_parentNode).getTag("active", &ok);
    if ( ok ) setActive(safeTag.getValue().toBool());
    else setActive(true);

    safeTag = tag.getTag(m_parentNode).getTag("start-value", &ok);
    if ( ok ) setStartValue(safeTag.getValue().toDouble());
    else setStartValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("upper-bounding-value", &ok);
    if ( ok ) setUpperBoundingValue(safeTag.getValue().toDouble());
    else      setUpperBoundingValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("upper-bounding-value-enabled", &ok);
    if ( ok ) setUpperBoundingEnabled(safeTag.getValue().toBool());
    else      setUpperBoundingEnabled(false);

    safeTag = tag.getTag(m_parentNode).getTag("lower-bounding-value", &ok);
    if ( ok ) setLowerBoundingValue(safeTag.getValue().toDouble());
    else      setLowerBoundingValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("lower-bounding-value-enabled", &ok);
    if ( ok ) setLowerBoundingEnabled(safeTag.getValue().toBool());
    else      setLowerBoundingEnabled(false);

    safeTag = tag.getTag(m_parentNode).getTag("fixed-value",          &ok);
    if ( ok ) setAsFixed(safeTag.getValue().toBool());
    else      setAsFixed(false);

    safeTag = tag.getTag(m_parentNode).getTag("fitted-value",         &ok);
    if ( ok ) setFitValue(safeTag.getValue().toDouble());
    else      setFitValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("fitted-value-error",   &ok);
    if ( ok ) setFitValueError(safeTag.getValue().toDouble());
    else      setFitValueError(0.0f);

    *m_parentNode  << m_name << m_alias << m_startValue << m_upperBounding << m_lowerBounding << m_fixed << m_fitValue << m_fitValueError << m_bupperBounding << m_blowerBounding << m_active;
    *(parent->getParent()) << m_parentNode;

    parent->addParameter(this);
}

PALSFitParameter::PALSFitParameter(PALSDeviceResolutionParameter *parent, const DSimpleXMLTag &tag, const DString &name)
{
    m_parentNode = new DSimpleXMLNode(name);

    m_active = new DSimpleXMLNode("active");
    m_name = new DSimpleXMLNode("parameter-name");
    m_alias = new DSimpleXMLNode("parameter-alias");
    m_startValue = new DSimpleXMLNode("start-value");
    m_upperBounding = new DSimpleXMLNode("upper-bounding-value");
    m_lowerBounding = new DSimpleXMLNode("lower-bounding-value");
    m_bupperBounding = new DSimpleXMLNode("upper-bounding-value-enabled");
    m_blowerBounding = new DSimpleXMLNode("lower-bounding-value-enabled");
    m_fixed = new DSimpleXMLNode("fixed-value");

    m_fitValue = new DSimpleXMLNode("fitted-value");
    m_fitValueError  = new DSimpleXMLNode("fitted-value-error");

    bool ok = false;
    DSimpleXMLTag safeTag = tag.getTag(m_parentNode).getTag("parameter-name", &ok);

    if ( ok ) setName(DString(safeTag.getValue().toString()));
    else      setName(DString(""));

    safeTag = tag.getTag(m_parentNode).getTag("parameter-alias", &ok);
    if ( ok ) setAlias(DString(safeTag.getValue().toString()));
    else setAlias(DString("undefined"));

    safeTag = tag.getTag(m_parentNode).getTag("active", &ok);
    if ( ok ) setActive(safeTag.getValue().toBool());
    else      setActive(true);

    safeTag = tag.getTag(m_parentNode).getTag("start-value", &ok);
    if ( ok ) setStartValue(safeTag.getValue().toDouble());
    else      setStartValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("upper-bounding-value", &ok);
    if ( ok ) setUpperBoundingValue(safeTag.getValue().toDouble());
    else      setUpperBoundingValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("upper-bounding-value-enabled", &ok);
    if ( ok ) setUpperBoundingEnabled(safeTag.getValue().toBool());
    else      setUpperBoundingEnabled(false);

    safeTag = tag.getTag(m_parentNode).getTag("lower-bounding-value", &ok);
    if ( ok ) setLowerBoundingValue(safeTag.getValue().toDouble());
    else      setLowerBoundingValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("lower-bounding-value-enabled", &ok);
    if ( ok ) setLowerBoundingEnabled(safeTag.getValue().toBool());
    else      setLowerBoundingEnabled(false);

    safeTag = tag.getTag(m_parentNode).getTag("fixed-value", &ok);
    if ( ok ) setAsFixed(safeTag.getValue().toBool());
    else      setAsFixed(false);

    safeTag = tag.getTag(m_parentNode).getTag("fitted-value", &ok);
    if ( ok ) setFitValue(safeTag.getValue().toDouble());
    else      setFitValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("fitted-value-error", &ok);
    if ( ok ) setFitValueError(safeTag.getValue().toDouble());
    else      setFitValueError(0.0f);

    *m_parentNode << m_name << m_alias << m_startValue << m_upperBounding << m_lowerBounding << m_fixed << m_fitValue << m_fitValueError << m_bupperBounding << m_blowerBounding << m_active;
    *(parent->getParent()) << m_parentNode;

    parent->addParameter(this);
}

PALSFitParameter::PALSFitParameter(PALSBackgroundParameter *parent, const DSimpleXMLTag &tag, const DString &name)
{
    m_parentNode = new DSimpleXMLNode(name);

    m_active = new DSimpleXMLNode("active");
    m_name = new DSimpleXMLNode("parameter-name");
    m_alias = new DSimpleXMLNode("parameter-alias");
    m_startValue = new DSimpleXMLNode("start-value");
    m_upperBounding = new DSimpleXMLNode("upper-bounding-value");
    m_lowerBounding = new DSimpleXMLNode("lower-bounding-value");
    m_bupperBounding = new DSimpleXMLNode("upper-bounding-value-enabled");
    m_blowerBounding = new DSimpleXMLNode("lower-bounding-value-enabled");
    m_fixed = new DSimpleXMLNode("fixed-value");

    m_fitValue = new DSimpleXMLNode("fitted-value");
    m_fitValueError  = new DSimpleXMLNode("fitted-value-error");

    bool ok = false;
    DSimpleXMLTag safeTag = tag.getTag(m_parentNode).getTag("parameter-name",   &ok);

    if ( ok ) setName(DString(safeTag.getValue().toString()));
    else      setName(DString(""));

    safeTag = tag.getTag(m_parentNode).getTag("parameter-alias", &ok);
    if ( ok ) setAlias(DString(safeTag.getValue().toString()));
    else setAlias(DString("undefined"));

    safeTag = tag.getTag(m_parentNode).getTag("active", &ok);
    if ( ok ) setActive(safeTag.getValue().toBool());
    else      setActive(true);

    safeTag = tag.getTag(m_parentNode).getTag("start-value", &ok);
    if ( ok ) setStartValue(safeTag.getValue().toDouble());
    else      setStartValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("upper-bounding-value", &ok);
    if ( ok ) setUpperBoundingValue(safeTag.getValue().toDouble());
    else      setUpperBoundingValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("upper-bounding-value-enabled", &ok);
    if ( ok ) setUpperBoundingEnabled(safeTag.getValue().toBool());
    else      setUpperBoundingEnabled(false);

    safeTag = tag.getTag(m_parentNode).getTag("lower-bounding-value", &ok);
    if ( ok ) setLowerBoundingValue(safeTag.getValue().toDouble());
    else      setLowerBoundingValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("lower-bounding-value-enabled", &ok);
    if ( ok ) setLowerBoundingEnabled(safeTag.getValue().toBool());
    else      setLowerBoundingEnabled(false);

    safeTag = tag.getTag(m_parentNode).getTag("fixed-value", &ok);
    if ( ok ) setAsFixed(safeTag.getValue().toBool());
    else      setAsFixed(false);

    safeTag = tag.getTag(m_parentNode).getTag("fitted-value", &ok);
    if ( ok ) setFitValue(safeTag.getValue().toDouble());
    else      setFitValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("fitted-value-error", &ok);
    if ( ok ) setFitValueError(safeTag.getValue().toDouble());
    else      setFitValueError(0.0f);

    *m_parentNode << m_name << m_alias << m_startValue << m_upperBounding << m_lowerBounding << m_fixed << m_fitValue << m_fitValueError << m_bupperBounding << m_blowerBounding << m_active;
    *(parent->getParent()) << m_parentNode;

    parent->addParameter(this);
}

PALSFitParameter::PALSFitParameter(PALSLifeTimeParameter *parent, const DSimpleXMLTag &tag, const DString &name)
{
    m_parentNode = new DSimpleXMLNode(name);

    m_active = new DSimpleXMLNode("active");
    m_name = new DSimpleXMLNode("parameter-name");
    m_alias = new DSimpleXMLNode("parameter-alias");
    m_startValue = new DSimpleXMLNode("start-value");
    m_upperBounding = new DSimpleXMLNode("upper-bounding-value");
    m_lowerBounding = new DSimpleXMLNode("lower-bounding-value");
    m_bupperBounding = new DSimpleXMLNode("upper-bounding-value-enabled");
    m_blowerBounding = new DSimpleXMLNode("lower-bounding-value-enabled");
    m_fixed = new DSimpleXMLNode("fixed-value");

    m_fitValue = new DSimpleXMLNode("fitted-value");
    m_fitValueError  = new DSimpleXMLNode("fitted-value-error");

    bool ok = false;
    DSimpleXMLTag safeTag = tag.getTag(m_parentNode).getTag("parameter-name",   &ok);

    if ( ok ) setName(DString(safeTag.getValue().toString()));
    else      setName(DString(""));

    safeTag = tag.getTag(m_parentNode).getTag("parameter-alias", &ok);
    if ( ok ) setAlias(DString(safeTag.getValue().toString()));
    else setAlias(DString("undefined"));

    safeTag = tag.getTag(m_parentNode).getTag("active", &ok);
    if ( ok ) setActive(safeTag.getValue().toBool());
    else      setActive(true);

    safeTag = tag.getTag(m_parentNode).getTag("start-value", &ok);
    if ( ok ) setStartValue(safeTag.getValue().toDouble());
    else      setStartValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("upper-bounding-value", &ok);
    if ( ok ) setUpperBoundingValue(safeTag.getValue().toDouble());
    else      setUpperBoundingValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("upper-bounding-value-enabled", &ok);
    if ( ok ) setUpperBoundingEnabled(safeTag.getValue().toBool());
    else      setUpperBoundingEnabled(false);

    safeTag = tag.getTag(m_parentNode).getTag("lower-bounding-value", &ok);
    if ( ok ) setLowerBoundingValue(safeTag.getValue().toDouble());
    else      setLowerBoundingValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("lower-bounding-value-enabled", &ok);
    if ( ok ) setLowerBoundingEnabled(safeTag.getValue().toBool());
    else      setLowerBoundingEnabled(false);

    safeTag = tag.getTag(m_parentNode).getTag("fixed-value", &ok);
    if ( ok ) setAsFixed(safeTag.getValue().toBool());
    else      setAsFixed(false);

    safeTag = tag.getTag(m_parentNode).getTag("fitted-value", &ok);
    if ( ok ) setFitValue(safeTag.getValue().toDouble());
    else      setFitValue(0.0f);

    safeTag = tag.getTag(m_parentNode).getTag("fitted-value-error", &ok);
    if ( ok ) setFitValueError(safeTag.getValue().toDouble());
    else      setFitValueError(0.0f);

    *m_parentNode << m_name << m_alias << m_startValue << m_upperBounding << m_lowerBounding << m_fixed << m_fitValue << m_fitValueError << m_bupperBounding << m_blowerBounding << m_active;
    *(parent->getParent()) << m_parentNode;

    parent->addParameter(this);
}

PALSFitParameter::~PALSFitParameter()
{
    DDELETE_SAFETY(m_fitValue);
    DDELETE_SAFETY(m_fitValueError);
    DDELETE_SAFETY(m_fixed);
    DDELETE_SAFETY(m_lowerBounding);
    DDELETE_SAFETY(m_upperBounding);
    DDELETE_SAFETY(m_blowerBounding);
    DDELETE_SAFETY(m_bupperBounding);
    DDELETE_SAFETY(m_startValue);
    DDELETE_SAFETY(m_name);
    DDELETE_SAFETY(m_alias);
    DDELETE_SAFETY(m_active);
    DDELETE_SAFETY(m_parentNode);
}

DSimpleXMLNode *PALSFitParameter::getParent() const
{
    return m_parentNode;
}

void PALSFitParameter::setActive(bool active)
{
    m_active->setValue(active);
}

void PALSFitParameter::setName(const DString &name)
{
    m_name->setValue((QString)name);
}

void PALSFitParameter::setAlias(const DString &name)
{
    m_alias->setValue(name);
}

void PALSFitParameter::setStartValue(double value)
{
    m_startValue->setValue(value);
}

void PALSFitParameter::setUpperBoundingValue(double value)
{
    m_upperBounding->setValue(value);
}

void PALSFitParameter::setUpperBoundingEnabled(bool enabled)
{
    m_bupperBounding->setValue(enabled);
}

void PALSFitParameter::setLowerBoundingValue(double value)
{
    m_lowerBounding->setValue(value);
}

void PALSFitParameter::setLowerBoundingEnabled(bool enabled)
{
    m_blowerBounding->setValue(enabled);
}

void PALSFitParameter::setAsFixed(bool fixed)
{
    m_fixed->setValue(fixed);
}

void PALSFitParameter::setFitValue(double value)
{
    m_fitValue->setValue(value);
}

void PALSFitParameter::setFitValueError(double error)
{
    m_fitValueError->setValue(error);
}

bool PALSFitParameter::isActive() const
{
    return m_active->getValue().toBool();
}

DString PALSFitParameter::getName() const
{
    return (DString)m_name->getValue().toString();
}

DString PALSFitParameter::getAlias() const
{
    return (DString)m_alias->getValue().toString();
}

double PALSFitParameter::getStartValue() const
{
    return m_startValue->getValue().toDouble();
}

bool PALSFitParameter::isUpperBoundingEnabled() const
{
    return m_bupperBounding->getValue().toBool();
}

double PALSFitParameter::getUpperBoundingValue() const
{
    return m_upperBounding->getValue().toDouble();
}

double PALSFitParameter::getLowerBoundingValue() const
{
    return m_lowerBounding->getValue().toDouble();
}

bool PALSFitParameter::isLowerBoundingEnabled() const
{
    return m_blowerBounding->getValue().toBool();
}

bool PALSFitParameter::isFixed() const
{
    return m_fixed->getValue().toBool();
}

double PALSFitParameter::getFitValue() const
{
    return m_fitValue->getValue().toDouble();
}

double PALSFitParameter::getFitValueError() const
{
    return m_fitValueError->getValue().toDouble();
}


PALSDeviceResolutionParameter::PALSDeviceResolutionParameter(PALSFitSet *parent)
{
    m_parentNode = new DSimpleXMLNode("device-resolution-params");
    *(parent->getParent()) << m_parentNode;
}

PALSDeviceResolutionParameter::PALSDeviceResolutionParameter(PALSFitSet *parent, const DSimpleXMLTag &tag)
{
    m_parentNode = new DSimpleXMLNode("device-resolution-params");

    bool ok = true;
    int  paramCnt = 0;
    do {
        const DSimpleXMLTag safeTag = tag.getTag("device-resolution-params").getTag("param_" + QVariant(paramCnt).toString(), &ok);
        DUNUSED_PARAM(safeTag);

        if ( ok )
        {
            PALSFitParameter *param = new PALSFitParameter(this, tag.getTag("device-resolution-params"), "param_" + QVariant(paramCnt).toString());
            DUNUSED_PARAM(param);
        }

        paramCnt ++;
    }while ( ok );

    *(parent->getParent()) << m_parentNode;
}

PALSDeviceResolutionParameter::~PALSDeviceResolutionParameter()
{
    //clear all old project settings:
    while ( m_fitParamSettings.size() > 0 )
    {
        PALSFitParameter *param = m_fitParamSettings.takeFirst();
        DDELETE_SAFETY(param);
    }

    m_fitParamSettings.clear();

    DDELETE_SAFETY(m_parentNode);
}

DSimpleXMLNode *PALSDeviceResolutionParameter::getParent() const
{
    return m_parentNode;
}

unsigned int PALSDeviceResolutionParameter::getSize() const
{
    return (unsigned int)m_fitParamSettings.size();
}

void PALSDeviceResolutionParameter::addParameter(PALSFitParameter *param)
{
    if ( param )
        m_fitParamSettings.append(param);
}

void PALSDeviceResolutionParameter::removeParameter(unsigned int index)
{
    if ( index >= getSize() )
        return;


    PALSFitParameter* param = m_fitParamSettings.takeAt(index);
    DDELETE_SAFETY(param);

    for ( unsigned int i = 0 ; i < getSize() ; ++ i )
    {
        m_fitParamSettings[i]->getParent()->setNodeName("param_" + QVariant(i).toString());
    }
}

void PALSDeviceResolutionParameter::removeParameter(PALSFitParameter *param)
{
    if ( !param )
        return;


    const int index = m_fitParamSettings.indexOf(param);

    if ( index > -1 )
        removeParameter(index);
}

PALSFitParameter *PALSDeviceResolutionParameter::operator[](unsigned int index) const
{
    if ( index >= getSize() )
        return nullptr;
    else
        return m_fitParamSettings[index];
}

PALSFitParameter *PALSDeviceResolutionParameter::getParameterAt(unsigned int index) const
{
    if ( index >= getSize() )
        return nullptr;
    else
        return m_fitParamSettings[index];
}


PALSBackgroundParameter::PALSBackgroundParameter(PALSFitSet *parent)
{
    m_parentNode = new DSimpleXMLNode("background-params");

    m_fitParamSettings = new PALSFitParameter(this);

    *(parent->getParent()) << m_parentNode;
}

PALSBackgroundParameter::PALSBackgroundParameter(PALSFitSet *parent, const DSimpleXMLTag &tag)
{
    m_parentNode = new DSimpleXMLNode("background-params");

    bool ok = true;
    const DSimpleXMLTag safeTag = tag.getTag("background-params").getTag("param_bg", &ok);
    DUNUSED_PARAM(safeTag);

    if ( ok ){
        /*m_fitParamSettings*/PALSFitParameter *param = new PALSFitParameter(this, tag.getTag("background-params"), DString("param_bg"));
        DUNUSED_PARAM(param);
    }else{
        PALSFitParameter *param = new PALSFitParameter(this);
        DUNUSED_PARAM(param);
        /*m_fitParamSettings*/
    }

    *(parent->getParent()) << m_parentNode;
}

PALSBackgroundParameter::~PALSBackgroundParameter()
{
    DDELETE_SAFETY(m_fitParamSettings);
    DDELETE_SAFETY(m_parentNode);
}

DSimpleXMLNode *PALSBackgroundParameter::getParent() const
{
    return m_parentNode;
}

void PALSBackgroundParameter::addParameter(PALSFitParameter *param)
{
    if ( param )
        m_fitParamSettings = param;
}

PALSFitParameter *PALSBackgroundParameter::getParameter() const
{
    return m_fitParamSettings;
}


PALSLifeTimeParameter::PALSLifeTimeParameter(PALSFitSet *parent)
{
    m_parentNode = new DSimpleXMLNode("lifetime-params");
    *(parent->getParent()) << m_parentNode;
}

PALSLifeTimeParameter::PALSLifeTimeParameter(PALSFitSet *parent, const DSimpleXMLTag &tag)
{
    m_parentNode = new DSimpleXMLNode("lifetime-params");

    bool ok = true;
    int  paramCnt = 0;
    do {
        const DSimpleXMLTag safeTag = tag.getTag("lifetime-params").getTag("param_" + QVariant(paramCnt).toString(), &ok);
        DUNUSED_PARAM(safeTag);

        if ( ok )
        {
            PALSFitParameter *param = new PALSFitParameter(this, tag.getTag("lifetime-params"), "param_" + QVariant(paramCnt).toString());
            DUNUSED_PARAM(param);
        }

        paramCnt ++;
    }while ( ok );

    *(parent->getParent()) << m_parentNode;
}

PALSLifeTimeParameter::~PALSLifeTimeParameter()
{
    //clear all old project settings:
    while ( m_fitParamSettings.size() > 0 )
    {
        PALSFitParameter *param = m_fitParamSettings.takeFirst();
        DDELETE_SAFETY(param);
    }

    m_fitParamSettings.clear();

    DDELETE_SAFETY(m_parentNode);
}

DSimpleXMLNode *PALSLifeTimeParameter::getParent() const
{
    return m_parentNode;
}

unsigned int PALSLifeTimeParameter::getSize() const
{
    return (unsigned int)m_fitParamSettings.size();
}

void PALSLifeTimeParameter::addParameter(PALSFitParameter *param)
{
    if ( param )
        m_fitParamSettings.append(param);
}

void PALSLifeTimeParameter::removeParameter(unsigned int index)
{
    if ( index >= getSize() )
        return;


    PALSFitParameter* param = m_fitParamSettings.takeAt(index);
    DDELETE_SAFETY(param);

    for ( unsigned int i = 0 ; i < getSize() ; ++ i )
    {
        m_fitParamSettings[i]->getParent()->setNodeName("param_" + QVariant(i).toString());
    }
}

void PALSLifeTimeParameter::removeParameter(PALSFitParameter *param)
{
    if ( !param )
        return;


    const int index = m_fitParamSettings.indexOf(param);

    if ( index > -1 )
        removeParameter(index);
}

PALSFitParameter *PALSLifeTimeParameter::operator[](unsigned int index) const
{
    if ( index >= getSize() )
        return nullptr;
    else
        return m_fitParamSettings[index];
}

PALSFitParameter *PALSLifeTimeParameter::getParameterAt(unsigned int index) const
{
    if ( index >= getSize() )
        return nullptr;
    else
        return m_fitParamSettings[index];
}


PALSResultHistorie::PALSResultHistorie(PALSFitSet *parent)
{
    m_parentNode = new DSimpleXMLNode("result-historie");
    *(parent->getParent()) << m_parentNode;
}

PALSResultHistorie::PALSResultHistorie(PALSFitSet *parent, const DSimpleXMLTag &tag)
{
    m_parentNode = new DSimpleXMLNode("result-historie");

    bool ok = true;
    int  resultCnt = 0;
    do {
        const DSimpleXMLTag safeTag = tag.getTag("result-historie").getTag("result_" + QVariant(resultCnt).toString(), &ok);
        DUNUSED_PARAM(safeTag);

        if ( ok )
        {
            PALSResult *result = new PALSResult(this, tag.getTag("result-historie"), "result_" + QVariant(resultCnt).toString());
            DUNUSED_PARAM(result);
        }

        resultCnt ++;
    }while ( ok );

    *(parent->getParent()) << m_parentNode;
}

PALSResultHistorie::~PALSResultHistorie()
{
    //clear all old project settings:
    while ( m_resultList.size() > 0 )
    {
        PALSResult* result = m_resultList.takeFirst();
        DDELETE_SAFETY(result);
    }

    m_resultList.clear();

    DDELETE_SAFETY(m_parentNode);
}

DSimpleXMLNode *PALSResultHistorie::getParent() const
{
    return m_parentNode;
}

unsigned int PALSResultHistorie::getSize() const
{
    return (unsigned int)m_resultList.size();
}

void PALSResultHistorie::addResult(PALSResult *result)
{
    if ( result )
        m_resultList.append(result);
}

void PALSResultHistorie::removeResult(unsigned int index)
{
    if ( index >= getSize() )
        return;


    PALSResult* result = m_resultList.takeAt(index);
    DDELETE_SAFETY(result);

    for ( unsigned int i = 0 ; i < getSize() ; ++ i )
    {
        m_resultList[i]->getParent()->setNodeName("result_" + QVariant(i).toString());
    }
}

PALSResult *PALSResultHistorie::operator[](unsigned int index) const
{
    if ( index >= getSize() )
        return nullptr;
    else
        return m_resultList[index];
}

PALSResult *PALSResultHistorie::getResultAt(unsigned int index) const
{
    if ( index >= getSize() )
        return nullptr;
    else
        return m_resultList[index];
}


PALSResult::PALSResult(PALSResultHistorie *parent)
{
    m_parentNode = new DSimpleXMLNode("result_" + QVariant(parent->getSize()).toString());

    m_htmlTextNode = new DSimpleXMLNode("html-text");

    setResultText("");

    *m_parentNode << m_htmlTextNode;
    *(parent->getParent()) << m_parentNode;

    parent->addResult(this);
}

PALSResult::PALSResult(PALSResultHistorie *parent, const DSimpleXMLTag &tag, const DString &name)
{
    m_parentNode = new DSimpleXMLNode(name);

    m_htmlTextNode = new DSimpleXMLNode("html-text");

    bool ok = false;
    DSimpleXMLTag safeTag = tag.getTag(m_parentNode).getTag("html-text", &ok);

    if ( ok ) setResultText(DString(safeTag.getValue().toString()));
    else setResultText(DString(""));

    *m_parentNode  << m_htmlTextNode;
    *(parent->getParent()) << m_parentNode;

    parent->addResult(this);
}

PALSResult::~PALSResult()
{
    DDELETE_SAFETY(m_htmlTextNode);
    DDELETE_SAFETY(m_parentNode);
}

DSimpleXMLNode *PALSResult::getParent() const
{
    return m_parentNode;
}

void PALSResult::setResultText(const QString &resultText)
{
    m_htmlTextNode->setValue(resultText);
}

QString PALSResult::getResultText() const
{
    return m_htmlTextNode->getValue().toString();
}
