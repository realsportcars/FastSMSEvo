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

#include "fastsms.h"
#include "button.h"
#include "dictionary.h"
#include <QMessage>
#include <QMessageService>
#include <QMessageAddress>
#include <QMenuBar>
#include <QMainWindow>

#define STATE_HOLD 0
#define STATE_SYM 1
#define STATE_WORD 2
#define STATE_DEL 3
#define STATE_AFTERDEL 4
#define STATE_CURMOVED 5
#define STATE_AFTERCURMOVED 6
#define STATE_SYM_IN_WORD 7
//#define STATE_SEND 8

#define SETTING_LANGUAGE "language"
#define SETTING_ORIENTATION "orientation"

#define ORIENTATION_AUTO "auto"
#define ORIENTATION_LANDSCAPE "landscape"
#define ORIENTATION_PORTRAIT "portrait"

//using namespace QtMobility;
QTM_USE_NAMESPACE

fastsms::fastsms(QWidget *parent) :
    QWidget(parent)//,    ui(new Ui::fastsms)
{
    //ui->setupUi(this);

    //Setting up UI. TODO:use ui file
    Button *to = createButton(tr("To:"),SLOT(doNothing()));
    Button *send = createButton(tr("Send"),SLOT(sendSMS()));
    Button *symbol = createButton(tr("   "),SLOT(doNothing()));
    Button *newWord = createButton(tr("New"),SLOT(insertNewWord()));
    Button *delChar = createButton(tr("C"),SLOT(deleteChar()));
    Button *one = createButton(tr("1 .,!"),SLOT(digitClicked()));
    Button *two = createButton(tr("2 abc"),SLOT(digitClicked()));
    Button *three = createButton(tr("3 def"),SLOT(digitClicked()));
    Button *four = createButton(tr("4 ghi"),SLOT(digitClicked()));
    Button *five = createButton(tr("5 jkl"),SLOT(digitClicked()));
    Button *six = createButton(tr("6 mno"),SLOT(digitClicked()));
    Button *seven = createButton(tr("7 pqrs"),SLOT(digitClicked()));
    Button *eight = createButton(tr("8 tuv"),SLOT(digitClicked()));
    Button *nine = createButton(tr("9 wxyz"),SLOT(digitClicked()));
    Button *zero = createButton(tr("0"),SLOT(digitClicked()));
    Button *number = createButton(tr("#"),SLOT(nextWord()));
    Button *star = createButton(tr("*"),SLOT(prevWord()));

    m_phoneNumber = new QLineEdit();
    m_smsEdit = new QTextEdit();

    QGridLayout *queryLayout = new QGridLayout();
    queryLayout->addWidget(to,0,0);
    queryLayout->addWidget(m_phoneNumber,0,1);
    queryLayout->addWidget(send,0,2);
    queryLayout->addWidget(m_smsEdit,2,0,1,3);

    QGridLayout *keyboard = new QGridLayout();
    keyboard->addWidget(symbol,0,0);
    keyboard->addWidget(newWord,0,1);
    keyboard->addWidget(delChar,0,2);
    keyboard->addWidget(one,1,0);
    keyboard->addWidget(two,1,1);
    keyboard->addWidget(three,1,2);
    keyboard->addWidget(four,2,0);
    keyboard->addWidget(five,2,1);
    keyboard->addWidget(six,2,2);
    keyboard->addWidget(seven,3,0);
    keyboard->addWidget(eight,3,1);
    keyboard->addWidget(nine,3,2);
    keyboard->addWidget(number,4,0);
    keyboard->addWidget(zero,4,1);
    keyboard->addWidget(star,4,2);

    QVBoxLayout *mainLayout = new QVBoxLayout();

    mainLayout->addLayout(queryLayout);
    mainLayout->addLayout(keyboard);

    setLayout(mainLayout);

    //Debug purpose
//    QTime myTimer;
//    myTimer.start();

    //load configuration
    m_dict = NULL;
    m_settings = new QSettings("AimoneDavide","fastsms");
    checkConfig();

    //create dictionary
    qDebug() << "Setting language:" << m_settings->value(SETTING_LANGUAGE).toString();
    if(m_dict == NULL)
        m_dict = new dictionary(new QString(m_settings->value(SETTING_LANGUAGE).toString()));

    //Debug purpose
//    int nMilliseconds = myTimer.elapsed();
//    qDebug() << "Elapsed millisecond for creating dictionary: " << nMilliseconds;

    //init variables
    m_nWordIndex=0;
    m_bKnowWord = true;
    m_sCompleteValue=new QString();
    m_cursorSmsText = m_smsEdit->textCursor(); // retrieve the visible cursor

    //Menu
    QMenu *fileMenu = new QMenu("File",this);

    QAction *aOption = new QAction("Options",this);
    connect(aOption,SIGNAL(triggered()),this, SLOT(openOptionDialog()));
    fileMenu->addAction(aOption);

    fileMenu->addAction(new QAction("About",this));
    fileMenu->addAction(new QAction("Quit",this));
    QMenuBar *menu = new QMenuBar(this);
    menu->addMenu(fileMenu);

    //manage autorotate from start
    this->setFocus();

    //manage cursor
    connect(m_smsEdit,SIGNAL(textChanged()),this,SLOT(smsChanged()));
    connect(m_smsEdit,SIGNAL(selectionChanged()),this,SLOT(cursorPosChanged()));
}

