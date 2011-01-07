/****************************************************************************
**
**
** Definition of date and time edit classes
**
** Created : 001103
**
** Original QDateTimeEdit Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** >> modifications to introduce ExtDate (C) 2004 Jason Harris <jharris@30doradus.org>
** >> ExtDate modifications are licensed under the GPL: http://www.gnu.org/licenses/gpl.html
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
**********************************************************************/

#ifndef EXTDATETIMEEDIT_H
#define EXTDATETIMEEDIT_H

#ifndef QT_H
#include <tqwidget.h>
#include <tqstring.h>
#endif // QT_H

#include "extdatetime.h"

#ifndef QT_NO_DATETIMEEDIT

class QTimeEdit;

class ExtDateTimeEditBase : public QWidget
{
    Q_OBJECT
public:
    ExtDateTimeEditBase( TQWidget* parent=0, const char* name=0 )
	: TQWidget( parent, name ) {}

    virtual bool setFocusSection( int sec ) = 0;
    virtual TQString sectionFormattedText( int sec ) = 0;
    virtual void addNumber( int sec, int num ) = 0;
    virtual void removeLastNumber( int sec ) = 0;

public slots:
    virtual void stepUp() = 0;
    virtual void stepDown() = 0;

private:
#if defined(Q_DISABLE_COPY) // Disabled copy constructor and operator=
    ExtDateTimeEditBase( const ExtDateTimeEditBase & );
    ExtDateTimeEditBase &operator=( const ExtDateTimeEditBase & );
#endif
};

class ExtDateEditPrivate;

class KDE_EXPORT ExtDateEdit : public ExtDateTimeEditBase
{
    Q_OBJECT
    Q_ENUMS( Order )
    Q_PROPERTY( Order order READ order WRITE setOrder )
//    Q_PROPERTY( ExtDate date READ date WRITE setDate )
    Q_PROPERTY( bool autoAdvance READ autoAdvance WRITE setAutoAdvance )
//    Q_PROPERTY( ExtDate maxValue READ maxValue WRITE setMaxValue )
//    Q_PROPERTY( ExtDate minValue READ minValue WRITE setMinValue )

public:
    ExtDateEdit( TQWidget* parent=0,  const char* name=0 );
    ExtDateEdit( const ExtDate& date, TQWidget* parent=0,  const char* name=0 );
    ~ExtDateEdit();

    enum Order { DMY /**< Day-Month-Year */, 
      MDY /**< Month-Day-Year */, 
      YMD /**< Year-Month-Day, also the default */, 
      YDM /**< Year-Day-Month @deprecated Included for completeness. */ };

    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

public slots:
    virtual void setDate( const ExtDate& date );

public:
    ExtDate date() const;
    virtual void setOrder( Order order );
    Order order() const;
    virtual void setAutoAdvance( bool advance );
    bool autoAdvance() const;

    virtual void setMinValue( const ExtDate& d ) { setRange( d, maxValue() ); }
    ExtDate minValue() const;
    virtual void setMaxValue( const ExtDate& d ) { setRange( minValue(), d ); }
    ExtDate maxValue() const;
    virtual void setRange( const ExtDate& min, const ExtDate& max );
    TQString separator() const;
    virtual void setSeparator( const TQString& s );

    // Make removeFirstNumber() virtual in ExtDateTimeEditBase in 4.0
    void removeFirstNumber( int sec );

signals:
    void valueChanged( const ExtDate& date );

protected:
    bool event( TQEvent *e );
    void timerEvent( TQTimerEvent * );
    void resizeEvent( TQResizeEvent * );
    void stepUp();
    void stepDown();
    TQString sectionFormattedText( int sec );
    void addNumber( int sec, int num );

    void removeLastNumber( int sec );
    bool setFocusSection( int s );

    virtual void setYear( int year );
    virtual void setMonth( int month );
    virtual void setDay( int day );
    virtual void fix();
    virtual bool outOfRange( int y, int m, int d ) const;

protected slots:
    void updateButtons();

private:
    void init();
    int sectionOffsetEnd( int sec ) const;
    int sectionLength( int sec ) const;
    TQString sectionText( int sec ) const;
    ExtDateEditPrivate* d;

#if defined(Q_DISABLE_COPY)
    ExtDateEdit( const ExtDateEdit & );
    ExtDateEdit &operator=( const ExtDateEdit & );
#endif
};

