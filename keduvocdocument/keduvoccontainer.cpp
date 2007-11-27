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

#include "keduvoccontainer.h"

#include "keduvocexpression.h"

#include <QList>

/** private class to store information about a lesson */
class KEduVocContainer::Private
{
public:
    ~Private();

    // properties for this lesson
    QString m_name;
    bool m_inPractice;

    // other lessons in the tree
    KEduVocContainer *m_parentContainer;
    QList<KEduVocContainer*> m_childContainers;

    EnumContainerType m_type;

    /// Image url
    KUrl m_imageUrl;
};

KEduVocContainer::Private::~ Private()
{
    qDeleteAll(m_childContainers);
}

KEduVocContainer::KEduVocContainer(const QString& name, EnumContainerType type, KEduVocContainer *parent)
        : d( new Private )
{
    d->m_parentContainer = parent;
    d->m_name = name;
    d->m_inPractice = true;
    d->m_type = type;
}

KEduVocContainer::KEduVocContainer( const KEduVocContainer &other )
        : d( new Private )
{
    d->m_name = other.d->m_name;
    d->m_inPractice = other.d->m_inPractice;
}

KEduVocContainer::~KEduVocContainer()
{
    delete d;
}

void KEduVocContainer::appendChildContainer(KEduVocContainer * child)
{
    d->m_childContainers.append(child);
}

KEduVocContainer * KEduVocContainer::childContainer(int row)
{
    return d->m_childContainers.value(row);
}

int KEduVocContainer::childContainerCount() const
{
    return d->m_childContainers.count();
}

int KEduVocContainer::row() const
{
    if (d->m_parentContainer) {
        return d->m_parentContainer->d->m_childContainers.indexOf(const_cast<KEduVocContainer*>(this));
    }
    return 0;
}


KEduVocContainer& KEduVocContainer::operator= ( const KEduVocContainer &other )
{
    d->m_name = other.d->m_name;
    d->m_inPractice = other.d->m_inPractice;
    return *this;
}

bool KEduVocContainer::operator==(const KEduVocContainer &other)
{
    return  d->m_name == other.d->m_name &&
            d->m_inPractice == other.d->m_inPractice;;
}

void KEduVocContainer::setName( const QString &name )
{
    d->m_name = name;
}

QString KEduVocContainer::name()
{
    return d->m_name;
}

bool KEduVocContainer::inPractice()
{
    return d->m_inPractice;
}

void KEduVocContainer::setInPractice(bool inPractice)
{
    d->m_inPractice = inPractice;
}

void KEduVocContainer::removeTranslation(int translation)
{
    foreach(KEduVocContainer *childContainer, d->m_childContainers) {
        childContainer->removeTranslation(translation);
    }

    foreach(KEduVocExpression *entry, entries() ) {
        entry->removeTranslation( translation );
    }
}

QList< KEduVocExpression * > KEduVocContainer::entriesRecursive()
{
    QList< KEduVocExpression * > entryList = entries();
    foreach(KEduVocContainer *childContainer, d->m_childContainers) {
        foreach(KEduVocExpression *childEntry, childContainer->entriesRecursive()) {
            if(!entryList.contains(childEntry)) {
                entryList.append(childEntry);
            }
        }
    }
    return entryList;
}

QList< KEduVocContainer * > KEduVocContainer::childContainers()
{
    return d->m_childContainers;
}

KEduVocContainer * KEduVocContainer::parent()
{
    return d->m_parentContainer;
}

// KEduVocContainer * KEduVocContainer::childContainer(const QString & name)
// {
//     for(int i = 0; i<d->m_childContainers.count(); i++){
//         if(d->m_childContainers.value(i)->name() == name) {
//             return d->m_childContainers[i];
//         }
//     }
//     return 0;
// }

void KEduVocContainer::setContainerType(KEduVocContainer::EnumContainerType type)
{
    d->m_type = type;
}

KEduVocContainer::EnumContainerType KEduVocContainer::containerType()
{
    return d->m_type;
}


KUrl KEduVocContainer::imageUrl()
{
    return d->m_imageUrl;
}

void KEduVocContainer::setImageUrl(const KUrl &url)
{
    d->m_imageUrl = url;
}

KEduVocContainer * KEduVocContainer::childOfType(KEduVocContainer::EnumContainerType type)
{
    if(containerType()==type) {
        return this;
    }
    foreach(KEduVocContainer* child, childContainers()) {
        KEduVocContainer* result = child->childOfType(type);
        if(result) {
            return result;
        }
    }
    return 0;
}


