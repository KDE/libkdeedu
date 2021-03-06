/*
 Copyright 2014 Andreas Xavier <andxav at zoho dot com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
*/

#include "keduvocdocument.h"
#include "readermanager.h"
#include "keduvocpaukerreader.h"

#include "readerTestHelpers.h"

#include <kdebug.h>
#include <qtest_kde.h>

namespace PaukerReaderUnitTests
{

/**
 * @file Unit Tests of parsing of Pauker strings
 *
 * Each test creates a QString of Pauker format
 * reads, parses and verifies the parse.  The portions of the
 * XML generator object relevant to each test are defined in this file.
 *
 *This is the supported format
 * @verbatim
 <?xml version="1.0" encoding="UTF-8"?>
 <!--This is a lesson file for Pauker (http://pauker.sourceforge.net)-->
 <Lesson LessonFormat="1.7">
   <Description>Some Descrition</Description>
   <Batch>
     <Card>
       <FrontSide>
         <Text>Front text</Text>
       </FrontSide>
       <ReverseSide>
         <Text>Back Text</Text>
       </ReverseSide>
     </Card>
   ... more cards
    </Batch>
  </Lesson>
  * @endverbatim
  * */

/**
 * @brief Unit Tests of parsing of KVTML 2.0 strings
 *
 * Each test creates a string of XML
 * reads, parses and verifies the parse.
 * */

class PaukerReaderTest : public QObject
{
    Q_OBJECT

private slots:
    /** Initialize the string*/
    void init();
    /** One word doc*/
    void testParseOneWord();
    /** InvalidDoc*/
    void testParseInvalid();
private :
    QString oneGoodDoc;
    QString oneBadDoc;
    KEduVocDocument::FileType myType;
};

void PaukerReaderTest::init() {
    oneGoodDoc =                                    \
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n \
     <!--This is a lesson file for Pauker (http://pauker.sourceforge.net)-->\n \
     <Lesson LessonFormat=\"1.7\">\n \
       <Description>Some Description</Description>\n \
       <Batch>\n \
         <Card>\n \
           <FrontSide>\n \
             <Text>Front text</Text>\n \
           </FrontSide>\n \
           <ReverseSide>\n \
             <Text>Back Text</Text>\n \
           </ReverseSide>\n \
         </Card>\n \
        </Batch>\n \
      </Lesson>\n" ;
    oneBadDoc = oneGoodDoc + "\ninvalid XML characters ";

    myType = KEduVocDocument::Pauker;
}


void PaukerReaderTest::testParseOneWord()
{
    KVOCREADER_EXPECT( oneGoodDoc ,  KEduVocDocument::NoError , myType );
}

void PaukerReaderTest::testParseInvalid()
{
    KVOCREADER_EXPECT( oneBadDoc ,  KEduVocDocument::FileReaderFailed , myType );
}

}

QTEST_KDEMAIN_CORE( PaukerReaderUnitTests::PaukerReaderTest )



#include "paukerreadertest.moc"
