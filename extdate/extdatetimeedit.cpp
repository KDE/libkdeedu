/*****  --==>> JH: Commenting entire file, for now.
 *****  This class is derived from QDateTimeEdit, which makes use of 
 *****  QVariant(QDateTime).  It is not possible to translate this to 
 *****  ExtDateTime easily.  I will have to write a custom widget eventually.
 *****  However, we need this directory to compile so that the kde4 port 
 *****  can proceed.  So I am commenting out the class.
 */

#if 0
  --- ENTIRE CLASS COMMENTED OUT ---

/****************************************************************************
**
** Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <private/qabstractspinbox_p.h>
#include <qabstractspinbox.h>
#include <qapplication.h>
#include <qlineedit.h>
#include <qevent.h>
#include <math.h>

#include "extdatetimeedit.h"

#ifndef QT_NO_DATETIMEEDIT

enum {
    Neither = -1,
    AM = 0,
    PM = 1,
    PossibleAM = 2,
    PossiblePM = 3,
    PossibleBoth = 4
};

#ifdef Q_WS_MAC
#include <private/qt_mac_p.h>
extern QString qt_mac_from_pascal_string(const Str255); //qglobal.cpp
#endif

//#define EXTDATETIMEEDIT_EDTEDEBUG
#ifdef EXTDATETIMEEDIT_EDTEDEBUG
#  define EDTEDEBUG qDebug() << QString("%1:%2").arg(__FILE__).arg(__LINE__)
#  define EDTEDEBUGN qDebug
#else
#  define EDTEDEBUG if (false) qDebug()
#  define EDTEDEBUGN if (false) qDebug
#endif
#include <qdebug.h>

class ExtDateTimeEditPrivate : public QAbstractSpinBoxPrivate
{
    Q_DECLARE_PUBLIC(ExtDateTimeEdit)
public:
    enum Section {
        NoSection = 0x0000,
        AmPmSection = 0x0001,
        MSecSection = 0x0002,
        SecondSection = 0x0004,
        MinuteSection = 0x0008,
        HourSection = 0x0010,
        TimeSectionMask = (AmPmSection|MSecSection|SecondSection|MinuteSection|HourSection),
        Internal = 0x8000,
        AmPmLowerCaseSection = AmPmSection|Internal,
        DaySection = 0x0100,
        MonthSection = 0x0200,
        YearSection = 0x0400,
        MonthShortNameSection = MonthSection|Internal,
        YearTwoDigitsSection = YearSection|Internal,
        DateSectionMask = (DaySection|MonthSection|YearSection),
        FirstSection = 0x1000|Internal,
        LastSection = 0x2000|Internal
    }; // duplicated from qdatetimeedit.h

    struct SectionNode {
        Section section;
        int pos;
    };

    ExtDateTimeEditPrivate();

    void readLocaleSettings();

    void emitSignals(EmitPolicy ep, const QVariant &old);
    QString textFromValue(const QVariant &f) const;
    QVariant valueFromText(const QString &f) const;
    QVariant validateAndInterpret(QString &input, int &, QValidator::State &state) const;
    void editorCursorPositionChanged(int lastpos, int newpos);

    QVariant valueForPosition(int pos) const;

    void clearSection(Section s);

    int sectionSize(Section s) const;
    int sectionPos(Section s) const;

    SectionNode sectionNode(Section t) const;
    QVariant stepBy(Section s, int steps, bool test = false) const;
    QString sectionText(const QString &text, Section s) const;
    int getDigit(const QVariant &dt, Section s) const;
    void setDigit(QVariant &t, Section s, int newval) const;
    int sectionValue(Section s, QString &txt, QValidator::State &state) const;
    int absoluteMax(Section s) const;
    int absoluteMin(Section s) const;
    Section sectionAt(int index) const;
    Section closestSection(int index, bool forward) const;
    SectionNode nextPrevSection(Section current, bool forward) const;
    bool parseFormat(const QString &format);
    void setSelected(Section s, bool forward = false);
    QValidator::State checkIntermediate(const ExtDateTime &dt, const QString &str) const;

    bool addSection(QList<SectionNode> &list, Section ds, int pos) const;
    int findMonth(const QString &str1, int index = 1) const;
    int findAmPm(QString &str1, Section s) const;
    int maxChange(ExtDateTimeEditPrivate::Section s) const;
    int potentialValue(const QString &str, int min, int max, Section s) const;
    int potentialValueHelper(const QString &str, int min, int max, int size) const;
    int multiplier(Section s) const;
    QString sectionName(int s) const;
    QString stateName(int s) const;
    QString sectionFormat(int s) const;

    QString displayFormat;
    QString defaultDateFormat, defaultTimeFormat, defaultDateTimeFormat;
    QString escapedFormat;
    // this format is the same amount of letters as the resulting string
    // e.g. if the format is hh:ap and am/pm is translated to foobar/barfoo escapedFormat
    // will be "hh:ap    "
    QList<SectionNode> sectionNodes;
    SectionNode first, last;
    QStringList separators;
    ExtDateTimeEdit::Sections display;
    mutable int cachedDay;
    mutable Section currentSection;
    Qt::LayoutDirection layoutDirection;
};

// --- ExtDateTimeEdit ---

/*!
  \class ExtDateTimeEdit qdatetimeedit.h
  \brief The ExtDateTimeEdit class provides a widget for editing dates and times.

  \ingroup basic
  \mainclass

  ExtDateTimeEdit allows the user to edit dates by using the keyboard or
  the arrow keys to increase and decrease date and time values. The
  arrow keys can be used to move from section to section within the
  ExtDateTimeEdit box. Dates and times appear in accordance with the
  format set; see setDisplayFormat().

  \code
  ExtDateTimeEdit *dateEdit = new ExtDateTimeEdit(ExtDate::currentDate());
  dateEdit->setMinimumDate(ExtDate::currentDate().addDays(-365));
  dateEdit->setMaximumDate(ExtDate::currentDate().addDays(365));
  dateEdit->setDisplayFormat("yyyy.MM.dd");
  \endcode

  Here we've created a new ExtDateTimeEdit object initialized with
  today's date, and restricted the valid date range to today plus or
  minus 365 days. We've set the order to month, day, year.

  The maximum and minimum values for a date value in the date editor
  default to the maximum and minimum values for a ExtDate. You can
  change this by calling setMinimumDate(), setMaximumDate(),
  setMinimumTime(), and setMaximumTime().
*/

/*!
  \enum ExtDateTimeEdit::Section

  \value NoSection
  \value AmPmSection
  \value MSecSection
  \value SecondSection
  \value MinuteSection
  \value HourSection
  \value DaySection
  \value MonthSection
  \value YearSection
  \omitvalue DateSections_Mask
  \omitvalue TimeSections_Mask
*/

/*!
  \fn void ExtDateTimeEdit::dateTimeChanged(const ExtDateTime &datetime)

  This signal is emitted whenever the date or time is changed. The
  new date and time is passed in \a datetime.
*/

/*!
  \fn void ExtDateTimeEdit::timeChanged(const QTime &time)

  This signal is emitted whenever the time is changed. The new time
  is passed in \a time.
*/

/*!
  \fn void ExtDateTimeEdit::dateChanged(const ExtDate &date)

  This signal is emitted whenever the date is changed. The new date
  is passed in \a date.
*/


/*!
  Constructs an empty date time editor with a \a parent.
*/

ExtDateTimeEdit::ExtDateTimeEdit(QWidget *parent)
    : QAbstractSpinBox(*new ExtDateTimeEditPrivate, parent)
{
    Q_D(ExtDateTimeEdit);

    d->minimum = QVariant(DATETIME_MIN);
    d->maximum = QVariant(DATETIME_MAX);
    d->value = QVariant(ExtDateTime(DATE_INITIAL, TIME_MIN));
    setDisplayFormat(d->defaultDateTimeFormat);
    if (d->displayFormat.isEmpty()) {
        d->defaultDateTimeFormat = QLatin1String("MM/dd/yy hh:mm:ss");
        setDisplayFormat(d->defaultDateTimeFormat);
    }
}

/*!
  Constructs an empty date time editor with a \a parent. The value
  is set to \a datetime.
*/

ExtDateTimeEdit::ExtDateTimeEdit(const ExtDateTime &datetime, QWidget *parent)
    : QAbstractSpinBox(*new ExtDateTimeEditPrivate, parent)
{
    Q_D(ExtDateTimeEdit);
    d->minimum = QVariant(DATETIME_MIN);
    d->maximum = QVariant(DATETIME_MAX);
    d->value = datetime.isValid() ? QVariant(datetime) : QVariant(ExtDateTime(DATE_INITIAL, TIME_MIN));
    setDisplayFormat(d->defaultDateTimeFormat);
    if (d->displayFormat.isEmpty()) {
        d->defaultDateTimeFormat = QLatin1String("MM/dd/yy hh:mm:ss");
        setDisplayFormat(d->defaultDateTimeFormat);
    }
}

/*!
  \fn ExtDateTimeEdit::ExtDateTimeEdit(const ExtDate &date, QWidget *parent)

  Constructs an empty date time editor with a \a parent.
  The value is set to \a date.
*/

ExtDateTimeEdit::ExtDateTimeEdit(const ExtDate &date, QWidget *parent)
    : QAbstractSpinBox(*new ExtDateTimeEditPrivate, parent)
{
    Q_D(ExtDateTimeEdit);
    d->minimum = QVariant(DATETIME_MIN);
    d->maximum = QVariant(DATETIME_MAX);
    d->value = QVariant(ExtDateTime(date.isValid() ? date : DATE_INITIAL, TIME_MIN));
    setDisplayFormat(d->defaultDateFormat);
    if (d->displayFormat.isEmpty()) {
        d->defaultDateFormat = QLatin1String("MM/dd/yy");
        setDisplayFormat(d->defaultDateFormat);
    }
}

/*!
  \fn ExtDateTimeEdit::ExtDateTimeEdit(const QTime &time, QWidget *parent)

  Constructs an empty date time editor with a \a parent.
  The value is set to \a time.
*/

ExtDateTimeEdit::ExtDateTimeEdit(const QTime &time, QWidget *parent)
    : QAbstractSpinBox(*new ExtDateTimeEditPrivate, parent)
{
    Q_D(ExtDateTimeEdit);
    d->minimum = QVariant(DATETIME_MIN);
    d->maximum = QVariant(DATETIME_MAX);
    d->value = QVariant(ExtDateTime(DATE_INITIAL, time.isValid() ? time : TIME_MIN));
    setDisplayFormat(d->defaultTimeFormat);
    if (d->displayFormat.isEmpty()) {
        d->defaultDateFormat = QLatin1String("hh:mm:ss");
        setDisplayFormat(d->defaultTimeFormat);
    }
}

ExtDateTime ExtDateTimeEdit::dateTime() const
{
    Q_D(const ExtDateTimeEdit);
    return d->value.toDateTime();
}

void ExtDateTimeEdit::setDateTime(const ExtDateTime &datetime)
{
    Q_D(ExtDateTimeEdit);
    if (datetime.isValid()) {
        d->cachedDay = -1;
        d->setValue(QVariant(datetime), EmitIfChanged);
    }
}

/*!
  \property ExtDateTimeEdit::date
  \brief the ExtDate that is set in the ExtDateTimeEdit

  \sa time
*/

ExtDate ExtDateTimeEdit::date() const
{
    Q_D(const ExtDateTimeEdit);
    return d->value.toDate();
}

void ExtDateTimeEdit::setDate(const ExtDate &date)
{
    Q_D(ExtDateTimeEdit);
    if (date.isValid()) {
        d->cachedDay = -1;
        d->setValue(QVariant(ExtDateTime(date, d->value.toTime())), EmitIfChanged);
    }
}

/*!
  \property ExtDateTimeEdit::time
  \brief the QTime that is set in the ExtDateTimeEdit

  \sa date
*/

