/*
   Copyright (C) 2013,2014,2017 Roy R. Rankin

This file is part of the libgpsim library of gpsim

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, see
<http://www.gnu.org/licenses/lgpl-2.1.html>.
*/
/****************************************************************
*                                                               *
*  Modified 2018 by Santiago Gonzalez    santigoro@gmail.com    *
*                                                               *
*****************************************************************/

#include "apfcon.h"


APFCON::APFCON(Processor *pCpu, const char *pName, const char *pDesc)
    : sfr_register(pCpu,pName,pDesc),
        m_usart(0), m_ssp(0), m_t1gcon(0)
{
    int j;
    mValidBits=0xef;
    for(j =0; j <8; j++)
    {
        m_bitPin[0][j] = NULL;
        m_bitPin[1][j] = NULL;
    }
}

void APFCON::put(uint new_value)
{
    uint old_value = value.get();
    uint diff = (new_value ^ old_value) & mValidBits;

    new_value &= mValidBits;
    value.put(new_value);

    for(int i = 0; i < 8; i++)
    {
        uint bit = 1<<i;
        if(diff & bit)
        {

            if (m_bitPin[(new_value & bit)== bit][i] == 0)
            {
                fprintf(stderr, "APFCON::put File bug report m_bitPin[%d][%d] not set\n", (new_value & bit)== bit, i);
                assert(m_bitPin[(new_value & bit)== bit][i]);
            }

            switch(i)
            {
            case 0:
                assert(m_ccpcon);
                m_ccpcon->setIOPin1(m_bitPin[(new_value & bit)== bit][i]);
                break;

            case 1:
                assert(m_ccpcon);
                m_ccpcon->setIOPin2(m_bitPin[(new_value & bit)== bit][i]);
                break;

            case 2:
                assert(m_usart);
                m_usart->set_TXpin(m_bitPin[(new_value & bit)== bit][i]);
                break;

            case 3:
                assert(m_t1gcon);
                m_t1gcon->setGatepin(m_bitPin[(new_value & bit)== bit][i]);
                break;

            case 4:        // not used
                break;

            case 5:
                assert(m_ssp);
                m_ssp->set_ssPin(m_bitPin[(new_value & bit)== bit][i]);
                break;

            case 6:
                assert(m_ssp);
                m_ssp->set_sdoPin(m_bitPin[(new_value & bit)== bit][i]);
                break;

            case 7:
                assert(m_usart);
                m_usart->set_RXpin(m_bitPin[(new_value & bit)== bit][i]);
                break;
            }
        }
    }
}

// Alternate pin function for 16f1503
void APFCON2::put(uint new_value)
{
    uint old_value = value.get();
    uint diff = (new_value ^ old_value) & mValidBits;

    new_value &= mValidBits;
    value.put(new_value);

    for(int i = 0; i < 8; i++)
    {
        uint bit = 1<<i;
        if( diff & bit )
        {

            if (m_bitPin[(new_value & bit)== bit][i] == 0)
            {
                fprintf(stderr, "APFCON2::put File bug report m_bitPin[%d][%d] not set\n", (new_value & bit)== bit, i);
                assert(m_bitPin[(new_value & bit)== bit][i]);
            }

            switch(i)
            {
            case 0:
                assert(m_nco);
                m_nco->setNCOxPin(m_bitPin[(new_value & bit)== bit][i]);
                break;

            case 1:
                assert(m_clc);
                m_clc->setCLCxPin(m_bitPin[(new_value & bit)== bit][i]);
                break;

            case 2:         // not used
                break;

            case 3:
                assert(m_t1gcon);
                m_t1gcon->setGatepin(m_bitPin[(new_value & bit)== bit][i]);
                break;

            case 4:
                assert(m_ssp);
                m_ssp->set_ssPin(m_bitPin[(new_value & bit)== bit][i]);
                break;

            case 5:
                assert(m_ssp);
                m_ssp->set_sdoPin(m_bitPin[(new_value & bit)== bit][i]);
                break;

            case 6:        // not used
            case 7:
                break;
            }
        }
    }
}
