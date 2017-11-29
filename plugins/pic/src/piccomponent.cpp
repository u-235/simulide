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


#include "piccomponent.h"
#include "piccomponentpin.h"
#include "mainwindow.h"
#include "utils.h"

LibraryItem* PICComponent::libraryItem()
{
    return new LibraryItem(
        tr("PIC"),
        tr("Micro"),
        "ic2.png",
        "PIC",
    PICComponent::construct );
}

Component* PICComponent::construct( QObject* parent, QString type, QString id )
{
    if( m_canCreate ) 
    {
        PICComponent* pic = new PICComponent( parent, type,  id );
        if( m_error > 0 )
        {
            Circuit::self()->compList()->removeOne( pic );
            pic->deleteLater();
            pic = 0l;
            m_error = 0;
            m_canCreate = true;
        }
        return pic;
    }
    MessageBoxNB( tr("Error"), tr("Only 1 Mcu allowed\n to be in the Circuit.") );

    return 0l;
}

PICComponent::PICComponent( QObject* parent, QString type, QString id )
    : McuComponent( parent, type, id )
{
    m_pSelf = this;
    m_dataFile = "pics.xml";
    m_processor = PicProcessor::self();

    //if( m_id.startsWith("PIC") ) m_id.replace( "PIC", "pic16f876" );

    initPackage();
    if( m_error == 0 )
    {
        setFreq( 20 );
        
        qDebug() <<"     ..."<<m_id<<"OK\n";
    }
    else
    {
        qDebug() <<"     ..."<<m_id<<"Error!!!\n";
    }
}
PICComponent::~PICComponent() { }

void PICComponent::attachPins()
{
    PicProcessor* ap = dynamic_cast<PicProcessor*>( m_processor );
    pic_processor* cpu = ap->getCpu();

    for( int i=0; i < m_numpins; i++ )
    {
        PICComponentPin* pin = dynamic_cast<PICComponentPin*>( m_pinList[i] );
        pin->attach( cpu );
    }
    m_attached = true;
}

void PICComponent::addPin( QString id, QString type, QString label, int pos, int xpos, int ypos, int angle )
{
    m_pinList.append( new PICComponentPin( this, id, type, label, pos, xpos, ypos, angle ) );
}

void PICComponent::setFreq( int freq )
{ 
    if     ( freq < 0  ) freq = 0;
    else if( freq > 100 ) freq = 100;
    
    Simulator::self()->setMcuClock( freq/4 );
    m_freq = freq; 
}

#include "moc_piccomponent.cpp"