QTime ExtDateTimeEdit::time() const
{
    Q_D(const ExtDateTimeEdit);
    return d->value.toTime();
}

void ExtDateTimeEdit::setTime(const QTime &time)
{
    Q_D(ExtDateTimeEdit);
    if (time.isValid()) {
        d->cachedDay = -1;
        d->setValue(QVariant(ExtDateTime(d->value.toDate(), time)), EmitIfChanged);
    }
}

/*!
  \property ExtDateTimeEdit::dateTime
  \brief the ExtDateTime that is set in the ExtDateTimeEdit

  \sa minimumDate, minimumTime, maximumDate, maximumTime
*/

/*!
  \property ExtDateTimeEdit::minimumDate

  \brief the minimum date of the date time edit

  When setting this property the \l maximumDate is adjusted if
  necessary, to ensure that the range remains valid. If the date is
  not a valid ExtDate object, this function does nothing.

  \sa minimumTime, maximumTime, setDateRange()
*/

ExtDate ExtDateTimeEdit::minimumDate() const
{
    Q_D(const ExtDateTimeEdit);
    return d->minimum.toDate();
}

void ExtDateTimeEdit::setMinimumDate(const ExtDate &min)
{
    Q_D(ExtDateTimeEdit);
    if (min.isValid()) {
        const QVariant m(ExtDateTime(min, d->minimum.toTime()));
        d->setRange(m, qMax(d->maximum, m));
    }
}

void ExtDateTimeEdit::clearMinimumDate()
{
    setMinimumDate(DATE_MIN);
}

/*!
  \property ExtDateTimeEdit::maximumDate

  \brief the maximum date of the date time edit

  When setting this property the \l minimumDate is adjusted if
  necessary to ensure that the range remains valid. If the date is
  not a valid ExtDate object, this function does nothing.

  \sa minimumDate, minimumTime, maximumTime, setDateRange()
*/

ExtDate ExtDateTimeEdit::maximumDate() const
{
    Q_D(const ExtDateTimeEdit);
    return d->maximum.toDate();
}

void ExtDateTimeEdit::setMaximumDate(const ExtDate &max)
{
    Q_D(ExtDateTimeEdit);
    if (max.isValid()) {
        const QVariant m(ExtDateTime(max, d->maximum.toTime()));
        d->setRange(qMin(d->minimum, m), m);
    }
}

void ExtDateTimeEdit::clearMaximumDate()
{
    setMaximumDate(DATE_MAX);
}

/*!
  \property ExtDateTimeEdit::minimumTime

  \brief the minimum time of the date time edit

  When setting this property the \l maximumTime is adjusted if
  necessary, to ensure that the range remains valid. If the time is
  not a valid QTime object, this function does nothing.

  \sa maximumTime, minimumDate, maximumDate, setTimeRange()
*/

QTime ExtDateTimeEdit::minimumTime() const
{
    Q_D(const ExtDateTimeEdit);
    return d->minimum.toTime();
}

void ExtDateTimeEdit::setMinimumTime(const QTime &min)
{
    Q_D(ExtDateTimeEdit);
    if (min.isValid()) {
        const QVariant m(ExtDateTime(d->minimum.toDate(), min));
        d->setRange(m, qMax(d->maximum, m));
    }
}

void ExtDateTimeEdit::clearMinimumTime()
{
    setMinimumTime(TIME_MIN);
}

/*!
  \property ExtDateTimeEdit::maximumTime

  \brief the maximum time of the date time edit

  When setting this property, the \l minimumTime is adjusted if
  necessary to ensure that the range remains valid. If the time is
  not a valid QTime object, this function does nothing.

  \sa minimumTime, minimumDate, maximumDate, setTimeRange()
*/
QTime ExtDateTimeEdit::maximumTime() const
{
    Q_D(const ExtDateTimeEdit);
    return d->maximum.toTime();
}

void ExtDateTimeEdit::setMaximumTime(const QTime &max)
{
    Q_D(ExtDateTimeEdit);
    if (max.isValid()) {
        const QVariant m(ExtDateTime(d->maximum.toDate(), max));
        d->setRange(qMin(d->minimum, m), m);
    }
}

void ExtDateTimeEdit::clearMaximumTime()
{
    setMaximumTime(TIME_MAX);
}

/*!
  Convenience function to set minimum and maximum date with one
  function call.

  \code
  setDateRange(min, max);
  \endcode

  is analogous to:

  \code
  setMinimumDate(min);
  setMaximumDate(max);
  \endcode

  If either \a min or \a max are not valid, this function does
  nothing.

  \sa setMinimumDate(), maximumDate(), setMaximumDate(),
  clearMinimumDate(), setMinimumTime(), maximumTime(),
  setMaximumTime(), clearMinimumTime(), ExtDate::isValid()
*/

void ExtDateTimeEdit::setDateRange(const ExtDate &min, const ExtDate &max)
{
    Q_D(ExtDateTimeEdit);
    if (min.isValid() && max.isValid()) {
        d->setRange(QVariant(ExtDateTime(min, d->minimum.toTime())),
                    QVariant(ExtDateTime(max, d->maximum.toTime())));
    }
}

/*!
  Convenience function to set minimum and maximum time with one
  function call.

  \code
  setTimeRange(min, max);
  \endcode

  is analogous to:

  \code
  setMinimumTime(min);
  setMaximumTime(max);
  \endcode

  If either \a min or \a max are not valid, this function does
  nothing.

  \sa setMinimumDate(), maximumDate(), setMaximumDate(),
  clearMinimumDate(), setMinimumTime(), maximumTime(),
  setMaximumTime(), clearMinimumTime(), QTime::isValid()
*/

void ExtDateTimeEdit::setTimeRange(const QTime &min, const QTime &max)
{
    Q_D(ExtDateTimeEdit);
    if (min.isValid() && max.isValid()) {
        d->setRange(QVariant(ExtDateTime(d->minimum.toDate(), min)),
                    QVariant(ExtDateTime(d->maximum.toDate(), max)));
    }
}

/*!
  \property ExtDateTimeEdit::displayedSections

  \brief the currently displayed fields of the date time edit

  Returns a bit set of the displayed sections for this format.
  \a setDisplayFormat(), displayFormat()
*/

ExtDateTimeEdit::Sections ExtDateTimeEdit::displayedSections() const
{
    Q_D(const ExtDateTimeEdit);
    return d->display;
}

/*!
  \property ExtDateTimeEdit::currentSection

  \brief the current section of the spinbox
  \a setCurrentSection()
*/

ExtDateTimeEdit::Section ExtDateTimeEdit::currentSection() const
{
    Q_D(const ExtDateTimeEdit);
    switch (d->currentSection) {
    case ExtDateTimeEditPrivate::NoSection:
    case ExtDateTimeEditPrivate::FirstSection:
    case ExtDateTimeEditPrivate::LastSection:
        return ExtDateTimeEdit::NoSection;
    default:
        return (ExtDateTimeEdit::Section)(d->currentSection & (~ExtDateTimeEditPrivate::Internal));
    }
}

void ExtDateTimeEdit::setCurrentSection(Section section)
{
    Q_D(ExtDateTimeEdit);
    const ExtDateTimeEditPrivate::Section s = (ExtDateTimeEditPrivate::Section)section;
    if (s != ExtDateTimeEditPrivate::NoSection)
        d->edit->setCursorPosition(d->sectionNode(s).pos);
}

/*!
  \fn QString ExtDateTimeEdit::sectionText(Section section) const

  Returns the text from the given \a section.

  \sa currentSection()
*/

QString ExtDateTimeEdit::sectionText(Section s) const
{
    Q_D(const ExtDateTimeEdit);
    return d->sectionText(d->edit->displayText(), (ExtDateTimeEditPrivate::Section)s);
}

/*!
  \property ExtDateTimeEdit::displayFormat

  \brief the format used to display the time/date of the date time edit

  This format is a subset of the format described in ExtDateTime::toString()

  These expressions may be used:

  \table
  \header \i Expression \i Output
  \row \i hh
  \i the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)
  \row \i mm \i the minute with a leading zero (00 to 59)
  \row \i ss \i the second whith a leading zero (00 to 59)
  \row \i zzz \i the milliseconds with leading zeroes (000 to 999)
  \row \i AP
  \i use AM/PM display. \e AP will be replaced by either "AM" or "PM".
  \row \i ap
  \i use am/pm display. \e ap will be replaced by either "am" or "pm".
  \row \i dd \i the day as number with a leading zero (01 to 31)
  \row \i MM \i the month as number with a leading zero (01 to 12)
  \row \i MMM
  \i the abbreviated localized month name (e.g. 'Jan' to 'Dec').
  Uses ExtDate::shortMonthName().
  \row \i yy \i the year as two digit number (00 to 99)
  \row \i yyyy \i the year as four digit number (1752 to 8000)
  \endtable

  All other input characters or sequence of characters that are
  enclosed in singlequotes will be treated as text and can be used
  as delimiters.

  Example format strings (assuming that the date is 20 July 1969):

  \table
  \header \i Format \i Result
  \row \i dd.MM.yyyy    \i 20.07.1969
  \row \i MMM d yy \i Jul 20 69
  \endtable

  If you specify an invalid format the format will not be set.

  Multiple instances of the same field is not allowed.A format with
  no valid fields is not allowed either.

  \warning Since ExtDateTimeEdit internally always operates on a
  ExtDateTime, changing the format can change the minimum time or
  date and the current time or date. For example:

  \code
  ExtDateTimeEdit edit;     // default format is "yyyy.MM.dd hh:mm:ss"
  edit.setMinimumDate(ExtDate(2000, 1, 1));
  edit.setMaximumDate(ExtDate(2003, 1, 1));
  edit.setDateTime(ExtDateTime(ExtDate(2002, 5, 5), QTime(10, 10, 10)));
  edit.setDisplayFormat("hh:mm:ss");

  // edit can no longer display dates. This means that the
  // minimum and maximum date will be set to the current date,
  // e.g. 2002, 5, 5.
  \endcode

  \sa ExtDateTime::toString(), displayedSections()
*/

QString ExtDateTimeEdit::displayFormat() const
{
    Q_D(const ExtDateTimeEdit);
    return d->displayFormat;
}

void ExtDateTimeEdit::setDisplayFormat(const QString &format)
{
    Q_D(ExtDateTimeEdit);
    if (d->parseFormat(format)) {
        d->cachedValue.clear();
        d->cachedText.clear();
        const bool timeShown = (d->display & ExtDateTimeEditPrivate::TimeSectionMask);
        const bool dateShown = (d->display & ExtDateTimeEditPrivate::DateSectionMask);
        Q_ASSERT(dateShown || timeShown);
        if (timeShown && !dateShown) {
            setDateRange(d->value.toDate(), d->value.toDate());
        } else if (dateShown && !timeShown) {
            setTimeRange(TIME_MIN, TIME_MAX);
            d->value = QVariant(ExtDateTime(d->value.toDate(), QTime()));
        }
        d->update();
        d->edit->setCursorPosition(0);
        d->editorCursorPositionChanged(-1, 0);
    }
}

/*!
  \reimp
*/

QSize ExtDateTimeEdit::sizeHint() const
{
    Q_D(const QAbstractSpinBox);
    ensurePolished();

    const QFontMetrics fm(fontMetrics());
    int h = d->edit->sizeHint().height();
    int w = 0;
    QString s;
    s = d->textFromValue(d->minimum) + QLatin1String("   ");
    w = qMax<int>(w, fm.width(s));
    s = d->textFromValue(d->maximum) + QLatin1String("   ");
    w = qMax<int>(w, fm.width(s));
    if (d->specialValueText.size()) {
        s = d->specialValueText;
        w = qMax<int>(w, fm.width(s));
    }
    w += 2; // cursor blinking space

    QStyleOptionSpinBox opt = d->getStyleOption();
    QSize hint(w, h);
    QSize extra(35, 6);
    opt.rect.setSize(hint + extra);
    extra += hint - style()->subControlRect(QStyle::CC_SpinBox, &opt,
                                            QStyle::SC_SpinBoxEditField, this).size();
    // get closer to final result by repeating the calculation
    opt.rect.setSize(hint + extra);
    extra += hint - style()->subControlRect(QStyle::CC_SpinBox, &opt,
                                            QStyle::SC_SpinBoxEditField, this).size();
    hint += extra;

    opt.rect = rect();
    return style()->sizeFromContents(QStyle::CT_SpinBox, &opt, hint, this)
        .expandedTo(QApplication::globalStrut());
}

