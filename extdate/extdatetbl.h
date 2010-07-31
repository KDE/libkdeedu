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
#ifndef EXTDATETBL_H
#define EXTDATETBL_H

#include <tqvalidator.h>
#include <tqgridview.h>
#include <klineedit.h>
#include "extcalendarsystemgregorian.h"

class KPopupMenu;

/** Week selection widget.
* @internal
* @version $Id$
* @author Stephan Binner
*/
class ExtDateInternalWeekSelector : public KLineEdit
{
  Q_OBJECT
protected:
  TQIntValidator *val;
  int result;
public slots:
  void weekEnteredSlot();
  void setMaxWeek(int max);
signals:
  void closeMe(int);
public:
  ExtDateInternalWeekSelector( TQWidget* parent=0, const char* name=0);
  int getWeek();
  void setWeek(int week);

private:
  class ExtDateInternalWeekPrivate;
  ExtDateInternalWeekPrivate *d;
};

/**
* A table containing month names. It is used to pick a month directly.
* @internal
* @version $Id$
* @author Tim Gilman, Mirko Boehm
*/
class ExtDateInternalMonthPicker : public QGridView
{
  Q_OBJECT
protected:
  /**
   * Store the month that has been clicked [1..12].
   */
  int result;
  /**
   * the cell under mouse cursor when LBM is pressed
   */
  short int activeCol;
  short int activeRow;
  /**
   * Contains the largest rectangle needed by the month names.
   */
  TQRect max;
signals:
  /**
   * This is send from the mouse click event handler.
   */
  void closeMe(int);
public:
  /**
   * The constructor.
   */
  ExtDateInternalMonthPicker(const ExtDate& date, TQWidget* parent, const char* name=0);
  /**
   * The destructor.
   */
  ~ExtDateInternalMonthPicker();
  /**
   * The size hint.
   */
  TQSize sizeHint() const;
  /**
   * Return the result. 0 means no selection (reject()), 1..12 are the
   * months.
   */
  int getResult() const;
protected:
  /**
   * Set up the painter.
   */
  void setupPainter(TQPainter *p);
  /**
   * The resize event.
   */
  virtual void viewportResizeEvent(TQResizeEvent*);
  /**
   * Paint a cell. This simply draws the month names in it.
   */
  virtual void paintCell(TQPainter* painter, int row, int col);
  /**
   * Catch mouse click and move events to paint a rectangle around the item.
   */
  virtual void contentsMousePressEvent(TQMouseEvent *e);
  virtual void contentsMouseMoveEvent(TQMouseEvent *e);
  /**
   * Emit monthSelected(int) when a cell has been released.
   */
  virtual void contentsMouseReleaseEvent(TQMouseEvent *e);

private:
  class ExtDateInternalMonthPrivate;
  ExtDateInternalMonthPrivate *d;
};

/** Year selection widget.
* @internal
* @version $Id$
* @author Tim Gilman, Mirko Boehm
*/
class ExtDateInternalYearSelector : public QLineEdit
{
  Q_OBJECT
protected:
  TQIntValidator *val;
  int result;
public slots:
  void yearEnteredSlot();
signals:
  void closeMe(int);
public:
  ExtDateInternalYearSelector( TQWidget* parent=0, const char* name=0);
  ~ExtDateInternalYearSelector();
	int getYear();
  void setYear(int year);

private:
  class ExtDateInternalYearPrivate {
    public:
    ExtDateInternalYearPrivate() {
      calendar = new ExtCalendarSystemGregorian();
    }
		~ExtDateInternalYearPrivate() {
			delete calendar;
		}
		ExtCalendarSystem *calendar;
  };
  ExtDateInternalYearPrivate *d;

};

/**
 * Frame with popup menu behavior.
 * @author Tim Gilman, Mirko Boehm
 * @version $Id$
 */
class KPopupFrame : public QFrame
{
  Q_OBJECT
protected:
  /**
   * The result. It is returned from exec() when the popup window closes.
   */
  int result;
  /**
   * Catch key press events.
   */
  virtual void keyPressEvent(TQKeyEvent* e);
  /**
   * The only subwidget that uses the whole dialog window.
   */
  TQWidget *main;
public slots:
  /**
   * Close the popup window. This is called from the main widget, usually.
   * @p r is the result returned from exec().
   */
  void close(int r);
public:
  /**
   * The contructor. Creates a dialog without buttons.
   */
  KPopupFrame(TQWidget* parent=0, const char*  name=0);
  /**
   * Set the main widget. You cannot set the main widget from the constructor,
   * since it must be a child of the frame itselfes.
   * Be careful: the size is set to the main widgets size. It is up to you to
   * set the main widgets correct size before setting it as the main
   * widget.
   */
  void setMainWidget(TQWidget* m);
  /**
   * The resize event. Simply resizes the main widget to the whole
   * widgets client size.
   */
  virtual void resizeEvent(TQResizeEvent*);
  /**
   * Open the popup window at position pos.
   */
  void popup(const TQPoint &pos);
  /**
   * Execute the popup window.
   */
  int exec(TQPoint p);
  /**
   * Dito.
   */
  int exec(int x, int y);

private:

  virtual bool close(bool alsoDelete) { return TQFrame::close(alsoDelete); }
protected:
  virtual void virtual_hook( int id, void* data );
private:
  class KPopupFramePrivate;
  KPopupFramePrivate *d;
};

