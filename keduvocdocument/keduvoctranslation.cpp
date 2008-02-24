/***************************************************************************
                        Vocabulary Expression Translation for KDE Edu
    -----------------------------------------------------------------------

    Copyright 2007-2008 Frederik Gladhorn <frederik.gladhorn@kdemail.net>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "keduvoctranslation.h"

#include <QtCore/QMap>

#include <KDebug>

#include "keduvocdeclension.h"
#include "keduvocwordtype.h"

class KEduVocTranslation::KEduVocTranslationPrivate
{
public:
    KEduVocTranslationPrivate(KEduVocExpression* parent);

    ~KEduVocTranslationPrivate();

    KEduVocExpression* m_entry;

    /// Type of a word noun, verb, adjective etc
    KEduVocWordType* m_wordType;

    /// A comment giving additional information.
    QString m_comment;
    /// A hint, to make guessing the word easier.
    QString m_hint;
    /// Paraphrase
    QString m_paraphrase;
    /// A synonyme for a word: sick and ill, student and pupil
    QString m_synonym;
    /// An example
    QString m_example;
    /// An antonym - the opposite: hot - cold
    QString m_antonym;
    /// Pronunciation
    QString m_pronunciation;
    /// Image url
    KUrl m_imageUrl;
    /// Sound url
    KUrl m_soundUrl;

    /// When creating multiple choice tests, these are possible answers. (otherwise other words are added randomly)
    QStringList m_multipleChoice;

    /// Conjugations of a word (I go, you go, he goes... boring in english)
    QMap <QString, KEduVocConjugation> m_conjugations;

    /// The comparison forms of adjectives and adverbs: (fast), faster, fastest
    QString m_comparative;
    QString m_superlative;

    KEduVocDeclension* m_declension;

    /// One false friend string per other language
    QMap<int, QString> m_falseFriends;
};


KEduVocTranslation::KEduVocTranslationPrivate::KEduVocTranslationPrivate(KEduVocExpression* parent)
{
    m_entry = parent;
    m_wordType = 0;
    m_declension = 0;
}


KEduVocTranslation::KEduVocTranslationPrivate::~ KEduVocTranslationPrivate()
{
    delete m_declension;
}

KEduVocTranslation::KEduVocTranslation(KEduVocExpression* entry) : d( new KEduVocTranslationPrivate(entry) )
{}


KEduVocTranslation::KEduVocTranslation(KEduVocExpression* entry, const QString &translation ) : d( new KEduVocTranslationPrivate(entry) )
{
    setText(translation.simplified());
}

KEduVocTranslation::KEduVocTranslation( const KEduVocTranslation &other )
    : d( new KEduVocTranslationPrivate(other.d->m_entry) ),
         KEduVocText(other)
{
    d->m_wordType = other.d->m_wordType;
    d->m_comment = other.d->m_comment;
    d->m_paraphrase = other.d->m_paraphrase;
    d->m_synonym = other.d->m_synonym;
    d->m_example = other.d->m_example;
    d->m_antonym = other.d->m_antonym;
    d->m_pronunciation = other.d->m_pronunciation;
    d->m_conjugations = other.d->m_conjugations;
    d->m_comparative = other.d->m_comparative;
    d->m_superlative = other.d->m_superlative;
    d->m_multipleChoice = other.d->m_multipleChoice;
    d->m_falseFriends = other.d->m_falseFriends;
    d->m_imageUrl = other.d->m_imageUrl;
    d->m_soundUrl = other.d->m_soundUrl;
    /// @todo add declensions
}

KEduVocTranslation::~KEduVocTranslation()
{
    setWordType(0);
    delete d;
}

bool KEduVocTranslation::operator == ( const KEduVocTranslation & translation ) const
{
    return d->m_wordType == translation.d->m_wordType &&
           d->m_comment == translation.d->m_comment &&
           d->m_paraphrase == translation.d->m_paraphrase &&
           d->m_synonym == translation.d->m_synonym &&
           d->m_example == translation.d->m_example &&
           d->m_antonym == translation.d->m_antonym &&
           d->m_pronunciation == translation.d->m_pronunciation &&
           d->m_imageUrl == translation.d->m_imageUrl &&
           d->m_soundUrl == translation.d->m_soundUrl &&
           d->m_comparative == translation.d->m_comparative &&
           d->m_superlative == translation.d->m_superlative &&
           d->m_multipleChoice == translation.d->m_multipleChoice &&
           d->m_falseFriends == translation.d->m_falseFriends &&
           d->m_conjugations == translation.d->m_conjugations;
}

KEduVocTranslation & KEduVocTranslation::operator = ( const KEduVocTranslation & translation )
{
    KEduVocText::operator=(translation);
    d->m_entry = translation.d->m_entry;
    d->m_wordType = translation.d->m_wordType;
    d->m_comment = translation.d->m_comment;
    d->m_paraphrase = translation.d->m_paraphrase;
    d->m_synonym = translation.d->m_synonym;
    d->m_example = translation.d->m_example;
    d->m_antonym = translation.d->m_antonym;
    d->m_pronunciation = translation.d->m_pronunciation;
    d->m_imageUrl = translation.d->m_imageUrl;
    d->m_soundUrl = translation.d->m_soundUrl;
    d->m_comparative = translation.d->m_comparative;
    d->m_superlative = translation.d->m_superlative;
    d->m_multipleChoice = translation.d->m_multipleChoice;
    d->m_falseFriends = translation.d->m_falseFriends;
    d->m_conjugations = translation.d->m_conjugations;
    return *this;
}


QString KEduVocTranslation::comment() const
{
    return d->m_comment;
}


void KEduVocTranslation::setComment( const QString & expr )
{
    d->m_comment = expr.simplified();
}


void KEduVocTranslation::setFalseFriend( int indexFrom, const QString & expr )
{
    d->m_falseFriends[indexFrom] = expr.simplified();
}


QString KEduVocTranslation::falseFriend( int indexFrom ) const
{
    return d->m_falseFriends.value( indexFrom );
}


void KEduVocTranslation::setSynonym( const QString & expr )
{
    d->m_synonym = expr.simplified();
}


QString KEduVocTranslation::synonym() const
{
    return d->m_synonym;
}


void KEduVocTranslation::setExample( const QString & expr )
{
    d->m_example = expr.simplified();
}


QString KEduVocTranslation::example() const
{
    return d->m_example;
}


void KEduVocTranslation::setParaphrase( const QString & expr )
{
    d->m_paraphrase = expr.simplified();
}


QString KEduVocTranslation::paraphrase() const
{
    return d->m_paraphrase;
}


void KEduVocTranslation::setAntonym( const QString & expr )
{
    d->m_antonym = expr.simplified();
}


QString KEduVocTranslation::antonym() const
{
    return d->m_antonym;
}


void KEduVocTranslation::setConjugation( const QString& tense, const KEduVocConjugation& con )
{
    d->m_conjugations[tense] = con;
}


KEduVocConjugation& KEduVocTranslation::conjugation( const QString& tense )
{
    return d->m_conjugations[tense];
}


QStringList & KEduVocTranslation::multipleChoice()
{
    return d->m_multipleChoice;
}


QString KEduVocTranslation::pronunciation() const
{
    return d->m_pronunciation;
}


void KEduVocTranslation::setPronunciation( const QString & expr )
{
    d->m_pronunciation = expr.simplified();
}

QStringList KEduVocTranslation::conjugationTenses() const
{
    return d->m_conjugations.keys();
}

QMap< QString, KEduVocConjugation > KEduVocTranslation::conjugations() const
{
    return d->m_conjugations;
}

void KEduVocTranslation::setConjugations(const QMap< QString, KEduVocConjugation > & conjugations)
{
    d->m_conjugations = conjugations;
}

/** get the sound url for this translation if it exists */
KUrl KEduVocTranslation::soundUrl()
{
    return d->m_soundUrl;
}

