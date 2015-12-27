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

#include "gate_xor.h"
#include "itemlibrary.h"


Component* XorGate::construct( QObject* parent, QString type, QString id )
{
        return new XorGate( parent, type, id );
}

LibraryItem* XorGate::libraryItem()
{
    return new LibraryItem(
        tr( "Xor Gate" ),
        tr( "Gates" ),
        "xorgate.png",
        "Xor Gate",
        XorGate::construct );
}

XorGate::XorGate( QObject* parent, QString type, QString id )
        : Gate( parent, type, id, 2 )
{
}
XorGate::~XorGate(){}

bool XorGate::calcOutput( int inputs )
{
    if( inputs == 1 )  return true;

    return false;
}

void XorGate::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    int y_orig = this->boundingRect().y();
    int height = this->boundingRect().height();

    // Paint white background of gate
    Component::paint( p, option, widget );
    QPen pen = p->pen();
    
    p->setPen( Qt::NoPen );

    QPainterPath path;
    path.moveTo( -8, 0 );
    path.arcTo( -42, y_orig, 58, height, -90, 180 );

    path.moveTo( -8, 0 );
    path.arcTo( -18, y_orig, 10, height, -90, 180 );

    p->drawPath(path);

    // Draw curves
    //Component::paint( p, option, widget );
    
    pen.setWidth(2);
    p->setPen(pen);
    
    p->setBrush( Qt::NoBrush );

    // Output side arc
    p->drawArc( -38, y_orig, 54, height, -1520/*-16*95*/, 3040/*16*190*/ );

    // Input side arc
    p->drawArc( -18, y_orig, 10, height, -1440/*-16*90*/, 2880/*16*180*/ );

    // Input side arc close to pins
    p->drawArc( -24, y_orig, 9, height, -1440/*-16*90*/, 2880/*16*180*/ );
    
    Gate::paint( p, option, widget );            // draw inverted circle
}

#include "moc_gate_xor.cpp"


