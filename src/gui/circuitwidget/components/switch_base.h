/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#ifndef SWITCH_BASE_H
#define SWITCH_BASE_H

#include "e-element.h"
#include "pin.h"

class MAINMODULE_EXPORT SwitchBase : public Component, public eElement
{
    Q_OBJECT

    public:

        SwitchBase( QObject* parent, QString type, QString id );
        ~SwitchBase();

        void updateStep();

        virtual void initialize();

    public slots:
        void remove();

    protected:
        bool m_changed;
        bool m_closed;
        
        QPushButton          *m_button;
        QGraphicsProxyWidget *m_proxy;
};

#endif
