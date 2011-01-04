/*  -*- C++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1997 Tim D. Gilman (tdgilman@best.org)
              (C) 1998-2001 Mirko Boehm (mirko@kde.org)
              (C) 2004 Jason Harris (jharris@30doradus.org)

    These classes has been derived from those in kdatetbl.[h|cpp].
    The only differences are adaptations to use ExtDate instead of TQDate,
    to allow for more remote dates.  These changes by Jason Harris.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

/////////////////// ExtDateTable widget class //////////////////////
//
// Copyright (C) 1997 Tim D. Gilman
//           (C) 1998-2001 Mirko Boehm
// Written using Qt (http://www.troll.no) for the
// KDE project (http://www.kde.org)
//
// Modified to use ExtDate by Jason Harris, 2004
//
// This is a support class for the ExtDatePicker class.  It just
// draws the calender table without titles, but could theoretically
// be used as a standalone.
//
// When a date is selected by the user, it emits a signal:
//      dateSelected(ExtDate)

//#include "extdatepicker.h"
#include "extdatetbl.h"

#include <kapplication.h>
#include <kdebug.h>
#include <knotifyclient.h>
#include "kpopupmenu.h"
#include <tqpainter.h>
#include <tqdict.h>
#include <assert.h>


class ExtDateTable::ExtDateTablePrivate
{
public:
   ExtDateTablePrivate()
   {
      popupMenuEnabled=false;
      useCustomColors=false;
      calendar = new ExtCalendarSystemGregorian();

   }

   ~ExtDateTablePrivate()
   {
      delete calendar;
   }

   bool popupMenuEnabled;
   bool useCustomColors;

   struct DatePaintingMode
   {
     TQColor fgColor;
     TQColor bgColor;
     BackgroundMode bgMode;
   };
   TQDict <DatePaintingMode> customPaintingModes;
   ExtCalendarSystem *calendar;
};


ExtDateValidator::ExtDateValidator(TQWidget* parent, const char* name)
    : TQValidator(parent, name)
{
}

TQValidator::State
ExtDateValidator::validate(TQString& text, int&) const
{
  ExtDate temp;
  // ----- everything is tested in date():
  return date(text, temp);
}

TQValidator::State
ExtDateValidator::date(const TQString& text, ExtDate& ed) const
{
	//FIXME: Can't uncomment unless ExtDate is adopted by KDE
	//ExtDate tmp = KGlobal::locale()->readDate(text);
  ExtDate tmp = ExtDate::fromString( text );

  if (!tmp.isNull())
    {
      ed = tmp;
      return Acceptable;
    } else
      return Valid;
}

void
ExtDateValidator::fixup( TQString& ) const
{

}

ExtDateTable::ExtDateTable(TQWidget *parent, ExtDate date_, const char* name, WFlags f)
  : TQGridView(parent, name, f)
{
  d = new ExtDateTablePrivate;
  setFontSize(10);
  if(!date_.isValid())
    {
      kdDebug() << "ExtDateTable ctor: WARNING: Given date is invalid, using current date." << endl;
      date_=ExtDate::tqcurrentDate();
    }
  setFocusPolicy( TQWidget::StrongFocus );
  setNumRows(7); // 6 weeks max + headline
  setNumCols(7); // 7 days a week
  setHScrollBarMode(AlwaysOff);
  setVScrollBarMode(AlwaysOff);
  viewport()->setEraseColor(KGlobalSettings::baseColor());
  setDate(date_); // this initializes firstday, numdays, numDaysPrevMonth
}

ExtDateTable::~ExtDateTable()
{
  delete d;
}

int ExtDateTable::posFromDate( const ExtDate &dt )
{
//FIXME: Can't uncomment unless ExtDate is added to kdelibs
//  const ExtCalendarSystem * calendar = KGlobal::locale()->calendar();
//  const int firstWeekDay = KGlobal::locale()->weekStartDay();

  const int firstWeekDay = 7;

  int pos = d->calendar->day( dt );
  int offset = (firstday - firstWeekDay + 7) % 7;
  // make sure at least one day of the previous month is visible.
  // adjust this <1 if more days should be forced visible:
  if ( offset < 1 ) offset += 7;
  return pos + offset;
}

ExtDate ExtDateTable::dateFromPos( int pos )
{
  ExtDate pCellDate;

//FIXME: Can't uncomment unless ExtDate is added to kdelibs
//  const ExtCalendarSystem * calendar = KGlobal::locale()->calendar();
//  int firstWeekDay = KGlobal::locale()->weekStartDay();
  const int firstWeekDay = 7;

  d->calendar->setYMD(pCellDate, d->calendar->year(date), d->calendar->month(date), 1);

  int offset = (firstday - firstWeekDay + 7) % 7;
  // make sure at least one day of the previous month is visible.
  // adjust this <1 if more days should be forced visible:
  if ( offset < 1 ) offset += 7;
  pCellDate = d->calendar->addDays( pCellDate, pos - offset );

  return pCellDate;
}

void
ExtDateTable::paintCell(TQPainter *painter, int row, int col)
{
//FIXME: Can't uncomment unless ExtDate is added to kdelibs
//  const ExtCalendarSystem * calendar = KGlobal::locale()->calendar();
//  int firstWeekDay = KGlobal::locale()->weekStartDay();
  const int firstWeekDay = 7;

  TQRect rect;
  TQString text;
  TQPen pen;
  int w=cellWidth();
  int h=cellHeight();
  TQFont font=KGlobalSettings::generalFont();
  // -----

  if(row==0)
    { // we are drawing the headline
      font.setBold(true);
      painter->setFont(font);
      bool normalday = true;
      int daynum = ( col+firstWeekDay < 8 ) ? col+firstWeekDay :
                                              col+firstWeekDay-7;
      if ( daynum == d->calendar->weekDayOfPray() ||
         ( daynum == 6 && d->calendar->calendarName() == "gregorian" ) )
          normalday=false;

			TQBrush brushTitle();
 			TQBrush brushInvertTitle(tqcolorGroup().base());
			TQColor titleColor(isEnabled()?( KGlobalSettings::activeTitleColor() ):( KGlobalSettings::inactiveTitleColor() ) );
			TQColor textColor(isEnabled()?( KGlobalSettings::activeTextColor() ):( KGlobalSettings::inactiveTextColor() ) );
      if (!normalday)
        {
          painter->setPen(textColor);
          painter->setBrush(textColor);
          painter->drawRect(0, 0, w, h);
          painter->setPen(titleColor);
        } else {
          painter->setPen(titleColor);
          painter->setBrush(titleColor);
          painter->drawRect(0, 0, w, h);
          painter->setPen(textColor);
        }
      painter->drawText(0, 0, w, h-1, AlignCenter,
                        d->calendar->weekDayName(daynum, true), -1, &rect);
      painter->setPen(tqcolorGroup().text());
      painter->moveTo(0, h-1);
      painter->lineTo(w-1, h-1);
      // ----- draw the weekday:
    } else {
      bool paintRect=true;
      painter->setFont(font);
      int pos=7*(row-1)+col;

      ExtDate pCellDate = dateFromPos( pos );
      // First day of month
      text = d->calendar->dayString(pCellDate, true);
      if( d->calendar->month(pCellDate) != d->calendar->month(date) )
        { // we are either
          // � painting a day of the previous month or
          // � painting a day of the following month
          // TODO: don't hardcode gray here! Use a color with less contrast to the background than normal text.
          painter->setPen( tqcolorGroup().mid() );
//          painter->setPen(gray);
        } else { // paint a day of the current month
          if ( d->useCustomColors )
          {
            ExtDateTablePrivate::DatePaintingMode *mode=d->customPaintingModes[pCellDate.toString()];
            if (mode)
            {
              if (mode->bgMode != NoBgMode)
              {
                TQBrush oldbrush=painter->brush();
                painter->setBrush( mode->bgColor );
                switch(mode->bgMode)
                {
                  case(CircleMode) : painter->drawEllipse(0,0,w,h);break;
                  case(RectangleMode) : painter->drawRect(0,0,w,h);break;
                  case(NoBgMode) : // Should never be here, but just to get one
                                   // less warning when compiling
                  default: break;
                }
                painter->setBrush( oldbrush );
                paintRect=false;
              }
              painter->setPen( mode->fgColor );
            } else
              painter->setPen(tqcolorGroup().text());
          } else //if ( firstWeekDay < 4 ) // <- this doesn' make sense at all!
          painter->setPen(tqcolorGroup().text());
        }

      pen=painter->pen();
      int offset=firstday-firstWeekDay;
      if(offset<1)
        offset+=7;
      int dy = d->calendar->day(date);
      if( ((offset+dy) == (pos+1)) && hasFocus())
        {
           // draw the currently selected date
           painter->setPen(tqcolorGroup().highlight());
           painter->setBrush(tqcolorGroup().highlight());
           pen=tqcolorGroup().highlightedText();
        } else {
          painter->setBrush(paletteBackgroundColor());
          painter->setPen(paletteBackgroundColor());
//          painter->setBrush(tqcolorGroup().base());
//          painter->setPen(tqcolorGroup().base());
        }

      if ( pCellDate == ExtDate::tqcurrentDate() )
      {
         painter->setPen(tqcolorGroup().text());
      }

      if ( paintRect ) painter->drawRect(0, 0, w, h);
      painter->setPen(pen);
      painter->drawText(0, 0, w, h, AlignCenter, text, -1, &rect);
    }
  if(rect.width()>maxCell.width()) maxCell.setWidth(rect.width());
  if(rect.height()>maxCell.height()) maxCell.setHeight(rect.height());
}

void
ExtDateTable::keyPressEvent( TQKeyEvent *e )
{
//FIXME: Can't uncomment unless ExtDate is added to kdelibs
//    const ExtCalendarSystem * calendar = KGlobal::locale()->calendar();

    ExtDate temp = date;

    switch( e->key() ) {
    case Key_Prior:
        temp = d->calendar->addMonths( date, -1 );
        setDate(temp);
        return;
    case Key_Next:
        temp = d->calendar->addMonths( date, 1 );
        setDate(temp);
        return;
    case Key_Up:
        if ( d->calendar->day(date) > 7 ) {
            setDate(date.addDays(-7));
            return;
        }
        break;
    case Key_Down:
        if ( d->calendar->day(date) <= d->calendar->daysInMonth(date)-7 ) {
            setDate(date.addDays(7));
            return;
        }
        break;
    case Key_Left:
        if ( d->calendar->day(date) > 1 ) {
            setDate(date.addDays(-1));
            return;
        }
        break;
    case Key_Right:
        if ( d->calendar->day(date) < d->calendar->daysInMonth(date) ) {
            setDate(date.addDays(1));
            return;
        }
        break;
    case Key_Minus:
        setDate(date.addDays(-1));
        return;
    case Key_Plus:
        setDate(date.addDays(1));
        return;
    case Key_N:
        setDate(ExtDate::tqcurrentDate());
        return;
    case Key_Return:
    case Key_Enter:
        emit tableClicked();
        return;
    default:
        break;
    }

    KNotifyClient::beep();
}

void
ExtDateTable::viewportResizeEvent(TQResizeEvent * e)
{
  TQGridView::viewportResizeEvent(e);

  setCellWidth(viewport()->width()/7);
  setCellHeight(viewport()->height()/7);
}

void
ExtDateTable::setFontSize(int size)
{
//FIXME: Can't uncomment unless ExtDate is added to kdelibs
//  const ExtCalendarSystem * calendar = KGlobal::locale()->calendar();

  int count;
  TQFontMetrics metrics(fontMetrics());
  TQRect rect;
  // ----- store rectangles:
  fontsize=size;
  // ----- find largest day name:
  maxCell.setWidth(0);
  maxCell.setHeight(0);
  for(count=0; count<7; ++count)
    {
      rect=metrics.boundingRect(d->calendar->weekDayName(count+1, true));
      maxCell.setWidth(QMAX(maxCell.width(), rect.width()));
      maxCell.setHeight(QMAX(maxCell.height(), rect.height()));
    }
  // ----- compare with a real wide number and add some space:
  rect=metrics.boundingRect(TQString::tqfromLatin1("88"));
  maxCell.setWidth(QMAX(maxCell.width()+2, rect.width()));
  maxCell.setHeight(QMAX(maxCell.height()+4, rect.height()));
}

void
ExtDateTable::wheelEvent ( TQWheelEvent * e )
{
    setDate(date.addMonths( -(int)(e->delta()/120)) );
    e->accept();
}

void
ExtDateTable::contentsMousePressEvent(TQMouseEvent *e)
{

  if(e->type()!=TQEvent::MouseButtonPress)
    { // the ExtDatePicker only reacts on mouse press events:
      return;
    }
  if(!isEnabled())
    {
      KNotifyClient::beep();
      return;
    }

  // -----
  int row, col, pos, temp;
  TQPoint mouseCoord;
  // -----
  mouseCoord = e->pos();
  row=rowAt(mouseCoord.y());
  col=columnAt(mouseCoord.x());
  if(row<1 || col<0)
    { // the user clicked on the frame of the table
      return;
    }

  // Rows and columns are zero indexed.  The (row - 1) below is to avoid counting
  // the row with the days of the week in the calculation.

  // old selected date:
  temp = posFromDate( date );
  // new position and date
  pos = (7 * (row - 1)) + col;
  ExtDate clickedDate = dateFromPos( pos );

  // set the new date. If it is in the previous or next month, the month will
  // automatically be changed, no need to do that manually...
  setDate( clickedDate );

  // call updateCell on the old and new selection. If setDate switched to a different
  // month, these cells will be painted twice, but that's no problem.
  updateCell( temp/7+1, temp%7 );
  updateCell( row, col );

  emit tableClicked();

  if (  e->button() == Qt::RightButton && d->popupMenuEnabled )
  {
        KPopupMenu *menu = new KPopupMenu();

//FIXME: Uncomment the following line (and remove the one after it)
//       if ExtDate is added to kdelibs
//        menu->insertTitle( KGlobal::locale()->formatDate(clickedDate) );
        menu->insertTitle( clickedDate.toString() );

        emit aboutToShowContextMenu( menu, clickedDate );
        menu->popup(e->globalPos());
  }
}

bool
ExtDateTable::setDate(const ExtDate& date_)
{
  bool changed=false;
  ExtDate temp;
  // -----
  if(!date_.isValid())
    {
      kdDebug() << "ExtDateTable::setDate: refusing to set invalid date." << endl;
      return false;
    }
  if(date!=date_)
    {
      emit(dateChanged(date, date_));
      date=date_;
      emit(dateChanged(date));
      changed=true;
    }

//FIXME: Can't uncomment the following unless ExtDate is moved to kdelibs
//  const ExtCalendarSystem * calendar = KGlobal::locale()->calendar();

  d->calendar->setYMD(temp, d->calendar->year(date), d->calendar->month(date), 1);
  //temp.setYMD(date.year(), date.month(), 1);
  //kdDebug() << "firstDayInWeek: " << temp.toString() << endl;
  firstday=temp.dayOfWeek();
  numdays=d->calendar->daysInMonth(date);

  temp = d->calendar->addMonths(temp, -1);
  numDaysPrevMonth=d->calendar->daysInMonth(temp);
  if(changed)
    {
      tqrepaintContents(false);
    }
  return true;
}

const ExtDate&
ExtDateTable::getDate() const
{
  return date;
}

// what are those tqrepaintContents() good for? (pfeiffer)
void ExtDateTable::focusInEvent( TQFocusEvent *e )
{
//    tqrepaintContents(false);
    TQGridView::focusInEvent( e );
}

void ExtDateTable::focusOutEvent( TQFocusEvent *e )
{
//    tqrepaintContents(false);
    TQGridView::focusOutEvent( e );
}

QSize
ExtDateTable::tqsizeHint() const
{
  if(maxCell.height()>0 && maxCell.width()>0)
    {
      return TQSize(maxCell.width()*numCols()+2*frameWidth(),
             (maxCell.height()+2)*numRows()+2*frameWidth());
    } else {
      kdDebug() << "ExtDateTable::tqsizeHint: obscure failure - " << endl;
      return TQSize(-1, -1);
    }
}

void ExtDateTable::setPopupMenuEnabled( bool enable )
{
   d->popupMenuEnabled=enable;
}

bool ExtDateTable::popupMenuEnabled() const
{
   return d->popupMenuEnabled;
}

void ExtDateTable::setCustomDatePainting(const ExtDate &date, const TQColor &fgColor, BackgroundMode bgMode, const TQColor &bgColor)
{
    if (!fgColor.isValid())
    {
        unsetCustomDatePainting( date );
        return;
    }

    ExtDateTablePrivate::DatePaintingMode *mode=new ExtDateTablePrivate::DatePaintingMode;
    mode->bgMode=bgMode;
    mode->fgColor=fgColor;
    mode->bgColor=bgColor;

    d->customPaintingModes.tqreplace( date.toString(), mode );
    d->useCustomColors=true;
    update();
}

void ExtDateTable::unsetCustomDatePainting( const ExtDate &date )
{
    d->customPaintingModes.remove( date.toString() );
}

ExtDateInternalWeekSelector::ExtDateInternalWeekSelector
(TQWidget* parent, const char* name)
  : KLineEdit(parent, name),
    val(new TQIntValidator(this)),
    result(0)
{
  TQFont font;
  // -----
  font=KGlobalSettings::generalFont();
  setFont(font);
  setFrameStyle(TQFrame::NoFrame);
  setValidator(val);
  connect(this, TQT_SIGNAL(returnPressed()), TQT_SLOT(weekEnteredSlot()));
}

void
ExtDateInternalWeekSelector::weekEnteredSlot()
{
  bool ok;
  int week;
  // ----- check if this is a valid week:
  week=text().toInt(&ok);
  if(!ok)
    {
      KNotifyClient::beep();
      return;
    }
  result=week;
  emit(closeMe(1));
}

int
ExtDateInternalWeekSelector::getWeek()
{
  return result;
}

void
ExtDateInternalWeekSelector::setWeek(int week)
{
  TQString temp;
  // -----
  temp.setNum(week);
  setText(temp);
}

void
ExtDateInternalWeekSelector::setMaxWeek(int max)
{
  val->setRange(1, max);
}

// ### CFM To avoid binary incompatibility.
//     In future releases, remove this and tqreplace by  a ExtDate
//     private member, needed in ExtDateInternalMonthPicker::paintCell
class ExtDateInternalMonthPicker::ExtDateInternalMonthPrivate {
public:
        ExtDateInternalMonthPrivate (int y, int m, int d)
        : year(y), month(m), day(d)
        { calendar = new ExtCalendarSystemGregorian(); }
        ~ExtDateInternalMonthPrivate()
        { delete calendar; }

        ExtCalendarSystem *calendar;
        int year;
        int month;
        int day;
};

ExtDateInternalMonthPicker::~ExtDateInternalMonthPicker() {
   delete d;
}

ExtDateInternalMonthPicker::ExtDateInternalMonthPicker
(const ExtDate & date, TQWidget* parent, const char* name)
  : TQGridView(parent, name),
    result(0) // invalid
{
//FIXME: Can't uncomment the following unless ExtDate is moved to kdelibs
//  ExtCalendarSystem *calendar = KGlobal::locale()->calendar();

  TQRect rect;
  TQFont font;
  // -----
  activeCol = -1;
  activeRow = -1;
  font=KGlobalSettings::generalFont();
  setFont(font);
  setHScrollBarMode(AlwaysOff);
  setVScrollBarMode(AlwaysOff);
  setFrameStyle(TQFrame::NoFrame);
  setNumCols(3);
  d = new ExtDateInternalMonthPrivate(date.year(), date.month(), date.day());
  // For monthsInYear != 12
  setNumRows( (d->calendar->monthsInYear(date) + 2) / 3);
  // enable to find drawing failures:
  // setTableFlags(Tbl_clipCellPainting);
  viewport()->setEraseColor(KGlobalSettings::baseColor()); // for consistency with the datepicker
  // ----- find the preferred size
  //       (this is slow, possibly, but unfortunately it is needed here):
  TQFontMetrics metrics(font);
  for(int i = 1; ; ++i)
    {
      TQString str = d->calendar->monthName(i,
         d->calendar->year(date), false);
      if (str.isNull()) break;
      rect=metrics.boundingRect(str);
      if(max.width()<rect.width()) max.setWidth(rect.width());
      if(max.height()<rect.height()) max.setHeight(rect.height());
    }
}

QSize
ExtDateInternalMonthPicker::tqsizeHint() const
{
  return TQSize((max.width()+6)*numCols()+2*frameWidth(),
         (max.height()+6)*numRows()+2*frameWidth());
}

int
ExtDateInternalMonthPicker::getResult() const
{
  return result;
}

void
ExtDateInternalMonthPicker::setupPainter(TQPainter *p)
{
  p->setPen(KGlobalSettings::textColor());
}

void
ExtDateInternalMonthPicker::viewportResizeEvent(TQResizeEvent*)
{
  setCellWidth(width() / numCols());
  setCellHeight(height() / numRows());
}

void
ExtDateInternalMonthPicker::paintCell(TQPainter* painter, int row, int col)
{
  int index;
  TQString text;
  // ----- find the number of the cell:
  index=3*row+col+1;
  text=d->calendar->monthName(index,
    d->calendar->year(ExtDate(d->year, d->month,
    d->day)), false);
  painter->drawText(0, 0, cellWidth(), cellHeight(), AlignCenter, text);
  if ( activeCol == col && activeRow == row )
      painter->drawRect( 0, 0, cellWidth(), cellHeight() );
}

void
ExtDateInternalMonthPicker::contentsMousePressEvent(TQMouseEvent *e)
{
  if(!isEnabled() || e->button() != LeftButton)
    {
      KNotifyClient::beep();
      return;
    }
  // -----
  int row, col;
  TQPoint mouseCoord;
  // -----
  mouseCoord = e->pos();
  row=rowAt(mouseCoord.y());
  col=columnAt(mouseCoord.x());

  if(row<0 || col<0)
    { // the user clicked on the frame of the table
      activeCol = -1;
      activeRow = -1;
    } else {
      activeCol = col;
      activeRow = row;
      updateCell( row, col /*, false */ );
  }
}