void fastsms::checkConfig()
{
    qDebug() << "start config";

    if(!m_settings->contains(SETTING_LANGUAGE))
    {
        qDebug() << "Can't find application settings.Perhaps first run?";
        openOptionDialog();
    }

    m_settings->sync();
}

void fastsms::openOptionDialog()
{
    qDebug() << "opening options";
    m_dialOptions = new QDialog(this);
    m_dialOptions->setAttribute(Qt::WA_DeleteOnClose);
    m_uiOptions.setupUi(m_dialOptions);

    connect(m_uiOptions.done,SIGNAL(clicked()),this,SLOT(saveOptions()));

    QDir dictDir(QApplication::applicationDirPath() + "/data");
    QStringList dictList = dictDir.entryList(QStringList("*.idc"));

    QString sCurrLang = m_settings->value(SETTING_LANGUAGE).toString();
    for(int i=0; i<dictList.size(); i++)
    {
        m_uiOptions.cbLanguage->addItem(dictList.at(i));

        if(dictList.at(i).compare(sCurrLang) == 0)
            m_uiOptions.cbLanguage->setCurrentIndex(i);
    }

    m_uiOptions.orientationAuto->setChecked(true);

    QString sCurrOrient = m_settings->value(SETTING_ORIENTATION).toString();
    if(sCurrOrient.compare(ORIENTATION_LANDSCAPE) == 0)
        m_uiOptions.orientationLandscape->setChecked(true);
    else if(sCurrOrient.compare(ORIENTATION_PORTRAIT) == 0)
        m_uiOptions.orientationPortrait->setChecked(true);

    m_dialOptions->show();
}

void fastsms::saveOptions()
{
    QString sOldLang = m_settings->value(SETTING_LANGUAGE).toString();

    QString sDictFile = m_uiOptions.cbLanguage->currentText();
    m_settings->setValue(SETTING_LANGUAGE,sDictFile);

    if(m_uiOptions.orientationAuto->isChecked())
        m_settings->setValue(SETTING_ORIENTATION,ORIENTATION_AUTO);
    else if(m_uiOptions.orientationLandscape->isChecked())
        m_settings->setValue(SETTING_ORIENTATION,ORIENTATION_LANDSCAPE);
    else if(m_uiOptions.orientationPortrait->isChecked())
        m_settings->setValue(SETTING_ORIENTATION,ORIENTATION_PORTRAIT);

    if(sOldLang.compare(sDictFile) != 0)
    {
        dictionary::deleteDictionaryFile();
        m_dict = new dictionary(new QString(m_settings->value(SETTING_LANGUAGE).toString()));
        qDebug() << "Language changed: need rebuild dictionary!";
    }

    qDebug() << "Settings:" << m_settings->value(SETTING_LANGUAGE).toString() << "," << m_settings->value(SETTING_ORIENTATION).toString();

    m_dialOptions->close();
}



