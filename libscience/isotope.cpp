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
#include "isotope.h"
#include "chemicaldataobject.h"

#include <kdebug.h>

Isotope::Isotope()
{ 
}

Isotope::~Isotope()
{
}

void Isotope::addData( ChemicalDataObject o )
{
	if ( o.type() == ChemicalDataObject::exactMass )
		m_mass = o;
	else if ( o.type() == ChemicalDataObject::atomicNumber )
		m_identifier = o;
	else if ( o.type() == ChemicalDataObject::symbol )
		m_parentElementSymbol = o;
	else if ( o.type() == ChemicalDataObject::spin )
		m_spin = o;
	else if ( o.type() == ChemicalDataObject::magneticMoment )
		m_magmoment = o;
	else if ( o.type() == ChemicalDataObject::relativeAbundance )
		m_abundance = o;
	else if ( o.type() == ChemicalDataObject::halfLife )
		m_halflife = o;
	else if ( o.type() == ChemicalDataObject::ecDecay )
		m_ecdecay = o;
	else if ( o.type() == ChemicalDataObject::ecDecayLikeliness )
		m_eclikeliness = o;
	else if ( o.type() == ChemicalDataObject::betaplusDecay )
		m_betaplusdecay = o;
	else if ( o.type() == ChemicalDataObject::betaplusDecayLikeliness )
		m_betapluslikeliness = o;
	else if (  o.type() == ChemicalDataObject::betaminusDecay )
		m_betaminusdecay = o;
	else if (  o.type() == ChemicalDataObject::betaminusDecayLikeliness )
		m_betaminuslikeliness = o;
	else if ( o.type() == ChemicalDataObject::alphaDecay )
		m_alphadecay = o;
	else if ( o.type() == ChemicalDataObject::alphaDecayLikeliness )
		m_alphalikeliness = o;
	
}

double Isotope::mass() const
{
	//return m_mass ? m_mass.value().toDouble() : -1.0;
	return -1.0;
}

QString Isotope::errorMargin() const
{
	//return m_mass ? m_mass.errorValue().toString() : QString();
	return QString();
}

int Isotope::parentElementNumber() const
{
//	return m_identifier ? m_identifier.value().toInt() : -1;
	return -1;
}

QString Isotope::spin() const
{
	//return m_spin ? m_spin.value().toString() : QString();
	return QString();
}

QString Isotope::magmoment() const
{
	//return m_magmoment ? m_magmoment.value().toString() : QString();
	return QString();
}

QString Isotope::abundance() const
{
	//return m_abundance ? m_abundance.value().toString() : QString();
	return QString();
}

double Isotope::halflife() const
{
	//return m_halflife ? m_halflife.value().toDouble() : -1.0;
	return -1.0;
}
double Isotope::ecdecay() const
{
	//return m_ecdecay ? m_ecdecay.value().toDouble() : -1.0;
	return -1.0;
}
double Isotope::eclikeliness() const
{
	//return m_eclikeliness ? m_eclikeliness.value().toDouble() : -1.0;
	return -1.0;
}
double Isotope::betaplusdecay() const
{
	//return m_betaplusdecay ? m_betaplusdecay.value().toDouble() : -1.0;
	return -1.0;
}
double Isotope::betapluslikeliness() const
{
	//return m_betapluslikeliness ? m_betapluslikeliness.value().toDouble() : -1.0;
	return -1.0;
}
double Isotope::betaminusdecay() const
{
	//return m_betaminusdecay ? m_betaminusdecay.value().toDouble() : -1.0;
	return -1.0;
}
double Isotope::betaminuslikeliness() const
{
	//return m_betaminuslikeliness ? m_betaminuslikeliness.value().toDouble() : -1.0;
	return -1.0;
}
double Isotope::alphadecay() const
{
	//return m_alphadecay ? m_alphadecay.value().toDouble() : -1.0;
	return -1.0;
}
double Isotope::alphalikeliness() const
{
	//return m_alphalikeliness ? m_alphalikeliness.value().toDouble() : -1.0;
	return -1.0;
}
QString Isotope::parentElementSymbol() const
{
	//return m_parentElementSymbol ? m_parentElementSymbol.value().toString() : QString();
	return QString();
}

void Isotope::setNucleons( int number )
{
	//m_numberOfNucleons = number;
}

int Isotope::nucleons() const
{
	//return m_numberOfNucleons;
	return 0;
}

Isotope::Nucleons Isotope::nucleonsAfterDecay( Decay kind )
{
	Isotope::Nucleons n;
	int protons = m_identifier.value().toInt();
	int neutrons = m_numberOfNucleons - protons;
	n.protons = protons;
	n.neutrons = neutrons;

	switch ( kind )
	{
		case ALPHA:
			n.protons-=2;
			break;
		case BETAMINUS:
			n.protons+=1;
			n.neutrons-=1;
			break;
		case BETAPLUS:
			n.protons-=1;
			break;
		case EC:
			n.protons-=1;
			n.neutrons+=1;
			break;
	}

	return n;
}
