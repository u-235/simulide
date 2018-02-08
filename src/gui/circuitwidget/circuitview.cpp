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

#include "mainwindow.h"
#include "circuitview.h"
#include "circuit.h"
#include "component.h"
#include "utils.h"

CircuitView::CircuitView( QWidget *parent )
    : QGraphicsView( parent )
{
    m_circuit     = 0l;
    m_enterItem   = 0l;
    m_scalefactor = 1;

    clear();

    viewport()->setFixedSize(1600,1200);
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
    setCacheMode( CacheBackground );
    //setRenderHint( QPainter::Antialiasing );
    setRenderHints( QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    //setRenderHint( QPainter::SmoothPixmapTransform );
    setTransformationAnchor( AnchorUnderMouse );
    setResizeAnchor( AnchorUnderMouse );
    setDragMode( QGraphicsView::RubberBandDrag );

    setAcceptDrops(true);
}
CircuitView::~CircuitView() { }

void CircuitView::clear()
{
    if( m_circuit ) 
    {
        m_circuit->remove();
        m_circuit->deleteLater();
    }
    m_circuit = new Circuit( -800, -600, 1600, 1200, this );
    setScene( m_circuit );
    centerOn( 300, 200 );
}

void CircuitView::wheelEvent(QWheelEvent *event) { scaleView(pow( 2.0, event->delta() / 700.0)); }

void CircuitView::scaleView(qreal scaleFactor)
{
    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 8, 8)).width();

    if( factor < 2 || factor > 100 ) return;

    m_scalefactor *=  factor;
    scale(scaleFactor, scaleFactor);
}

void CircuitView::dragEnterEvent(QDragEnterEvent *event)
{
    Circuit::self()->saveState();

    event->accept();
    bool pauseSim = Simulator::self()->isRunning();
    if( pauseSim ) Simulator::self()->pauseSim();

    QString type = event->mimeData()->html();
    QString id = event->mimeData()->text()+"-"+m_circuit->newSceneId(); //event->mimeData()->text();

    m_enterItem = m_circuit->createItem( type, id );
    if( m_enterItem )
    {
        //qDebug()<<"CircuitView::dragEnterEvent"<<m_enterItem->itemID()<< type<< id;
        m_enterItem->setPos( mapToScene( event->pos() ) );
        m_circuit->addItem( m_enterItem );
    }
    if( pauseSim ) Simulator::self()->resumeSim();
}

void CircuitView::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
    if (m_enterItem ) m_enterItem->setPos( togrid( mapToScene( event->pos() ) ) );
}

void CircuitView::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
    if ( m_enterItem )
    {
        m_enterItem->remove();
        m_enterItem = 0l;
    }
}

void CircuitView::resizeEvent( QResizeEvent *event )
{
    int width = event->size().width();
    int height = event->size().height();

    m_circuit->setSceneRect(-width/2+2, -height/2+2, width-4, height-4);

    QGraphicsView::resizeEvent(event);
}

void CircuitView::keyPressEvent( QKeyEvent *event )
{
    if( event->key() == Qt::Key_Shift )
        setDragMode( QGraphicsView::ScrollHandDrag );
    
    QGraphicsView::keyPressEvent( event );
}

void CircuitView::keyReleaseEvent( QKeyEvent *event )
{
    //if( event->key() == Qt::Key_Shift )
        setDragMode( QGraphicsView::RubberBandDrag );
        
    QGraphicsView::keyReleaseEvent( event );
}

void CircuitView::contextMenuEvent(QContextMenuEvent* event)
{
    QGraphicsView::contextMenuEvent( event );

    if( !event->isAccepted() && !m_circuit->is_constarted() )
    {
        QPointF eventPos = mapToScene( event->globalPos() ) ;
        m_eventpoint = mapToScene( event->pos()  );

        QMenu menu;

        QAction* pasteAction = menu.addAction(QIcon(":/paste.png"),"Paste");
        connect( pasteAction, SIGNAL( triggered()), this, SLOT(slotPaste()) );

        QAction* undoAction = menu.addAction(QIcon(":/undo.png"),"Undo");
        connect( undoAction, SIGNAL( triggered()), Circuit::self(), SLOT(undo()) );

        QAction* redoAction = menu.addAction(QIcon(":/redo.png"),"Redo");
        connect( redoAction, SIGNAL( triggered()), Circuit::self(), SLOT(redo()) );
        menu.addSeparator();

        QAction* openCircAct = menu.addAction(QIcon(":/opencirc.png"), tr("Open Circuit") );
        connect(openCircAct, SIGNAL(triggered()), MainWindow::self(), SLOT(openCirc()));

        QAction* newCircAct = menu.addAction( QIcon(":/newcirc.png"), tr("New Circuit") );
        connect( newCircAct, SIGNAL(triggered()), MainWindow::self(), SLOT(newCircuit()));

        QAction* saveCircAct = menu.addAction(QIcon(":/savecirc.png"), tr("Save Circuit") );
        connect(saveCircAct, SIGNAL(triggered()), MainWindow::self(), SLOT(saveCirc()));

        QAction* saveCircAsAct = menu.addAction(QIcon(":/savecircas.png"),tr("Save Circuit As...") );
        connect(saveCircAsAct, SIGNAL(triggered()), MainWindow::self(), SLOT(saveCircAs()));
        menu.addSeparator();

        QAction* importCircAct = menu.addAction(QIcon(":/opencirc.png"), tr("Import Circuit") );
        connect(importCircAct, SIGNAL(triggered()), this, SLOT(importCirc()));

        QAction* saveImgAct = menu.addAction( QIcon(":/saveimage.png"), tr("Save Circuit as Image") );
        connect( saveImgAct, SIGNAL(triggered()), this, SLOT(saveImage()));

        QAction* createSubCircAct = menu.addAction(QIcon(":/load.png"), tr("Create SubCircuit") );
        connect(createSubCircAct, SIGNAL(triggered()), Circuit::self(), SLOT( createSubcircuit() ));

        menu.exec( mapFromScene( eventPos ) );
    }
}

void CircuitView::importCirc()
{
    Circuit::self()->importCirc( m_eventpoint );
}

void CircuitView::slotPaste()
{
    Circuit::self()->paste( m_eventpoint );
}

void CircuitView::saveImage()
{
    QString fileName= QFileDialog::getSaveFileName(this, "Save image", QCoreApplication::applicationDirPath(), "BMP Files (*.bmp);;JPEG (*.JPEG);;PNG (*.png)" );
    if (!fileName.isNull())
    {
        QPixmap pixMap = this->grab();
        pixMap.save(fileName);
    }
}
#include "moc_circuitview.cpp"
