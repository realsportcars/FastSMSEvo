/********************************************************************************
** Form generated from reading ui file 'dialogOptions.ui'
**
** Created: Mon Jun 14 01:51:53 2010
**      by: Qt User Interface Compiler version 4.5.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_DIALOGOPTIONS_H
#define UI_DIALOGOPTIONS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DialogOption
{
public:
    QWidget *gridLayoutWidget_3;
    QGridLayout *gridLayout_4;
    QSpacerItem *verticalSpacer;
    QPushButton *done;
    QLabel *label;
    QComboBox *cbLanguage;
    QRadioButton *orientationAuto;
    QRadioButton *orientationLandscape;
    QRadioButton *orientationPortrait;
    QLabel *label_2;

    void setupUi(QDialog *DialogOption)
    {
        if (DialogOption->objectName().isEmpty())
            DialogOption->setObjectName(QString::fromUtf8("DialogOption"));
        DialogOption->setWindowModality(Qt::NonModal);
        gridLayout_4 = new QGridLayout(DialogOption);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_4->addItem(verticalSpacer, 0, 3, 6, 1);

        done = new QPushButton();
        done->setObjectName(QString::fromUtf8("done"));

        gridLayout_4->addWidget(done, 6, 3, 1, 1);

        label = new QLabel();
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout_4->addWidget(label, 0, 1, 1, 1);

        cbLanguage = new QComboBox();
        cbLanguage->setObjectName(QString::fromUtf8("cbLanguage"));

        gridLayout_4->addWidget(cbLanguage, 1, 1, 1, 2);

        orientationAuto = new QRadioButton();
        orientationAuto->setObjectName(QString::fromUtf8("orientationAuto"));

        gridLayout_4->addWidget(orientationAuto, 4, 1, 1, 2);

        orientationLandscape = new QRadioButton();
        orientationLandscape->setObjectName(QString::fromUtf8("orientationLandscape"));

        gridLayout_4->addWidget(orientationLandscape, 5, 1, 1, 2);

        orientationPortrait = new QRadioButton();
        orientationPortrait->setObjectName(QString::fromUtf8("orientationPortrait"));

        gridLayout_4->addWidget(orientationPortrait, 6, 1, 1, 2);

        label_2 = new QLabel();
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout_4->addWidget(label_2, 3, 1, 1, 2);


        retranslateUi(DialogOption);

        QMetaObject::connectSlotsByName(DialogOption);
    } // setupUi

    void retranslateUi(QDialog *DialogOption)
    {
        DialogOption->setWindowTitle(QApplication::translate("DialogOption", "Options", 0, QApplication::UnicodeUTF8));
        done->setText(QApplication::translate("DialogOption", "Done", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DialogOption", "Input Language", 0, QApplication::UnicodeUTF8));
        orientationAuto->setText(QApplication::translate("DialogOption", "Autorotate", 0, QApplication::UnicodeUTF8));
        orientationLandscape->setText(QApplication::translate("DialogOption", "Landascape", 0, QApplication::UnicodeUTF8));
        orientationPortrait->setText(QApplication::translate("DialogOption", "Portrait", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DialogOption", "Orientation (need restart)", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(DialogOption);
    } // retranslateUi

};

namespace Ui {
    class DialogOption: public Ui_DialogOption {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGOPTIONS_H