void fastsms::cursorPosChanged()
{
    if(m_nState != STATE_AFTERCURMOVED)
    {
        m_smsEdit->disconnect();

        qDebug() << "cursor moved";
        m_cursorSmsText = m_smsEdit->textCursor(); // retrieve the visible cursor
        m_cursorSmsText.select(QTextCursor::WordUnderCursor);
        m_smsEdit->setTextCursor(m_cursorSmsText);

        if(m_cursorSmsText.hasSelection())
        {
            qDebug() << "Selected text from Cursor moved" << m_cursorSmsText.selectedText().toAscii();

            QString *sWord = new QString(m_cursorSmsText.selectedText());
            m_sCompleteValue = m_dict->calculateKey(sWord);
            m_nWordIndex = 0;
            lWord = m_dict->getWordList(m_sCompleteValue);
            m_bKnowWord = true;
            m_nState = STATE_AFTERCURMOVED;
        }
        else
        {
            m_sCompleteValue->clear();
            m_nWordIndex = 0;
            lWord.clear();
        }

        connect(m_smsEdit,SIGNAL(textChanged()),this,SLOT(smsChanged()));
        connect(m_smsEdit,SIGNAL(selectionChanged()),this,SLOT(cursorPosChanged()));
    }
    else
    {
        m_nState = STATE_CURMOVED;
    }
}

void fastsms::smsChanged()
{
    m_smsEdit->disconnect();

    m_cursorSmsText = m_smsEdit->textCursor();

    if(m_nState == STATE_SYM || m_nState == STATE_SYM_IN_WORD) //cosi si riescono a gestire forse tutti i simboli
    {
        m_cursorSmsText.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
        m_cursorSmsText.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        m_smsEdit->setTextCursor(m_cursorSmsText);
    }
    else
    {
        qDebug() << "Before selected text from smsChanged" << m_cursorSmsText.selectedText().toAscii();
        m_cursorSmsText.select(QTextCursor::WordUnderCursor);

        //Manage bug: todo resolve it in other way
        if(m_cursorSmsText.hasSelection())
        {
            QString *sStartWord = new QString(m_cursorSmsText.selectedText().at(0));
            QString *sStartKey = m_dict->calculateKey(sStartWord);
            if(sStartKey->compare("1") == 0)
            {
                qDebug() << "Arrange cursor position from" << m_cursorSmsText.position() << "-" << m_cursorSmsText.anchor();
                m_cursorSmsText.setPosition(m_cursorSmsText.anchor()-1,QTextCursor::MoveAnchor);
                qDebug() << "To:" << m_cursorSmsText.position() << "-" << m_cursorSmsText.anchor();
                m_cursorSmsText.select(QTextCursor::WordUnderCursor);

                qDebug() << "Arranged selected text from smsChanged" << m_cursorSmsText.selectedText().toAscii();
            }
        }

        m_smsEdit->setTextCursor(m_cursorSmsText);
        qDebug() << "Selected text from smsChanged" << m_cursorSmsText.selectedText().toAscii();

        if(m_nState == STATE_DEL)
        {
            QString *sWord = new QString(m_cursorSmsText.selectedText());
            m_sCompleteValue = m_dict->calculateKey(sWord);
            m_nWordIndex = 0;
            lWord = m_dict->getWordList(m_sCompleteValue);
            m_bKnowWord = true;
            m_nState = STATE_AFTERDEL;
        }
    }

    connect(m_smsEdit,SIGNAL(textChanged()),this,SLOT(smsChanged()));
    connect(m_smsEdit,SIGNAL(selectionChanged()),this,SLOT(cursorPosChanged()));
}

/**Create button */
Button *fastsms::createButton(const QString &text, const char *member)
{
    Button *button = new Button(text);
    connect(button, SIGNAL(clicked()), this, member);
    button->setFocusPolicy(Qt::NoFocus);
    return button;
}


