              //          t.ds = 0;
            return;
        }
    }
#else
    if ( ts == Qt::LocalTime )
        brokenDown = localtime( &tmp );
    if ( !brokenDown ) {
        brokenDown = gmtime( &tmp );
        if ( !brokenDown ) {
            d.setJD( ExtDate::GregorianToJD( 1970, 1, 1 ) );
//          t.ds = 0;
            t.setHMS(0,0,0);
                        return;
        }
    }
#endif

    d.setJD( ExtDate::GregorianToJD( brokenDown->tm_year + 1900,
                                     brokenDown->tm_mon + 1,
                                     brokenDown->tm_mday ) );
    t.setHMS( brokenDown->tm_hour, brokenDown->tm_min, brokenDown->tm_sec );
//              t.ds = MSECS_PER_HOUR * brokenDown->tm_hour +
//         MSECS_PER_MIN * brokenDown->tm_min +
//         1000 * brokenDown->tm_sec;
}
#ifndef QT_NO_DATESTRING
#ifndef QT_NO_SPRINTF
/*!
    \overload

    Returns the datetime as a string. The \a f parameter determines
    the format of the string.

    If \a f is \c Qt::TextDate, the string format is "Wed May 20
    03:40:13 1998" (using ExtDate::shortDayName(), ExtDate::shortMonthName(),
    and QTime::toString() to generate the string, so the day and month
    names will have localized names).

    If \a f is \c Qt::ISODate, the string format corresponds to the
    ISO 8601 extended specification for representations of dates and
    times, which is YYYY-MM-DDTHH:MM:SS.

    If \a f is \c Qt::LocalDate, the string format depends on the
    locale settings of the system.

    If the format \a f is invalid or the datetime is invalid, toString()
    returns a null string.

    \sa ExtDate::toString() QTime::toString()
*/

QString ExtDateTime::toString( Qt::DateFormat f ) const
{
    if ( !isValid() )
        return QString::null;

    if ( f == Qt::ISODate ) {
        return d.toString( Qt::ISODate ) + "T" + t.toString( Qt::ISODate );
    }
#ifndef QT_NO_TEXTDATE
    else if ( f == Qt::TextDate ) {
#ifndef Q_WS_WIN
        QString buf = d.shortDayName( d.dayOfWeek() );
        buf += ' ';
        buf += d.shortMonthName( d.month() );
        buf += ' ';
        buf += QString().setNum( d.day() );
        buf += ' ';
#else
        QString buf;
        QString winstr;
        QT_WA( {
            TCHAR out[255];
            GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_ILDATE, out, 255 );
            winstr = QString::fromUcs2( (ushort*)out );
        } , {
            char out[255];
            GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_ILDATE, (char*)&out, 255 );
            winstr = QString::fromLocal8Bit( out );
        } );
        switch ( winstr.toInt() ) {
        case 1:
            buf = d.shortDayName( d.dayOfWeek() ) + " " + QString().setNum( d.day() ) + ". " + d.shortMonthName( d.month() ) + " ";
            break;
        default:
            buf = d.shortDayName( d.dayOfWeek() ) + " " + d.shortMonthName( d.month() ) + " " + QString().setNum( d.day() ) + " ";
            break;
        }
#endif
        buf += t.toString();
        buf += ' ';
        buf += QString().setNum( d.year() );
        return buf;
    }
#endif
    else if ( f == Qt::LocalDate ) {
        return d.toString( Qt::LocalDate ) + " " + t.toString( Qt::LocalDate );
    }
    return QString::null;
}
#endif

/*!
    Returns the datetime as a string. The \a format parameter
    determines the format of the result string.

    These expressions may be used for the date:

    \table
    \header \i Expression \i Output
    \row \i d \i the day as number without a leading zero (1-31)
    \row \i dd \i the day as number with a leading zero (01-31)
    \row \i ddd
            \i the abbreviated localized day name (e.g. 'Mon'..'Sun').
            Uses ExtDate::shortDayName().
    \row \i dddd
            \i the long localized day name (e.g. 'Monday'..'Sunday').
            Uses ExtDate::longDayName().
    \row \i M \i the month as number without a leading zero (1-12)
    \row \i MM \i the month as number with a leading zero (01-12)
    \row \i MMM
            \i the abbreviated localized month name (e.g. 'Jan'..'Dec').
            Uses ExtDate::shortMonthName().
    \row \i MMMM
            \i the long localized month name (e.g. 'January'..'December').
            Uses ExtDate::longMonthName().
    \row \i yy \i the year as two digit number (00-99)
    \row \i yyyy \i the year as four digit number (1752-8000)
    \endtable

    These expressions may be used for the time:

    \table
    \header \i Expression \i Output
    \row \i h
            \i the hour without a leading zero (0..23 or 1..12 if AM/PM display)
    \row \i hh
            \i the hour with a leading zero (00..23 or 01..12 if AM/PM display)
    \row \i m \i the minute without a leading zero (0..59)
    \row \i mm \i the minute with a leading zero (00..59)
    \row \i s \i the second whithout a leading zero (0..59)
    \row \i ss \i the second whith a leading zero (00..59)
    \row \i z \i the milliseconds without leading zeroes (0..999)
    \row \i zzz \i the milliseconds with leading zeroes (000..999)
    \row \i AP
            \i use AM/PM display. \e AP will be replaced by either "AM" or "PM".
    \row \i ap
            \i use am/pm display. \e ap will be replaced by either "am" or "pm".
    \endtable

    All other input characters will be ignored.

    Example format strings (assumed that the ExtDateTime is
    21<small><sup>st</sup></small> May 2001 14:13:09)

    \table
    \header \i Format \i Result
    \row \i dd.MM.yyyy      \i11 21.05.2001
    \row \i ddd MMMM d yy   \i11 Tue May 21 01
    \row \i hh:mm:ss.zzz    \i11 14:13:09.042
    \row \i h:m:s ap        \i11 2:13:9 pm
    \endtable

    If the datetime is an invalid datetime, then QString::null will be returned.

    \sa ExtDate::toString() QTime::toString()
*/
QString ExtDateTime::toString( const QString& format ) const
{
    return fmtDateTime( format, &t, &d );
}
#endif //QT_NO_DATESTRING