/*!
  \reimp
*/

bool ExtDateTimeEdit::event(QEvent *e)
{
    Q_D(ExtDateTimeEdit);
    switch (e->type()) {
    case QEvent::ApplicationLayoutDirectionChange:
        setDisplayFormat(d->displayFormat);
        break;
    default:
        break;
    }
    return QAbstractSpinBox::event(e);
}

/*!
  \reimp
*/

void ExtDateTimeEdit::clear()
{
    Q_D(ExtDateTimeEdit);
    d->clearSection(d->currentSection);
}
/*!
  \reimp
*/

void ExtDateTimeEdit::keyPressEvent(QKeyEvent *e)
{
    Q_D(ExtDateTimeEdit);
    const ExtDateTimeEditPrivate::Section oldCurrent = d->currentSection;
    bool select = true;

    if ((e->key() == Qt::Key_Backspace || (e->key() == Qt::Key_H && e->key() & Qt::ControlModifier))
        && !d->edit->hasSelectedText()) {
        const int pos = d->edit->cursorPosition();
        if (pos <= d->separators.first().size()) {
            e->accept();
            return;
        }
        select = false;
        const ExtDateTimeEditPrivate::Section s = d->sectionAt(pos);
        const ExtDateTimeEditPrivate::Section closest = d->closestSection(pos - 1, false);
        EDTEDEBUG << "found those two" << d->sectionName(s)<< d->sectionName(closest);
        if ((s == ExtDateTimeEditPrivate::LastSection && d->separators.last().size() > 0)
            || (s != ExtDateTimeEditPrivate::NoSection && pos == d->sectionPos(s))) {
            QString copy = d->edit->displayText();
            int cursorCopy = pos;
            if (validate(copy, cursorCopy) != QValidator::Acceptable) {
                d->interpret(EmitIfChanged);
            }
            d->ignoreCursorPositionChanged = true;
            d->edit->setCursorPosition(d->sectionPos(closest) + d->sectionSize(closest));
            d->currentSection = closest;
            d->ignoreCursorPositionChanged = false;
        }
    }

    bool forward = true;
    switch ((Qt::Key)e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        d->interpret(AlwaysEmit);
        d->setSelected(d->currentSection);
        e->ignore();
        emit editingFinished();
        return;

    case Qt::Key_Left:
        forward = false;
    case Qt::Key_Right:
        if (!(e->modifiers() & Qt::ControlModifier)) {
            select = false;
            break;
        }
#ifdef Q_WS_MAC
        else {
            select = (e->modifiers() & Qt::ShiftModifier);
            break;
        }
#endif

        // fallthroughs intended
    case Qt::Key_Backtab:
    case Qt::Key_Tab: {
        e->accept();
        if (d->specialValue()) {
            d->edit->setSelection(d->edit->cursorPosition(), 0);
            return;
        }
        if (e->key() == Qt::Key_Backtab || (e->key() == Qt::Key_Tab && e->modifiers() & Qt::ShiftModifier)) {
            forward = false;
        }

        const ExtDateTimeEditPrivate::SectionNode newSection = d->nextPrevSection(d->currentSection, forward);
        if (select) {
            d->setSelected(newSection.section);
        } else {
            d->edit->setCursorPosition(forward ? newSection.pos : d->sectionPos(d->currentSection));
        }
        if (!select)
            d->edit->deselect();
        return; }
    default:
        select = !e->text().isEmpty() && e->text().at(0).isPrint();
        break;
    }

    QAbstractSpinBox::keyPressEvent(e);
    if (select && d->currentSection != oldCurrent && !(e->modifiers() & Qt::ShiftModifier) && !d->edit->hasSelectedText()) {
        d->setSelected(d->currentSection);
    }
    if (d->specialValue()) {
        d->edit->setSelection(d->edit->cursorPosition(), 0);
    }
}

/*!
  \reimp
*/

#ifndef QT_NO_WHEELEVENT
void ExtDateTimeEdit::wheelEvent(QWheelEvent *e)
{
    Q_D(ExtDateTimeEdit);
    int fw = d->frame ? style()->pixelMetric(QStyle::PM_SpinBoxFrameWidth) : 0;
    QPoint pnt(e->pos() - QPoint(fw, fw));
    pnt.rx() -= d->edit->x();
    int index = d->edit->cursorPositionAt(pnt);
    const ExtDateTimeEditPrivate::Section s = d->closestSection(index, d->edit->cursorPosition() > index); // should it be > pos?
    if (s != d->currentSection)
        d->edit->setCursorPosition(d->sectionNode(s).pos);
    switch (s) {
    case ExtDateTimeEditPrivate::NoSection:
    case ExtDateTimeEditPrivate::FirstSection:
    case ExtDateTimeEditPrivate::LastSection:
        break;
    default:
        QAbstractSpinBox::wheelEvent(e);
        break;
    }
}
#endif

/*!
  \reimp
*/

void ExtDateTimeEdit::focusInEvent(QFocusEvent *e)
{
    Q_D(ExtDateTimeEdit);
    QAbstractSpinBox::focusInEvent(e);
    QString *frm = 0;
    if (d->displayFormat == d->defaultTimeFormat) {
        frm = &d->defaultTimeFormat;
    } else if (d->displayFormat == d->defaultDateFormat) {
        frm = &d->defaultDateFormat;
    } else if (d->displayFormat == d->defaultDateTimeFormat) {
        frm = &d->defaultDateTimeFormat;
    }

    if (frm) {
        d->readLocaleSettings();
        setDisplayFormat(*frm);
    }
    bool first;
    switch (e->reason()) {
    case Qt::ShortcutFocusReason:
    case Qt::TabFocusReason: first = true; break;
    case Qt::BacktabFocusReason: first = false; break;
    default: return;
    }
    if (QApplication::isRightToLeft())
        first = !first;
    d->setSelected(first ? d->sectionNodes.first().section : d->sectionNodes.at(d->sectionNodes.size() - 1).section);
}

/*!
  \reimp
*/

bool ExtDateTimeEdit::focusNextPrevChild(bool next)
{
    Q_D(ExtDateTimeEdit);
    if (!focusWidget())
        return false;

    const ExtDateTimeEditPrivate::Section newSection =
        d->nextPrevSection(d->currentSection, next).section;
    switch (newSection) {
    case ExtDateTimeEditPrivate::NoSection:
    case ExtDateTimeEditPrivate::FirstSection:
    case ExtDateTimeEditPrivate::LastSection:
        break;
    default:
        d->setSelected(newSection);
        return true;
    }
    return QAbstractSpinBox::focusNextPrevChild(next);
}

/*!
  \reimp
*/

void ExtDateTimeEdit::stepBy(int steps)
{
    Q_D(ExtDateTimeEdit);
    const ExtDateTimeEditPrivate::Section s = d->currentSection;
    d->setValue(d->stepBy(s, steps, false), EmitIfChanged);
    d->setSelected(s);
}

/*!
  This virtual function is used by the date time edit whenever it
  needs to display \a dateTime.

  If you reimplement this, you may also need to reimplement
  valueFromText() and validate().

  \sa dateTimeFromText(), validate()
*/
QString ExtDateTimeEdit::textFromDateTime(const ExtDateTime &dateTime) const
{
    const char zero = '0';

    Q_D(const ExtDateTimeEdit);
    QVariant var(dateTime);
    if (var == d->cachedValue) {
        EDTEDEBUG << "cached and var is the same so returning cachedText" << dateTime << d->cachedText;
        return d->cachedText;
    }
    QString ret = d->escapedFormat;
    for (int i=0; i<d->sectionNodes.size(); ++i) {
        int l = d->sectionSize(d->sectionNodes.at(i).section);
        int pos = d->sectionNodes.at(i).pos;
        const ExtDateTimeEditPrivate::Section s = d->sectionNodes.at(i).section;
        switch (s) {
        case ExtDateTimeEditPrivate::AmPmSection:
        case ExtDateTimeEditPrivate::AmPmLowerCaseSection: {
            QString input;
            if (s == ExtDateTimeEditPrivate::AmPmSection) {
                input = var.toTime().hour() > 11
                        ? ExtDateTimeEdit::tr("PM")
                        : ExtDateTimeEdit::tr("AM");
            } else {
                input = var.toTime().hour() > 11 ?
                        ExtDateTimeEdit::tr("pm") :
                        ExtDateTimeEdit::tr("am");
            }
            input.truncate(l);

            ret.replace(pos, l, input);
            break; }
        case ExtDateTimeEditPrivate::MonthShortNameSection:
            ret.replace(pos, l, ExtDate::shortMonthName(var.toDate().month()));
            break;
        case ExtDateTimeEditPrivate::YearTwoDigitsSection:
            ret.replace(pos, l,
                        QString::number(d->getDigit(var, ExtDateTimeEditPrivate::YearTwoDigitsSection) - 2000)
                        .rightJustified(l, zero, true));
            break;
        case ExtDateTimeEditPrivate::HourSection:
            if (d->display & ExtDateTimeEditPrivate::AmPmSection) {
                int h = var.toTime().hour();
                h = h % 12;
                if (h == 0)
                    h = 12;
                ret.replace(pos, l,
                            QString::number(h).rightJustified(l, zero, true));
                break;
            }
        default:
            ret.replace(pos, l,
                        QString::number(d->getDigit(var, d->sectionNodes.at(i).section)).
                        rightJustified(l, zero));
            break;
        }
    }

    d->cachedValue = var;
    d->cachedText = ret;
//    qDebug() << "setting cached to" << d->cachedValue << " and cachedText to" << d->cachedText;
    return ret;
}


/*!
  Returns an appropriate datetime for the given \a text.

  This virtual function is used by the datetime edit whenever it
  needs to interpret text entered by the user as a value.

  \sa textFromDateTime(), validate()
*/
ExtDateTime ExtDateTimeEdit::dateTimeFromText(const QString &text) const
{
    Q_D(const ExtDateTimeEdit);
    QString copy = text;
    int pos = d->edit->cursorPosition();
    QValidator::State state = QValidator::Acceptable;
    return d->validateAndInterpret(copy, pos, state).toDateTime();
}

/*!
  \reimp
*/

QValidator::State ExtDateTimeEdit::validate(QString &text, int &pos) const
{
    Q_D(const ExtDateTimeEdit);
    QValidator::State state;
    d->validateAndInterpret(text, pos, state);
    return state;
}\

/*!
  \reimp
*/

ExtDateTimeEdit::StepEnabled ExtDateTimeEdit::stepEnabled() const
{
    Q_D(const ExtDateTimeEdit);
    if (d->readOnly)
        return StepEnabled(0);
    if (d->specialValue()) {
        if (d->minimum == d->maximum)
            return StepEnabled(0);
        return d->wrapping
            ? StepEnabled(StepDownEnabled|StepUpEnabled)
            : StepEnabled(StepUpEnabled);
    }
    switch (d->currentSection) {
    case ExtDateTimeEditPrivate::NoSection:
    case ExtDateTimeEditPrivate::FirstSection:
    case ExtDateTimeEditPrivate::LastSection: return 0;
    default: break;
    }
    if (!style()->styleHint(QStyle::SH_SpinControls_DisableOnBounds)
        || d->wrapping)
        return StepEnabled(StepUpEnabled | StepDownEnabled);

    QAbstractSpinBox::StepEnabled ret = 0;

    QVariant v = d->stepBy(d->currentSection, 1, true);
    if (v != d->value) {
        ret |= QAbstractSpinBox::StepUpEnabled;
    }
    v = d->stepBy(d->currentSection, -1, true);
    if (v != d->value) {
        ret |= QAbstractSpinBox::StepDownEnabled;
    }

    return ret;
}


