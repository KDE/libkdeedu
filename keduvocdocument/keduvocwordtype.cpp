/***************************************************************************

    Copyright 2007 Jeremy Whiting <jeremywhiting@scitools.com>
    Copyright 2007 Frederik Gladhorn <frederik.gladhorn@kdemail.net>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "keduvocwordtype.h"

#include "keduvocexpression.h"

#include <QtCore/QList>
#include <QtCore/QSet>

class KEduVocWordType::Private
{
public:
    // entries
    QList<KEduVocTranslation*> m_translations;
};


KEduVocWordType::KEduVocWordType(const QString& name, KEduVocWordType *parent)
        : d( new Private ), KEduVocContainer(name, WordTypeContainer, parent)
{
}

KEduVocWordType::KEduVocWordType( const KEduVocWordType &other )
        : d( new Private ), KEduVocContainer(other)
{
    d->m_translations = other.d->m_translations;
}

KEduVocWordType::~KEduVocWordType()
{
    delete d;
}


QList<KEduVocExpression*> KEduVocWordType::entries()
{
    QSet<KEduVocExpression*> entries;
    foreach(KEduVocTranslation* translation, d->m_translations) {
        entries.insert(translation->entry());
    }
    return entries.toList();
}

int KEduVocWordType::entryCount()
{
    return entries().count();
}

void KEduVocWordType::addTranslation(KEduVocTranslation* translation)
{
    d->m_translations.append( translation );
}

void KEduVocWordType::removeTranslation(KEduVocTranslation* translation)
{
    d->m_translations.removeAt( d->m_translations.indexOf(translation) );
}

KEduVocTranslation * KEduVocWordType::translation(int row)
{
    return d->m_translations.value(row);
}

// KEduVocWordType * KEduVocWordType::parent()
// {
//     if(KEduVocContainer::parent()) {
//         ///@todo check if it is a word type container?
//         return static_cast<KEduVocWordType*>(KEduVocContainer::parent());
//     }
//     return 0;
// }

KEduVocExpression * KEduVocWordType::entry(int row)
{
    return entries().value(row);
}


