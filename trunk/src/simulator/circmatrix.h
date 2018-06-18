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

#ifndef CIRCMATRIX_H
#define CIRCMATRIX_H

#include <vector>
//#include <iostream>
//#include <cmath>

#include <QList>

#include "e-node.h"

class MAINMODULE_EXPORT CircMatrix
{
    typedef std::vector<int>                  i_vector_t;
    typedef std::vector<double>               d_vector_t;
    typedef std::vector<double*>              dp_vector_t;
    typedef std::vector<std::vector<double>>  d_matrix_t;
    typedef std::vector<std::vector<double*>> dp_matrix_t;
    
    public:
        CircMatrix();
        ~CircMatrix();

 static CircMatrix* self() { return m_pSelf; }

        void printMatrix();
        void createMatrix( QList<eNode*> &eNodeList, QList<eElement*> &elementList  );
        void stampMatrix( int row, int col, double value );
        void stampCoef( int row, double value );
        bool solveMatrix();
        
        void setCircChanged();
        
        d_matrix_t getMatrix(){return m_circMatrix; }
        d_vector_t getCoeffVect(){ return m_coefVect; }

    private:
 static CircMatrix* m_pSelf;
        
        void factorMatrix( dp_matrix_t& ap, i_vector_t& ipvt, int n, int group );
        bool luSolve( d_matrix_t& a, dp_vector_t& bp, i_vector_t& ipvt, int n );
        void addConnections( int enodNum, QList<int>* nodeGroup );
        
        int m_numEnodes;
        QList<eNode*>* m_eNodeList;
        QList<eElement*> m_elementList;
        
        QList<dp_matrix_t> m_aList;
        QList<d_matrix_t>  m_aFaList;
        QList<dp_vector_t> m_bList;
        QList<i_vector_t>  m_ipvtList;
        
        QHash<int, eNode*> m_eNodeActive;
        QList<QHash<int, eNode*>> m_eNodeActList;

        d_matrix_t m_circMatrix;
        d_vector_t m_coefVect;
        
        bool m_admitChanged;
        bool m_circChanged;
        bool m_currChanged;
};
 #endif


