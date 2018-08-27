/***************************************************************************
 *   Copyright (C) 2012 by santiago González                               *
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

#include <QtGui>

#include "picprocessor.h"
#include "simulator.h"
#include "utils.h"

// GpSim includes
#include "gpsim/pic-processor.h"
#include "gpsim/uart.h"
#include "gpsim/pir.h"


PicProcessor::PicProcessor( QObject* parent ) 
            : BaseProcessor( parent )
{
    m_pSelf = this;
    m_pPicProcessor = 0l;
    m_loadStatus    = false;
    m_DoneGpsimInit = false;
}
PicProcessor::~PicProcessor(){}

void PicProcessor::terminate()
{
    //qDebug() << "PicProcessor::terminate";
    BaseProcessor::terminate();
    
    //QByteArray symbolFile = m_symbolFile.toLocal8Bit();
    QByteArray device = m_device.toLocal8Bit();

    globalSymbolTable().removeSymbol( device.constData() );
    
    m_pPicProcessor = 0l;
}

bool PicProcessor::loadFirmware( QString fileN )
{
    if( fileN == "" ) return false;
    
    m_symbolFile = fileN; 
    
    if( !QFile::exists(m_symbolFile) )
    {
        QMessageBox::warning( 0, tr("File Not Found"), tr("The file \"%1\" was not found.").arg(m_symbolFile) );
        return false;
    }
    if( !m_DoneGpsimInit )
    {
        qDebug() << "\n        Initializing GpSim...\n";
        int init = initialize_gpsim_core();
        initialization_is_complete();
        m_DoneGpsimInit = (init==0);

        if( m_DoneGpsimInit )
            qDebug() << "    ...GpSim Initialization is complete\n\n";
        else
            qDebug() << "    ...Error!!! GpSim Initialization Failed!!!\n\n";
    }
    
    qDebug() << "PicProcessor::loadFirmware hexFile"<<m_symbolFile ;
    
    QByteArray symbolFile = m_symbolFile.toLocal8Bit();
    QByteArray device = m_device.toLocal8Bit();
    
    if( !m_pPicProcessor )
    {
        //qDebug() << "\nPicProcessor::loadFirmware Breakpoints:";
        //get_cycles().dump_breakpoints();
        Cycle_Counter_breakpoint_list* l1 = &(get_cycles().active);

        while(l1->next)            // Clear CycleCounter breakpoint list
        {
            //qDebug() << "clear_break"<<l1->break_value;
            l1->clear();
            Cycle_Counter_breakpoint_list* l2 = l1;
            l1 = l1->next;
            l2->next = 0l;
        }
        gpsimObject* psn = globalSymbolTable().find( device.constData() );

        if( psn ) globalSymbolTable().removeSymbol( device.constData() );

        ProcessorConstructor *pc = ProcessorConstructorList::GetList()->findByType(device.constData());
        Processor* p = pc->ConstructProcessor(device.constData());
        m_pPicProcessor = dynamic_cast<pic_processor*>( p );

        if( !m_pPicProcessor )
        {
            QMessageBox::warning( 0, tr("Unkown Error:")
                                   , tr("Could not Create Pic Processor for: \"%1\"").arg(m_symbolFile) );
            return false;
        }
    }
    FILE* pFile  = fopen( symbolFile.constData(), "r" );
    m_loadStatus = m_pPicProcessor->LoadProgramFile( symbolFile.constData(), pFile, device.constData() );

    if( !m_loadStatus )
    {
        QMessageBox::warning( 0, tr("Unkown Error:"),  tr("Could not Load: \"%1\"").arg(m_symbolFile) );
        return false;
    }
    int cpi = m_pPicProcessor->get_ClockCycles_per_Instruction();
    m_ipc = 1/(double)cpi;

    qDebug() << "\nPicProcessor::loadFirmware      Device: " << m_pPicProcessor->name().c_str();
    
    qDebug() << "    frequency=" << m_pPicProcessor->get_frequency();
    qDebug() << "    Cycles_per_Instruction=" << cpi;
    qDebug() << "    OSCperiod=" << m_pPicProcessor->get_OSCperiod();
    qDebug() << "    instruction period=" << m_pPicProcessor->get_InstPeriod();
    qDebug() << "    internal OSC=" << (m_pPicProcessor->get_int_osc() ? "true" : "false");
    qDebug() << "    use PLLx4=" << (m_pPicProcessor->get_pplx4_osc() ? "true" : "false");
    
    QString tmr1Freq = m_device + ".tmr1_freq";
    device = tmr1Freq.toLocal8Bit();
    gpsimObject *tmr1 = globalSymbolTable().find( device.constData() );
    Float *tmr1FreqVal = dynamic_cast<Float*>( tmr1 );
    if (tmr1FreqVal) {
        qDebug() << "    tmr1_freq=" << tmr1FreqVal->getVal();
    }
    initialized();
    
    m_lastTrmtBit = 0; // for usart
    m_pendingIpc = 0;
    m_nextCycle = m_mcuStepsPT/cpi;
    if( m_nextCycle == 0 ) m_nextCycle = 1;
    
    get_cycles().preset( 0 );
    
    int address = getRegAddress( "RCREG" );
    Register* rcreg = m_pPicProcessor->rma.get_register( address );
    m_rcReg = dynamic_cast<_RCREG*>(rcreg); 
    
    address = getRegAddress( "TXSTA" );
    Register* txsta = m_pPicProcessor->rma.get_register( address );
    m_txsta = dynamic_cast<_TXSTA*>(txsta); 
    
    address = getRegAddress( "PIR1" );
    Register* pir1 = m_pPicProcessor->rma.get_register( address );
    m_pir = dynamic_cast<PIR*>(pir1); 
    
    //m_tmrtBitSink = new RegBitSink( this, "TXSTA", 1 );
    //qDebug() << m_regsTable["TXSTA"];
    
    return true;
}

void PicProcessor::step()                 // Run 1 step 
{
    if( !m_loadStatus || m_resetStatus ) return;
    
    double dCycles = (double)m_mcuStepsPT*m_ipc + m_pendingIpc;
    
    int cycles = (int)dCycles;
    
    m_pendingIpc = dCycles-(double)cycles;

    for( int k=0; k<cycles; k++ ) 
    {
        m_pPicProcessor->step_cycle();
    }

    if( m_usartTerm ) readUsart();
}

void PicProcessor::stepOne() 
{
    if( m_nextCycle > 0 )
    {
        m_pPicProcessor->step_cycle();
        m_nextCycle--;
    }
    
    if( m_nextCycle == 0 )
    {
        double dCycles = (double)m_mcuStepsPT*m_ipc + m_pendingIpc;
        int cycles = (int)dCycles;
        m_pendingIpc = dCycles-(double)cycles;
        m_nextCycle = cycles;
        
        runSimuStep(); // 1 simu step = 1uS
    }
}

int PicProcessor::pc() { return m_pPicProcessor->pc->get_value(); }

void PicProcessor::reset()
{
    if( !m_loadStatus ) return;
    
    m_pPicProcessor->setBreakOnReset(false);

    m_pPicProcessor->reset( POR_RESET ); // POR_RESET  MCLR_RESET EXIT_RESET
}

int PicProcessor::getRamValue( int address )
{ 
    return m_pPicProcessor->rma[address].get_value(); 
}

int PicProcessor::validate( int address ) { return address; }

void PicProcessor::uartIn( uint32_t value ) // Receive one byte on Uart
{
    if( m_usartTerm && m_pPicProcessor)
    {
        BaseProcessor::uartIn( value );
        m_rcReg->push( value );
        //qDebug() << "AvrProcessor::uartIn: " << value<<m_pPicProcessor->rma[26].get_value();
    }
}

void PicProcessor::readUsart()
{
static int txreg;
static int txreg0;

    if( !(m_txsta->get_bit(5)) ) return;                           // TXEN bit

    bool trmtBit = m_txsta->get_bit(1); //txstaReg & 2;
    
    txreg = getRamValue( m_regsTable["TXREG"] );
    //qDebug() << "PicProcessor::readUsart:.... " << txreg<<trmtBit;
//qDebug() << "PicProcessor::readUsart:.... " << txreg<<m_lastTrmtBit<<trmtBit;
    if( txreg != txreg0  )
    {
        if( !m_lastTrmtBit & !trmtBit ) BaseProcessor::uartOut( txreg );
        txreg0 = txreg;
        //qDebug() << "PicProcessor::readUsart:.... " << text;
    }
    if( m_lastTrmtBit != trmtBit ) 
    {
        if(  m_lastTrmtBit & !trmtBit ) BaseProcessor::uartOut( txreg0 );
        m_lastTrmtBit = trmtBit;
        //qDebug() << "PicProcessor::readUsart: " << text;
    }
}

void PicProcessor::bitChange( QString regName, int bit, bool value )
{
    // Implement stuff depending on changing reg.bit
    qDebug() << "PicProcessor::bitChange" << regName << bit << value;
}

// RegBitSink inform us about bit changes in a register
/*RegBitSink::RegBitSink(PicProcessor* processor, QString name, int bit ) 
{
    m_picProcessor = processor;
    m_regName      = name;
    m_bit          = bit;
    
    unsigned int address = processor->getRegAddress( name );
    
    Register* reg = processor->getCpu()->rma.get_register( address );
    
    sfr_register* sfrReg = dynamic_cast<sfr_register*>(reg); 
    
    if (!sfrReg->assignBitSink( bit, this ) ) // Register for bit changes callback
        qDebug() << "RegBitSink: Error assingning BitSink" << name << bit<<address<<sfrReg->getAddress();
}
RegBitSink::~RegBitSink(){}
        
void RegBitSink::setSink(bool b)  // Called by gpsim when bit changes
{
    if( m_picProcessor )
        m_picProcessor->bitChange( m_regName, m_bit, b );
}*/

#include "moc_picprocessor.cpp"

