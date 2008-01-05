/***************************************************************************

             manage grammar parts (articles, conjugation)

    -----------------------------------------------------------------------

    begin     : Sat Nov 27 09:50:53 MET 1999

    copyright : (C) 1999-2001 Ewald Arnold <kvoctrain@ewald-arnold.de>

                (C) 2004, 2005, 2007 Peter Hedlund <peter.hedlund@kdemail.net>
                (C) 2007 Frederik Gladhorn <frederik.gladhorn@kdemail.net>

    -----------------------------------------------------------------------

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "keduvocgrammar.h"

#include <QtCore/QMap>
#include <KDebug>

class KEduVocArticle::Private
{
public:
    QMap <int, QString>    m_articles;
};

KEduVocArticle::KEduVocArticle()
        :d( new Private )
{}

KEduVocArticle::KEduVocArticle( const KEduVocArticle &other )
        :d( new Private )
{
    d->m_articles = other.d->m_articles;
}

KEduVocArticle &KEduVocArticle::operator= ( const KEduVocArticle& other )
{
    d->m_articles = other.d->m_articles;
    return *this;
}

KEduVocArticle::KEduVocArticle( const QString &fem_def, const QString &fem_indef, const QString &mal_def, const QString &mal_indef, const QString &neu_def, const QString &neu_indef )
        :d( new Private )
{
    setArticle( mal_def, Singular, Definite, Masculine );
    setArticle( fem_def, Singular, Definite, Feminine );
    setArticle( neu_def, Singular, Definite, Neutral );

    setArticle( mal_indef, Singular, Indefinite, Masculine );
    setArticle( fem_indef, Singular, Indefinite, Feminine );
    setArticle( neu_indef, Singular, Indefinite, Neutral );
}

KEduVocArticle::~KEduVocArticle()
{
    delete d;
}


QString KEduVocArticle::article(ArticleNumber number, ArticleDefiniteness definite, ArticleGender gender)
{
    if ( d->m_articles.contains( indexOf(number, definite, gender) ) ) {
        return d->m_articles.value( indexOf(number, definite, gender) );
    }
    return QString();
}

void KEduVocArticle::setArticle(const QString & article, ArticleNumber number, ArticleDefiniteness definite, ArticleGender gender)
{
    kDebug() << article << "#" << number << "def" << definite << "indef"  << gender << "index" << indexOf(number, definite, gender);
    d->m_articles[indexOf(number, definite, gender)] = article;
}

int KEduVocArticle::indexOf(ArticleNumber number, ArticleDefiniteness definite, ArticleGender gender)
{
    return number + (definite * NumberMAX) + (gender * NumberMAX * DefinitenessMAX);
}

bool KEduVocArticle::isArticle(const QString & article) const
{
    return d->m_articles.values().contains(article);
}

bool KEduVocArticle::isEmpty()
{
    return d->m_articles.isEmpty();
}

