/***************************************************************************
 *   Copyright (C) 2010 by santiago González                               *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "gate_and.h"
#include "itemlibrary.h"

Component* AndGate::construct( QObject* parent, QString type, QString id )
{
    return new AndGate( parent, type, id );
}

LibraryItem* AndGate::libraryItem()
{
    return new LibraryItem(
        tr( "And Gate" ),
        tr( "Logic" ),
        "andgate.png",
        "And Gate",
        AndGate::construct );
}

AndGate::AndGate( QObject* parent, QString type, QString id )
       : Gate( parent, type, id, 2 )
{
}
AndGate::~AndGate(){}

void AndGate::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );

    QPen pen = p->pen();
    pen.setWidth(2);
    p->setPen(pen);

    p->drawChord( -48, boundingRect().y(), 64, boundingRect().height(), -1440/*-16*90*/, 2880/*16*180*/ );
}

#include "moc_gate_and.cpp"
