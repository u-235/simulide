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
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#include "e-bincounter.h"


eBinCounter::eBinCounter( std::string id )
          : eLogicDevice( id )
{
    m_TopValue = 1;
}
eBinCounter::~eBinCounter() {}

void eBinCounter::initialize()
{
    m_Counter = 0;
    eNode* enode = m_input[0]->getEpin()->getEnode();              // Reset pin
    if( enode ) enode->addToChangedFast(this);
    
    eLogicDevice::initialize();
    
    eLogicDevice::setOut( 0, false );                              // Q
}

void eBinCounter::createPins()
{
    createClockPin();
    eLogicDevice::createPins( 1, 1 );          // Create Inputs, Outputs

    // Input 0 - Reset
    
    // Output 0 - Q
}

void eBinCounter::setVChanged()
{
    bool clkRising = (eLogicDevice::getClockState() == Rising);
    
    if( eLogicDevice::getInputState( 0 ) == false ) // Reset
    {
       m_Counter = 0;
       eLogicDevice::setOut( 0, false );
    }
    else if( clkRising )
    {
        m_Counter++;

        if( m_Counter == m_TopValue )
        {
            eLogicDevice::setOut( 0, true );
        }
        else if ( m_Counter > m_TopValue )
        {
            eLogicDevice::setOut( 0, false );
            m_Counter = 0;
        }
    }
}

int eBinCounter::TopValue() const
{
    return m_TopValue;
}

void eBinCounter::setTopValue( int TopValue )
{
    m_TopValue = TopValue;
}
