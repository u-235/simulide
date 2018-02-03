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

#ifndef WAVEGEN_H
#define WAVEGEN_H

#include "clock-base.h"
#include "itemlibrary.h"
#include <QObject>

class MAINMODULE_EXPORT WaveGen : public ClockBase
{
    Q_OBJECT
    Q_PROPERTY( wave_type Wave_Type  READ waveType      WRITE setWaveType      DESIGNABLE true USER true )
    Q_ENUMS( wave_type )
    
    public:

        WaveGen( QObject* parent, QString type, QString id );
        ~WaveGen();
        
        enum wave_type {
            Sine = 0,
            Saw,
            Triangle,
            Square
        };

        static Component* construct( QObject* parent, QString type, QString id );
        static LibraryItem *libraryItem();
        
        wave_type waveType()              { return m_type; }
        void setWaveType( wave_type typ ) { m_type = typ; }
        
        virtual void updateStep();
        virtual void simuClockStep();

        virtual void paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget );
        
    private:
        void genSine();
        void genSaw();
        void genTriangle();
        void genSquare();
        
        wave_type m_type;
        double m_vOut;
};

#endif
