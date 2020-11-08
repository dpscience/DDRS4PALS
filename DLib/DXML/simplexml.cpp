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

#include "simplexml.h"

static int dnodeCnt = 0;

QString getTabJumpString(int cnt)
{
    QString tabString = "";
    for ( int i = 0 ; i < cnt ; ++ i )
        tabString += "    ";

    return tabString;
}

DSimpleXMLWriter::DSimpleXMLWriter() :
    QFile(),
    m_tabCnt(0)
{
    dnodeCnt = 0;
}

DSimpleXMLWriter::DSimpleXMLWriter(const QString &fileName) :
    QFile(fileName),
    m_tabCnt(0)
{
    dnodeCnt = 0;
}

DSimpleXMLWriter::DSimpleXMLWriter(const DString &fileName) :
    QFile((QString)fileName),
    m_tabCnt(0)
{
    dnodeCnt = 0;
}

DSimpleXMLWriter::~DSimpleXMLWriter() {}

void DSimpleXMLWriter::writeNode(const QList<DSimpleXMLNode*>& nodeList, QTextStream *stream)
{
    m_tabCnt ++;
    (*stream) << QString("\r\n");

    for ( int index = 0 ; index < nodeList.size() ; ++ index )
    {
        if ( !nodeList.at(index) )
            continue;


        if ( nodeList.at(index)->hasValue() )
        {
            (*stream) << DSIMPLEXML_TABJUMP(m_tabCnt) % DSIMPLEXML_STARTNODE(nodeList.at(index)->nodeName());
                (*stream) << nodeList.at(index)->getValue().toString();
            (*stream) << DSIMPLEXML_ENDNODE(nodeList.at(index)->nodeName());
        }
        else if ( nodeList.at(index)->hasChilds() )
        {
            (*stream) << DSIMPLEXML_TABJUMP(m_tabCnt) % DSIMPLEXML_STARTNODE(nodeList.at(index)->nodeName());
                writeNode(nodeList.at(index)->getChilds(), stream);
            (*stream) << DSIMPLEXML_TABJUMP(m_tabCnt) % DSIMPLEXML_ENDNODE(nodeList.at(index)->nodeName());
        }

#ifdef QT_DEBUG
            qDebug() << nodeList.at(index)->nodeName();
#endif
        (*stream) << QString("\r\n");
    }

    m_tabCnt --;
}

bool DSimpleXMLWriter::writeToFile(DSimpleXMLNode *rootNode, bool encryptFile, const char* key)
{
    if ( !rootNode )
    {
        DERRORLOG("DSimpleXMLNode: !Root-Node is nullptr.");
        return false;
    }


    QTextStream xmlStream(this);

    if ( open(QIODevice::ReadWrite|QIODevice::Truncate) )
    {
        xmlStream << DSIMPLEXML_STARTNODE(rootNode->nodeName());

        if ( rootNode->hasValue() )
            xmlStream << rootNode->getValue().toString();
        else if ( rootNode->hasChilds() )
            writeNode(rootNode->getChilds(), &xmlStream);

        xmlStream << DSIMPLEXML_ENDNODE(rootNode->nodeName());

        close();

        if ( encryptFile )
        {
            open(QIODevice::ReadOnly);
                TinyAES crypter;
                const QByteArray encrypted = crypter.Encrypt(readAll(), QCryptographicHash::hash(QByteArray(key), QCryptographicHash::Md5));
            close();

            open(QIODevice::WriteOnly|QIODevice::Truncate);
                write(encrypted);
            close();
        }
    }
    else
        return false;

    return true;
}

bool DSimpleXMLWriter::writeToFile(const QList<DSimpleXMLNode *> &rootNodeList, bool encryptFile, const char *key)
{
    QTextStream xmlStream(this);

    if ( open(QIODevice::ReadWrite|QIODevice::Truncate) )
    {
        for ( int index = 0 ; index < rootNodeList.size() ; ++ index )
        {
            if ( !rootNodeList.at(index) )
                continue;

            xmlStream << DSIMPLEXML_STARTNODE(rootNodeList.at(index)->nodeName());

            if ( rootNodeList.at(index)->hasValue() )
                xmlStream << rootNodeList.at(index)->getValue().toString();
            else if ( rootNodeList.at(index)->hasChilds() )
                writeNode(rootNodeList.at(index)->getChilds(), &xmlStream);

            xmlStream << DSIMPLEXML_ENDNODE(rootNodeList.at(index)->nodeName());

            xmlStream << QString("\r\n");
        }

        close();

        if ( encryptFile )
        {
            open(QIODevice::ReadOnly);
                TinyAES crypter;
                const QByteArray encrypted = crypter.Encrypt(readAll(), QCryptographicHash::hash(QByteArray(key), QCryptographicHash::Md5));
            close();

            open(QIODevice::WriteOnly|QIODevice::Truncate);
                write(encrypted);
            close();
        }
    }
    else
        return false;

    return true;
}


