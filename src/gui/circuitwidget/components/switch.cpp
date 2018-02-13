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

#include "switch.h"
#include "itemlibrary.h"


Component* Switch::construct( QObject* parent, QString type, QString id )
{ return new Switch( parent, type, id ); }

LibraryItem* Switch::libraryItem()
{
    return new LibraryItem(
            tr( "Switch" ),
            tr( "Switches" ),
            "switch.png",
            "Switch",
            Switch::construct);
}

Switch::Switch( QObject* parent, QString type, QString id )
    : SwitchBase( parent, type, id )
{
    connect( m_button, SIGNAL( clicked() ),
             this,     SLOT  ( onbuttonclicked() ));
}
Switch::~Switch(){}

void Switch::onbuttonclicked()
{
    m_closed = !m_closed;
    m_changed = true;
}

void Switch::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );
    
    QPen pen = p->pen();
    pen.setWidth(3);
    p->setPen(pen);

    if( m_closed ) p->drawLine(-10, 0, 10, -2 );
    else           p->drawLine(-10.5, 0, 8, -8 );
}

#include "moc_switch.cpp"
