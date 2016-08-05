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

#include "avrprocessor.h"
#include "simulator.h"
#include "utils.h"

// simavr includes
#include "sim_elf.h"
#include "sim_hex.h"
#include "sim_core.h"
#include "avr_uart.h"

AvrProcessor* AvrProcessor::m_pSelf = 0l;

AvrProcessor::AvrProcessor( QObject* parent ) : BaseProcessor( parent )
{
    m_pSelf = this;
    m_avrProcessor = 0l;
}
AvrProcessor::~AvrProcessor() {}

void AvrProcessor::terminate()
{
    BaseProcessor::terminate();
    if( m_avrProcessor ) avr_terminate( m_avrProcessor );
    m_avrProcessor = 0l;
}

bool AvrProcessor::loadFirmware( QString fileN )
{
    if ( fileN == "" ) return false;

    if( m_avrProcessor ) terminate();

    fileN.replace( fileN.split(".").last(), "hex" );

    if( !QFile::exists(fileN) )     // File not found
    {
        QMessageBox::warning( 0, tr("File Not Found"),
        tr("The file \"%1\" was not found.").arg(fileN) );

        return false;
    }

    uint32_t loadBase = AVR_SEGMENT_OFFSET_FLASH;
    char name[16] = "";
    elf_firmware_t f = {{0}};

    //f.frequency = 16000000;

    strcpy( name, m_device.toLatin1() );

    char filename[200]="";
    strcpy( filename, fileN.toLatin1() );

    if( fileN.endsWith("hex") )
    {
        //struct ihex_chunk_t chunk[4];
        //int cnt = read_ihex_chunks(filename, chunk, 4);

        ihex_chunk_p chunk = NULL;
        int cnt = read_ihex_chunks(filename, &chunk);

        if (cnt <= 0) 
        {
            QMessageBox::warning(0,"Error:",
            tr(" Unable to load IHEX file %1\n").arg(fileN) );
            return false;
        }
        for (int ci = 0; ci < cnt; ci++) 
        {
            if (chunk[ci].baseaddr < (1*1024*1024))
            {
                f.flash = chunk[ci].data;
                f.flashsize = chunk[ci].size;
                f.flashbase = chunk[ci].baseaddr;
            } 
            else if (chunk[ci].baseaddr >= AVR_SEGMENT_OFFSET_EEPROM ||
                    chunk[ci].baseaddr + loadBase >= AVR_SEGMENT_OFFSET_EEPROM) 
            {
                f.eeprom = chunk[ci].data;
                f.eesize = chunk[ci].size;
            }
        }
    }
    //else if ( fileN.endsWith("axf") ) elf_read_firmware(filename, &f);
    else                                    // File extension not valid
    {
        QMessageBox::warning(0,"Error:", tr("%1 should be .hex \n").arg(fileN) );
        return false;
    }

    if( strlen(name) ) strcpy( f.mmcu, name );
    m_avrProcessor = avr_make_mcu_by_name(f.mmcu);

    if( !m_avrProcessor )
    {
        QMessageBox::warning(0,"Error:",
        tr("The firmware is not for %1 \n").arg(f.mmcu) );
        return false;
    }
    int started = avr_init( m_avrProcessor );
    qDebug() << "\nAvrProcessor::loadFirmware Avr Init: "<< name << (started==0);
    avr_load_firmware( m_avrProcessor, &f );

    if( f.flashbase ) m_avrProcessor->pc = f.flashbase;

    m_avrProcessor->frequency = 16000000;
    m_symbolFile = fileN;
    
    // Usart interface
    avr_irq_t* src = avr_io_getirq(m_avrProcessor, AVR_IOCTL_UART_GETIRQ('0'), UART_IRQ_OUTPUT);
    avr_irq_register_notify(src, uart_pty_in_hook, this);
    
    initialized();
    
    return true;
}

void AvrProcessor::step()
{ 
    //avr_run( m_avrProcessor );
    m_avrProcessor->run(m_avrProcessor);
}

void AvrProcessor::reset()
{
    if( !m_loadStatus ) return;
    avr_reset( m_avrProcessor );
    BaseProcessor::reset();
}

int AvrProcessor::getRamValue( QString name )
{
    if( m_regsTable.isEmpty() ) return -1;

    bool isNumber = false;
    int address = name.toInt( &isNumber );      // Try to convert to integer

    if( !isNumber ) {address = m_regsTable[name];  /* Is a register name*/}
    
    int value = m_avrProcessor->data[address];//_avr_get_ram( m_avrProcessor, address ); //

    return value;
}

QHash<QString, int> AvrProcessor::getRegsTable( QString lstFileName )// get register addresses from lst file
{
    QHash<QString, int> regsTable;

    QStringList lineList = fileToStringList( lstFileName, "AvrProcessor::setRegisters" );

    if( !regsTable.isEmpty() ) regsTable.clear();

    foreach( QString line, lineList )
    {
        line = line.toLower().replace("\t"," ").replace("="," ");
        if( line.contains("equ ") || line.contains("def "))      // This line contains a definition
        {
            QString name    = "";
            QString addrtxt = "";
            int address   = 0;
            bool isNumber = false;

            line.remove("equ").remove(".def").remove(".");
            QStringList wordList = line.split(QRegExp("\\s+")); // Split in words
            while( name.isEmpty() ) name = wordList.takeFirst();

            /*name.right(1).toInt( &isNumber, 10 );
            if( isNumber ) continue;        // reject names ends in number (pin defs)
            isNumber = false;*/

            while( addrtxt.isEmpty() ) addrtxt = wordList.takeFirst();

            int base = 10;

            if( addrtxt.startsWith("H") )                  // IS hexadecimal??
            {
                addrtxt.remove("H").remove("'");           // Get the digits
                base = 16;
            }
            else if( addrtxt.startsWith("0x") )            // IS hexadecimal??
            {
                addrtxt.remove(0, 2);                      // Get the digits
                base = 16;
            }
            if( addrtxt.startsWith("r"))
            {
                addrtxt.remove("r");
                address = addrtxt.toInt( &isNumber );
            }
            else
            {
                address = addrtxt.toInt( &isNumber, base );
                if( isNumber && address < 64 ) address += 32;

            }
            if( isNumber ) regsTable.insert(name, address); // If found a valid address add to map
            //qDebug() << name << address<<"\n";
        }
    }
    return regsTable;
}

void AvrProcessor::uartOut( uint32_t value )
{
    if( !m_usartTerm ) return;

    QString text;
    OutPanelText::self()->appendText( text.append( value ) );
}

#include "moc_avrprocessor.cpp"