void
ExtDateInternalMonthPicker::contentsMouseMoveEvent(TQMouseEvent *e)
{
  if (e->state() & LeftButton)
    {
      int row, col;
      TQPoint mouseCoord;
      // -----
      mouseCoord = e->pos();
      row=rowAt(mouseCoord.y());
      col=columnAt(mouseCoord.x());
      int tmpRow = -1, tmpCol = -1;
      if(row<0 || col<0)
        { // the user clicked on the frame of the table
          if ( activeCol > -1 )
            {
              tmpRow = activeRow;
              tmpCol = activeCol;
            }
          activeCol = -1;
          activeRow = -1;
        } else {
          bool differentCell = (activeRow != row || activeCol != col);
          if ( activeCol > -1 && differentCell)
            {
              tmpRow = activeRow;
              tmpCol = activeCol;
            }
          if ( differentCell)
            {
              activeRow = row;
              activeCol = col;
              updateCell( row, col /*, false */ ); // mark the new active cell
            }
        }
      if ( tmpRow > -1 ) // tqrepaint the former active cell
          updateCell( tmpRow, tmpCol /*, true */ );
    }
}

void
ExtDateInternalMonthPicker::contentsMouseReleaseEvent(TQMouseEvent *e)
{
  if(!isEnabled())
    {
      return;
    }
  // -----
  int row, col, pos;
  TQPoint mouseCoord;
  // -----
  mouseCoord = e->pos();
  row=rowAt(mouseCoord.y());
  col=columnAt(mouseCoord.x());
  if(row<0 || col<0)
    { // the user clicked on the frame of the table
      emit(closeMe(0));
    }

  pos=3*row+col+1;
  result=pos;
  emit(closeMe(1));
}