DSimpleXMLNode::DSimpleXMLNode() :
       m_nodeName(""),
       m_value(""),
       m_parent(DNULLPTR)
{
    if ( m_nodeName.isEmpty() )
    {
        m_nodeName = "node_" % QVariant(dnodeCnt).toString();
        dnodeCnt ++;

        DERRORLOG("DSimpleXMLNode-Name (temporarely) set to '%s'.\n Use function 'setNodeName(...)'.", m_nodeName.toStdString().c_str());
    }
}

DSimpleXMLNode::DSimpleXMLNode(const QString &nodeName) :
    m_nodeName(nodeName),
    m_value(""),
    m_parent(DNULLPTR)
{
    if ( m_nodeName.isEmpty() )
    {
        m_nodeName = "node_" % QVariant(dnodeCnt).toString();
        dnodeCnt ++;

        DERRORLOG("DSimpleXMLNode-Name (temporarely) set to '%s'.\n Use function 'setNodeName(...)'.", m_nodeName.toStdString().c_str());
    }
}

DSimpleXMLNode::DSimpleXMLNode(const DString &nodeName) :
    m_nodeName((QString)nodeName),
    m_value(""),
    m_parent(DNULLPTR)
{
    if ( m_nodeName.isEmpty() )
    {
        m_nodeName = "node_" % QVariant(dnodeCnt).toString();
        dnodeCnt ++;

        DERRORLOG("DSimpleXMLNode-Name (temporarely) set to '%s'.\n Use function 'setNodeName(...)'.", m_nodeName.toStdString().c_str());
    }
}

DSimpleXMLNode& DSimpleXMLNode::operator<<(DSimpleXMLNode *childNode)
{
    if ( !childNode )
    {
        DERRORLOG("DSimpleXMLNode: !Child-Pointer is nullptr.");
        return *this;
    }
    else
    {
        addChild(childNode);
        childNode->setParent(this);
    }

    return *this;
}

DSimpleXMLNode::~DSimpleXMLNode()
{
    const int iNum = m_childs.count();
    for ( int i = 0 ; i < iNum ; ++ i )
    {
        DSimpleXMLNode* child = m_childs.takeFirst();
        DDELETE_SAFETY(child);
    }

    m_childs.clear();

    if ( m_parent )
        m_parent->removeFromParent(this);
}

QString DSimpleXMLNode::nodeName() const
{
    return m_nodeName;
}

bool DSimpleXMLNode::hasChilds() const
{
    if ( m_childs.isEmpty() )
        return false;
    else
        return true;
}

bool DSimpleXMLNode::hasParent() const
{
    return m_parent ? true : false;
}

bool DSimpleXMLNode::hasValue() const
{
    if ( !hasChilds() )
        return true;
    else
        return false;
}

void DSimpleXMLNode::addChild(DSimpleXMLNode *childNode)
{
    if ( childNode )
    {
        m_childs.append(childNode);
        childNode->setParent(this);
        return;
    }

    DERRORLOG("DSimpleXMLNode: !Child-Pointer is nullptr.");
}

QList<DSimpleXMLNode*> DSimpleXMLNode::getChilds() const
{
     return m_childs;
}

QVariant DSimpleXMLNode::getValue() const
{
     return m_value;
}

void DSimpleXMLNode::setValue(const QVariant &value)
{
    m_value = value;
}

DSimpleXMLNode *DSimpleXMLNode::getParent() const
{
    return m_parent;
}

void DSimpleXMLNode::setParent(DSimpleXMLNode *parentNode)
{
    m_parent = parentNode;
}

void DSimpleXMLNode::removeFromParent(DSimpleXMLNode *childNode)
{
    if ( !childNode )
        return;


    const int index = m_childs.indexOf(childNode);
    m_childs.takeAt(index);
}

void DSimpleXMLNode::XMLMessageBox()
{
     DMSGBOX((QString)DSimpleXMLString(this));
}

void DSimpleXMLNode::setNodeName(const QString &nodeName)
{
     m_nodeName = nodeName;
}

bool DSimpleXMLNode::isValid() const
{
     if ( hasChilds() || hasValue() )
         return true;
     else
         return false;
}


DSimpleXMLTag::DSimpleXMLTag() :
    DString("") {}

DSimpleXMLTag::DSimpleXMLTag(const QString &value) :
    DString(value) {}

DSimpleXMLTag::DSimpleXMLTag(const DString &value) :
    DString(value) {}

