#ifndef KDEEDUGLOSSARY_H
#define KDEEDUGLOSSARY_H
/***************************************************************************

    copyright            : (C) 2005 by Carsten Niehaus
    email                : cniehaus@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <khtml_part.h>
#include <kdialogbase.h>

class TQChar;
class TQDomDocument;
class TQListViewItem;
class KListView;
class KListViewSearchLine;
class KActionCollection;
class GlossaryItem;

/**
 * @class Glossary
 * @author Carsten Niehaus
 *
 * This class stores all items to be displayed. It also
 * has access-methods to the items
 */
class Glossary
{
	public:
		Glossary();
		virtual ~Glossary();

		/**
		 * add the item @p item to the glossary
		 */
		void addItem( GlossaryItem* item ){
			m_itemlist.append( item );
		}

		TQValueList<GlossaryItem*> itemlist()const{
			return m_itemlist;
		}

		/**
		 * clear the Glossary
		 */
		void clear(){
			m_itemlist.clear();
		}

		/**
		 * does this glossary have items?
		 */
		bool isEmpty() const;

		/**
		 * Every glossary can have a name. It will be
		 * set to @p name
		 */
		void setName( const TQString& name ){
			m_name = name;
		}

		/**
		 * @returns the name of the glossary
		 */
		TQString name()const{
			return m_name;
		}

		/**
		 * sets the internal list of items to @p list
		 */
		void setItemlist( TQValueList<GlossaryItem*> list ){
			m_itemlist = list;
		}

		/**
		 * Read a glossary from an XML file.
		 *
		 * @param url The path of the file to load
		 * @param path The path of the pictures. Will be used as m_picturepath
		 *
		 * @return a pointer to the loaded glossary. Even in case of
		 *         error, this won't return 0 but an empty Glossary.
		 */
		static Glossary* readFromXML( const KURL& url, const TQString& path = 0 );

		/**
		 * Every glossaryitem can show pictures. [img src="foo.png]
		 * will look for the file foo.png in the path defined be
		 * @p path
		 */
		void setPicturePath( const TQString& path ){
			m_picturepath = path;
		}

		TQString picturePath()const{
			return m_picturepath;
		}

		/**
		 * defines which picture to use as the background
		 * of the htmlview. The dialog
		 * will use the file specifiec by the @p filename
		 */
		void setBackgroundPicture( const TQString& filename ){
			m_backgroundpicture = filename;
		}

		/**
		 * @return the picuture used as the background in 
		 * this background
		 */
		TQString backgroundPicture()const{
			return m_backgroundpicture;
		}
	
	private:
		/**
		 * This methods parses the given xml-code. It will extract
		 * the information of the items and return them as a
		 * TQValueList<GlossaryItem*>
		 */
		virtual TQValueList<GlossaryItem*> readItems( TQDomDocument &itemDocument );
		
		TQString m_backgroundpicture;

		/**
		 * tqreplaces the [img]-pseudocode with valid html. The path where
		 * the pictures are stored will be used for pictures
		 */
		void fixImagePath();

		/**
		 * the path in which pictures of the glossary will be searched
		 */
		TQString m_picturepath;
		
		/**
		 * Load the tqlayout from an XML file.
		 *
		 * @param doc The TQDomDocument which will contain the read XML
		 *            contents.
		 * @param url The path of the file to load
		 *
		 * @return a bool indicating whether the loading of the XML was
		 *         successfull or not
		 */
		bool loadLayout( TQDomDocument& doc, const KURL& url );
	
		TQValueList<GlossaryItem*> m_itemlist;
		
		/**
		 * the name of the glossary
		 */
		TQString m_name;
};

/**
 * @class GlossaryItem
 * @author Carsten Niehaus
 *
 * A GlossaryItem stores the information of the content of
 * the item and its name. Furthermore, every item can have 
 * a number of pictures or references associated to it.
 * These are stored as QStringLists.
 */
class GlossaryItem
{
	public:
		GlossaryItem(){}
		~GlossaryItem(){}

		void setName( const TQString& s ){
			m_name = s;
		}

		void setDesc( const TQString& s){
			m_desc = s;
		}

		void setRef( const TQStringList& s){
			m_ref = s;
		}
	
		void setPictures( const TQString& s ){
			m_pic = s;
		}

		TQString name() const {
			return m_name;
		}
		
		TQString desc() const {
			return m_desc;
		}
		
		TQStringList ref() const {
			return m_ref;
		}
		
		TQStringList pictures() const {
			return m_pic;
		}
		
		/**
		 * @return the formated HTML code for current item.
		 **/
		TQString toHtml() const;

		/**
		 * This method parses the references.
		 * @return the HTML code with the references as HTML links
		 */
		TQString parseReferences() const;

	private:
		TQString m_name;
		TQString m_desc;
		TQStringList m_ref;
		TQStringList m_pic;
};

/**
 * @class GlossaryDialog
 * @author Pino Toscano
 * @author Carsten Niehaus
 */
class GlossaryDialog : public KDialogBase
{
	Q_OBJECT

	public:
		GlossaryDialog( bool folded = true, TQWidget *parent=0, const char *name=0);
		~GlossaryDialog();

		void keyPressEvent(TQKeyEvent*);

		/**
		 * add a new glossary
		 *
		 * @param newgloss the new glossary to add
		 */
		void addGlossary( Glossary* newgloss );

	private:
		TQValueList<Glossary*> m_glossaries;

		/**
		 * if true the items will be displayed folded
		 */
		bool m_folded;

		void updateTree();

		KHTMLPart *m_htmlpart;
		KListView *m_glosstree;
		TQString m_htmlbasestring;

		KActionCollection* m_actionCollection;

		TQListViewItem* findTreeWithLetter( const TQChar&, TQListViewItem* );

		KListViewSearchLine *m_search;

	private slots:
		void slotClicked( TQListViewItem * );
		/**
		 * The user clicked on a href. Emit the corresponding item
		 */
		void displayItem( const KURL& url, const KParts::URLArgs& args );

	protected slots:
		virtual void slotClose();
	
	signals:
		void closed();
};

#endif // KDEEDUGLOSSARY_H