void fastsms::digitClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString *lastPress = new QString(clickedButton->text().at(0));

    if(m_phoneNumber->hasFocus())
    {
        m_phoneNumber->insert(lastPress->toAscii());
    }
    else
    {
        if(lastPress->compare("0") == 0)
        {
            if(m_nState == STATE_WORD && m_cursorSmsText.hasSelection())
            {
                QString sChoosen = m_cursorSmsText.selectedText();
                m_dict->updateWordOrder(sChoosen);
            }

            m_nState = STATE_HOLD;

            //update cursor
            m_smsEdit->disconnect();

            m_cursorSmsText.clearSelection();
            m_smsEdit->setTextCursor(m_cursorSmsText);

            connect(m_smsEdit,SIGNAL(textChanged()),this,SLOT(smsChanged()));
            connect(m_smsEdit,SIGNAL(selectionChanged()),this,SLOT(cursorPosChanged()));

            m_sCompleteValue->clear();
            m_sCompleteValue = lastPress;

            m_bKnowWord=true;
        }
        else if(lastPress->compare("1") == 0)
        {
            //manage words with simbols
            m_sCompleteValue->append(lastPress);

            if(!m_dict->existsWord(m_sCompleteValue) || m_nState == STATE_HOLD)
            {
                if(m_nState == STATE_WORD && m_cursorSmsText.hasSelection())
                {
                    QString sChoosen = m_cursorSmsText.selectedText();
                    m_dict->updateWordOrder(sChoosen);
                }

                m_nState = STATE_SYM;

                //update cursor
                m_smsEdit->disconnect();

                m_cursorSmsText.clearSelection();
                m_smsEdit->setTextCursor(m_cursorSmsText);

                connect(m_smsEdit,SIGNAL(textChanged()),this,SLOT(smsChanged()));
                connect(m_smsEdit,SIGNAL(selectionChanged()),this,SLOT(cursorPosChanged()));

                m_sCompleteValue->clear();
                m_sCompleteValue = lastPress;

                m_bKnowWord=true;
            }
            else
            {
                m_nState = STATE_SYM_IN_WORD;
            }
        }
        else
        {
            //In other words: if im start writing a new word
            if((m_nState != STATE_WORD && m_nState != STATE_DEL && m_nState != STATE_AFTERDEL
               && m_nState != STATE_CURMOVED && m_nState != STATE_AFTERCURMOVED)|| m_nState == STATE_SYM_IN_WORD)
            {
                //update cursor
                m_smsEdit->disconnect();

                m_cursorSmsText.clearSelection();
                m_smsEdit->setTextCursor(m_cursorSmsText);

                connect(m_smsEdit,SIGNAL(textChanged()),this,SLOT(smsChanged()));
                connect(m_smsEdit,SIGNAL(selectionChanged()),this,SLOT(cursorPosChanged()));

                m_sCompleteValue->clear();
            }

            m_nState = STATE_WORD;

            m_sCompleteValue->append(lastPress);
        }

        QString sChoosenWord;
        m_nWordIndex=0;
        lWord = m_dict->getWordList(m_sCompleteValue);

        if(lWord.size()!=0)
        {
            sChoosenWord = lWord.at(0);
            updateText(sChoosenWord);
        }
        else
        {
            m_bKnowWord = false;
            qDebug() << "Can't find word for key:" << m_sCompleteValue->toAscii();
            m_sCompleteValue = new QString(m_sCompleteValue->left(m_sCompleteValue->length()-1));
        }

        qDebug() << "Looked for key" << m_sCompleteValue->toAscii();
    }
}

void fastsms::nextWord()
{
    if(m_nState == STATE_SYM_IN_WORD)
    {
        m_nWordIndex = -1;
        m_sCompleteValue->clear();
        m_sCompleteValue->append("1");
        lWord = m_dict->getWordList(m_sCompleteValue);
    }

    if(m_bKnowWord)
    {
        if(lWord.size()==0)
            return;

        m_nWordIndex++;
        if(m_nWordIndex>INT_MAX-1)
            m_nWordIndex=INT_MAX;

        QString sChoosenWord = getWord(m_nWordIndex);
        if(!sChoosenWord.isEmpty())
            updateText(sChoosenWord);
    }
}

void fastsms::prevWord()
{
    if(m_nState == STATE_SYM_IN_WORD)
    {
        m_nWordIndex = -1;
        m_sCompleteValue->clear();
        m_sCompleteValue->append("1");
        lWord = m_dict->getWordList(m_sCompleteValue);
    }

    if(lWord.size()==0)
        return;

    m_nWordIndex--;
    if(m_nWordIndex<0)
        m_nWordIndex=0;
    else if(!m_bKnowWord)
    {
        m_nWordIndex=lWord.size()-2;
        m_bKnowWord = true;
    }

    QString sChoosenWord = getWord(m_nWordIndex);
    if(!sChoosenWord.isEmpty())
        updateText(sChoosenWord);
}

QString fastsms::getWord(int nIndex)
{
    QString sWord;

    if(nIndex<lWord.length() && nIndex >= 0)
    {
        sWord = lWord.at(nIndex);
    }
    else
    {
        m_bKnowWord = false;
    }

    return sWord;
}