/*!
    Returns a ExtDateTime object containing a datetime \a ndays days
    later than the datetime of this object (or earlier if \a ndays is
    negative).

    \sa daysTo(), addMonths(), addYears(), addSecs()
*/

ExtDateTime ExtDateTime::addDays( int ndays ) const
{
    return ExtDateTime( d.addDays(ndays), t );
}

/*!
    Returns a ExtDateTime object containing a datetime \a nmonths months
    later than the datetime of this object (or earlier if \a nmonths
    is negative).

    \sa daysTo(), addDays(), addYears(), addSecs()
*/

ExtDateTime ExtDateTime::addMonths( int nmonths ) const
{
    return ExtDateTime( d.addMonths(nmonths), t );
}

/*!
    Returns a ExtDateTime object containing a datetime \a nyears years
    later than the datetime of this object (or earlier if \a nyears is
    negative).

    \sa daysTo(), addDays(), addMonths(), addSecs()
*/

ExtDateTime ExtDateTime::addYears( int nyears ) const
{
    return ExtDateTime( d.addYears(nyears), t );
}

/*!
    Returns a ExtDateTime object containing a datetime \a nsecs seconds
    later than the datetime of this object (or earlier if \a nsecs is
    negative).

    \sa secsTo(), addDays(), addMonths(), addYears()
*/

ExtDateTime ExtDateTime::addSecs( int nsecs ) const
{
        long int dd = d.jd();
        int tt = MSECS_PER_HOUR*t.hour() + MSECS_PER_MIN*t.minute() + 1000*t.second() + t.msec();
        tt += nsecs*1000;

        while ( tt < 0 ) {
                tt += MSECS_PER_DAY;
                --dd;
        }

        while ( tt > MSECS_PER_DAY ) {
                tt -= MSECS_PER_DAY;
                ++dd;
        }

        ExtDateTime ret;
        ret.setTime( QTime().addMSecs( tt ) );
        ret.setDate( ExtDate( dd ) );

        return ret;
}

/*!
    Returns the number of days from this datetime to \a dt (which is
    negative if \a dt is earlier than this datetime).

    \sa addDays(), secsTo()
*/

int ExtDateTime::daysTo( const ExtDateTime &dt ) const
{
    return d.daysTo( dt.d );
}

/*!
    Returns the number of seconds from this datetime to \a dt (which
    is negative if \a dt is earlier than this datetime).

    Example:
    \code
    ExtDateTime dt = ExtDateTime::currentDateTime();
    ExtDateTime xmas( ExtDate(dt.date().year(),12,24), QTime(17,00) );
    qDebug( "There are %d seconds to Christmas", dt.secsTo(xmas) );
    \endcode

    \sa addSecs(), daysTo(), QTime::secsTo()
*/

int ExtDateTime::secsTo( const ExtDateTime &dt ) const
{
    return t.secsTo(dt.t) + d.daysTo(dt.d)*SECS_PER_DAY;
}


/*!
    Returns TRUE if this datetime is equal to \a dt; otherwise returns FALSE.

    \sa operator!=()
*/

bool ExtDateTime::operator==( const ExtDateTime &dt ) const
{
    return  t == dt.t && d == dt.d;
}

/*!
    Returns TRUE if this datetime is different from \a dt; otherwise
    returns FALSE.

    \sa operator==()
*/

bool ExtDateTime::operator!=( const ExtDateTime &dt ) const
{
    return  t != dt.t || d != dt.d;
}

/*!
    Returns TRUE if this datetime is earlier than \a dt; otherwise
    returns FALSE.
*/

