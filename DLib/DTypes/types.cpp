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

#include "types.h"

DString::~DString() {}

DString DString::parseBetween(const DString &start, const DString &stop) const
{
    QStringList startList = split(start);

    if ( startList.size() <= 1 )
        return DString("");

    QStringList stopList = startList.at(startList.size()-1).split(stop);

    if ( stopList.size() == 0 )
        return DString("");
    else
        return DString(stopList[0]);
}

DString DString::parseBetween(const QString &start, const QString &stop) const
{
    return parseBetween((DString)start, (DString)stop);
}

QStringList DString::parseBetween2(const QString &start, const QString &stop) const
{
    QStringList startList = split(start);

    if ( startList.size() <= 1 )
        return QStringList("");

    QStringList returnList;
    for ( int i = 1 ; i < startList.size() ; ++ i )
    {
        const QStringList stopList =  startList.at(i).split(stop);
        if ( stopList.size() > 0 )
            returnList.append(stopList[0]);
    }

    return returnList;
}

QStringList DString::parseBetween2(const DString &start, const DString &stop) const
{
    return parseBetween2((DString)start, (DString)stop);
}


QImage DSVGImage::getImage(const QString &document, double width, double height, bool antialiasing)
{
    QDomDocument xml;
    QSvgRenderer renderer;


    QFile file(document);

    if( !file.open(QIODevice::ReadOnly) )
        return QImage();

    xml.setContent(file.readAll());
    file.close();


    if( document.isNull() )
        return QImage();

    renderer.load(xml.toByteArray());


    if ( width > 0 && height > 0 )
    {
        width  = renderer.defaultSize().width()*(width/renderer.defaultSize().width());
        height = renderer.defaultSize().height()*(height/renderer.defaultSize().height());
    }
    else
    {
        width  = renderer.defaultSize().width();
        height = renderer.defaultSize().height();
    }

    QImage result = QImage(width, height, QImage::Format_ARGB32);
    result.fill(Qt::transparent);

    QPainter painter(&result);

    painter.setRenderHint(QPainter::Antialiasing, antialiasing);
    renderer.render(&painter);


    return result;
}

QImage DSVGImage::getImage(const DString &document, double width, double height,  bool antialiasing)
{
    return getImage((QString)document, width, height, antialiasing);
}