/*!
    \class QTimeEdit
    \brief The QTimeEdit class provides a widget for editing times based on
    the ExtDateTimeEdit widget.

    \ingroup basic
    \mainclass

    \sa ExtDateEdit ExtDateTimeEdit
*/

/*!
  Constructs an empty time editor with a \a parent.
*/


QTimeEdit::QTimeEdit(QWidget *parent)
    : ExtDateTimeEdit(TIME_MIN, parent)
{
}

/*!
  Constructs an empty time editor with a \a parent. The time is set
  to \a time.
*/

QTimeEdit::QTimeEdit(const QTime &time, QWidget *parent)
    : ExtDateTimeEdit(time, parent)
{
}

/*!
    \class ExtDateEdit
    \brief The ExtDateEdit class provides a widget for editing dates based on
    the ExtDateTimeEdit widget.

    \ingroup basic
    \mainclass

    \sa QTimeEdit ExtDateTimeEdit
*/

/*!
  Constructs an empty date editor with a \a parent.
*/

ExtDateEdit::ExtDateEdit(QWidget *parent)
    : ExtDateTimeEdit(DATE_INITIAL, parent)
{
}

/*!
  Constructs an empty date editor with a \a parent. The date is set
  to \a date.
*/

ExtDateEdit::ExtDateEdit(const ExtDate &date, QWidget *parent)
    : ExtDateTimeEdit(date, parent)
{
}


// --- ExtDateTimeEditPrivate ---

/*!
  \internal
  Constructs a ExtDateTimeEditPrivate object
*/


ExtDateTimeEditPrivate::ExtDateTimeEditPrivate()
{
    type = QVariant::DateTime;
    display = (ExtDateTimeEdit::Sections)0;
    cachedDay = -1;
    currentSection = NoSection;
    layoutDirection = QApplication::layoutDirection();
    first.section = FirstSection;
    last.section = LastSection;
    first.pos = 0;
    last.pos = -1;
    readLocaleSettings();
}

/*!
  \internal
  \reimp
*/

void ExtDateTimeEditPrivate::emitSignals(EmitPolicy ep, const QVariant &old)
{
    Q_Q(ExtDateTimeEdit);
    if (ep == NeverEmit) {
        return;
    }
    pendingEmit = false;

    const bool dodate = value.toDate().isValid() && (display & DateSectionMask);
    const bool datechanged = (ep == AlwaysEmit || old.toDate() != value.toDate());
    const bool dotime = value.toTime().isValid() && (display & TimeSectionMask);
    const bool timechanged = (ep == AlwaysEmit || old.toTime() != value.toTime());

    if (dodate && dotime && (datechanged || timechanged))
        emit q->dateTimeChanged(value.toDateTime());
    if (dodate && datechanged)
        emit q->dateChanged(value.toDate());
    if (dotime && timechanged)
        emit q->timeChanged(value.toTime());
}

/*!
  \internal
  \reimp
*/

void ExtDateTimeEditPrivate::editorCursorPositionChanged(int oldpos, int newpos)
{
    Q_Q(ExtDateTimeEdit);
    if (ignoreCursorPositionChanged || specialValue())
        return;
    const bool allowChange = !edit->hasSelectedText();
    ignoreCursorPositionChanged = true;
    Section s = sectionAt(newpos);
    int c = newpos;

    const int selstart = edit->selectionStart();
    const Section selSection = sectionAt(selstart);
    const int l = sectionSize(selSection);

    if (s == NoSection) {
        if (l > 0 && selstart == sectionPos(selSection) && edit->selectedText().size() == l) {
            s = selSection;
            if (allowChange)
                setSelected(selSection, true);
            c = -1;
        } else {
            const SectionNode &sn = sectionNode(closestSection(newpos, oldpos < newpos));
            c = sn.pos + (oldpos < newpos ? 0 : qMax<int>(0, sectionSize(sn.section) - 1));
            if (allowChange)
                edit->setCursorPosition(c);
            s = sn.section;
        }
    }

    if (allowChange && currentSection != s) {
        QString tmp = edit->displayText();
        int pos = edit->cursorPosition();
        if (q->validate(tmp, pos) != QValidator::Acceptable) {
            interpret(EmitIfChanged);
            if (c == -1) {
                setSelected(s, true);
            } else {
                edit->setCursorPosition(pos);
            }
        }
        updateSpinBox();
    }
    currentSection = s;
    ignoreCursorPositionChanged = false;

}

#ifdef Q_WS_MAC
static QString macParseDateLocale(QVariant::Type type)
{
    CFGregorianDate macGDate;
    macGDate.year = 2999;
    macGDate.month = 11;
    macGDate.day = 22;
    macGDate.hour = 10;
    macGDate.minute = 34;
    macGDate.second = 56.0;
    QCFType<CFDateRef> myDate = CFDateCreate(0, CFGregorianDateGetAbsoluteTime(macGDate, QCFType<CFTimeZoneRef>
                                                                               (CFTimeZoneCopySystem())));
    switch (type) {
    case QVariant::Date: {
#if (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3)
        if (QSysInfo::MacintoshVersion >= QSysInfo::MV_10_3) {
            QCFType<CFLocaleRef> mylocale = CFLocaleCopyCurrent();
            QCFType<CFDateFormatterRef> myFormatter = CFDateFormatterCreate(kCFAllocatorDefault,
                                                                            mylocale, kCFDateFormatterShortStyle,
                                                                            kCFDateFormatterNoStyle);
            return QCFString(CFDateFormatterCreateStringWithDate(0, myFormatter, myDate));

        } else
#endif
        {
            Handle intlHandle = GetIntlResource(0);
            LongDateTime oldDate;
            UCConvertCFAbsoluteTimeToLongDateTime(CFGregorianDateGetAbsoluteTime(macGDate, 0),
                                                  &oldDate);
            Str255 pString;
            LongDateString(&oldDate, shortDate, pString, intlHandle);
            return qt_mac_from_pascal_string(pString);
        }
    }
    case QVariant::DateTime: {
#if (MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_3)
        if (QSysInfo::MacintoshVersion >= QSysInfo::MV_10_3) {
            QCFType<CFLocaleRef> mylocale = CFLocaleCopyCurrent();
            QCFType<CFDateFormatterRef> myFormatter = CFDateFormatterCreate(kCFAllocatorDefault,
                                                                            mylocale, kCFDateFormatterShortStyle,
                                                                            kCFDateFormatterMediumStyle);
            return QCFString(CFDateFormatterCreateStringWithDate(0, myFormatter, myDate));

        } else
#endif
        {
            Handle intlHandle = GetIntlResource(0);
            LongDateTime oldDate;
            UCConvertCFAbsoluteTimeToLongDateTime(CFGregorianDateGetAbsoluteTime(macGDate, 0),
                                                  &oldDate);
            Str255 pString;
            LongDateString(&oldDate, shortDate, pString, intlHandle);
            QString final = qt_mac_from_pascal_string(pString);
            LongTimeString(&oldDate, true, pString, intlHandle);
            return final + QLatin1Char(' ') + qt_mac_from_pascal_string(pString);
        }
    }
    default: return QString();
    }
}
#endif

/*!
  \internal

  Try to get the format from the local settings
*/
void ExtDateTimeEditPrivate::readLocaleSettings()
{
    EDTEDEBUG << ExtDateTimeEdit::tr("PM")
              << ExtDateTimeEdit::tr("AM")
              << ExtDateTimeEdit::tr("pm")
              << ExtDateTimeEdit::tr("am");

    // Time
    QString str = QTime(10, 34, 56).toString(Qt::LocalDate);
    int index = str.indexOf(QLatin1String("10"));
    if (index != -1)
        str.replace(index, 2, QLatin1String("hh"));

    index = str.indexOf(QLatin1String("34"));
    if (index != -1)
        str.replace(index, 2, QLatin1String("mm"));

    index = str.indexOf(QLatin1String("56"));
    if (index != -1)
        str.replace(index, 2, QLatin1String("ss"));

    QString am = QLatin1String("am");
    index = str.indexOf(am);
    if (index != -1) {
        str.replace(index, am.size(), QLatin1String("ap"));
    } else {
        am = QLatin1String("AM");

        index = str.indexOf(am);
        if (index != -1)
            str.replace(index, am.size(), QLatin1String("AP"));
    }

    defaultTimeFormat = str;

    // Date

    const ExtDate date(2999, 11, 22);
    const QString shortMonthName = ExtDate::shortMonthName(date.month());
    const QString longMonthName = ExtDate::longMonthName(date.month());
    const QString shortDayName = ExtDate::shortDayName(date.dayOfWeek());
    const QString longDayName = ExtDate::longDayName(date.dayOfWeek());

#ifdef Q_WS_MAC
    str = macParseDateLocale(QVariant::Date);
#else 
    str = date.toString(Qt::LocalDate);
#endif
    
    index = str.indexOf(QLatin1String("22"));
    if (index != -1)
        str.replace(index, 2, QLatin1String("dd"));

    index = str.indexOf(shortDayName);
    if (index != -1)
        str.remove(index, shortDayName.size());

    index = str.indexOf(longDayName);
    if (index != -1)
        str.remove(index, longDayName.size());

    index = str.indexOf(QLatin1String("11"));
    if (index != -1) {
        str.replace(index, 2, QLatin1String("MM"));
    } else if ((index = str.indexOf(longMonthName)) != -1) {
        str.replace(index, longMonthName.size(), QLatin1String("MMM"));
    } else if ((index = str.indexOf(shortMonthName)) != -1) {
        str.replace(index, shortMonthName.size(), QLatin1String("MMM"));
    }

    index = str.indexOf(QLatin1String("2999"));
    if (index != -1) {
        str.replace(index, 4, QLatin1String("yyyy"));
    } else {
        index = str.indexOf(QLatin1String("99"));
        if (index != -1)
            str.replace(index, 2, QLatin1String("yy"));
    }

    defaultDateFormat = str;

    // DateTime
#ifdef Q_WS_MAC
    str = macParseDateLocale(QVariant::DateTime);
#else
    str = ExtDateTime(ExtDate(2999, 11, 22), QTime(10, 34, 56)).toString(Qt::LocalDate);
#endif
    index = str.indexOf(QLatin1String("10"));
    if (index != -1)
        str.replace(index, 2, QLatin1String("hh"));

    index = str.indexOf(QLatin1String("34"));
    if (index != -1)
        str.replace(index, 2, QLatin1String("mm"));

    index = str.indexOf(QLatin1String("56"));
    if (index != -1)
        str.replace(index, 2, QLatin1String("ss"));

    am = QLatin1String("am");

    index = str.indexOf(am);
    if (index != -1) {
        str.replace(index, am.size(), QLatin1String("ap"));
    } else {
        am = QLatin1String("AM");

        index = str.indexOf(am);
        if (index != -1)
            str.replace(index, am.size(), QLatin1String("AP"));
    }

    index = str.indexOf(QLatin1String("22"));
    if (index != -1)
        str.replace(index, 2, QLatin1String("dd"));

    index = str.indexOf(shortDayName);
    if (index != -1)
        str.remove(index, shortDayName.size());

    index = str.indexOf(longDayName);
    if (index != -1)
        str.remove(index, longDayName.size());

    index = str.indexOf(QLatin1String("11"));
    if (index != -1) {
        str.replace(index, 2, QLatin1String("MM"));
    } else if ((index = str.indexOf(longMonthName)) != -1) {
        str.replace(index, longMonthName.size(), QLatin1String("MMM"));
    } else if ((index = str.indexOf(shortMonthName)) != -1) {
        str.replace(index, shortMonthName.size(), QLatin1String("MMM"));
    }

    index = str.indexOf(QLatin1String("2999"));
    if (index != -1) {
        str.replace(index, 4, QLatin1String("yyyy"));
    } else {
        index = str.indexOf(QLatin1String("99"));
        if (index != -1)
            str.replace(index, 2, QLatin1String("yy"));
    }

    defaultDateTimeFormat = str;

    EDTEDEBUG << "default Time:" << defaultTimeFormat << "default date:" << defaultDateFormat << "default date/time" << defaultDateTimeFormat;
}

