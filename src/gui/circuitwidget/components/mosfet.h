/***************************************************************************
 *   Copyright (C) 2016 by santiago González                               *
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

#ifndef MOSFET_H
#define MOSFET_H

#include "pin.h"
#include "e-mosfet.h"
#include "e-source.h"

class LibraryItem;

class MAINMODULE_EXPORT Mosfet : public Component, public eMosfet
{
    Q_OBJECT
    Q_PROPERTY( double RDSon     READ RDSon      WRITE setRDSon     DESIGNABLE true USER true )
    Q_PROPERTY( double Threshold READ threshold  WRITE setThreshold DESIGNABLE true USER true )
    Q_PROPERTY( bool   P_Channel READ pChannel   WRITE setPchannel  DESIGNABLE true USER true )
    public:

        Mosfet( QObject* parent, QString type, QString id );
        ~Mosfet();

        static Component* construct( QObject* parent, QString type, QString id );
        static LibraryItem* libraryItem();
        
        double RDSon()                { return m_RDSon; }
        void  setRDSon( double rdson ){ m_RDSon = rdson; }
        
        virtual void paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget );
        
    public slots:
        void remove();
};

#endif
