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

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include  <QtGui>

class dictionary
{
public:
    dictionary(QString *sInputFile);
    void insertWord(QString *sWord);
    void saveDictionaryToFile();
    void updateWordOrder(QString sKey,QString sWord);
    void updateWordOrder(QString sWord);
    bool existsWord(QString *sKey);
    static void deleteDictionaryFile();
    QList<QString> getWordList(QString *key);
    QString* calculateKey(QString *sWord);
    QFile *m_fInput;
    QFile *m_fDict;

private:
    int init();
    int createNewDictionaryFile();
    void loadDictLine(QString line);
    int createTranslater(bool bFirstRun);
    void insertSingleWord(QString *sWord,QString *sKey);
    bool m_bNextIsCode;
    QMultiHash<QString,QString> *m_dict;
    QMultiHash<QString,QString> *m_hmTranslater;
    QString m_sCode;
    QString m_sDataPath;
    QString *m_sInputFile;


};

#endif // DICTIONARY_H