/*!
  \internal
  Gets the digit from a corevariant. E.g.

  QVariant var(ExtDate(2004, 02, 02));
  int digit = getDigit(var, Year);
  // digit = 2004
  */

int ExtDateTimeEditPrivate::getDigit(const QVariant &t, Section s) const
{
    switch (s) {
    case HourSection: return t.toTime().hour();
    case MinuteSection: return t.toTime().minute();
    case SecondSection: return t.toTime().second();
    case MSecSection: return t.toTime().msec();
    case YearTwoDigitsSection:
    case YearSection: return t.toDate().year();
    case MonthShortNameSection:
    case MonthSection: return t.toDate().month();
    case DaySection: return t.toDate().day();
    case AmPmSection:
    case AmPmLowerCaseSection:
        return t.toTime().hour() > 11 ? 1 : 0;

    default: break;
    }
    qFatal("%s passed to getDigit. This should never happen", sectionName(s).toLatin1().constData());
    return -1;
}

/*!
  \internal
  Sets a digit in a variant. E.g.

  QVariant var(ExtDate(2004, 02, 02));
  int digit = getDigit(var, Year);
  // digit = 2004
  setDigit(&var, Year, 2005);
  digit = getDigit(var, Year);
  // digit = 2005

  returns true if the newVal was changed to make it work. E.g. If you set 31st when you're in february
*/

void ExtDateTimeEditPrivate::setDigit(QVariant &v, Section section, int newVal) const
{
    int year, month, day, hour, minute, second, msec;
    const ExtDateTime &dt = v.toDateTime();
    year = dt.date().year();
    month = dt.date().month();
    day = dt.date().day();
    hour = dt.time().hour();
    minute = dt.time().minute();
    second = dt.time().second();
    msec = dt.time().msec();

    switch (section) {
    case HourSection: hour = newVal; break;
    case MinuteSection: minute = newVal; break;
    case SecondSection: second = newVal; break;
    case MSecSection: msec = newVal; break;
    case YearTwoDigitsSection:
    case YearSection: year = newVal; break;
    case MonthSection:
    case MonthShortNameSection: month = newVal; break;
    case DaySection: day = newVal; break;
    case AmPmSection:
    case AmPmLowerCaseSection: hour = (newVal == 0 ? hour % 12 : (hour % 12) + 12); break;
    default:
        qFatal("%s passed to setDigit. This should never happen", sectionName(section).toLatin1().constData());
        break;
    }

    if (section != DaySection) {
        day = qMax<int>(cachedDay, day);
    }

    if (!ExtDate::isValid(year, month, day)) {
        if (year <= DATE_MIN.year() && (month < DATE_MIN.month() || (month == DATE_MIN.month() && day < DATE_MIN.day()))) {
            month = DATE_MIN.month();
            day = DATE_MIN.day();
        } else {
            day = qMin<int>(day, ExtDate(year, month, 1).daysInMonth());
        }
    }
    v = QVariant(ExtDateTime(ExtDate(year, month, day), QTime(hour, minute, second, msec)));
}


/*!
  \internal

  Internal function called by ExtDateTimeEdit::stepBy(). Also takes a
  Section for which section to step on and a bool \a test for
  whether or not to modify the internal cachedDay variable. This is
  necessary because the function is called from the const function
  ExtDateTimeEdit::stepEnabled() as well as ExtDateTimeEdit::stepBy().
*/

QVariant ExtDateTimeEditPrivate::stepBy(Section s, int steps, bool test) const
{
    Q_Q(const ExtDateTimeEdit);
    QVariant v = value;
    QString str = edit->displayText();
    int pos = edit->cursorPosition();
    bool specVal = specialValue();

    int val;
    // to make sure it behaves reasonably when typing something and then stepping in non-tracking mode
    if (!test && pendingEmit) {
        if (q->validate(str, pos) != QValidator::Acceptable) {
            v = value;
        } else {
            v = valueFromText(str);
        }
        val = getDigit(v, s);
    } else {
        QValidator::State state;
        if (!specVal) {
        val = sectionValue(s, str, state);
        if (state == QValidator::Invalid) {
            return value;
        }
        } else {
            val = getDigit(v, s);
        }
    }

    val += steps;

    const int min = absoluteMin(s);
    const int max = absoluteMax(s);

    if (val < min) {
        val = (wrapping ? max - (min - val) + 1 : min);
    } else if (val > max) {
        val = (wrapping ? min + val - max - 1 : max);
    }

    const int tmp = v.toDate().day();
    setDigit(v, s, val); // if this sets year or month it will make
    // sure that days are lowered if needed.

    // changing one section should only modify that section, if possible
    if (s != AmPmSection && (v < minimum || v > maximum)) {

        const int localmin = getDigit(minimum, s);
        const int localmax = getDigit(maximum, s);

        if (wrapping) {
            // just because we hit the roof in one direction, it
            // doesn't mean that we hit the floor in the other
            if (steps > 0) {
                setDigit(v, s, min);
                if (s != DaySection) {
                    int daysInMonth = v.toDate().daysInMonth();
                    if (v.toDate().day() < tmp && v.toDate().day() < daysInMonth)
                        setDigit(v, DaySection, qMin(tmp, daysInMonth));
                }

                if (v < minimum) {
                    setDigit(v, s, localmin);
                    if (v < minimum)
                        setDigit(v, s, localmin + 1);
                }
            } else {
                setDigit(v, s, max);
                if (s != DaySection) {
                    int daysInMonth = v.toDate().daysInMonth();
                    if (v.toDate().day() < tmp && v.toDate().day() < daysInMonth)
                        setDigit(v, DaySection, qMin(tmp, daysInMonth));
                }

                if (v > maximum) {
                    setDigit(v, s, localmax);
                    if (v > maximum)
                        setDigit(v, s, localmax - 1);
                }
            }
        } else {
            setDigit(v, s, (steps > 0 ? localmax : localmin));
        }
    }
    if (!test && tmp != v.toDate().day() && s != DaySection) {
        // this should not happen when called from stepEnabled
        cachedDay = qMax<int>(tmp, cachedDay);
    }

    if (v < minimum) {
        if (wrapping) {
            QVariant t = v;
            setDigit(t, s, steps < 0 ? max : min);
            if (t >= minimum && t <= maximum) {
                v = t;
            } else {
                setDigit(t, s, getDigit(steps < 0 ? maximum : minimum, s));
                if (t >= minimum && t <= maximum) {
                    v = t;
                }
            }
        } else {
            v = value;
        }
    } else if (v > maximum) {
        if (wrapping) {
            QVariant t = v;
            setDigit(t, s, steps > 0 ? min : max);
            if (t >= minimum && t <= maximum) {
                v = t;
            } else {
                setDigit(t, s, getDigit(steps > 0 ? minimum : maximum, s));
                if (t >= minimum && t <= maximum) {
                    v = t;
                }
            }
        } else {
            v = value;
        }
    }

    return bound(v, value, steps);
}

/*!
  \internal

  Returns the absolute maximum for a section
*/

int ExtDateTimeEditPrivate::absoluteMax(Section s) const
{
    switch (s) {
    case HourSection: return 23;
    case MinuteSection:
    case SecondSection: return 59;
    case MSecSection: return 999;
    case YearTwoDigitsSection: return 2099;
    case YearSection: return 7999;
    case MonthSection:
    case MonthShortNameSection: return 12;
    case DaySection: return 31;
    case AmPmSection:
    case AmPmLowerCaseSection: return 1;
    default: break;
    }
    qFatal("%s passed to max. This should never happen", sectionName(s).toLatin1().constData());
    return -1;

}

/*!
  \internal

  Returns the absolute minimum for a section
*/

int ExtDateTimeEditPrivate::absoluteMin(Section s) const
{
    switch (s) {
    case HourSection:
    case MinuteSection:
    case SecondSection:
    case MSecSection: return 0;
    case YearTwoDigitsSection: return 2000;
    case YearSection: return 1753;
    case MonthSection:
    case MonthShortNameSection:
    case DaySection: return 1;
    case AmPmSection:
    case AmPmLowerCaseSection: return 0;
    default: break;
    }
    qFatal("%s passed to min. This should never happen", sectionName(s).toLatin1().constData());
    return -1;
}

/*!
  \internal

  Returns a copy of the sectionNode for the Section \a s.
*/

ExtDateTimeEditPrivate::SectionNode ExtDateTimeEditPrivate::sectionNode(Section s) const
{
    if (s == FirstSection) {
        return first;
    } else if (s == LastSection) {
        return last;
    }

    for (int i=0; i<sectionNodes.size(); ++i)
        if ((sectionNodes.at(i).section & ~Internal) == (s & ~Internal))
            return sectionNodes.at(i);
    SectionNode sn;
    sn.section = NoSection;
    sn.pos = -1;
    return sn;
}

/*!
  \internal

  Returns the starting position for section \a s.
*/

int ExtDateTimeEditPrivate::sectionPos(Section s) const
{
    if (s == FirstSection) {
        return first.pos;
    } else if (s == LastSection) {
        return last.pos;
    }

    for (int i=0; i<sectionNodes.size(); ++i)
        if (sectionNodes.at(i).section == s)
            return sectionNodes.at(i).pos;
    return -1;
}

/*!
  \internal

  Adds a section to \a list. If this section already exists returns false.
*/

bool ExtDateTimeEditPrivate::addSection(QList<SectionNode> &list, Section ds, int pos) const
{
    for (int i=0; i<list.size(); ++i) {
        if ((list.at(i).section & ~Internal) == (ds & ~Internal)) {
            EDTEDEBUGN("Could not add section %s to pos %d because it is already in the list", sectionName(ds).toLatin1().constData(), pos);
            return false;
        }
    }
    SectionNode s;
    s.section = ds;
    s.pos = pos;
    list << s;

    return true;
}


/*!
  \internal

  Selects the section \a s. If \a forward is false selects backwards.
*/

void ExtDateTimeEditPrivate::setSelected(Section s, bool forward)
{
    if (s == NoSection)
        return;
    if (specialValue())
        edit->selectAll();
    if (forward) {
        edit->setSelection(sectionPos(s), sectionSize(s));
    } else {
        edit->setSelection(sectionPos(s) + sectionSize(s), -sectionSize(s));
    }
}

/*!
  \internal helper function for parseFormat. removes quotes that are
  not escaped and removes the escaping on those that are escaped

*/

static QString unquote(const QString &str)
{
    const char quote = '\'';
    const char slash = '\\';
    const char zero = '0';
    QString ret;
    QChar status = zero;
    for (int i=0; i<str.size(); ++i) {
        if (str.at(i) == quote) {
            if (status != quote) {
                status = quote;
            } else if (!ret.isEmpty() && str.at(i - 1) == slash) {
                ret[ret.size() - 1] = quote;
            } else {
                status = zero;
            }
        } else {
            ret += str.at(i);
        }
    }
    return ret;
}
/*!
  \internal

  Parses the format \a newFormat. If successful, returns true and
  sets up the format. Else keeps the old format and returns false.

*/

