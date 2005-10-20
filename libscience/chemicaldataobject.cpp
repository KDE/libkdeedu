/***************************************************************************
 *   Copyright (C) 2005 by Carsten Niehaus                                 *
 *   cniehaus@kde.org                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.          *
 ***************************************************************************/
#include "chemicaldataobject.h"

#include <kdebug.h>

ChemicalDataObject::ChemicalDataObject( QVariant v, BlueObelisk type )
{
	m_value = v;
	m_type = type;
};

ChemicalDataObject::ChemicalDataObject() 
{
}

QString ChemicalDataObject::valueAsString()
{
	return m_value.toString();
}

bool ChemicalDataObject::operator==( const int v )
{
	kdDebug() << "ChemicalDataObject::operator==() with int" << endl;
	if ( m_value.type() != QVariant::Int )	
		return false;
	
	return m_value.toInt() == v;
}

bool ChemicalDataObject::operator==( const bool v )
{
	kdDebug() << "ChemicalDataObject::operator==() with bool" << endl;
	if ( m_value.type() != QVariant::Bool )	
		return false;
	
	return m_value.toBool() == v;
}

bool ChemicalDataObject::operator==( const double v )
{
	kdDebug() << "ChemicalDataObject::operator==() with double" << endl;
	if ( m_value.type() != QVariant::Double )	
		return false;
	
	return m_value.toDouble() == v;
}

bool ChemicalDataObject::operator==( const QString& v )
{
	kdDebug() << "ChemicalDataObject::operator==() with QString" << endl;
	if ( m_value.type() != QVariant::String )	
		return false;

	return m_value.toString() == v;
}

ChemicalDataObject::~ChemicalDataObject()
{}

QString ChemicalDataObject::dictRef()
{
	QString botype;
	switch ( m_type ){
		case atomicNumber:
			botype = "atomicNumber";
			break;
		case symbol:
			botype = "symbol";
			break;
		case name:
			botype = "name";
			break;
		case mass:
			botype = "mass";
			break;
		case exactMass:
			botype = "exactMass";
			break;
		case ionization:
			botype = "ionization";
			break;
		case electronAffinity:
			botype = "electronAffinity";
			break;
		case electronegativityPauling:
			botype = "electronegativityPauling";
			break;
		case radiusCovalent:
			botype = "radiusCovalent";
			break;
		case radiusVDW:
			botype = "radiusVDW";
			break;
	}
	
	botype = botype.prepend( "bo:" );
	
	return botype;
}
	