/**
* Validates user-entered dates.
*/
class ExtDateValidator : public QValidator
{
public:
    ExtDateValidator(TQWidget* parent=0, const char* name=0);
    virtual State validate(TQString&, int&) const;
    virtual void fixup ( TQString & input ) const;
    State date(const TQString&, ExtDate&) const;
};

/**
 * Date selection table.
 * This is a support class for the ExtDatePicker class.  It just
 * draws the calender table without titles, but could theoretically
 * be used as a standalone.
 *
 * When a date is selected by the user, it emits a signal:
 * dateSelected(ExtDate)
 *
 * @internal
 * @version $Id$
 * @author Tim Gilman, Mirko Boehm
 */
class ExtDateTable : public QGridView
{
    Q_OBJECT
    //Q_PROPERTY( ExtDate date READ getDate WRITE setDate )
    Q_PROPERTY( bool popupMenu READ popupMenuEnabled WRITE setPopupMenuEnabled )

public:
    /**
     * The constructor.
     */
    ExtDateTable(TQWidget *parent=0,
	       ExtDate date=ExtDate::currentDate(),
	       const char* name=0, WFlags f=0);

    /**
     * The destructor.
     */
    ~ExtDateTable();

    /**
     * Returns a recommended size for the widget.
     * To save some time, the size of the largest used cell content is
     * calculated in each paintCell() call, since all calculations have
     * to be done there anyway. The size is stored in maxCell. The
     * sizeHint() simply returns a multiple of maxCell.
     */
    virtual TQSize sizeHint() const;
    /**
     * Set the font size of the date table.
     */
    void setFontSize(int size);
    /**
     * Select and display this date.
     */
    bool setDate(const ExtDate&);
    // ### 4.0 rename to date()
    const ExtDate& getDate() const;

    /**
     * Enables a popup menu when right clicking on a date.
     *
     * When it's enabled, this object emits a aboutToShowContextMenu signal
     * where you can fill in the menu items.
     *
     * @since 3.2
     */
    void setPopupMenuEnabled( bool enable );

    /**
     * Returns if the popup menu is enabled or not
     */
    bool popupMenuEnabled() const;

    enum BackgroundMode { NoBgMode=0, RectangleMode, CircleMode };

    /**
     * Makes a given date be painted with a given foregroundColor, and background
     * (a rectangle, or a circle/ellipse) in a given color.
     *
     * @since 3.2
     */
    void setCustomDatePainting( const ExtDate &date, const TQColor &fgColor, BackgroundMode bgMode=NoBgMode, const TQColor &bgColor=TQColor());

    /**
     * Unsets the custom painting of a date so that the date is painted as usual.
     *
     * @since 3.2
     */
    void unsetCustomDatePainting( const ExtDate &date );

protected:
    /**
     * calculate the position of the cell in the matrix for the given date. The result is the 0-based index.
     */
    int posFromDate( const ExtDate &date ); // KDE4: make this virtual, so subclasses can reimplement this and use a different default for the start of the matrix
    /**
     * calculate the date that is displayed at a given cell in the matrix. pos is the
     * 0-based index in the matrix. Inverse function to posForDate().
     */
    ExtDate dateFromPos( int pos ); // KDE4: make this virtual

    /**
     * Paint a cell.
     */
    virtual void paintCell(TQPainter*, int, int);
    /**
     * Handle the resize events.
     */
    virtual void viewportResizeEvent(TQResizeEvent *);
    /**
     * React on mouse clicks that select a date.
     */
    virtual void contentsMousePressEvent(TQMouseEvent *);
    virtual void wheelEvent( TQWheelEvent * e );
    virtual void keyPressEvent( TQKeyEvent *e );
    virtual void focusInEvent( TQFocusEvent *e );
    virtual void focusOutEvent( TQFocusEvent *e );

    // ### KDE 4.0 make the following private and mark as members

    /**
     * The font size of the displayed text.
     */
    int fontsize;
    /**
     * The currently selected date.
     */
    ExtDate date;
    /**
     * The day of the first day in the month [1..7].
     */
    int firstday;
    /**
     * The number of days in the current month.
     */
    int numdays;
    /**
     * The number of days in the previous month.
     */
    int numDaysPrevMonth;
    /**
     * unused
     * ### remove in KDE 4.0
     */
    bool unused_hasSelection;
    /**
     * Save the size of the largest used cell content.
     */
    TQRect maxCell;
signals:
    /**
     * The selected date changed.
     */
    void dateChanged(const ExtDate&);
    /**
     * This function behaves essentially like the one above.
     * The selected date changed.
     * @param cur The current date
     * @param old The date before the date was changed
     */
    void dateChanged(const ExtDate& cur, const ExtDate& old);
    /**
     * A date has been selected by clicking on the table.
     */
    void tableClicked();

    /**
     * A popup menu for a given date is about to be shown (as when the user
     * right clicks on that date and the popup menu is enabled). Connect
     * the slot where you fill the menu to this signal.
     *
     * @since 3.2
     */
    void aboutToShowContextMenu( KPopupMenu * menu, const ExtDate &date);

protected:
  virtual void virtual_hook( int id, void* data );
private:
    class ExtDateTablePrivate;
    ExtDateTablePrivate *d;
};

#endif // EXTDATETBL_H