bool ExtDateTimeEditPrivate::parseFormat(const QString &newFormat)
{
    const char space = ' ';
    const char quote = '\'';
    const char slash = '\\';
    const char zero = '0';
    if (newFormat == displayFormat && !newFormat.isEmpty() && layoutDirection == QApplication::layoutDirection())
        return true;
    layoutDirection = QApplication::layoutDirection();

    QList<SectionNode> newSectionNodes;
    ExtDateTimeEdit::Sections newDisplay = 0;
    QStringList newSeparators;
    int i, index = 0;
    int add = 0;
    QChar status = zero;
    for (i = 0; i<newFormat.size(); ++i) {
        if (newFormat.at(i) == quote) {
            ++add;
            if (status != quote) {
                status = quote;
            } else if (newFormat.at(i - 1) != slash) {
                status = zero;
            }
        } else if (i + 1 < newFormat.size() && status != quote) {
            switch (newFormat.at(i).cell()) {
            case 'h':
                if (newFormat.at(i+1) == QLatin1Char('h')) {
                    if (!addSection(newSectionNodes, HourSection, i - add))
                        return false;
                    newSeparators << unquote(newFormat.mid(index, i - index));
                    index = ++i + 1;
                    newDisplay |= ExtDateTimeEdit::HourSection;
                }
                break;
            case 'm':
                if (newFormat.at(i+1) == QLatin1Char('m')) {
                    if (!addSection(newSectionNodes, MinuteSection, i - add))
                        return false;
                    newSeparators << unquote(newFormat.mid(index, i - index));
                    index = ++i + 1;
                    newDisplay |= ExtDateTimeEdit::MinuteSection;
                }
                break;
            case 's':
                if (newFormat.at(i+1) == QLatin1Char('s')) {
                    if (!addSection(newSectionNodes, SecondSection, i - add))
                        return false;
                    newSeparators << unquote(newFormat.mid(index, i - index));
                    index = ++i + 1;
                    newDisplay |= ExtDateTimeEdit::SecondSection;
                }
                break;
            case 'z':
                if (i + 2 <newFormat.size()
                    && newFormat.at(i+1) == QLatin1Char('z')
                    && newFormat.at(i+2) == QLatin1Char('z')) {
                    if (!addSection(newSectionNodes, MSecSection, i - add))
                        return false;
                    newSeparators << unquote(newFormat.mid(index, i - index));
                    index = (i += 2) + 1;
                    newDisplay |= ExtDateTimeEdit::MSecSection;
                }
                break;
            case 'A':
            case 'a': {
                const bool cap = newFormat.at(i) == QLatin1Char('A');
                if (newFormat.at(i+1) == (cap ? QLatin1Char('P') : QLatin1Char('p'))) {
                    if (!addSection(newSectionNodes, cap ? AmPmSection : AmPmLowerCaseSection, i - add))
                        return false;
                    newSeparators << unquote(newFormat.mid(index, i - index));
                    index = ++i + 1;
                    newDisplay |= ExtDateTimeEdit::AmPmSection;
                }
                break; }
            case 'y':
                if (newFormat.at(i+1) == QLatin1Char('y')) {
                    const ExtDate YY_MIN(2000, 1, 1);
                    const ExtDate YY_MAX(2099, 12, 31);
                    const bool four = (i + 3 <newFormat.size()
                                       && newFormat.at(i+2) == QLatin1Char('y') && newFormat.at(i+3) == QLatin1Char('y'));
                    if (!addSection(newSectionNodes, four ? YearSection : YearTwoDigitsSection, i - add)
                        || (!four && (maximum.toDate() < YY_MIN || minimum.toDate() > YY_MAX))) {
                        return false;
                    }

                    newSeparators << unquote(newFormat.mid(index, i - index));
                    index = (i += (four ? 3 : 1)) + 1;
                    newDisplay |= ExtDateTimeEdit::YearSection;
                }
                break;
            case 'M':
                if (newFormat.at(i+1) == QLatin1Char('M')) {
                    const bool three = (i + 2 <newFormat.size() && newFormat.at(i+2) == QLatin1Char('M'));
                    if (!addSection(newSectionNodes, three ? MonthShortNameSection : MonthSection, i - add))
                        return false;
                    newSeparators << unquote(newFormat.mid(index, i - index));
                    index = (i += (three ? 2 : 1)) + 1;
                    newDisplay |= ExtDateTimeEdit::MonthSection;
                }
                break;

            case 'd':
                if (newFormat.at(i+1) == QLatin1Char('d')) {
                    if (!addSection(newSectionNodes, DaySection, i - add))
                        return false;
                    newSeparators << unquote(newFormat.mid(index, i - index));
                    index = ++i + 1;
                    newDisplay |= ExtDateTimeEdit::DaySection;
                }
                break;

            default: break;
            }
        }
    }
    if (newSectionNodes.isEmpty()) {
        EDTEDEBUGN("Could not parse format. No sections in format '%s'.", newFormat.toLatin1().constData());
        return false;
    }

    newSeparators << (index < newFormat.size() ? unquote(newFormat.mid(index)) : QString());

    display = newDisplay;
    last.pos = newFormat.size();

    if (!QApplication::isRightToLeft()) {
        displayFormat = newFormat;
        separators = newSeparators;
        sectionNodes = newSectionNodes;
    } else {
        displayFormat.clear();;
        int total = newFormat.size();
        int i;
        sectionNodes.clear();
        separators.clear();

        for (i=newSectionNodes.size() - 1; i>=0; --i) {
            displayFormat += newSeparators.at(i + 1);
            displayFormat += sectionFormat(newSectionNodes.at(i).section);
            SectionNode sn = newSectionNodes.at(i);
            sn.pos = total - sn.pos - sectionSize(sn.section);

            sectionNodes.append(sn);
            separators.append(newSeparators.at(i + 1));
        }
        displayFormat += newSeparators.at(0);
        separators.append(newSeparators.at(0));
    }
    escapedFormat.clear();
    status = zero;
    int ampmsize = sectionSize(AmPmSection);
    for (int i = 0; i < displayFormat.size(); ++i) {
        if (displayFormat.at(i) == quote){
            if (status == quote) {
                if (!escapedFormat.isEmpty() && displayFormat.at(i - 1) == slash) {
                    escapedFormat[escapedFormat.size() - 1] = quote;
                } else {
                    status = zero;
                }
            } else {
                status = quote;
            }
        } else {
            escapedFormat += displayFormat.at(i);
            if (i > 1 && ampmsize != 2
                && ((displayFormat.at(i - 1) == QLatin1Char('a') && displayFormat.at(i) == 'p')
                    || (displayFormat.at(i - 1) == QLatin1Char('A') && displayFormat.at(i) == 'P'))) {
                if (ampmsize > 2) {
                    escapedFormat.append(QString().leftJustified(ampmsize - 2, space));
                } else if (ampmsize == 1) {
                    escapedFormat.remove(i, 1);
                } else {
                    qWarning("Translating am/pm to an empty string will "
                             "cause problems for ExtDateTimeEdit");
                }
            }
        }
    }

    if (QApplication::isRightToLeft()) {
        // we want displayFormat to return what you set with
        // setDisplayFormat() in rtol mode as well. I needed to
        // reverse it to calculate the escapedFormat correctly. Set it
        // back here.
        displayFormat = newFormat;
    }


//     qDebug() << newFormat << displayFormat;
//     qDebug("escapedFormat = [%s]", escapedFormat.toLatin1().constData());
//     qDebug("separators:\n'%s'", separators.join("|").toLatin1().constData());

//     qDebug("display is [%0x]", (uint)display);

    return true;
}

/*!
  \internal

  Returns the section at index \a index or NoSection if there are no sections there.
*/

ExtDateTimeEditPrivate::Section ExtDateTimeEditPrivate::sectionAt(int index) const
{
    if (index < separators.first().size()) {
        return (index == 0 ? FirstSection : NoSection);
    } else if (escapedFormat.size() - index < separators.last().size() + 1) {
        if (separators.last().size() == 0) {
            return sectionNodes.last().section;
        }
        return (index == last.pos ? LastSection : NoSection);
    }

//    QString deb;
    for (int i=0; i<sectionNodes.size(); ++i) {
        const int tmp = sectionNodes.at(i).pos;
//         deb += QString("%1 %2 %3 %4 %5 %6\n").arg(sectionName(sectionNodes.at(i).section)).arg(tmp).
//                arg(index).arg(i).arg(sectionSize(sectionNodes.at(i).section)).arg(layoutDirection == Qt::RightToLeft);

        if (index < tmp + sectionSize(sectionNodes.at(i).section)) {
            return (index < tmp ? NoSection : sectionNodes.at(i).section);
        }
    }
//    qDebug() << deb;
    return NoSection;
}

/*!
  \internal

  Returns the closest section of index \a index. Searches forward
  for a section if \a forward is true. Otherwise searches backwards.
*/

ExtDateTimeEditPrivate::Section ExtDateTimeEditPrivate::closestSection(int index, bool forward) const
{
    Q_ASSERT(index >= 0);
    if (index < separators.first().size()) {
        return forward ? sectionNodes.first().section : FirstSection;
    } else if (last.pos - index < separators.last().size() + 1) {
        return forward ? LastSection : sectionNodes.last().section;
    }
    for (int i=0; i<sectionNodes.size(); ++i) {
        int tmp = sectionNodes.at(i).pos;
        if (index < tmp + sectionSize(sectionNodes.at(i).section)) {
            if (index < tmp && !forward)
                return sectionNodes.at(i-1).section;
            return sectionNodes.at(i).section;
        } else if (i == sectionNodes.size() - 1 && index > tmp) {
            return sectionNodes.at(i).section;
        }
    }
    qWarning("2index return NoSection. This should not happen");
    return NoSection;
}

/*!
  \internal

  Returns a copy of the section that is before or after \a current, depending on \a forward.
*/

ExtDateTimeEditPrivate::SectionNode ExtDateTimeEditPrivate::nextPrevSection(Section current, bool forward) const
{
    if (QApplication::isRightToLeft())
        forward = !forward;
    if (current == FirstSection) {
        return (forward ? sectionNodes.first() : first);
    } else if (current == LastSection) {
        return (forward ? last : sectionNodes.last());
    }
    for (int i=0; i<sectionNodes.size(); ++i) {
        if (sectionNodes.at(i).section == current) {
            int index = i + (forward ? 1 : -1);
            if (index >= 0 && index < sectionNodes.size()) {
                return sectionNodes.at(index);
            } else {
                break;
            }
        }
    }
    return (forward ? last : first);
}

/*!
  \internal

  Clears the text of section \a s.
*/

void ExtDateTimeEditPrivate::clearSection(Section s)
{
    const char space = ' ';
    int cursorPos = edit->cursorPosition();
    bool blocked = edit->blockSignals(true);
    QString t = edit->text();
    t.replace(sectionPos(s), sectionSize(s), QString().fill(space, sectionSize(s)));
    edit->setText(t);
    edit->setCursorPosition(cursorPos);
    edit->blockSignals(blocked);
}

/*!
  \internal

  Returns the size of section \a s.
*/

int ExtDateTimeEditPrivate::sectionSize(Section s) const
{
    switch (s) {
    case FirstSection:
    case NoSection:
    case LastSection: return 0;

    case AmPmSection:
    case AmPmLowerCaseSection: {
        int lower = qMin(ExtDateTimeEdit::tr("pm").size(), ExtDateTimeEdit::tr("am").size());
        int upper = qMin(ExtDateTimeEdit::tr("PM").size(), ExtDateTimeEdit::tr("AM").size());
        return qMin(4, qMin(lower, upper));
    }

    case HourSection:
    case MinuteSection:
    case SecondSection:
    case DaySection:
    case MonthSection:
    case YearTwoDigitsSection: return 2;

    case MonthShortNameSection:
    case MSecSection: return 3;

    case YearSection: return 4;

    case Internal:
    case TimeSectionMask:
    case DateSectionMask: qWarning("Invalid section %s", sectionName(s).toLatin1().constData());
    }
    return -1;
}

