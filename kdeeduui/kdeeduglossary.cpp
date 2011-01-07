/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kdeeduglossary.h"

#include <kdebug.h>
#include <klocale.h>
#include <kiconloader.h>
#include <khtml_part.h>
#include <khtmlview.h>
#include <kglobal.h>
#include <klistview.h>
#include <klistviewsearchline.h>
#include <kstandarddirs.h>
#include <kactioncollection.h>

#include <tqfile.h>
#include <tqlabel.h>
#include <tqheader.h>
#include <tqlayout.h>
#include <tqpushbutton.h>
#include <tqsplitter.h>
#include <tqstringlist.h>
#include <tqtoolbutton.h>

Glossary::Glossary()
{
	// setting a generic name for a new glossary
	m_name = i18n( "Glossary" );
}

Glossary::~Glossary()
{
}

bool Glossary::loadLayout( TQDomDocument &Document, const KURL& url )
{
        TQFile layoutFile( url.path() );

        if (!layoutFile.exists())
	{
		kdDebug() << "no such file: " << layoutFile.name() << endl;
		return false;
	}

        if (!layoutFile.open(IO_ReadOnly))
                return false;

        ///Check if document is well-formed
        if (!Document.setContent(&layoutFile))
        {
                kdDebug() << "wrong xml" << endl;
                layoutFile.close();
                return false;
        }
        layoutFile.close();

        return true;
}

bool Glossary::isEmpty() const
{
	return m_itemlist.count() == 0;
}


Glossary* Glossary::readFromXML( const KURL& url, const TQString& path )
{
	TQDomDocument doc( "document" );

	Glossary *glossary = new Glossary();
	
	glossary->setPicturePath( path );

	if ( glossary->loadLayout( doc, url ) )
	{
		TQValueList<GlossaryItem*> itemList;
		itemList = glossary->readItems( doc );
		glossary->setItemlist( itemList );
		glossary->fixImagePath();
	}

	return glossary;
}

void Glossary::fixImagePath()
{
	kdDebug() << "Glossary::fixImagePath()" << endl;
	TQValueList<GlossaryItem*>::iterator it = m_itemlist.begin();
	const TQValueList<GlossaryItem*>::iterator itEnd = m_itemlist.end();
	TQString path = m_picturepath;
	TQString firstpart = "<img src=\"";
	firstpart += path;

	for ( ; it != itEnd ; ++it )
	{
		( *it )->setDesc( ( *it )->desc().replace("[img]", firstpart ) );
		( *it )->setDesc( ( *it )->desc().replace("[/img]", "\" />" ) );
	}
}
	
TQValueList<GlossaryItem*> Glossary::readItems( TQDomDocument &itemDocument )
{
	TQValueList<GlossaryItem*> list;

	TQDomNodeList itemList;
	TQDomNodeList refNodeList;
	TQDomElement itemElement;
	TQStringList reflist;

	itemList = itemDocument.elementsByTagName( "item" );

	const uint num = itemList.count();
	for ( uint i = 0; i < num; ++i )
	{
		reflist.clear();
		GlossaryItem *item = new GlossaryItem();
		
		itemElement = ( const TQDomElement& ) itemList.item( i ).toElement();
		
		TQDomNode nameNode = itemElement.namedItem( "name" );
		TQDomNode descNode = itemElement.namedItem( "desc" );
		
		TQString picName = itemElement.namedItem( "picture" ).toElement().text();
		TQDomElement refNode = ( const TQDomElement& ) itemElement.namedItem( "references" ).toElement();

		TQString desc = i18n( descNode.toElement().text().utf8() );
		if ( !picName.isEmpty() )
			desc.prepend("[img]"+picName +"[/img]" );

		item->setName( i18n( nameNode.toElement( ).text().utf8() ) );
		
		item->setDesc( desc.replace("[b]", "<b>" ) );
		item->setDesc( item->desc().replace("[/b]", "</b>" ) );
		item->setDesc( item->desc().replace("[i]", "<i>" ) );
		item->setDesc( item->desc().replace("[/i]", "</i>" ) );
		item->setDesc( item->desc().replace("[sub]", "<sub>" ) );
		item->setDesc( item->desc().replace("[/sub]", "</sub>" ) );
		item->setDesc( item->desc().replace("[sup]", "<sup>" ) );
		item->setDesc( item->desc().replace("[/sup]", "</sup>" ) );
		item->setDesc( item->desc().replace("[br]", "<br />" ) );
		
		refNodeList = refNode.elementsByTagName( "refitem" );
		for ( uint it = 0; it < refNodeList.count(); it++ )
		{
			reflist << i18n( refNodeList.item( it ).toElement().text().utf8() );
		}
		reflist.sort();
		item->setRef( reflist );
		
		list.append( item );
	}
	
	return list;
}



