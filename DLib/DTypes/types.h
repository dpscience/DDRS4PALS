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

#ifndef DTYPES_H
#define DTYPES_H

#include "../DTypes/defines.h"

template<typename T> class DVector
{
    T* m_array;
    DVEC_INDEX m_size;

public:
    inline DVector<T>() :
        m_array(nullptr),
        m_size(0) {}

    inline DVector<T>(const DVEC_INDEX& size) :
        m_array(nullptr)
    {
        if ( size < 0 )
        {
            DERRORLOG("DVector: Bad Vector-Size: Size set to Zero.");
            m_size = 0;
        }
        else
            m_size = size;

        m_array = new T[m_size];
    }

    inline DVector<T>& operator=(const DVector& cpyDVec)
    {
        DDELETE_SAFETY(m_array);

        m_array = new T(cpyDVec.getSize());
        m_size  = cpyDVec.getSize();

        for ( DVEC_INDEX i = 0 ; i < cpyDVec.getSize() ; ++ i )
        {
            m_array[i] = cpyDVec.getAt(i);
        }

        return *this;
    }

    inline DVector<T>(const DVector& cpyDVec)
    {
        DDELETE_SAFETY(m_array);

        m_array = new T(cpyDVec.getSize());
        m_size  = cpyDVec.getSize();

        for ( DVEC_INDEX i = 0 ; i < cpyDVec.getSize() ; ++ i )
        {
            m_array[i] = cpyDVec.getAt(i);
        }
    }

    inline DVector<T>(const QList<T>& list) :
        m_array(nullptr)
    {
        m_array = new T((DVEC_INDEX)list.size());
        m_size  = (DVEC_INDEX)list.size();

        for ( DVEC_INDEX i = 0 ; i < getSize() ; ++ i )
        {
            m_array[i] = list[i];
        }
    }

    inline ~DVector<T>() { DDELETE_SAFETY(m_array); }

    inline void resize(const DVEC_INDEX& size)
    {
        if ( size < 0 )
        {
            DERRORLOG("DVector: Bad Vector-Size: Size set to Zero.");
            m_size = 0;
        }
        else
            m_size = size;

        DDELETE_SAFETY(m_array);

        m_array = new T[m_size];
    }

    DVEC_INDEX getSize() const { return m_size; }

    inline void setAt(const DVEC_INDEX& index, const T& value)
    {
        if ( index < 0 || index >= m_size )
        {
            DERRORLOG("DVector: Bad Vector-Access: Index: %i @ Size: %i", index, m_size);
            return;
        }

        m_array[index] = value;
    }

    T getAt(const DVEC_INDEX& index) const
    {
        if ( index < 0 || index >= m_size )
        {
            DERRORLOG("DVector: Bad Vector-Access: Index: %i @ Size: %i", index, m_size);
            return T();
        }

        return m_array[index];
    }

    T& operator[](const DVEC_INDEX& index)
    {
        if ( index < 0 || index >= m_size )
        {
            DERRORLOG("DVector: Bad Vector-Access: Index: %i @ Size: %i", index, m_size);
            return m_array[0];
        }

        return m_array[index];
    }
};

class DPointF : public QObject
{
    double m_x, m_y;

    Q_OBJECT
public:
    inline DPointF() : QObject(nullptr), m_x(0.0f), m_y(0.0f) {}
    inline DPointF(const double& x, const double& y) : QObject(nullptr), m_x(x), m_y(y) {}
    inline DPointF(const DPointF& p) : QObject(nullptr)
    {
        m_x = p.getX();
        m_y = p.getY();
    }

    inline DPointF& operator=(const DPointF& p)
    {
        m_x = p.getX();
        m_y = p.getY();

        return *this;
    }

    inline virtual ~DPointF() {}

    inline double getX() const { return m_x; }
    inline double getY() const { return m_y; }

public slots:
    inline void setX(const double& x) { m_x = x; }
    inline void setY(const double& y) { m_y = y; }
};


class DColor : public QColor
{
public:
    inline DColor() : QColor() {}
    inline DColor(const QColor& color) : QColor(color) {}
    inline DColor(const DColor& color) : QColor((QColor)color) {}

    inline virtual ~DColor() {}
};


class DString : public QString
{
public:
    inline DString() : QString("") {}
    inline DString(const QString& val) : QString(val) {}
    inline DString(const DString& val) : QString((QString)val) {}
    inline DString(const QChar* val) : QString(val) {}

    virtual ~DString();

    DString parseBetween(const DString& start, const DString& stop) const;
    DString parseBetween(const QString& start, const QString& stop) const;
    QStringList parseBetween2(const QString &start, const QString &stop) const;
    QStringList parseBetween2(const DString &start, const DString &stop) const;
};


class DSVGImage
{
public:
    static QImage getImage(const QString &document, double width, double height, bool antialiasing = true);
    static QImage getImage(const DString &document, double width, double height, bool antialiasing = true);

private:
    inline DSVGImage()  {}
    inline virtual ~DSVGImage() {}
};

#endif // DTYPES_H
