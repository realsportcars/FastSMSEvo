/****************************************************************************
**
** Copyright (C) 2010  Aimone Davide realsportcars@hotmail.com
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "dictionary.h"
#include <QtGui>

#define DICT_FILE "/dictionary.txt"
#define SYMBOL_FILE "/basekey.txt"
#define DATA_PATH "/data/"
#define WORD_SEPARATOR ';'

dictionary::dictionary(QString *sInputFile)
{
    if(sInputFile->isEmpty())
        return;

    m_sDataPath = QApplication::applicationDirPath() + DATA_PATH;
    m_sInputFile = sInputFile;
    m_dict = new QMultiHash<QString,QString>();

    init();
}


int dictionary::init()
{
    QString sDictFilePath = m_sDataPath + DICT_FILE;
    m_fDict = new QFile(sDictFilePath);

    if(m_fDict->exists())
    {
        if(!m_fDict->open(QIODevice::ReadWrite | QIODevice::Text))
        {
            QMessageBox msgBox;
            msgBox.setText("Warning!!!\nDictionary file exists but can't be opened");
            msgBox.exec();
            return -1;
        }

        if(createTranslater(false) != 0)
        {
            return -1;
        }

        QTextStream in(m_fDict);

        m_bNextIsCode=true;
        while (!in.atEnd())
        {
            QString line = in.readLine();
            loadDictLine(line);
        }
    }
    else
    {
        if(createTranslater(true) != 0)
            return -1;

        QString sInputFilePath = m_sDataPath + m_sInputFile->toAscii();
        m_fInput = new QFile(sInputFilePath);

        qDebug() << "Can't find dictionary file. Creating new one...";
        if(createNewDictionaryFile() != 0)
            return -1;

        m_fInput->close();
    }

    m_fDict->close();
    return 0;
}

void dictionary::loadDictLine(QString line)
{
    if(!line.isEmpty())
    {
        QStringList lToken = line.split(WORD_SEPARATOR);
        m_sCode=lToken.at(0);

        for(int i=1; i!=lToken.size() -1; i++)
        {
            m_dict->insert(m_sCode,lToken.at(i));
        }
    }
}

QList<QString> dictionary::getWordList(QString *key)
{
    QList<QString> list = m_dict->values(*key);
    return list;
}

int dictionary::createNewDictionaryFile()
{
    if (!m_fInput->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText("Warning!!!\nCan't open input dictionary file!");
        msgBox.exec();
        return -1;
    }

    if (!m_fDict->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText("Warning!!!\nCan't create dictionary file!");
        msgBox.exec();
        return -1;
    }

    QTextStream in(m_fInput);

    while (!in.atEnd())
    {
        QString *line = new QString(in.readLine());
        QString *sKey;

        sKey = calculateKey(line);
        insertWord(line);
    }

    qDebug() << "Loaded elements in dictionary:" << m_dict->size();

    //Start writing new dictionary file
    saveDictionaryToFile();

    return 0;
}

void dictionary::saveDictionaryToFile()
{
    //TODO: controllare che sia aperto prima in particolare al primo girno,quando crea dict.txt
    m_fDict->remove();
    m_fDict->open(QIODevice::ReadWrite | QIODevice::Text);

    QTextStream out(m_fDict);
    QString sOldKey = "-1";
    QMultiHash<QString, QString>::iterator it = m_dict->end();

    if(it == m_dict->begin())  //TODO: vuol dire che qualcosa non ha funzionato
    {
        qDebug() << "Dictionary is empty.Abort"; //TODO: cancellare il file???
        return;
    }

    it--;
    while (it != m_dict->begin())
    {
        if(sOldKey.compare(it.key())!=0)
        {
             if(it.key().length() > 0)
             {
                 out << "\n" + it.key() + ";";
             }
             sOldKey = it.key();
        }

        out << it.value()+ ";";
        --it;
    }

    out.flush();
    m_fDict->flush();
}

bool dictionary::existsWord(QString *sKey)
{
    return m_dict->contains(sKey->toAscii());
}

void dictionary::insertWord(QString *sWord)
{
    sWord = new QString(sWord->toLower());
    QString *sKey = calculateKey(sWord);

    //if there is a symbol split word in two or more
    if(sKey->contains("1"))
    {
        int nIdx = sKey->indexOf("1");
        sWord = new QString(sWord->left(nIdx+1));
        sKey = new QString(sKey->left(nIdx+1));

        int nQty = sKey->count("1");
        for(int i=0; i<nQty; i++)
        {
            int nIdx = sKey->indexOf("1");
            QString *partWord = new QString(sWord->left(nIdx+1));
            QString *partKey = new QString(sKey->left(nIdx+1));

            insertSingleWord(partWord,partKey);
            sWord->remove(0,nIdx);
            sKey->remove(0,nIdx);
        }
    }
    else
        insertSingleWord(sWord,sKey);
}

void dictionary::insertSingleWord(QString *sWord,QString *sKey)
{
    //Do insert...
    if(!sKey->isEmpty())
    {
        //qDebug() << "Key for new word is:" << sKey->toAscii();
        for(int i=1;i<=sWord->length();i++)
        {
            QString sAbbrevation = sWord->left(i);
            QString sPartialKey = sKey->left(i);

            if(!m_dict->contains(sPartialKey,sAbbrevation))
            {
                m_dict->insert(sPartialKey,sAbbrevation);
                //qDebug() << "Inserted word:" << sAbbrevation.toAscii() << "for key:" << sPartialKey.toAscii();
            }
        }
    }
}

void dictionary::updateWordOrder(QString sWord)
{
    QString *sWords = new QString(sWord.toAscii());
    QString *sKey = calculateKey(sWords);
    QString sKeys = sKey->toAscii();
    updateWordOrder(sKeys,sWord);
}

void dictionary::updateWordOrder(QString sKey,QString sWord)
{
    //TODO: can be handle in best way???
    if(m_dict->contains(sKey,sWord))
    {
        m_dict->remove(sKey,sWord);
        m_dict->insert(sKey,sWord);
    }
}

QString* dictionary::calculateKey(QString *sWord)
{
    QString *sKey = new QString();

    for(int i=0;i<sWord->length();i++)
    {
        QChar letter = sWord->at(i);

        sKey->append(m_hmTranslater->value(letter.toLower()));

        //Debug purpose:remove it on production
//        QString sCheck = hmTranslater->value(letter.toLower());
//        if(sCheck.isEmpty())
//            qDebug() << "Cant't recognize character:" << letter ;
    }

    return sKey;
}

int dictionary::createTranslater(bool bFirstRun)
{
    QString sSymFilePath = m_sDataPath + SYMBOL_FILE;
    QFile *fSymb = new QFile(sSymFilePath);

    if(!fSymb->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msgBox;
        msgBox.setText("Warning!!!\nCan't open " + sSymFilePath);
        msgBox.exec();
        return -1;
    }

    m_hmTranslater = new QMultiHash <QString,QString>();
    QTextStream in(fSymb);

    while (!in.atEnd())
    {
        QString *line = new QString(in.readLine());
        m_sCode=line->at(0);

        for(int i=1; i!=line->length(); i++)
        {
            m_hmTranslater->insert(line->at(i),m_sCode);

            if(bFirstRun)
                m_dict->insert(m_sCode,line->at(i));
        }
    }

    fSymb->close();
    return 0;
}

void dictionary::deleteDictionaryFile()
{
    QString sDictFilePath = QApplication::applicationDirPath() + DATA_PATH + DICT_FILE;
    QFile *fDelete = new QFile(sDictFilePath);
    fDelete->remove();

    qDebug() << sDictFilePath.toAscii() << "deleted.";
}
