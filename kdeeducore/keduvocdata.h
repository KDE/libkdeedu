/* This file is part of the KDE Edu Library
   Copyright (C) 2002 Scott Wheeler <wheeler@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEDUVOCDATA_H
#define KEDUVOCDATA_H

#include <QtXml>
#include <QList>

#include <kdemacros.h>

class KDE_EXPORT KEduVocDataItem
{
public:
    KEduVocDataItem();
    KEduVocDataItem(QDomElement &entry);
    virtual ~KEduVocDataItem();

    QString originalText() const;
    QString translatedText() const;

protected:
    QString getText(const QString &tagName) const;

private:
    QDomElement domElement;
};

typedef QList<KEduVocDataItem> KEduVocDataItemList;

class KDE_EXPORT KEduVocData
{
public:
    static KEduVocDataItemList parse(const QString &fileName);
};

#endif // KEDUVOCDATA_H