DSimpleXMLTag::DSimpleXMLTag(const DSimpleXMLString &value) :
    DString(value) {}

DSimpleXMLTag::~DSimpleXMLTag() {}

DSimpleXMLTag DSimpleXMLTag::getTag(const QString &tagName, bool *ok) const
{
    const DString pValue = parseBetween(DSIMPLEXML_STARTNODE(tagName), DSIMPLEXML_ENDNODE(tagName));

    if ( pValue.isEmpty() )
    {
        if ( ok ) *ok = false;
        return DSimpleXMLTag("");
    }
    else
    {
        if ( ok ) *ok = true;
        return DSimpleXMLTag(pValue);
    }
}

DSimpleXMLTag DSimpleXMLTag::getTag(const DString &tagName, bool *ok) const
{
    return getTag((QString)tagName, ok);
}

DSimpleXMLTag DSimpleXMLTag::getTag(const DSimpleXMLString &tagName, bool *ok) const
{
    return getTag((QString)tagName, ok);
}

DSimpleXMLTag DSimpleXMLTag::getTag(const DSimpleXMLNode &node, bool *ok) const
{
    return getTag(node.nodeName(), ok);
}

DSimpleXMLTag DSimpleXMLTag::getTag(DSimpleXMLNode *node, bool *ok) const
{
    if ( !node )
        return DSimpleXMLTag("");
    else
        return getTag(node->nodeName(), ok);
}

QVariant DSimpleXMLTag::getValue() const
{
    return QVariant(*this);
}

QVariant DSimpleXMLTag::getValueAt(const QString &tagName, bool *ok) const
{
    return getTag(tagName, ok).getValue();
}

QVariant DSimpleXMLTag::getValueAt(const DString &tagName, bool *ok) const
{
    return getTag(tagName, ok).getValue();
}

QVariant DSimpleXMLTag::getValueAt(const DSimpleXMLNode &node, bool *ok) const
{
    return getTag(node, ok).getValue();
}

QVariant DSimpleXMLTag::getValueAt(DSimpleXMLNode *node, bool *ok) const
{
    if ( node )
        return getTag(node, ok).getValue();
    else
        return QVariant("");
}

void DSimpleXMLTag::XMLMessageBox()
{
    DMSGBOX((QString)*this);
}

QVariant DSimpleXMLTag::getValueAt(const DSimpleXMLString &tagName, bool *ok) const
{
    return getTag(tagName, ok).getValue();
}


DSimpleXMLReader::DSimpleXMLReader() :
    QFile("") {}

DSimpleXMLReader::DSimpleXMLReader(const QString &fileName) :
    QFile(fileName) {}

DSimpleXMLReader::DSimpleXMLReader(const DString &fileName) :
    QFile((QString)fileName) {}

DSimpleXMLReader::~DSimpleXMLReader() {}

bool DSimpleXMLReader::readFromFile(DSimpleXMLTag *xmlContent, bool encryptFile, const QString &key)
{
    if ( !xmlContent )
        return false;


    QTextStream xmlStream(this);

    if ( encryptFile )
    {
        DUNUSED_PARAM(xmlStream);
    }

    OpenMode mode;

    if ( encryptFile )
        mode = QIODevice::ReadOnly;
    else
        mode = QIODevice::ReadOnly|QIODevice::Text;

    if ( open(mode) )
    {
        if ( encryptFile )
        {
            TinyAES crypter;
            const QByteArray decrypted = crypter.Decrypt(readAll(), QCryptographicHash::hash(QByteArray(key.toStdString().c_str()), QCryptographicHash::Md5));

            *xmlContent = DSimpleXMLTag(QString(decrypted));
        }
        else
        {
            *xmlContent = DSimpleXMLTag(xmlStream.readAll());
        }

        close();
    }
    else
        return false;


    return true;
}


DSimpleXMLString::DSimpleXMLString() :
    DString(""),
    m_tabCnt(0)
{
    dnodeCnt = 0;
}

DSimpleXMLString::DSimpleXMLString(DSimpleXMLNode *rootNode) :
    DString(""),
    m_tabCnt(0)
{
    dnodeCnt = 0;
    setXMLNode(rootNode);
}

DSimpleXMLString::DSimpleXMLString(const DSimpleXMLNode *rootNode) :
    DString(""),
    m_tabCnt(0)
{
    dnodeCnt = 0;
    setXMLNode(rootNode);
}

DSimpleXMLString::DSimpleXMLString(const QList<DSimpleXMLNode *> &rootNodeList) :
    DString(""),
    m_tabCnt(0)
{
    dnodeCnt = 0;
    setXMLNode(rootNodeList);
}

