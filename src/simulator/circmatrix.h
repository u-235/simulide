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

#ifndef CIRCMATRIX_H
#define CIRCMATRIX_H

#include <vector>
//#include <iostream>
//#include <cmath>

#include <QList>

#include "e-node.h"

class MAINMODULE_EXPORT CircMatrix
{
    public:
        CircMatrix();
        ~CircMatrix();

 static CircMatrix* self() { return m_pSelf; }

        void printMatrix();
        void createMatrix( QList<eNode*> &eNodeList, QList<eElement*> &elementList  );
        void stampMatrix( int row, int col, double value );
        void stampCoef( int row, double value );
        void simplify();
        bool solveMatrix();
        
        std::vector<std::vector<double> > getMatrix(){return m_circMatrix; }
        std::vector<double>               getCoeffVect(){ return m_coefVect; }

    private:
 static CircMatrix* m_pSelf;
 
        void swap( int x, int y );
 
        int m_numEnodes;
        QList<eNode*>* m_eNodeList;
        QList<eNode*>  m_eNodeList2;
        QList<eElement*> m_elementList;

        std::vector<std::vector<double> > m_circMatrix;
        //std::vector<std::vector<double> > m_totalMatrix;
        std::vector<double> m_coefVect;
        std::vector<double> m_nodeVolt;
        
        std::vector<std::vector<double> > a;
        std::vector<double> b;
        std::vector<int> ipvt;
        
        bool m_admitChanged;
        bool m_currChanged;

        
};
 #endif


