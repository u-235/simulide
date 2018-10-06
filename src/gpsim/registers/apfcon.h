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

#ifndef __APFCON_H__
#define __APFCON_H__

#include "14bit-processors.h"
#include "14bit-tmrs.h"
#include "nco.h"
#include "clc.h"

class APFCON2;

class APFCON : public  sfr_register
{
    public:
        virtual void put(uint new_value);
        void set_pins(uint bit, PinModule *pin0, PinModule *pin1)
        {
            m_bitPin[0][bit] = pin0;
            m_bitPin[1][bit] = pin1;
        }
        void set_usart( USART_MODULE*_usart ) { m_usart = _usart;};
        void set_ssp( SSP1_MODULE*_ssp )      { m_ssp = _ssp;};
        void set_t1gcon( T1GCON*_t1gcon )     { m_t1gcon = _t1gcon;};
        void set_ccpcon( CCPCON*_ccpcon )     { m_ccpcon = _ccpcon;};
        void set_ValidBits( uint _mask )      {mValidBits = _mask;}

        APFCON(Processor *pCpu, const char *pName, const char *pDesc);

        private:

        friend APFCON2;
      
        PinModule	*m_bitPin[2][8];
        USART_MODULE 	*m_usart;
        SSP1_MODULE 	*m_ssp;
        T1GCON    	*m_t1gcon;
        CCPCON		*m_ccpcon;
};

    // APFCON for 16f1503
class APFCON2 : public APFCON
{
    public:

        APFCON2(Processor *pCpu, const char *pName, const char *pDesc) :
        APFCON(pCpu, pName, pDesc), m_nco(0), m_clc(0)
        {;}
        virtual void put(uint new_value);
        void set_CLC( CLC*_clc ) { m_clc = _clc;}
        void set_NCO( NCO*_nco ) { m_nco = _nco;}

    private:
        NCO *m_nco;
        CLC *m_clc;
};

#endif