/** set the sound url for this translation
 * @param url               url of the sound file */
void KEduVocTranslation::setSoundUrl(const KUrl &url)
{
    d->m_soundUrl = url;
}

/** get the image url for this translation if it exists */
KUrl KEduVocTranslation::imageUrl()
{
    return d->m_imageUrl;
}

/** set the image url for this translation
 * @param url               url of the image
 */
void KEduVocTranslation::setImageUrl(const KUrl &url)
{
    d->m_imageUrl = url;
}

KEduVocWordType * KEduVocTranslation::wordType() const
{
    return d->m_wordType;
}

void KEduVocTranslation::setWordType(KEduVocWordType * wordType)
{
    if ( d->m_wordType ) {
        d->m_wordType->removeTranslation(this);
    }
    if ( wordType ) {
        wordType->addTranslation(this);
    }
    d->m_wordType = wordType;
}

KEduVocExpression * KEduVocTranslation::entry()
{
    return d->m_entry;
}

QString KEduVocTranslation::comparative() const
{
    return d->m_comparative;
}

void KEduVocTranslation::setComparative(const QString & comparative)
{
    d->m_comparative = comparative;
}

QString KEduVocTranslation::superlative() const
{
    return d->m_superlative;
}

void KEduVocTranslation::setSuperlative(const QString & superlative)
{
    d->m_superlative = superlative;
}

KEduVocDeclension * KEduVocTranslation::declension()
{
    return d->m_declension;
}

void KEduVocTranslation::setDeclension(KEduVocDeclension * declension)
{
    // remove the old declension object
    delete d->m_declension;
    d->m_declension = declension;
}

void KEduVocTranslation::toXML(QDomElement & parent)
{
    kDebug() << "Write translation xml.";
    KEduVocText::toXML(parent);
    if (d->m_declension) {
//         d->m_declension->toXML(parent);
    }
}




