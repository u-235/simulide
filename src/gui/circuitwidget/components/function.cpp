/***************************************************************************
 *   Copyright (C) 2018 by santiago González                               *
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

#include "function.h"
#include "connector.h"
#include "circuit.h"
#include "itemlibrary.h"



/*static const char* ADC_properties[] = {
    QT_TRANSLATE_NOOP("App::Property","Vref")
};*/

Component* Function::construct( QObject* parent, QString type, QString id )
{
        return new Function( parent, type, id );
}

LibraryItem* Function::libraryItem()
{
    return new LibraryItem(
        tr( "Function" ),
        tr( "Logic/Arithmetic" ),
        "subc.png",
        "Function",
        Function::construct );
}

Function::Function( QObject* parent, QString type, QString id )
        : LogicComponent( parent, type, id )
        , eFunction( id.toStdString() )
{
    setNumInps( 2 );                           // Create Input Pins
    setNumOuts( 1 );
    
    m_functions.append( "i0 | i1" );
    
    //bool ok;
    //QString text = QInputDialog::getText(0l, tr("QInputDialog::getText()"),
                                  //           tr("Function:"), QLineEdit::Normal,
                                  //           QDir::home().dirName(), &ok);
        //if( ok && !text.isEmpty() )
}
Function::~Function(){
}

void Function::remove()
{
    
    LogicComponent::remove();
}

void Function::setNumInps( int inputs )
{
    if( inputs == m_numInPins ) return;
    if( inputs < 1 ) return;
    
    LogicComponent::setNumInps( inputs );
    eLogicDevice::deleteInputs( m_numInputs );


    for( int i=0; i<inputs; i++ )
    {
        QString num = QString::number(i);
        m_inPin[i] = new Pin( 180, QPoint(-24,-8*inputs+i*8+8 ), m_id+"-in"+num, i, this );
                               
        m_inPin[i]->setLabelText( " I"+num );
        m_inPin[i]->setLabelColor( QColor( 0, 0, 0 ) );

        eLogicDevice::createInput( m_inPin[i] );
    }
    m_height = m_numOutputs*2;
    if( m_numInputs > m_height ) m_height = m_numInputs;
    m_area = QRect( -16, -8*m_height, 32, 8*m_height+8 );
    
    Circuit::self()->update();
}

void Function::setNumOuts( int outs )
{
    if( outs == m_numOutputs ) return;
    if( outs < 1 ) return;

    LogicComponent::setNumOuts( outs );
    eLogicDevice::deleteOutputs( m_numOutputs );

    for( int i=0; i<outs; i++ )
    {
        QString num = QString::number(i);
        m_outPin[i] = new Pin( 0, QPoint(24,-8*outs*2+i*8*2+16 ), m_id+"-out"+num, i, this );

        m_outPin[i]->setLabelText( "O"+num+" " );
        m_outPin[i]->setLabelColor( QColor( 0, 0, 0 ) );

        eLogicDevice::createOutput( m_outPin[i] );
    }
    m_height = m_numOutputs*2-1;
    if( m_numInputs > m_height ) m_height = m_numInputs;
    m_area = QRect( -16, -8*m_height, 32, 8*m_height+8 );
    
    Circuit::self()->update();
    
}

#include "moc_function.cpp"
