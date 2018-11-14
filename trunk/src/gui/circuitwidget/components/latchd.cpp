/***************************************************************************
 *   Copyright (C) 2016 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#include "latchd.h"
#include "circuit.h"
#include "pin.h"


Component* LatchD::construct( QObject* parent, QString type, QString id )
{
        return new LatchD( parent, type, id );
}

LibraryItem* LatchD::libraryItem()
{
    return new LibraryItem(
        tr( "Latch" ),
        tr( "Logic/Memory" ),
        "subc.png",
        "LatchD",
        LatchD::construct );
}

LatchD::LatchD( QObject* parent, QString type, QString id )
      : LogicComponent( parent, type, id )
      , eLatchD( id.toStdString() )
{
    m_width  = 4;
    m_height = 10;
    
    m_tristate = true;

    /*QStringList pinList;

    pinList // Inputs:
            << "IL01 D0"
            << "IL02 D1"
            << "IL03 D2"
            << "IL04 D3"
            << "IL05 D4"
            << "IL06 D5"
            << "IL07 D6"
            << "IL08 D7"

            << "IL09 IE"
            << "IR09OE"

            // Outputs:
            << "OR01O0 "
            << "OR02O1 "
            << "OR03O2 "
            << "OR04O3 "
            << "OR05O4 "
            << "OR06O5 "
            << "OR07O6 "
            << "OR08O7 "
            ;
    init( pinList );*/
    m_inputEnPin = new Pin( 180, QPoint( 0,0 ), m_id+"-Pin-inputEnable", 0, this );
    m_inputEnPin->setLabelText( " IE" );
    m_inputEnPin->setLabelColor( QColor( 0, 0, 0 ) );
    
    m_outEnPin   = new Pin(   0, QPoint( 0,0 ), m_id+"-Pin-outEnable"  , 0, this );
    m_outEnPin->setLabelText( "OE " );
    m_outEnPin->setLabelColor( QColor( 0, 0, 0 ) );

    eLogicDevice::createInEnablePin( m_inputEnPin );     // Input Enable
    eLogicDevice::createOutEnablePin( m_outEnPin );     // Output Enable

    setChannels( 8 );
}
LatchD::~LatchD(){}

void LatchD::setChannels( int channels )
{
    if( channels == m_channels ) return;
    if( channels < 1 ) return;
    m_channels = channels;

    m_height = channels+2;
    int origY = -(m_height/2)*8;

    LogicComponent::setNumOuts( channels );
    LogicComponent::setNumInps( channels );
    eLogicDevice::deleteOutputs( m_numOutputs );
    eLogicDevice::deleteInputs( m_numInputs );

    for( int i=0; i<channels; i++ )
    {
        QString number = QString::number(i);

        m_inPin[i] = new Pin( 180, QPoint(-24,origY+8+i*8 ), m_id+"-in"+number, i, this );
        m_inPin[i]->setLabelText( " D"+number );
        m_inPin[i]->setLabelColor( QColor( 0, 0, 0 ) );
        eLogicDevice::createInput( m_inPin[i] );

        m_outPin[i] = new Pin( 0, QPoint(24,origY+8+i*8 ), m_id+"-out"+number, i, this );
        m_outPin[i]->setLabelText( "O"+number+" " );
        m_outPin[i]->setLabelColor( QColor( 0, 0, 0 ) );
        eLogicDevice::createOutput( m_outPin[i] );
    }
    m_inputEnPin->setPos( QPoint(-24,origY+8+channels*8 ) );
    m_inputEnPin->setLabelPos();
    m_outEnPin->setPos( QPoint(24,origY+8+channels*8) );
    m_outEnPin->setLabelPos();

    m_area   = QRect( -(m_width/2)*8, origY, m_width*8, m_height*8 );
    Circuit::self()->update();
}

void LatchD::setTristate( bool t )
{
    if( !t ) 
    {
        if( m_outEnPin->isConnected() ) m_outEnPin->connector()->remove();
        m_outEnPin->reset();
        m_outEnPin->setLabelText( "" );
    }
    else m_outEnPin->setLabelText( "OE " );
    m_outEnPin->setVisible( t );
    m_tristate = t;
    eLogicDevice::updateOutEnabled();
}
#include "moc_latchd.cpp"
