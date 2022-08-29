/****************************************************************************
**
**  DDRS4PALS, a software for the acquisition of lifetime spectra using the
**  DRS4 evaluation board of PSI: https://www.psi.ch/drs/evaluation-board
**
**  Copyright (C) 2016-2022 Dr. Danny Petschke
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
**  @author: Dr. Danny Petschke
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

#ifndef SIMPLEXML_H
#define SIMPLEXML_H

#include "../DTypes/types.h"
#include "../DThirdParty/Crypt/tinyaes.h"


QString getTabJumpString(int cnt);

#define DSIMPLEXML_STARTNODE(__nodeName__)     QString("<"  % __nodeName__ % ">")
#define DSIMPLEXML_ENDNODE(__nodeName__)        QString("</" % __nodeName__ % ">")
#define DSIMPLEXML_TABJUMP(__cnt__)                    getTabJumpString(__cnt__)


class DSimpleXMLNode;
class DSimpleXMLTag;
class DSimpleXMLString;

class DSimpleXMLWriter;
class DSimpleXMLReader;

class DSimpleXMLWriter : public QFile
{
    int m_tabCnt;

public:
    DSimpleXMLWriter();
    DSimpleXMLWriter(const QString& fileName);
    DSimpleXMLWriter(const DString& fileName);

    virtual ~DSimpleXMLWriter();

    virtual bool writeToFile(DSimpleXMLNode *rootNode, bool encryptFile = true, const char *key = {"EnterYourKey"});
    virtual bool writeToFile(const QList<DSimpleXMLNode*>& rootNodeList, bool encryptFile = true, const char *key = {"EnterYourKey"});

private:
    void writeNode(const QList<DSimpleXMLNode *> &nodeList, QTextStream *stream);
};


class DSimpleXMLReader : public QFile
{
public:
    DSimpleXMLReader();
    DSimpleXMLReader(const QString& fileName);
    DSimpleXMLReader(const DString& fileName);

    virtual ~DSimpleXMLReader();

    virtual bool readFromFile(DSimpleXMLTag *xmlContent, bool encryptFile = true, const QString &key = "EnterYourKey");
};


class DSimpleXMLString : public DString
{
    int m_tabCnt;

public:
    DSimpleXMLString();
    DSimpleXMLString(DSimpleXMLNode *rootNode);
    DSimpleXMLString(const DSimpleXMLNode *rootNode);
    DSimpleXMLString(const QList<DSimpleXMLNode*>& rootNodeList);

    virtual ~DSimpleXMLString();

    virtual void setXMLNode(DSimpleXMLNode *rootNode);
    virtual void setXMLNode(const DSimpleXMLNode *rootNode);
    virtual void setXMLNode(const QList<DSimpleXMLNode*>& rootNodeList);

private:
    void writeNode(const QList<DSimpleXMLNode *> &nodeList, QTextStream *stream);
};


class DSimpleXMLNode
{
    QString m_nodeName;
    QVariant m_value;
    QList<DSimpleXMLNode* > m_childs;
    DSimpleXMLNode *m_parent;

public:
    DSimpleXMLNode();
    DSimpleXMLNode(const QString& nodeName);
    DSimpleXMLNode(const DString& nodeName);

    DSimpleXMLNode& operator<<(DSimpleXMLNode* childNode);


    virtual ~DSimpleXMLNode();

    void setNodeName(const QString& nodeName);

    bool isValid() const;
    QString nodeName() const;
    bool hasChilds() const;
    bool hasParent() const;
    bool hasValue() const;

    void addChild(DSimpleXMLNode* childNode);
    QList<DSimpleXMLNode*> getChilds() const;
    QVariant getValue() const;

    void setValue(const QVariant& value);

private:
    DSimpleXMLNode* getParent() const;
    void setParent(DSimpleXMLNode* parentNode);
    void removeFromParent(DSimpleXMLNode* childNode);

public slots:
    void XMLMessageBox();
};


class DSimpleXMLTag : public DString
{
public:
    DSimpleXMLTag();
    DSimpleXMLTag(const QString& value);
    DSimpleXMLTag(const DString& value);
    DSimpleXMLTag(const DSimpleXMLString& value);

    virtual ~DSimpleXMLTag();

    DSimpleXMLTag getTag(const QString& tagName, bool *ok = DNULLPTR) const;
    DSimpleXMLTag getTag(const DString& tagName, bool *ok = DNULLPTR) const;
    DSimpleXMLTag getTag(const DSimpleXMLString& tagName, bool *ok = DNULLPTR) const;
    DSimpleXMLTag getTag(const DSimpleXMLNode& node, bool *ok = DNULLPTR) const;
    DSimpleXMLTag getTag(DSimpleXMLNode* node, bool *ok = DNULLPTR) const;

    QVariant getValue() const;
    QVariant getValueAt(const QString& tagName, bool *ok = DNULLPTR) const;
    QVariant getValueAt(const DString& tagName, bool *ok = DNULLPTR) const;
    QVariant getValueAt(const DSimpleXMLString& tagName, bool *ok = DNULLPTR) const;
    QVariant getValueAt(const DSimpleXMLNode& node, bool *ok = DNULLPTR) const;
    QVariant getValueAt(DSimpleXMLNode *node, bool *ok = DNULLPTR) const;

public slots:
    void XMLMessageBox();
};


class DXMLTreeWidgetItem : public QTreeWidgetItem
{
public:
    DXMLTreeWidgetItem(DSimpleXMLNode *node, QTreeWidget *parent = DNULLPTR);
};

#endif // SIMPLEXML_H
