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

#ifndef FASTSMS_H
#define FASTSMS_H

#include <QWidget>
#include <QtGui>
#include "dictionary.h"
#include "ui_dialogoptions.h"

class Button;

namespace Ui {
    class fastsms;
}

class fastsms : public QWidget
{
    Q_OBJECT

public:
    explicit fastsms(QWidget *parent = 0);
    ~fastsms();
    QList<QString> lWord;
    QTextEdit *m_smsEdit;
    QLineEdit *m_phoneNumber;
    dictionary *m_dict;

protected:
    QString getWord(int nIndex);
    QLineEdit *inputNewWord;    
    QDialog* m_dialNewWord;
    void changeEvent(QEvent *e);

private slots:    
    void digitClicked();
    void nextWord();
    void prevWord();
    void sendSMS();
    void deleteChar();
    void insertNewWord();
    void newWordInserted();
    void saveDictionaryOnExit();
    void doNothing();
    void cursorPosChanged();
    void smsChanged();
    void openOptionDialog();
    void saveOptions();
    //void openRubrica();

private:
    Ui::fastsms *m_ui;
    Ui::DialogOption m_uiOptions;
    Button *createButton(const QString &text, const char *member);
    void checkConfig();
    void updateText(QString sWord);
    int m_nWordIndex;
    bool m_bKnowWord;    
    int m_nState;    
    QString *m_sCompleteValue;
    QTextCursor m_cursorSmsText;
    QSettings *m_settings;
    QDialog *m_dialOptions;
};

#endif // FASTSMS_H
