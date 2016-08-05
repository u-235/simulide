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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 ***************************************************************************/
#ifndef NO_PIC
#include "gpsimprocessor.h"
#include "simulator.h"
#include "utils.h"

// GpSim includes
#include "gpsim/cod.h"
#include "gpsim/interface.h"
#include "gpsim/gpsim_classes.h"
#include "gpsim/pic-processor.h"

GpsimProcessor* GpsimProcessor::m_pSelf = 0l;

GpsimProcessor::GpsimProcessor() : BaseProcessor()
{
    m_pSelf = this;
    m_pPicProcessor = 0l;
    m_loadStatus    = false;

    if (!m_DoneGpsimInit)
    {
        qDebug() << "\n        Initializing GpSim...\n";
        int init = initialize_gpsim_core();
        initialization_is_complete();
        m_DoneGpsimInit = (init==0);
        
        if( m_DoneGpsimInit ) 
            qDebug() << "\n    ...GpSim Initialization is complete\n\n";
        else
            qDebug() << "\n    ...Error!!! GpSim Initialization Failed!!!\n\n";
    }
}
GpsimProcessor::~GpsimProcessor(){}

void GpsimProcessor::terminate()
{
    //qDebug() << "GpsimProcessor::terminate";
    BaseProcessor::terminate();
    if( m_pPicProcessor ) 
    {
        delete m_pPicProcessor;//reset(EXIT_RESET);
    }
    m_pPicProcessor = 0l;
}

bool GpsimProcessor::loadFirmware( QString fileN )
{
    if( fileN == "" ) return false;
    
    m_symbolFile = fileN.replace( fileN.split(".").last(), "cod" );
    
    //qDebug() << m_device;

    if( !QFile::exists(m_symbolFile) )
    {
        QMessageBox::warning( 0, tr("File Not Found"), tr("The cod file \"%1\" was not found.").arg(m_symbolFile) );
        return false;
    }
    
    if( m_pPicProcessor )
    {
        m_symbolFile = fileN.replace( fileN.split(".").last(), "hex" );
        const char *fileName = m_symbolFile.toLatin1();
        FILE       *pFile    = fopen( fileName, "r" );
        const char *procName = m_device.toLatin1();
        m_loadStatus = m_pPicProcessor->LoadProgramFile( fileName, pFile, procName );
    }
    else
    {
        const char *fileName = m_symbolFile.toLatin1();
        FILE       *pFile    = fopen( fileName, "r" );
        Processor  *tempProc = 0l;
        qDebug() << m_symbolFile << fileName;
        m_loadStatus = ( ProgramFileTypeList::GetList().LoadProgramFile( &tempProc, fileName, pFile ) );
        m_pPicProcessor = dynamic_cast<pic_processor*>(tempProc);
    }

    /*m_pPicProcessor->finish();
    const char *procName = m_device.toLatin1();
    m_loadStatus = m_pPicProcessor->LoadProgramFile( fileName, pFile, procName );*/

    if( !m_loadStatus )
    {
        //QMessageBox::warning( 0, tr("Unkown Error:"),  tr("Could not Load: \"%1\"").arg(m_symbolFile) );
        return false;
    }
    QString name = m_pPicProcessor->name().c_str();
    qDebug() << "GpsimProcessor::loadFirmware      Device: " << name;

    //if ( device.split("-").first().toUpper() == m_device )
    {
        //if( needAtach ) attachPins();
        //m_ready = m_loadStatus && m_haveMclr && m_haveVdd;
        //reset();
    }
    m_lastTrmtBit = 0; // for usart
    
    initialized();
    
    return true;
    /*else
    {
        QMessageBox::warning( 0, tr("Device Type Error"),
                                 tr("ERROR: The cod file \"%1\"\nIs for %2.\nCannot load to %3.")
                                 .arg(m_symbolFile).arg(device).arg(m_device) );
    }*/
    //qDebug() << "mem: " << m_pPicProcessor->rma[329].get_value(); ******** get RAM value *********
    //m_pPicProcessor->list(2, 0, 0, 100);
    //qDebug() << m_pPicProcessor->pma->get_src_line(5);
    //m_pPicProcessor->pma->get_PC();
    //m_pPicProcessor->pma->address_has_break(address);
    //find_address_from_line(FileContext *fc, int src_line)
    //m_pPicProcessor->pma->set_break_at_line(0,40);
    //m_pPicProcessor->run(false);
    //int cpupc = m_pPicProcessor->pc->get_value();
    //m_pPicProcessor->pma->find_closest_address_to_line(0,line);
    //m_pPicProcessor->pma->find_address_from_line(0, line);
    //qDebug() << "PC: " << cpupc << "line:" << m_pPicProcessor->pma->get_src_line(cpupc);
}