GlossaryDialog::GlossaryDialog( bool folded, TQWidget *parent, const char *name)
    : KDialogBase( Plain, i18n( "Glossary" ), Close, NoDefault, parent, name, false )
{
	//this string will be used for all items. If a backgroundpicture should
	//be used call Glossary::setBackgroundPicture().
	m_htmlbasestring = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\"><html><body>" ;

	m_folded = folded;
	
	TQVBoxLayout *vbox = new TQVBoxLayout( plainPage(), 0, KDialog::spacingHint() );
	vbox->activate();

	TQHBoxLayout *hbox = new TQHBoxLayout( 0L, 0, KDialog::spacingHint() );
	hbox->activate();

	TQToolButton *clear = new TQToolButton( plainPage() );
	clear->setIconSet( SmallIconSet( "locationbar_erase" ) );
	hbox->addWidget( clear );

	TQLabel *lbl = new TQLabel( plainPage() );
	lbl->setText( i18n( "Search:" ) );
	hbox->addWidget( lbl );

	m_search = new KListViewSearchLine( plainPage(), 0, "search-line" );
	hbox->addWidget( m_search );
	vbox->addLayout( hbox );
	setFocusProxy(m_search);
 
	TQSplitter *vs = new TQSplitter( plainPage() );
	vbox->addWidget( vs );

	m_glosstree = new KListView( vs, "treeview" );
	m_glosstree->addColumn( "entries" );
	m_glosstree->header()->hide();
	m_glosstree->setFullWidth( true );
	m_glosstree->setRootIsDecorated( true );
 
	m_search->setListView( m_glosstree );
 
	m_htmlpart = new KHTMLPart( vs, "html-part" );
 
	connect( m_htmlpart->browserExtension(), TQT_SIGNAL( openURLRequestDelayed( const KURL &, const KParts::URLArgs & ) ), this, TQT_SLOT( displayItem( const KURL &, const KParts::URLArgs & ) ) );
	connect( m_glosstree, TQT_SIGNAL(clicked( TQListViewItem * )), this, TQT_SLOT(slotClicked( TQListViewItem * )));
	connect( clear, TQT_SIGNAL(clicked()), m_search, TQT_SLOT(clear()));
 
	resize( 600, 400 );
}

GlossaryDialog::~GlossaryDialog()
{
}

void GlossaryDialog::keyPressEvent(TQKeyEvent* e)
{
	if (e->key() == Qt::Key_Return) {
		e->ignore();
	}
	KDialogBase::keyPressEvent(e);
}

void GlossaryDialog::displayItem( const KURL& url, const KParts::URLArgs& )
{
	// using the "host" part of a kurl as reference
	TQString myurl = url.host().lower();
	m_search->setText( "" );
	m_search->updateSearch( "" );
	TQListViewItem *found = 0;
	TQListViewItemIterator it( m_glosstree );
	TQListViewItem *item;
	while ( it.current() )
	{
		item = it.current();
		if ( item->text(0).lower() == myurl )
		{
			found = item;
			break;
		}
		++it;
	}
	if ( found )
	{
		m_glosstree->ensureItemVisible( found );
		m_glosstree->setCurrentItem( found );
		slotClicked( found );
	}
}

