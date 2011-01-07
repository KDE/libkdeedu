/*************************************************************************
** Definition of extended range date class
** (c) 2004 by Michel Guitel <michel.guitel@sap.ap-hop-paris.fr>
** modifications by Jason Harris <kstars@30doradus.org>
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#ifndef EXTDATETIME_H
#define EXTDATETIME_H

#include <limits.h>
#include "tqstring.h"
#include "tqnamespace.h"
#include "tqdatetime.h"
#include <kdemacros.h>

#define INVALID_DAY LONG_MIN

class ExtDateTime;

/*****************************************************************************
  ExtDate class
 *****************************************************************************/

extern	void test2_unit(int y, int m, int d);

class ExtDateTime;

class KDE_EXPORT ExtDate
{
public:
	ExtDate() : m_jd(INVALID_DAY), m_year(0), m_month(0), m_day(0) {}
	ExtDate( int y, int m, int d );
	ExtDate( const TQDate &q ) { ExtDate( q.year(), q.month(), q.day() ); }
	ExtDate( long int jd );

	bool isNull() const { return m_jd == INVALID_DAY; }
	bool isValid() const;

	TQDate qdate() const;

	int year() const { return m_year; }
	int month() const { return m_month; }
	int day() const { return m_day; }
	int dayOfWeek() const;
	int dayOfYear() const;
	int daysInMonth() const;
	int daysInYear() const;
	int weekNumber( int *yearNum = 0 ) const;
	long int jd() const { return m_jd; }

#ifndef QT_NO_TEXTDATE
#ifndef QT_NO_COMPAT
	static TQString monthName( int month ) { return shortMonthName( month ); }
	static TQString dayName( int weekday ) { return shortDayName( weekday ); }
#endif
	static TQString shortMonthName( int month );
	static TQString shortDayName( int weekday );
	static TQString longMonthName( int month );
	static TQString longDayName( int weekday );
#endif //QT_NO_TEXTDATE
#ifndef QT_NO_TEXTSTRING
#if !defined(QT_NO_SPRINTF)
	TQString toString( Qt::DateFormat f = Qt::TextDate ) const;
#endif
	TQString toString( const TQString& format ) const;
#endif
	bool setYMD( int y, int m, int d );
	bool setJD( long int _jd );

	ExtDate addDays( int days ) const;
	ExtDate addMonths( int months ) const;
	ExtDate addYears( int years ) const;
	int daysTo( const ExtDate & ) const;

	bool operator==( const ExtDate &d ) const { return m_jd == d.jd(); }
	bool operator!=( const ExtDate &d ) const { return m_jd != d.jd(); }
	bool operator<( const ExtDate &d )  const { return m_jd  < d.jd(); }
	bool operator<=( const ExtDate &d ) const { return m_jd <= d.jd(); }
	bool operator>( const ExtDate &d )  const { return m_jd  > d.jd(); }
	bool operator>=( const ExtDate &d ) const { return m_jd >= d.jd(); }

	static ExtDate currentDate( Qt::TimeSpec ts = Qt::LocalTime );
#ifndef QT_NO_DATESTRING
	static ExtDate fromString( const TQString &s );
	static ExtDate fromString( const TQString &s, Qt::DateFormat f );
#endif
	static bool isValid( int y, int m, int d );
	static bool leapYear( int year );

	static long int GregorianToJD( int y, int m, int d );
	static void JDToGregorian( long int jd, int &y, int &m, int &d );

private:
	static int dayOfYear(int y, int m, int d);

	long int m_jd;
	int m_year, m_month, m_day;
	static uint m_monthLength[12];
	static uint m_monthOrigin[12];
	static TQString m_shortMonthNames[12];
	static TQString m_shortDayNames[7];
	static TQString m_longMonthNames[12];
	static TQString m_longDayNames[7];

	friend class ExtDateTime;

#ifndef QT_NO_DATASTREAM
	friend Q_EXPORT TQDataStream &operator<<( TQDataStream &, const ExtDate & );
	friend Q_EXPORT TQDataStream &operator>>( TQDataStream &, ExtDate & );
	friend Q_EXPORT TQDataStream &operator<<( TQDataStream &, const ExtDateTime & );
	friend Q_EXPORT TQDataStream &operator>>( TQDataStream &, ExtDateTime & );
#endif
};

/*****************************************************************************
  ExtDateTime class
 *****************************************************************************/

class KDE_EXPORT ExtDateTime
{
public:
    ExtDateTime() {}				// set null date and null time
    ExtDateTime( const ExtDate & );
    ExtDateTime( const ExtDate &, const TQTime & );

    bool   isNull()	const		{ return d.isNull() && t.isNull(); }
    bool   isValid()	const		{ return d.isValid() && t.isValid(); }

    ExtDate  date()	const		{ return d; }
    TQTime  time()	const		{ return t; }
    uint   toTime_t()	const;
    void   setDate( const ExtDate &date ) { d = date; }
    void   setTime( const TQTime &time ) { t = time; }
    void   setTime_t( uint secsSince1Jan1970UTC );
    void   setTime_t( uint secsSince1Jan1970UTC, Qt::TimeSpec );
#ifndef QT_NO_DATESTRING
#ifndef QT_NO_SPRINTF
    TQString toString( Qt::DateFormat f = Qt::TextDate )	const;
#endif
    TQString toString( const TQString& format ) const;
#endif
    ExtDateTime addDays( int days )	const;
    ExtDateTime addMonths( int months )   const;
    ExtDateTime addYears( int years )     const;
    ExtDateTime addSecs( int secs )	const;
    int	   daysTo( const ExtDateTime & )	const;
    int	   secsTo( const ExtDateTime & )	const;

    bool   operator==( const ExtDateTime &dt ) const;
    bool   operator!=( const ExtDateTime &dt ) const;
    bool   operator<( const ExtDateTime &dt )  const;
    bool   operator<=( const ExtDateTime &dt ) const;
    bool   operator>( const ExtDateTime &dt )  const;
    bool   operator>=( const ExtDateTime &dt ) const;

    static ExtDateTime currentDateTime();
    static ExtDateTime currentDateTime( Qt::TimeSpec );
#ifndef QT_NO_DATESTRING
    static ExtDateTime fromString( const TQString &s );
    static ExtDateTime fromString( const TQString &s, Qt::DateFormat f );
#endif
private:
    ExtDate  d;
    TQTime  t;
#ifndef QT_NO_DATASTREAM
    friend Q_EXPORT TQDataStream &operator<<( TQDataStream &, const ExtDateTime &);
    friend Q_EXPORT TQDataStream &operator>>( TQDataStream &, ExtDateTime & );
#endif
};

/*****************************************************************************
  Date and time stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
Q_EXPORT TQDataStream &operator<<( TQDataStream &, const ExtDate & );
Q_EXPORT TQDataStream &operator>>( TQDataStream &, ExtDate & );
#endif // QT_NO_DATASTREAM

#endif // EXTDATE_H

