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

#include <QtGui>
#include "fastsms.h"
#include <QInputContext>

//#ifdef Q_WS_HILDON
//#include <QInputContext>
//#endif

//Copy this from fastsms.cpp
#define SETTING_LANGUAGE "language"
#define SETTING_ORIENTATION "orientation"

#define ORIENTATION_AUTO "auto"
#define ORIENTATION_LANDSCAPE "landscape"
#define ORIENTATION_PORTRAIT "portrait"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setAutoSipEnabled(false);

    fastsms w;

#if defined(Q_WS_MAEMO_5)

    //Manage Orientation for Maemo
    QSettings *settings = new QSettings("AimoneDavide","fastsms");
    QString sOrientation = settings->value(SETTING_ORIENTATION).toString();

    if(sOrientation.compare(ORIENTATION_AUTO) == 0)
        w.setAttribute(Qt::WA_Maemo5AutoOrientation, true);
    else if(sOrientation.compare(ORIENTATION_PORTRAIT) == 0)
        w.setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
    else if(sOrientation.compare(ORIENTATION_LANDSCAPE) == 0)
        w.setAttribute(Qt::WA_Maemo5LandscapeOrientation, true);


#endif

    a.connect(&a, SIGNAL(aboutToQuit()), &w, SLOT(saveDictionaryOnExit()));

#if defined(Q_WS_S60)
    w.showMaximized();
#else
    w.show();    
#endif

    return a.exec();
}