void GlossaryDialog::updateTree()
{
	m_glosstree->clear();

	TQValueList<Glossary*>::const_iterator itGl = m_glossaries.begin();
	const TQValueList<Glossary*>::const_iterator itGlEnd = m_glossaries.end();
	
	for ( ; itGl != itGlEnd ; ++itGl )
	{
		TQValueList<GlossaryItem*> items = ( *itGl )->itemlist();
		TQValueList<GlossaryItem*>::iterator it = items.begin();
		const TQValueList<GlossaryItem*>::iterator itEnd = items.end();

		TQListViewItem *main = new TQListViewItem( m_glosstree, ( *itGl )->name() );
		main->setExpandable( true );
		main->setSelectable( false );
		//XXX TMP!!!
		bool foldinsubtrees = m_folded;

		for ( ; it != itEnd ; ++it )
		{
			if ( foldinsubtrees )
			{
				TQChar thisletter = ( *it )->name().upper()[0];
				TQListViewItem *thisletteritem = findTreeWithLetter( thisletter, main );
				if ( !thisletteritem )
				{
					thisletteritem = new TQListViewItem( main, thisletter );
					thisletteritem->setExpandable( true );
					thisletteritem->setSelectable( false );
				}
				new TQListViewItem( thisletteritem, ( *it )->name() );
			}
			else
				new TQListViewItem( main, ( *it )->name() );
		}
		main->sort();
	}
}

void GlossaryDialog::addGlossary( Glossary* newgloss )
{
	if ( !newgloss ) return;
	if ( newgloss->isEmpty() ) return;
	m_glossaries.append( newgloss );

	kdDebug() << "Count of the new glossary: " << newgloss->itemlist().count() << endl;
	kdDebug() << "Number of glossaries: " << m_glossaries.count() << endl;

	updateTree();
}

TQListViewItem* GlossaryDialog::findTreeWithLetter( const TQChar& l, TQListViewItem* i )
{
	TQListViewItem *it = i->firstChild();
	while ( it )
	{
		if ( it->text(0)[0] == l )
			return it;
		it = it->nextSibling();
	}
	return 0;
}

void GlossaryDialog::slotClicked( TQListViewItem *item )
{
	if ( !item )
		return;
	
	/**
	 * The next lines are searching for the correct KnowledgeItem
	 * in the m_itemList. When it is found the HTML will be
	 * generated
	 */
	TQValueList<Glossary*>::iterator itGl = m_glossaries.begin();
	const TQValueList<Glossary*>::iterator itGlEnd = m_glossaries.end();
	bool found = false;
	GlossaryItem *i = 0;

	TQString bg_picture;
	
	while ( !found && itGl != itGlEnd )
	{
		TQValueList<GlossaryItem*> items = ( *itGl )->itemlist();
		TQValueList<GlossaryItem*>::const_iterator it = items.begin();
		const TQValueList<GlossaryItem*>::const_iterator itEnd = items.end();
		while ( !found && it != itEnd )
		{
			if ( ( *it )->name() == item->text( 0 ) )
			{
				i = *it;
				bg_picture = ( *itGl )->backgroundPicture();
				found = true;
			}
			++it;
		}
		++itGl;
	}
	if ( found && i )
	{
		TQString html;
		if ( !bg_picture.isEmpty() )
		{
			html = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\"><html><body background=\"" ;
			html.append( bg_picture );
			html.append("\">");
		}else
			html = m_htmlbasestring;
	
		html += i->toHtml() + "</body></html>";
		m_htmlpart->begin();
		m_htmlpart->write( html );

		m_htmlpart->end();
		return;
	}
}

void GlossaryDialog::slotClose()
{
	emit closed();
	accept();
}

TQString GlossaryItem::toHtml() const
{
	TQString code = "<h1>" + m_name + "</h1>" + m_desc;

	if ( !m_ref.isEmpty() )
	{
		TQString refcode = parseReferences();
		code += refcode;
	}
	return code;
}

TQString GlossaryItem::parseReferences() const
{
	TQString htmlcode = "<h3>" + i18n( "References" ) + "</h3>";
	
	bool first = true;
	for ( uint i = 0; i < m_ref.size(); i++ )
	{
		if ( !first )
			htmlcode += "<br>";
		else
			first = false;
		htmlcode += TQString( "<a href=\"item://%1\">%2</a>" ).arg( m_ref[i], m_ref[i] );
	}

	return htmlcode;
}


#include "kdeeduglossary.moc"
