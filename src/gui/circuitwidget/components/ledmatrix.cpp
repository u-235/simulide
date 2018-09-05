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

#include "ledmatrix.h"
#include "connector.h"
#include "circuit.h"
#include "pin.h"


Component* LedMatrix::construct( QObject* parent, QString type, QString id )
{ return new LedMatrix( parent, type, id ); }

LibraryItem* LedMatrix::libraryItem()
{
    return new LibraryItem(
            tr( "LedMatrix" ),
            tr( "Outputs" ),
            "ledmatrix.png",
            "LedMatrix",
            LedMatrix::construct);
}

LedMatrix::LedMatrix( QObject* parent, QString type, QString id )
         : Component( parent, type, id )
         , eElement( id.toStdString() )
{
    m_rows = 8;
    m_cols = 8;
    createMatrix();
}
LedMatrix::~LedMatrix(){}

void LedMatrix::initialize()
{
    for( int row=0; row<m_rows; row++ )
    {
        eNode* rowEnode = m_rowPin[row]->getEnode();
        
        for( int col=0; col<m_cols; col++ )
        {
            eNode* colEnode = m_colPin[col]->getEnode();
            
            LedSmd* lsmd = m_led[row][col];
            lsmd->getEpin(0)->setEnode( rowEnode );
            lsmd->getEpin(1)->setEnode( colEnode );
        }
    }
}

void LedMatrix::setupMatrix( int rows, int cols )
{
    deleteMatrix();
    m_rows = rows;
    m_cols = cols;
    createMatrix();
}

void LedMatrix::createMatrix()
{
    m_area = QRect( -8, -8, m_cols*8+8, m_rows*8+8 );
    
    m_led.resize( m_rows, std::vector<LedSmd*>(m_cols) );
    m_rowPin.resize( m_rows );
    m_colPin.resize( m_cols );
    
    for( int row=0; row<m_rows; row++ )
    {
        QString pinId = m_id;
        pinId.append( QString("-pinRow"+QString::number(row)));
        QPoint nodpos = QPoint(-16, row*8 );
        m_rowPin[row] = new Pin( 180, nodpos, pinId, 0, this);
        
        for( int col=0; col<m_cols; col++ )
        {
            QString ledid = m_id;
            ledid.append( QString( "-led"+QString::number(row)+"_"+QString::number(col) ) );
            LedSmd* lsmd = new LedSmd( this, "LEDSMD", ledid, QRectF(-2, -2, 4, 4) );
            m_led[row][col] = lsmd;
            lsmd->setParentItem(this);
            lsmd->setNumEpins(2);
            lsmd->setMaxCurrent( 0.02 );
            lsmd->setPos( col*8, row*8 );
            lsmd->setEnabled(false);
            lsmd->setAcceptedMouseButtons(0);
        }
    }
    for( int col=0; col<m_cols; col++ )
    {
        QString pinId = m_id;
        pinId.append( QString("-pinCol"+QString::number(col)));
        QPoint nodpos = QPoint( col*8, m_rows*8+8 );
        m_colPin[col] = new Pin( 270, nodpos, pinId, 1, this);
    }
}

void LedMatrix::deleteMatrix()
{
    for( int row=0; row<m_rows; row++ )
    {
        Pin* pin = m_rowPin[row];
        if( pin->isConnected() ) pin->connector()->remove();
        delete pin;
        
        for( int col=0; col<m_cols; col++ )
        {
            Circuit::self()->removeComp( m_led[row][col] );
        }
    }
    for( int col=0; col<m_cols; col++ )
    {
        Pin* pin = m_colPin[col];
        if( pin->isConnected() ) pin->connector()->remove();
        delete pin;
    }
    m_led.resize(0);
}

void LedMatrix::setColor( LedBase::LedColor color ) 
{ 
    for( int row=0; row<m_rows; row++ )
    {
        for( int col=0; col<m_cols; col++ )
        {
            m_led[row][col]->setColor( color ); 
        }
    }
}

LedBase::LedColor LedMatrix::color() 
{ 
    return m_led[0][0]->color(); 
}

int LedMatrix::rows()
{
    return m_rows;
}

void LedMatrix::setRows( int rows )
{
    if( rows == m_rows ) return;
    setupMatrix( rows, m_cols );
}

int LedMatrix::cols()
{
    return m_cols;
}

void LedMatrix::setCols( int cols )
{
    if( cols == m_cols ) return;
    setupMatrix( m_rows, cols );
}

double LedMatrix::threshold()                     
{ 
    return m_led[0][0]->threshold(); 
}

void LedMatrix::setThreshold( double threshold ) 
{ 
    for( int row=0; row<m_rows; row++ )
    {
        for( int col=0; col<m_cols; col++ )
        {
            m_led[row][col]->setThreshold( threshold ); 
        }
    }
}

double LedMatrix::maxCurrent()                   
{ 
    return m_led[0][0]->maxCurrent(); 
}
void LedMatrix::setMaxCurrent( double current ) 
{ 
    for( int row=0; row<m_rows; row++ )
    {
        for( int col=0; col<m_cols; col++ )
        {
            m_led[row][col]->setMaxCurrent( current ); 
        }
    }
}

double LedMatrix::res() 
{ 
    return m_led[0][0]->res(); 
}

void LedMatrix::setRes( double resist )
{
    if( resist == 0 ) resist = 1e-14;

    for( int row=0; row<m_rows; row++ )
    {
        for( int col=0; col<m_cols; col++ )
        {
            m_led[row][col]->setRes( resist ); 
        }
    }
}

void LedMatrix::remove()
{
    deleteMatrix();
    
    Component::remove();
}

void LedMatrix::paint( QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
    Component::paint( p, option, widget );
    
    p->setBrush( QColor( 0, 0, 0) );

    p->drawRoundRect( m_area, 4, 4 );
}

#include "moc_ledmatrix.cpp"