void fastsms::insertNewWord()
{
    m_dialNewWord = new QDialog(this);
    inputNewWord = new QLineEdit( m_dialNewWord );
    Button *ok = createButton(tr("Ok"),SLOT(newWordInserted()));
    QGridLayout *insWordLayout = new QGridLayout();
    insWordLayout->addWidget(inputNewWord,0,0);
    insWordLayout->addWidget(ok,1,0);
    inputNewWord->setFocus();

    m_dialNewWord->setWindowTitle("Type new word");
    m_dialNewWord->setLayout(insWordLayout);
    m_dialNewWord->show();
}

void fastsms::newWordInserted()
{
    QString *newWord = new QString(inputNewWord->text().toLower());
    if(!newWord->isEmpty())
    {
        qDebug() << "New word is:" << newWord->toAscii();
        m_dict->insertWord(newWord);
    }

    m_dialNewWord->hide();

    m_nState = STATE_WORD;
    updateText(newWord->toAscii());

    m_bKnowWord=true;

    this->setFocus();
}

void fastsms::updateText(QString sWord)
{
    disconnect(m_smsEdit,SIGNAL(selectionChanged()),0,0);
    qDebug() << "Selected text before insert word:" << m_cursorSmsText.selectedText().toAscii();
    m_cursorSmsText.insertText(sWord);
    qDebug() << "Selected text after insert word:" << m_cursorSmsText.selectedText().toAscii();
    connect(m_smsEdit,SIGNAL(selectionChanged()),this,SLOT(cursorPosChanged()));

    qDebug() << "Write sms:" << m_smsEdit->toPlainText();
}

void fastsms::deleteChar()
{
    if(m_phoneNumber->hasFocus())
    {
        m_phoneNumber->backspace();
    }
    else
    {
        m_nState = STATE_DEL;

        m_smsEdit->disconnect();
        m_cursorSmsText.clearSelection();
        m_cursorSmsText.deletePreviousChar();

        m_sCompleteValue = new QString(m_sCompleteValue->left(m_sCompleteValue->length()-1));
        smsChanged();
        connect(m_smsEdit,SIGNAL(textChanged()),this,SLOT(smsChanged()));
        connect(m_smsEdit,SIGNAL(selectionChanged()),this,SLOT(cursorPosChanged()));
    }
}

//void fastsms::openRubrica()
//{
//    //PhoneBook phoneBook;// = new PhoneBook();
//    //phoneBook.show();
//}

void fastsms::sendSMS()
{
    QMessageService service;
    QMessageManager manager;

    //Find right account for SMS: default one doesnt work well
    QMessageAccountIdList lstMsgAccountId = manager.queryAccounts();

    if(!lstMsgAccountId.isEmpty())
    {
        qDebug() << "SMS text" << m_smsEdit->toPlainText();
        for(int i = 0; i < lstMsgAccountId.count(); ++i)
        {
            QMessageAccount account(lstMsgAccountId[i]);
            bool isSmsAccount = (account.messageTypes() & QMessage::Sms) > 0;

            if(isSmsAccount)
            {
                QMessage myMessage;
                myMessage.setType(QMessage::Sms);
                myMessage.setParentAccountId(lstMsgAccountId[i]);
                myMessage.setTo(QMessageAddress(QMessageAddress::Phone, m_phoneNumber->text()));
                myMessage.setBody(m_smsEdit->toPlainText());

                qDebug() << "state:" << service.send(myMessage);
                break;
            }
        }
    }

    m_smsEdit->disconnect();

    m_nState = STATE_HOLD;
    //TODO: substitute smsEdit->clear(); with selectAll(). and manage C for all input text
    m_smsEdit->clear();
    m_sCompleteValue->clear();
    m_phoneNumber->clear();
    lWord.clear();
    m_nWordIndex=0;

    connect(m_smsEdit,SIGNAL(textChanged()),this,SLOT(smsChanged()));
    connect(m_smsEdit,SIGNAL(selectionChanged()),this,SLOT(cursorPosChanged()));
}

void fastsms::saveDictionaryOnExit()
{
    qDebug() << "Saving dictionary";
    m_dict->saveDictionaryToFile();
}

void fastsms::doNothing()
{

}

fastsms::~fastsms()
{
    //delete ui;
}

void fastsms::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        //ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