ExtDateInternalYearSelector::ExtDateInternalYearSelector
(TQWidget* parent, const char* name)
  : TQLineEdit(parent, name),
    val(new TQIntValidator(this)),
    result(0),
    d(new ExtDateInternalYearPrivate())
{
  TQFont font;
  // -----
  font=KGlobalSettings::generalFont();
  setFont(font);
  setFrameStyle(TQFrame::NoFrame);
  // set year limits (perhaps we should get rid of limits altogether)
  //there si also a year limit in ExtCalendarSystemGregorian...
  val->setRange(-50000, 50000);
  setValidator(val);
  connect(this, TQT_SIGNAL(returnPressed()), TQT_SLOT(yearEnteredSlot()));
}

ExtDateInternalYearSelector::~ExtDateInternalYearSelector() {
	delete val;
	delete d;
}

void
ExtDateInternalYearSelector::yearEnteredSlot()
{
  bool ok;
  int year;
  ExtDate date;
  // ----- check if this is a valid year:
  year=text().toInt(&ok);
  if(!ok)
    {
      KNotifyClient::beep();
      return;
    }
  //date.setYMD(year, 1, 1);
  d->calendar->setYMD(date, year, 1, 1);
  if(!date.isValid())
    {
      KNotifyClient::beep();
      return;
    }
  result=year;
  emit(closeMe(1));
}