DSimpleXMLString::~DSimpleXMLString() {}

void DSimpleXMLString::setXMLNode(DSimpleXMLNode *rootNode)
{
    if ( !rootNode )
    {
        DERRORLOG("DSimpleXMLString: !Root-Node is nullptr.");
        return;
    }


    QTextStream xmlStream(this);

    xmlStream << DSIMPLEXML_STARTNODE(rootNode->nodeName());

    if ( rootNode->hasValue() )
        xmlStream << rootNode->getValue().toString();
    else if ( rootNode->hasChilds() )
        writeNode(rootNode->getChilds(), &xmlStream);

    xmlStream << DSIMPLEXML_ENDNODE(rootNode->nodeName());
}

void DSimpleXMLString::setXMLNode(const DSimpleXMLNode *rootNode)
{
    if ( !rootNode )
    {
        DERRORLOG("DSimpleXMLString: !Root-Node is nullptr.");
        return;
    }


    QTextStream xmlStream(this);

    xmlStream << DSIMPLEXML_STARTNODE(rootNode->nodeName());

    if ( rootNode->hasValue() )
        xmlStream << rootNode->getValue().toString();
    else if ( rootNode->hasChilds() )
        writeNode(rootNode->getChilds(), &xmlStream);

    xmlStream << DSIMPLEXML_ENDNODE(rootNode->nodeName());
}

void DSimpleXMLString::setXMLNode(const QList<DSimpleXMLNode *> &rootNodeList)
{
    QTextStream xmlStream(this);

    for ( int index = 0 ; index < rootNodeList.size() ; ++ index )
    {
        if ( !rootNodeList.at(index) )
            continue;

        xmlStream << DSIMPLEXML_STARTNODE(rootNodeList.at(index)->nodeName());

        if ( rootNodeList.at(index)->hasValue() )
            xmlStream << rootNodeList.at(index)->getValue().toString();
        else if ( rootNodeList.at(index)->hasChilds() )
            writeNode(rootNodeList.at(index)->getChilds(), &xmlStream);

        xmlStream << DSIMPLEXML_ENDNODE(rootNodeList.at(index)->nodeName());

        xmlStream << QString("\r\n");
    }
}

void DSimpleXMLString::writeNode(const QList<DSimpleXMLNode *> &nodeList, QTextStream *stream)
{
    m_tabCnt ++;
    (*stream) << QString("\r\n");

    for ( int index = 0 ; index < nodeList.size() ; ++ index )
    {
        if ( !nodeList.at(index) )
            continue;


        if ( nodeList.at(index)->hasValue() )
        {
            (*stream) << DSIMPLEXML_TABJUMP(m_tabCnt) % DSIMPLEXML_STARTNODE(nodeList.at(index)->nodeName());
                (*stream) << nodeList.at(index)->getValue().toString();
            (*stream) << DSIMPLEXML_ENDNODE(nodeList.at(index)->nodeName());
        }
        else if ( nodeList.at(index)->hasChilds() )
        {
            (*stream) << DSIMPLEXML_TABJUMP(m_tabCnt) % DSIMPLEXML_STARTNODE(nodeList.at(index)->nodeName());
                writeNode(nodeList.at(index)->getChilds(), stream);
            (*stream) << DSIMPLEXML_TABJUMP(m_tabCnt) % DSIMPLEXML_ENDNODE(nodeList.at(index)->nodeName());
        }

        (*stream) << QString("\r\n");
    }

    m_tabCnt --;
}

DXMLTreeWidgetItem::DXMLTreeWidgetItem(DSimpleXMLNode *node, QTreeWidget *parent) :
    QTreeWidgetItem(parent)
{
    if (!node)
        return;

    if ( node->hasChilds() ) {
        QFont f = font(0);
        f.setBold(true);

        setFont(0, f);

        QBrush brush = foreground(0);
        brush.setColor(Qt::darkBlue);

        setForeground(0, brush);
    }

    QBrush brush = foreground(0);
    brush.setColor(Qt::darkCyan);

    setForeground(1, brush);

    QFont f = font(0);
    f.setBold(true);

    setFont(1, f);

    for ( DSimpleXMLNode *childNode : node->getChilds() )
    {
        if (!childNode)
            continue;

        if (!childNode->isValid())
            continue;

        DXMLTreeWidgetItem *treeWidget = new DXMLTreeWidgetItem(childNode);

        treeWidget->setText(0, childNode->nodeName() % QString(":"));

        if (childNode->hasValue())
            treeWidget->setText(1, childNode->getValue().toString());

        addChild((QTreeWidgetItem*)treeWidget);
    }

    if (parent)
        parent->addTopLevelItem(this);
}