/*!
  \internal

  Returns the text of section \a s. This function operates on the
  arg text rather than edit->text().
*/


QString ExtDateTimeEditPrivate::sectionText(const QString &text, Section s) const
{
    const SectionNode sn = sectionNode(s);
    return sn.section == NoSection ? QString() : text.mid(sn.pos, sectionSize(s));
}

/*!
  \internal

  Parses the part of \a text that corresponds to \a s and returns
  the value of that field. Sets *stateptr to the right state if
  stateptr != 0.
*/

int ExtDateTimeEditPrivate::sectionValue(Section s, QString &text, QValidator::State &state) const
{
    const char space = ' ';
    state = QValidator::Invalid;
    int num = 0;
    QString st = sectionText(text, s);
    EDTEDEBUG << "sectionValue for" << sectionName(s)
              << "with text" << text << "and st" << st;

    if (st.trimmed().isEmpty()) {
        state = QValidator::Intermediate;
    } else {
        const int index = sectionNode(s).pos;
        const int size = sectionSize(s);
        const bool done = !st.contains(space);
        switch (s) {
        case AmPmSection:
        case AmPmLowerCaseSection: {
            int ampm = findAmPm(st, s);
            switch (ampm) {
            case AM: // st == AM
            case PM: // st == PM
                num = ampm;
                state = QValidator::Acceptable;
                break;
            case PossibleAM: // st => AM
            case PossiblePM: // st => PM
                num = ampm - 2;
                state = QValidator::Intermediate;
                break;
            case PossibleBoth: // st => AM|PM
                num = 0;
                state = QValidator::Intermediate;
                break;
            case Neither:
                state = QValidator::Invalid;
                EDTEDEBUG << "invalid because findAmPm(" << st << ") returned -1";
                break;
            default:
                EDTEDEBUGN("This should never happen (findAmPm returned %d", ampm);
                break;
            }
            if (state != QValidator::Invalid) {
                text.replace(index, size, st);
            }
            break;
        }
        case MonthShortNameSection: {
            st = st.toLower();
            int tmp = findMonth(st);
            if (tmp != -1) {
                num = tmp;
                if (done) {
                    state = QValidator::Acceptable;
                    st = ExtDate::shortMonthName(num);
                    text.replace(index, size, st);
                } else {
                    state = QValidator::Intermediate;
                }
            } else {
                state = QValidator::Invalid;
                EDTEDEBUG << "invalid because" << st << "doesn't match any month name";
            }
            break;
        }
        case YearTwoDigitsSection: num = 2000;
        case YearSection:
        case MonthSection:
        case HourSection:
        case MinuteSection:
        case SecondSection:
        case MSecSection:
        case DaySection: {
            bool ok;
            num += (int)(st.toUInt(&ok));
            if (!ok) {
                state = QValidator::Invalid;
                EDTEDEBUG << "invalid because" << st << "can't become a uint";
            } else {
                if (s == HourSection && display & AmPmSection) {
                    bool pm = (sectionText(text, AmPmSection).toLower() == "pm");
                    if (pm && num < 12) {
                        num += 12;
                    } else if (!pm && num == 12) {
                        num = 0;
                    } else if (num > 12) {
                        state = QValidator::Invalid;
                        EDTEDEBUG << "invalid because" << st << "num is" << num;

                        break;
                    }
                }
                if (num < absoluteMin(s) || num > absoluteMax(s)) {
                    state = done ? QValidator::Invalid : QValidator::Intermediate;
                    if (done)
                        EDTEDEBUG << "invalid because" << st << "num is" << num
                                  << "outside absoluteMin and absoluteMax" << absoluteMin(s) << absoluteMax(s);

                } else {
                    state = QValidator::Acceptable;
                }
            }
            break;
        }
        default: qFatal("NoSection or Internal. This should never happen"); break; }
    }

    return (state != QValidator::Invalid ? num : -1);
}

/*!
  \internal
  \reimp
*/

QVariant ExtDateTimeEditPrivate::validateAndInterpret(QString &input,
                                                    int &pos, QValidator::State &state) const
{
    const char space = ' ';
    if (cachedText == input) {
        state = cachedState;
        EDTEDEBUG << "state" << state << "cachedText" << cachedText << "cachedValue" << cachedValue;
        return cachedValue;
    }
    QVariant tmp;
    SectionNode sn = {NoSection, 0};
    int index = 0;

    EDTEDEBUG << "validateAndInterpret" << input;
    int diff = input.size() - escapedFormat.size();
    bool specval = false;
    if (!specialValueText.isEmpty() && input == specialValueText) {
        specval = true;
        state = QValidator::Acceptable;
        tmp = minimum;
        goto end;
    }
    if (diff > 0) {
        const Section s = closestSection(pos - 1, false);
        if (s == FirstSection && s == LastSection) {
            EDTEDEBUG << "invalid because s ==" << sectionName(s);
            return QValidator::Invalid;
        }
        sn = sectionNode(s);
        const int sectionstart = sn.pos;
        const int sectionsize = sectionSize(s);

        QString sub = input.mid(sectionstart, sectionsize + diff);
        if (sub.count(space) < diff) {
            EDTEDEBUGN("sub is '%s' diff is %d sub.count is %d", sub.toLatin1().constData(), diff, sub.count(space));
            state = QValidator::Invalid;
            goto end;
        }

        sub.remove(space);
        input.replace(sectionstart, sectionsize + diff, sub.leftJustified(sectionsize, space));
    } else if (diff < 0) {
        const Section s = closestSection(pos, false);
        if (s == FirstSection && s == LastSection) {
            EDTEDEBUG << "invalid because s == " << sectionName(s);
            state = QValidator::Invalid;
            goto end;
        }
        sn = sectionNode(s);
        const int sectionstart = sn.pos;
        const int sectionsize = sectionSize(s);

        QString sub = input.mid(sectionstart, sectionsize + diff);
        sub.remove(space);
        input.replace(sectionstart, sectionsize + diff, sub.leftJustified(sectionsize, space));

        sn = sectionNode(currentSection);
    }

    for (int i=0; i<sectionNodes.size(); ++i) {
        sn = sectionNodes.at(i);
        if (input.mid(index, sn.pos - index) != separators.at(i)) {
            EDTEDEBUG << "invalid because" << input.mid(index, sn.pos - index) << "!=" << separators.at(i);
            state = QValidator::Invalid;
            goto end;
        }
        index = sn.pos + sectionSize(sn.section);
    }

    if (sn.pos + sectionSize(sn.section) < input.size()
        && input.mid(sn.pos + sectionSize(sn.section)) != separators.last()) {
        EDTEDEBUG << "invalid because" << input.mid(sn.pos + sectionSize(sn.section))
                  << "!=" << separators.last();
        state = QValidator::Invalid;
        goto end;
    }

    {
        int year, month, day, hour, minute, second, msec;
        const ExtDateTime &dt = value.toDateTime();
        year = dt.date().year();
        month = dt.date().month();
        day = dt.date().day();
        hour = dt.time().hour();
        minute = dt.time().minute();
        second = dt.time().second();
        msec = dt.time().msec();

        state = QValidator::Acceptable;
        for (int i=0; state != QValidator::Invalid && i<sectionNodes.size(); ++i) {
            const Section s = sectionNodes.at(i).section;
            QValidator::State tmpstate;
            int num = sectionValue(s, input, tmpstate);
            // Apple's GCC 3.3 and GCC 4.0 CVS flags a warning on qMin,
            // so code by hand to remove the warning.
            state = state < tmpstate ? state : tmpstate;

            if (state != QValidator::Invalid) {
                switch (s) {
                case HourSection: hour = num; break;
                case MinuteSection: minute = num; break;
                case SecondSection: second = num; break;
                case MSecSection: msec = num; break;
                case YearTwoDigitsSection:
                case YearSection: year = (num == 0 ? DATE_INITIAL.year() : num); break;
                case MonthSection:
                case MonthShortNameSection: month = qMax<int>(1, num); break;
                case DaySection: day = qMax<int>(1, num); break;
                case AmPmSection:
                case AmPmLowerCaseSection: hour = (num == 0 ? hour % 12 : (hour % 12) + 12); break;
                default:
                    qFatal("%s found in sections validateAndInterpret. This should never happen",
                           sectionName(s).toLatin1().constData());
                    break;
                }
            }
        }

        if (state == QValidator::Invalid) {
            tmp = getZeroVariant();
        } else {
            bool fixday = false;
            if (currentSection == DaySection) {
                cachedDay = day;
            } else if (cachedDay > day) {
                day = cachedDay;
                fixday = true;
            }

            if (!ExtDate::isValid(year, month, day)) {
                if (day < 32) {
                    cachedDay = day;
                } else if (day > 28 && ExtDate::isValid(year, month, 1)) {
                    fixday = true;
                }
            }
            if (fixday) {
                day = qMin<int>(day, ExtDate(year, month, 1).daysInMonth());
                const SectionNode &sn = sectionNode(DaySection);
                input.replace(sn.pos, sectionSize(DaySection), QString::number(day));
            }

            EDTEDEBUG << year << month << day << hour << minute << second << msec;
            tmp = QVariant(ExtDateTime(ExtDate(year, month, day), QTime(hour, minute, second, msec)));
        }
        EDTEDEBUGN("'%s' => '%s' (%s)", input.toLatin1().constData(),
                   tmp.toString().toLatin1().constData(), stateName(state).toLatin1().constData());
    }
end:
    if (tmp.toDateTime().isValid()) {
        if (!specval && state != QValidator::Invalid && tmp < minimum) {
            state = checkIntermediate(tmp.toDateTime(), input);
        } else {
            if (tmp > maximum)
                state = QValidator::Invalid;
            EDTEDEBUG << "not checking intermediate because tmp is" << tmp << minimum << maximum;
        }
    }
    cachedText = input;
    cachedState = state;
    cachedValue = tmp;
    return tmp;
}

/*!
  \internal
  finds the first possible monthname that \a str1 can match. Starting from \a index;
*/

int ExtDateTimeEditPrivate::findMonth(const QString &str1, int index) const
{
    Q_ASSERT(str1.size() == 3);

    for (int month=index; month<=12; ++month) {
        QString str2 = ExtDate::shortMonthName(month).toLower();
        bool found = true;
        for (int i=0; i<str1.size(); ++i) {
            if (str1.at(i) != str2.at(i) && !str1.at(i).isSpace()) {
                found = false;
                break;
            }
        }
        if (found)
            return month;
    }
    return -1;
}

/*!
  \internal

  returns
  0 if str == ExtDateTimeEdit::tr("AM")
  1 if str == ExtDateTimeEdit::tr("PM")
  2 if str can become ExtDateTimeEdit::tr("AM")
  3 if str can become ExtDateTimeEdit::tr("PM")
  4 if str can become ExtDateTimeEdit::tr("PM") and can become ExtDateTimeEdit::tr("AM")
  -1 can't become anything sensible

*/

