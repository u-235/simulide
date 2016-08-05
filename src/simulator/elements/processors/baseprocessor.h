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

#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QtGui>

#include "ramtable.h"
#include "outpaneltext.h"

class BaseProcessor : public QObject
{
    Q_OBJECT
    public:
        BaseProcessor( QObject* parent=0 );
        ~BaseProcessor();

        QString getFileName();

        virtual void    setDevice( QString device );
        virtual QString getDevice();
        
        virtual void setDataFile( QString datafile );

        virtual bool loadFirmware( QString file )=0;
        virtual bool getLoadStatus() { return m_loadStatus; }
        virtual void terminate();

        virtual void step()=0;
        virtual void reset();

        virtual int getRamValue( QString name )=0;
        virtual int getRegAddress( QString name );
        //virtual QStringList getDefsList( QString fileName );
        
        virtual void setUsart( bool usart ) { m_usartTerm = usart; }
        
        virtual void initialized();

    protected:
        
        virtual void setRegisters();
        virtual QHash<QString, int> getRegsTable( QString lstFileName )=0;

        QString   m_symbolFile;
        QString   m_dataFile;
        QString   m_device;

        RamTable *m_ramTable;
        QHash<QString, int> m_regsTable;

        bool m_loadStatus;
        bool m_usartTerm;
};

#endif