int
ExtDateInternalYearSelector::getYear()
{
  return result;
}

void
ExtDateInternalYearSelector::setYear(int year)
{
  TQString temp;
  // -----
  temp.setNum(year);
  setText(temp);
}

KPopupFrame::KPopupFrame(TQWidget* parent, const char*  name)
  : TQFrame(parent, name, WType_Popup),
    result(0), // rejected
    main(0)
{
  setFrameStyle(TQFrame::Box|TQFrame::Raised);
  setMidLineWidth(2);
}

void
KPopupFrame::keyPressEvent(TQKeyEvent* e)
{
  if(e->key()==Key_Escape)
    {
      result=0; // rejected
      tqApp->exit_loop();
    }
}

void
KPopupFrame::close(int r)
{
  result=r;
  tqApp->exit_loop();
}

void
KPopupFrame::setMainWidget(TQWidget* m)
{
  main=m;
  if(main!=0)
    {
      resize(main->width()+2*frameWidth(), main->height()+2*frameWidth());
    }
}

void
KPopupFrame::resizeEvent(TQResizeEvent*)
{
  if(main!=0)
    {
      main->setGeometry(frameWidth(), frameWidth(),
          width()-2*frameWidth(), height()-2*frameWidth());
    }
}

void
KPopupFrame::popup(const TQPoint &pos)
{
  // Make sure the whole popup is visible.
  TQRect d = KGlobalSettings::desktopGeometry(pos);

  int x = pos.x();
  int y = pos.y();
  int w = width();
  int h = height();
  if (x+w > d.x()+d.width())
    x = d.width() - w;
  if (y+h > d.y()+d.height())
    y = d.height() - h;
  if (x < d.x())
    x = 0;
  if (y < d.y())
    y = 0;

  // Pop the thingy up.
  move(x, y);
  show();
}

int
KPopupFrame::exec(TQPoint pos)
{
  popup(pos);
  tqrepaint();
  tqApp->enter_loop();
  hide();
  return result;
}

int
KPopupFrame::exec(int x, int y)
{
  return exec(TQPoint(x, y));
}

void KPopupFrame::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

void ExtDateTable::virtual_hook( int, void* )
{ /*BASE::virtual_hook( id, data );*/ }

#include "extdatetbl.moc"