bool ExtDateTime::operator<( const ExtDateTime &dt ) const
{
    if ( d < dt.d )
        return TRUE;
    return d == dt.d ? t < dt.t : FALSE;
}

/*!
    Returns TRUE if this datetime is earlier than or equal to \a dt;
    otherwise returns FALSE.
*/

bool ExtDateTime::operator<=( const ExtDateTime &dt ) const
{
    if ( d < dt.d )
        return TRUE;
    return d == dt.d ? t <= dt.t : FALSE;
}

/*!
    Returns TRUE if this datetime is later than \a dt; otherwise
    returns FALSE.
*/

bool ExtDateTime::operator>( const ExtDateTime &dt ) const
{
    if ( d > dt.d )
        return TRUE;
    return d == dt.d ? t > dt.t : FALSE;
}

/*!
    Returns TRUE if this datetime is later than or equal to \a dt;
    otherwise returns FALSE.
*/

bool ExtDateTime::operator>=( const ExtDateTime &dt ) const
{
    if ( d > dt.d )
        return TRUE;
    return d == dt.d ? t >= dt.t : FALSE;
}

/*!
    \overload

    Returns the current datetime, as reported by the system clock.

    \sa ExtDate::currentDate(), QTime::currentTime()
*/

ExtDateTime ExtDateTime::currentDateTime()
{
    return currentDateTime( Qt::LocalTime );
}

/*!
  Returns the current datetime, as reported by the system clock, for the
  TimeSpec \a ts. The default TimeSpec is LocalTime.

  \sa ExtDate::currentDate(), QTime::currentTime(), Qt::TimeSpec
*/

ExtDateTime ExtDateTime::currentDateTime( Qt::TimeSpec ts )
{
    ExtDateTime dt;
    dt.setDate( ExtDate::currentDate(ts) );
    QTime t = t.currentTime(ts);
    if ( t.hour()==0 && t.minute()==0 )         // midnight or right after?
        dt.setDate( ExtDate::currentDate(ts) ); // fetch date again
    dt.setTime( t );
    return dt;
}

#ifndef QT_NO_DATESTRING
/*!
    Returns the ExtDateTime represented by the string \a s, using the
    format \a f, or an invalid datetime if this is not possible.

    Note for \c Qt::TextDate: It is recommended that you use the
    English short month names (e.g. "Jan"). Although localized month
    names can also be used, they depend on the user's locale settings.

    \warning Note that \c Qt::LocalDate cannot be used here.
*/
ExtDateTime ExtDateTime::fromString( const QString& s, Qt::DateFormat f )
{
        ExtDateTime dt;

        if ( ( s.isEmpty() ) || ( f == Qt::LocalDate ) ) {
#if defined(QT_CHECK_RANGE)
                qWarning( "ExtDateTime::fromString: Parameter out of range" );
#endif
                dt.d.setJD( INVALID_DAY );
                return dt;
        }

        if ( f == Qt::ISODate ) {
                return ExtDateTime( ExtDate::fromString( s.mid(0,10), Qt::ISODate ),
                                        QTime::fromString( s.mid(11), Qt::ISODate ) );
        }
#if !defined(QT_NO_REGEXP) && !defined(QT_NO_TEXTDATE)
        else if ( f == Qt::TextDate ) {

                //parse the time, if it exists.
                QTime time;
                QString sd = s;
                int hour, minute, second;
                int pivot = s.find( QRegExp(QString::fromLatin1("[0-9][0-9]:[0-9][0-9]:[0-9][0-9]")) );
                if ( pivot != -1 ) {
                        hour = s.mid( pivot, 2 ).toInt();
                        minute = s.mid( pivot+3, 2 ).toInt();
                        second = s.mid( pivot+6, 2 ).toInt();
                        time.setHMS( hour, minute, second );

                        sd = s.left( pivot - 1 );
                }

                //sd is now just the date string.
                ExtDate date = ExtDate::fromString( s, Qt::TextDate );
                return ExtDateTime( date, time );
        }

#endif //QT_NO_REGEXP
        return ExtDateTime();
}
#endif //QT_NO_DATESTRING


#ifndef QT_NO_DATASTREAM
Q_EXPORT QDataStream &operator<<( QDataStream & ostream, const ExtDate & date)
{
        return ostream << (Q_UINT32)(date.jd());
}

Q_EXPORT QDataStream &operator>>( QDataStream & ostream, ExtDate & date)
{
        Q_UINT32 julday;
        ostream >> julday;
        date.setJD( julday );
        return ostream;
}

Q_EXPORT QDataStream &operator<<( QDataStream & ostream, const ExtDateTime & dt)
{
        return ostream << dt.d << dt.t;
}

Q_EXPORT QDataStream &operator>>( QDataStream & ostream, ExtDateTime & dt)
{
        ostream >> dt.d >> dt.t;
        return ostream;
}

#endif // QT_NO_DATASTREAM