int ExtDateTimeEditPrivate::findAmPm(QString &str, ExtDateTimeEditPrivate::Section s) const
{
    const char space = ' ';
    const int size = sectionSize(AmPmSection);
    Q_ASSERT(str.size() == size);

    enum {
        amindex = 0,
        pmindex = 1
    };
    QString ampm[2];
    if (s == AmPmSection) {
        ampm[amindex] = ExtDateTimeEdit::tr("AM");
        ampm[pmindex] = ExtDateTimeEdit::tr("PM");
    } else {
        ampm[amindex] = ExtDateTimeEdit::tr("am");
        ampm[pmindex] = ExtDateTimeEdit::tr("pm");
    }
    for (int i=0; i<2; ++i)
        ampm[i].truncate(size);

    EDTEDEBUG << "findAmPm" << str << ampm[0] << ampm[1];

    if (str.indexOf(ampm[amindex], 0, Qt::CaseInsensitive) == 0) {
        str = ampm[amindex];
        return AM;
    } else if (str.indexOf(ampm[pmindex], 0, Qt::CaseInsensitive) == 0) {
        str = ampm[pmindex];
        return PM;
    } else if (str.count(space) == 0) {
        return Neither;
    }

    bool broken[2] = {false, false};
    for (int i=0; i<size; ++i) {
        if (str.at(i) != space) {
            for (int j=0; j<2; ++j) {
                if (!broken[j]) {
                    int index = ampm[j].indexOf(str.at(i));
                    EDTEDEBUG << "looking for" << str.at(i)
                              << "in" << ampm[j] << "and got" << index;
                    if (index == -1) {
                        if (str.at(i).category() == QChar::Letter_Uppercase) {
                            index = ampm[j].indexOf(str.at(i).toLower());
                            EDTEDEBUG << "trying with" << str.at(i).toLower()
                                      << "in" << ampm[j] << "and got" << index;
                        } else if (str.at(i).category() == QChar::Letter_Lowercase) {
                            index = ampm[j].indexOf(str.at(i).toUpper());
                            EDTEDEBUG << "trying with" << str.at(i).toUpper()
                                      << "in" << ampm[j] << "and got" << index;
                        }
                        if (index == -1) {
                            broken[j] = true;
                            if (broken[amindex] && broken[pmindex]) {
                                EDTEDEBUG << str << "didn't make it";
                                return Neither;
                            }
                            continue;
                        } else {
                            str[i] = ampm[j].at(index); // fix case
                        }
                    }
                    ampm[j].remove(index, 1);
                }
            }
        }
    }
    if (!broken[pmindex] && !broken[amindex])
        return PossibleBoth;
    return (!broken[amindex] ? PossibleAM : PossiblePM);
}

/*!
  \internal
  Max number of units that can be changed by this section.
*/

int ExtDateTimeEditPrivate::maxChange(ExtDateTimeEditPrivate::Section s) const
{
    switch (s) {
        // Time. unit is msec
    case MSecSection: return 999;
    case SecondSection: return 59 * 1000;
    case MinuteSection: return 59 * 60 * 1000;
    case HourSection: return 59 * 60 * 60 * 1000;

        // Date. unit is day
    case DaySection: return 30;
    case MonthShortNameSection:
    case MonthSection: return 365 - 31;
    case YearSection: return (7999 - 1753) * 365;
    case YearTwoDigitsSection: return 100 * 365;

    default: break;
    }
    qFatal("%s passed to maxChange. This should never happen", sectionName(s).toLatin1().constData());
    return -1;
}


int ExtDateTimeEditPrivate::multiplier(ExtDateTimeEditPrivate::Section s) const
{
    switch (s) {
        // Time. unit is msec
    case MSecSection: return 1;
    case SecondSection: return 1000;
    case MinuteSection: return 60 * 1000;
    case HourSection: return 60 * 60 * 1000;

        // Date. unit is day
    case DaySection: return 1;
    case MonthShortNameSection:
    case MonthSection: return 30;
    case YearSection: return 365;
    case YearTwoDigitsSection: return 365;

    default: break;
    }
    qFatal("%s passed to multiplier. This should never happen", sectionName(s).toLatin1().constData());
    return -1;
}

/*!
  \internal Get a number that str can become which is between min
  and max or -1 if this is not possible.
*/

QString ExtDateTimeEditPrivate::sectionFormat(int s) const
{
    switch (s) {
    case AmPmSection: return QLatin1String("AP");
    case MSecSection: return QLatin1String("zzz");
    case SecondSection: return QLatin1String("ss");
    case MinuteSection: return QLatin1String("mm");
    case HourSection: return QLatin1String("hh");
    case AmPmLowerCaseSection: return QLatin1String("ap");
    case DaySection: return QLatin1String("dd");
    case MonthSection: return QLatin1String("MM");
    case YearSection: return QLatin1String("yyyy");
    case MonthShortNameSection: return QLatin1String("MMM");
    case YearTwoDigitsSection: return QLatin1String("yy");
    default: break;
    }
    qFatal("%s passed to sectionFormat. This should never happen", sectionName(s).toLatin1().constData());
    return QString();
}

/*!
  \internal Get a number that str can become which is between min
  and max or -1 if this is not possible.
*/

int ExtDateTimeEditPrivate::potentialValue(const QString &str, int min, int max, Section s) const
{
    int size = sectionSize(s);
    if (s == YearTwoDigitsSection) {
        min -= 2000;
        max -= 2000; // doesn't matter if max is -1 checking for < 0
    }
    QString simplified = str.simplified();
    Q_ASSERT(str != simplified);
    if (simplified.isEmpty()) {
        return min + (s == YearTwoDigitsSection ? 2000 : 0);
    } else if (simplified.toInt() > max && max >= 0) {
        return -1;
    } else {
        QString temp = simplified;
        while (temp.size() < size)
            temp.prepend(QLatin1Char('9'));
        int t = temp.toInt();
        if (t < min) {
            return -1;
        } else if (t <= max || max < 0) {
            return t + (s == YearTwoDigitsSection ? 2000 : 0);
        }
    }

    int ret = potentialValueHelper(simplified, min, max, size);
    if (ret == -1)
        return -1;
    return ret + (s == YearTwoDigitsSection ? 2000 : 0);
}

/*!
  \internal internal helper function called by potentialValue
*/

int ExtDateTimeEditPrivate::potentialValueHelper(const QString &str, int min, int max, int size) const
{
    if (str.size() == size) {
        const int val = str.toInt();
        if (val < min || val > max)
            return -1;
        EDTEDEBUG << "SUCCESS" << val << "is >=" << min << "and <=" << max;
        return val;
    }

    for (int i=0; i<=str.size(); ++i) {
        for (int j=0; j<10; ++j) {
            QString tmp = str;
            if (i == str.size()) {
                tmp.append(QChar('0' + j));
            } else {
                tmp.insert(i, QChar('0' + j));
            }
            int ret = potentialValueHelper(tmp, min, max, size);
            if (ret != -1)
                return ret;
        }
    }
    return -1;
}

/*!
  \internal
  \reimp
*/

QString ExtDateTimeEditPrivate::textFromValue(const QVariant &f) const
{
    Q_Q(const ExtDateTimeEdit);
    return q->textFromDateTime(f.toDateTime());
}

/*!
  \internal
  \reimp
*/

QVariant ExtDateTimeEditPrivate::valueFromText(const QString &f) const
{
    Q_Q(const ExtDateTimeEdit);
    return QVariant(q->dateTimeFromText(f));
}

/*!
  \internal Returns whether \a str is a string which value cannot be
  parsed but still might turn into something valid.
*/

QValidator::State ExtDateTimeEditPrivate::checkIntermediate(const ExtDateTime &dt,
                                                          const QString &s) const
{
    const char space = ' ';

    Q_ASSERT(dt < minimum);

    bool found = false;
    for (int i=0; i<sectionNodes.size(); ++i) {
        const SectionNode sn = sectionNodes.at(i);
        QString t = sectionText(s, sn.section).toLower();
        if (t.contains(space)) {
            if (found) {
                EDTEDEBUG << "Invalid because no spaces";
                return QValidator::Invalid;
            }
            found = true;
            switch (sn.section) {
            case MonthShortNameSection: {
                int tmp = dt.date().month();
                // I know the first possible month makes the date too early
                while ((tmp = findMonth(t, tmp + 1)) != -1) {
                    const QVariant copy(dt.addMonths(tmp - dt.date().month()));
                    if (copy >= minimum && copy <= maximum)
                        break;
                }
                if (tmp == -1)
                    return QValidator::Invalid;
            }
            case AmPmSection:
            case AmPmLowerCaseSection:
                if (t.count(space) == 2 || t.contains('m')) {
                    const QVariant copy(dt.addSecs(12 * 60 * 60));
                    if (copy >= minimum && copy <= maximum)
                        break;
                }
                return QValidator::Invalid;
            default: {
                int toMin;
                int toMax;
                int multi = multiplier(sn.section);

                if (sn.section & TimeSectionMask) {
                    if (dt.daysTo(minimum.toDateTime()) != 0) {
                        EDTEDEBUG << "if (dt.daysTo(minimum.toDateTime()) != 0)" << dt.daysTo(minimum.toDateTime());
                        return QValidator::Invalid;
                    }
                    toMin = dt.time().msecsTo(minimum.toDateTime().time());
                    if (dt.daysTo(maximum.toDateTime()) > 0) {
                        toMax = -1; // can't get to max
                    } else {
                        toMax = dt.time().msecsTo(maximum.toDateTime().time());
                    }
                } else {
                    toMin = dt.daysTo(minimum.toDateTime());
                    toMax = dt.daysTo(maximum.toDateTime());
                }
                int maxChange = ExtDateTimeEditPrivate::maxChange(sn.section);
                int maxChangeUnits = maxChange * multi;
                if (toMin > maxChangeUnits) {
                    EDTEDEBUG << "invalid because toMin > maxChangeUnits" << toMin
                              << maxChangeUnits << t << dt << minimum.toDateTime()
                              << multi;

                    return QValidator::Invalid;
                } else if (toMax > maxChangeUnits) {
                    toMax = -1; // can't get to max
                }

                int min = getDigit(minimum, sn.section);
                int max = toMax != -1 ? getDigit(maximum, sn.section) : -1;
                int tmp = potentialValue(t, min, max, sn.section);
                EDTEDEBUG << tmp << t << min << max << sectionName(sn.section)  << minimum.toDate() << maximum.toDate();
                if (tmp == -1) {
                    EDTEDEBUG << "invalid because potentialValue(" << t << min << max
                              << sectionName(sn.section) << "returned" << tmp;
                    return QValidator::Invalid;
                }

                QVariant var(dt);
                setDigit(var, sn.section, tmp);
                if (var > maximum) {
                    EDTEDEBUG << "invalid because" << var.toString() << ">" << maximum.toString();
                    return QValidator::Invalid;
                }
                break; }
            }
        }
    }
    return found ? QValidator::Intermediate : QValidator::Invalid;
}

/*!
  \internal
  For debugging. Returns the name of the section \a s.
*/

QString ExtDateTimeEditPrivate::sectionName(int s) const
{
    switch (s) {
    case ExtDateTimeEditPrivate::AmPmSection: return "AmPmSection";
    case ExtDateTimeEditPrivate::AmPmLowerCaseSection: return "AmPmLowerCaseSection";
    case ExtDateTimeEditPrivate::DaySection: return "DaySection";
    case ExtDateTimeEditPrivate::HourSection: return "HourSection";
    case ExtDateTimeEditPrivate::MSecSection: return "MSecSection";
    case ExtDateTimeEditPrivate::MinuteSection: return "MinuteSection";
    case ExtDateTimeEditPrivate::MonthSection: return "MonthSection";
    case ExtDateTimeEditPrivate::MonthShortNameSection: return "MonthShortNameSection";
    case ExtDateTimeEditPrivate::SecondSection: return "SecondSection";
    case ExtDateTimeEditPrivate::YearSection: return "YearSection";
    case ExtDateTimeEditPrivate::YearTwoDigitsSection: return "YearTwoDigitsSection";
    case ExtDateTimeEditPrivate::NoSection: return "NoSection";
    case ExtDateTimeEditPrivate::FirstSection: return "FirstSection";
    case ExtDateTimeEditPrivate::LastSection: return "LastSection";
    default: return "Unknown section " + QString::number(s);
    }
}

/*!
  \internal
  For debugging. Returns the name of the state \a s.
*/

QString ExtDateTimeEditPrivate::stateName(int s) const
{
    switch (s) {
    case QValidator::Invalid: return "Invalid";
    case QValidator::Intermediate: return "Intermediate";
    case QValidator::Acceptable: return "Acceptable";
    default: return "Unknown state " + QString::number(s);
    }
}

#include "extdatetimeedit.moc"

#endif // QT_NO_DATETIMEEDIT

#endif //0
