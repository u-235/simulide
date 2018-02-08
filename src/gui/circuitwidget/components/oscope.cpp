/***************************************************************************
 *   Copyright (C) 2018 by santiago González                               *
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

#include "oscope.h"
#include "connector.h"
#include "circuit.h"
#include "itemlibrary.h"
#include "circuitwidget.h"
#include "oscopewidget.h"

Component* Oscope::construct( QObject* parent, QString type, QString id )
{
    return new Oscope( parent, type, id );
}

LibraryItem* Oscope::libraryItem()
{
    return new LibraryItem(
        tr( "Oscope" ),
        tr( "Meters" ),
        "oscope.png",
        "Oscope",
        Oscope::construct );
}

Oscope::Oscope( QObject* parent, QString type, QString id )
    : Component( parent, type, id )
    , eElement( (id+"-eElement").toStdString() )
    , m_pinP( 180, QPoint(-120,0 ), id+"-PinP", 0, this )
    , m_pinN( 180, QPoint(-120,16), id+"-PinN", 0, this )
    , m_topW( )
{
    m_area = QRectF( -115, -65, 230, 130 );
    
    m_ePin.resize(2);
    m_ePin[0] = &m_pinP;
    m_ePin[1] = &m_pinP;
    
    m_pinP.setLabelText( "+" );
    m_pinN.setLabelText( "_" );
    m_pinP.setLabelColor( QColor( 0, 0, 0 ) );
    m_pinN.setLabelColor( QColor( 0, 0, 0 ) );
    m_pinP.setLength( 5 );
    m_pinN.setLength( 5 );
    
    m_oscopeW = new OscopeWidget( &m_topW );
    m_oscopeW->setupWidget( 116 );
    m_oscopeW->setFixedSize( 220, 120 );
    m_oscopeW->setVisible( true );
    m_topW.setupWidget( m_oscopeW );
    
    m_proxy = Circuit::self()->addWidget( &m_topW);
    m_proxy->setParentItem( this );
    m_proxy->setPos( QPoint( -110, -60) );
    //m_proxy->setFlag(QGraphicsItem::ItemNegativeZStacksBehindParent, true );
}

Oscope::~Oscope() 
{
}

void Oscope::initialize()
{
    m_oscopeW->setOscope( this );
    //m_changed = true;
    //updateStep();
}

double Oscope::getVolt()
{
    //qDebug() <<m_pinP.getVolt() - m_pinN.getVolt();
    return m_pinP.getVolt() - m_pinN.getVolt();
}

void Oscope::remove()
{    
    Simulator::self()->remFromUpdateList( this );
    
    Component::remove();
}

void Oscope::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );
    
    p->setBrush( Qt::darkGray );
    p->drawRoundedRect( m_area, 4, 4 );
    
    p->setBrush( Qt::white );
    QPen pen = p->pen();
    pen.setWidth( 0 );
    pen.setColor( Qt::white );
    p->setPen(pen);
    
    p->drawRoundedRect( QRectF( -114, -64, 225, 125 ), 3, 3 );

}

#include "moc_oscope.cpp"