void GpsimProcessor::step()                 // Run 1 step 
{
    //if( !m_loadStatus ) return 0;       // No program loaded
    if( m_usartTerm ) readUsart();
    m_pPicProcessor->step_cycle();
    
}

void GpsimProcessor::reset()
{
    if( !m_loadStatus ) return;
    m_pPicProcessor->reset( POR_RESET ); // POR_RESET  MCLR_RESET EXIT_RESET
    BaseProcessor::reset();
}

int GpsimProcessor::getRamValue( QString name ) // Returns the value stored in Ram address
{
    if( m_regsTable.isEmpty() ) return -1;

    bool isNumber = false;
    int address = name.toInt( &isNumber );  // Try to convert to integer

    if( !isNumber ) address = m_regsTable[name];   // Is a register name
    
    int value = m_pPicProcessor->rma[address].get_value();

    return value;
}

QHash<QString, int>  GpsimProcessor::getRegsTable( QString lstFileName )// get register addresses from lst file
{
    QHash<QString, int> regsTable;

    QStringList lineList = fileToStringList( lstFileName, "GpsimProcessor::setRegisters" );

    if( !regsTable.isEmpty() ) regsTable.clear();

    foreach( QString line, lineList )
    {
        line = line.toLower().replace("\t"," ").replace("="," ");
        if( line.contains("equ ") )      // This line contains a definition
        {
            QString name    = "";
            QString addrtxt = "";
            int address   = 0;
            bool isNumber = false;

            line.remove("equ");
            QStringList wordList = line.split(QRegExp("\\s+")); // Split in words
            while( name.isEmpty() ) name = wordList.takeFirst();

            while( addrtxt.isEmpty() ) addrtxt = wordList.takeFirst();

            int base = 10;

            address = addrtxt.toInt( &isNumber, base );

            if( isNumber ) regsTable.insert(name, address); // If found a valid address add to map
            //qDebug() << name << address<<"\n";
        }
    }
    return regsTable;
}

void GpsimProcessor::readUsart()
{
static int txreg;
static int txreg0;

    int txstaReg = getRamValue( "txsta" );
    if( !(txstaReg & 32) ) return;                           // TXEN bit
    
    //int pir1Reg = getRamValue( "pir1" );
    //bool txifBit = pir1Reg & 16;
    
    bool trmtBit = txstaReg & 2;
    
    txreg = getRamValue( "txreg" );
    
    //txreg = getRamValue( "txreg" );
    QString text = "";
    if( txreg != txreg0  )
    {
        if( !m_lastTrmtBit & !trmtBit ) text.append( txreg );
        txreg0 = txreg;
        //qDebug() << "GpsimProcessor::readUsart:.... " << text;
    }
    if( m_lastTrmtBit != trmtBit ) 
    {
        
        if(  m_lastTrmtBit & !trmtBit ) text.append( txreg );
        m_lastTrmtBit = trmtBit;
        //qDebug() << "GpsimProcessor::readUsart: " << text;
    }
    if( text !="" ) 
    {
        OutPanelText::self()->appendText( text );
        //qDebug() << "GpsimProcessor::readUsart: " << text << " txifBit " << txifBit;
        //qDebug() << m_lastTrmtBit << trmtBit;
    }
}

#include "moc_gpsimprocessor.cpp"
#endif