// class QTimeEditPrivate;
//
// class Q_EXPORT QTimeEdit : public ExtDateTimeEditBase
// {
//     Q_OBJECT
//     Q_SETS( Display )
//     Q_PROPERTY( TQTime time READ time WRITE setTime )
//     Q_PROPERTY( bool autoAdvance READ autoAdvance WRITE setAutoAdvance )
//     Q_PROPERTY( TQTime maxValue READ maxValue WRITE setMaxValue )
//     Q_PROPERTY( TQTime minValue READ minValue WRITE setMinValue )
//     Q_PROPERTY( Display display READ display WRITE setDisplay )
//
// public:
//     enum Display {
// 	Hours	= 0x01,
// 	Minutes	= 0x02,
// 	Seconds	= 0x04,
// 	/*Reserved = 0x08,*/
// 	AMPM	= 0x10
//     };
//
//     QTimeEdit( TQWidget* parent=0,  const char* name=0 );
//     QTimeEdit( const TQTime& time, TQWidget* parent=0,  const char* name=0 );
//     ~QTimeEdit();
//
//     TQSize sizeHint() const;
//     TQSize minimumSizeHint() const;
//
// public slots:
//     virtual void setTime( const TQTime& time );
//
// public:
//     TQTime time() const;
//     virtual void setAutoAdvance( bool advance );
//     bool autoAdvance() const;
//
//     virtual void setMinValue( const TQTime& d ) { setRange( d, maxValue() ); }
//     TQTime minValue() const;
//     virtual void setMaxValue( const TQTime& d ) { setRange( minValue(), d ); }
//     TQTime maxValue() const;
//     virtual void setRange( const TQTime& min, const TQTime& max );
//     TQString separator() const;
//     virtual void setSeparator( const TQString& s );
//
//     uint display() const;
//     void setDisplay( uint disp );
//
//     // Make removeFirstNumber() virtual in ExtDateTimeEditBase in 4.0
//     void removeFirstNumber( int sec );
//
// signals:
//     void valueChanged( const TQTime& time );
//
// protected:
//     bool event( TQEvent *e );
//     void timerEvent( TQTimerEvent *e );
//     void resizeEvent( TQResizeEvent * );
//     void stepUp();
//     void stepDown();
//     TQString sectionFormattedText( int sec );
//     void addNumber( int sec, int num );
//     void removeLastNumber( int sec );
//     bool setFocusSection( int s );
//
//     virtual bool outOfRange( int h, int m, int s ) const;
//     virtual void setHour( int h );
//     virtual void setMinute( int m );
//     virtual void setSecond( int s );
//
// protected slots:
//     void updateButtons();
//
// private:
//     void init();
//     TQString sectionText( int sec );
//     QTimeEditPrivate* d;
//
// #if defined(Q_DISABLE_COPY)
//     QTimeEdit( const QTimeEdit & );
//     QTimeEdit &operator=( const QTimeEdit & );
// #endif
// };
//

class ExtDateTimeEditPrivate;

class KDE_EXPORT ExtDateTimeEdit : public QWidget
{
    Q_OBJECT
//    Q_PROPERTY( ExtDateTime dateTime READ dateTime WRITE setDateTime )

public:
    ExtDateTimeEdit( TQWidget* parent=0, const char* name=0 );
    ExtDateTimeEdit( const ExtDateTime& datetime, TQWidget* parent=0,
		   const char* name=0 );
    ~ExtDateTimeEdit();

    TQSize sizeHint() const;
    TQSize minimumSizeHint() const;

public slots:
    virtual void setDateTime( const ExtDateTime & dt );

public:
    ExtDateTime dateTime() const;

    ExtDateEdit* dateEdit() { return de; }
    QTimeEdit* timeEdit() { return te; }

    virtual void setAutoAdvance( bool advance );
    bool autoAdvance() const;

signals:
    void valueChanged( const ExtDateTime& datetime );

protected:
    // ### make init() private in Qt 4.0
    void init();
    void resizeEvent( TQResizeEvent * );

protected slots:
    // ### make these two functions private in Qt 4.0,
    //     and merge them into one with no parameter
    void newValue( const ExtDate& d );
    void newValue( const TQTime& t );

private:
    ExtDateEdit* de;
    QTimeEdit* te;
    ExtDateTimeEditPrivate* d;

#if defined(Q_DISABLE_COPY)
    ExtDateTimeEdit( const ExtDateTimeEdit & );
    ExtDateTimeEdit &operator=( const ExtDateTimeEdit & );
#endif
};

class QNumberSection
{
public:
    QNumberSection( int selStart = 0, int selEnd = 0, bool separat = TRUE, int actual = -1 )
	: selstart( selStart ), selend( selEnd ), act( actual ), sep( separat )
    {}
    int selectionStart() const { return selstart; }
    void setSelectionStart( int s ) { selstart = s; }
    int selectionEnd() const { return selend; }
    void setSelectionEnd( int s ) { selend = s; }
    int width() const { return selend - selstart; }
    int index() const { return act; }
    bool separator() const { return sep; }
    Q_DUMMY_COMPARISON_OPERATOR( QNumberSection )
private:
    int selstart :12;
    int selend	 :12;
    int act	 :7;
    bool sep	 :1;
};

class ExtDateTimeEditorPrivate;

class ExtDateTimeEditor : public QWidget
{
    Q_OBJECT
public:
    ExtDateTimeEditor( ExtDateTimeEditBase * parent=0,
		       const char * name=0 );
    ~ExtDateTimeEditor();

//    void setControlWidget( ExtDateTimeEditBase * widget );
//    ExtDateTimeEditBase * controlWidget() const;

    void setSeparator( const TQString& s );
    TQString separator() const;

    int  focusSection() const;
    bool setFocusSection( int s );
    void appendSection( const QNumberSection& sec );
    void clearSections();
    void setSectionSelection( int sec, int selstart, int selend );
    bool eventFilter( TQObject *o, TQEvent *e );
    int  sectionAt( const TQPoint &p );
    int mapSection( int sec );

protected:
    void init();
    bool event( TQEvent *e );
    void resizeEvent( TQResizeEvent * );
    void paintEvent( TQPaintEvent * );
    void mousePressEvent( TQMouseEvent *e );

private:
    ExtDateTimeEditBase* cw;
    ExtDateTimeEditorPrivate* d;
};

#endif
#endif
