/***************************************************************************
 *   Copyright (C) 2017 by santiago González                               *
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

#include "servo.h"
#include "simulator.h"


Component* Servo::construct( QObject* parent, QString type, QString id )
{
    return new Servo( parent, type, id );
}

LibraryItem* Servo::libraryItem()
{
    return new LibraryItem(
        tr( "Servo" ),
        tr( "Outputs" ),
        "servo.png",
        "Servo",
        Servo::construct );
}

Servo::Servo( QObject* parent, QString type, QString id )
     : LogicComponent( parent, type, id )
     , eLogicDevice( id.toStdString() )
{
    m_width  = 10;
    m_height = 6;
    
    QStringList pinList;

    pinList // Inputs:
            << "IL01 V+"
            << "IL03 Gnd"
            << "IL05 Sig"
            ;
    init( pinList );

    for( int i=0; i<3;i++ ) m_inPin[i]->setLabelColor( QColor( 250, 250, 200 ) );

    eLogicDevice::createInput( m_inPin[0] );                       // V+
    eLogicDevice::createInput( m_inPin[1] );                      // Gnd
    eLogicDevice::createClockPin( m_inPin[2] );           // Input Clock

    m_pos = 90;
    m_targetPos = 90;
    m_pulseStart = 0;
    m_speed = 0.2;

    Simulator::self()->addToUpdateList( this );
}
Servo::~Servo(){}

void Servo::initialize()
{
    if( m_inPin[0]->isConnected()
      & m_inPin[1]->isConnected()
      & m_inPin[2]->isConnected() )
    {
        eNode* enode = m_input[0]->getEpin()->getEnode();         // Gnd pin
        if( enode ) enode->addToChangedFast(this);

        enode = m_input[1]->getEpin()->getEnode();                 // V+ pin
        if( enode ) enode->addToChangedFast(this);

        eLogicDevice::initialize();
    }
}

void Servo::resetState()
{
    m_targetPos = 90;
    m_pulseStart = 0;
    
    eLogicDevice::resetState();
}

void Servo::updateStep()
{
    unsigned long long step = Simulator::self()->step();

    if( m_targetPos != m_pos )
    {
        double updateTime = (step - m_lastUpdate)/1e6;

        int maxMove  = updateTime/m_speed*60; // Maximum to move since last update
        int deltaPos = m_targetPos - m_pos;
        int absDeltaPos = abs(deltaPos);

        if( absDeltaPos > maxMove )
            deltaPos = absDeltaPos/deltaPos*maxMove;      // keep sign of deltaPos
        m_pos += deltaPos;
    }
    m_lastUpdate = step;
    update();
}

void Servo::setVChanged()
{
    int clkState = eLogicDevice::getClockState();
    
    if(!(eLogicDevice::getInputState(0)-eLogicDevice::getInputState(1)))// not power
    {
        m_targetPos = 90;
        m_pulseStart = 0;
    }
    else if( clkState == Rising )
    {
        m_pulseStart = Simulator::self()->step();
    }
    else if( clkState == Falling )
    {
        if( m_pulseStart == 0 ) return;
        
        int steps = Simulator::self()->step() - m_pulseStart;
        
        m_targetPos = (steps-1000)*180/1000;         // Map 1mS-2mS to 0-180ª

        if     ( m_targetPos>180 ) m_targetPos = 180;
        else if( m_targetPos<0 )   m_targetPos = 0;
        
        m_pulseStart = 0;
        //qDebug() << "Servo::setVChanged() m_targetPos" << m_targetPos;
    }
}

void Servo::remove()
{
    if( m_inPin[0]->isConnected() ) m_inPin[0]->connector()->remove();
    if( m_inPin[1]->isConnected() ) m_inPin[1]->connector()->remove();
    if( m_inPin[2]->isConnected() ) m_inPin[2]->connector()->remove();

    Simulator::self()->remFromUpdateList( this );

    Component::remove();
}

void Servo::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );

    p->setBrush( QColor(50, 70, 100) );
    p->drawRoundedRect( m_area, 4, 4 );

    //p->setBrush( QColor(50, 70, 100) );

    QPen pen = p->pen();
    pen.setColor( Qt::white);;
    p->setPen(pen);
    p->drawEllipse( 0,-16, 32, 32 );

    pen.setColor( Qt::black);;
    p->setPen(pen);
    p->setBrush( QColor( 255, 255, 255) );
    p->save();
    p->translate( 16, 0);
    p->rotate( m_pos-90 );
    p->drawRoundedRect( -8, -8, 48, 16, 8, 8 );
    p->restore();
}

#include "moc_servo.cpp"
