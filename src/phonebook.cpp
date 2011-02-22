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

#include "phonebook.h"
#include <QtGui>
#include <QContact>
#include <QContactManager>
#include <QContactPhoneNumber>

////using namespace QtMobility;
QTM_USE_NAMESPACE

PhoneBook::PhoneBook(QWidget *parent) :
    QWidget(parent)
{
    QStringList availableManagers = QContactManager::availableManagers();

    for(int managerIdx = 0; managerIdx < availableManagers.count(); managerIdx++) {
        QContactManager *manager = new QContactManager(availableManagers.at(managerIdx));

        if(manager) {
            QList<QContact> contacts = manager->contacts();

            for(int contactIdx =0; contactIdx < contacts.count() -1; contactIdx++)
            {
                QContact contact = contacts.at(contactIdx);
                QString label =contact.displayLabel(); // display label of contact
                //QList<QContactDetail> contactDetails = contact.details();

//                foreach (const QContactDetail& details, contactDetails)
//                {
//                    QList<QString> fieldKeys = details.value().keys();
//                    foreach (const QString& key, fieldKeys)
//                    {
//                        qDebug() << key;
//                        qDebug() << details.value(key);
//                        // key : give you the Field name
//                        // details.value(key) :  give you Field value
//                    }
//                }
                QString nameAndNumber(contact.displayLabel() + ": " + contact.detail(QContactPhoneNumber::DefinitionName).value(QContactPhoneNumber::FieldNumber));
                qDebug() << nameAndNumber;
            }
            delete manager;
        }
    }
}
