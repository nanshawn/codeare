/*
 *  codeare Copyright (C) 2007-2010 
 *                        Kaveh Vahedipour
 *                        Forschungszentrum Juelich, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 
 *  02110-1301  USA
 */

#ifndef __MATRIX_H__
#define __MATRIX_H__

#ifdef PARC_MODULE_NAME

  #include     "MrServers/MrVista/include/Ice/IceBasic/IceAs.h"
  #include     "MrServers/MrVista/include/Ice/IceBasic/IceObj.h"
  #include     "MrServers/MrVista/include/Parc/Trace/IceTrace.h"

#endif

#include "config.h"
#include "common.h"

#include "OMP.hpp"
#include "Complex.hpp"
#include "Vector.hpp"
#include "RangeParser.hpp"

#ifdef EXPLICIT_SIMD
#    include "SIMD.hpp"
#endif

#include <iostream>
#include <memory>
#include <fstream>
#include <typeinfo>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <numeric>

#include <ostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <utility>

#include <Assert.hpp>

/**
 * @brief Is matrix is a vector.
 */
# define VECT(M) assert((M)->width() == 1 || (M)->height() == 1);

 
/**
 * Return minimum of two numbers
 */
# define MAX(A,B) (A > B ? A : B)


/**
 * Return maximum of two numbers 
 */
# define MIN(A,B) (A < B ? A : B)


/**
 * Return rounded value as in MATLAB.
 * i.e. ROUND( 0.1) ->  0
 *      ROUND(-0.5) -> -1
 *      ROUND( 0.5) ->  1
 */
# define ROUND(A) ( floor(A) + ((A - floor(A) >= 0.5) ? (A>0 ? 1 : 0) : 0))



/**
 * @brief   Matrix template.<br/>
 *          Core data structure
 *          
 * @author  Kaveh Vahedipour
 * @date    Mar 2010
 */
template <class T, paradigm P=SHM
#if !defined(_MSC_VER) || _MSC_VER>1200
    ,const bool& b = TypeTraits<T>::Supported
#endif
>
class Matrix {
	
	
public:
    
    
    /**
     * @name Constructors and destructors
     *       Constructors and destructors
     */
    //@{
    
    
    /**
     * @brief           Contruct 1-dim with single element.
     */
	inline
    Matrix              () NOEXCEPT {

        _dim.resize(1,1);
        _res.resize(1,1.0);
        
        Allocate();
        
    }
	
	
    /**
     * @brief           Construct matrix with aligned dimension vector
     *
     * @param  dim      All dimensions
     */
	inline
    Matrix              (const Vector<size_t>& dim) NOEXCEPT {

	    size_t ds = dim.size();
	    assert(ds &&
	    	   std::find(dim.begin(),dim.end(),size_t(0))==dim.end());

		_dim.resize(ds);
		std::copy (dim.begin(),dim.end(),_dim.begin());
		_res.resize(ds,1.0);

        Allocate();
		
	}
	
	
    /**
     * @brief           Construct matrix with dimension and resolution arrays
     *
     * @param  dim      All 16 Dimensions
     * @param  res      All 16 Resolutions
     */
	inline explicit
    Matrix              (const Vector<size_t>& dim, const Vector<float>& res) NOEXCEPT {
		
	    assert(!dim.Empty() &&
	    	    std::find(dim.begin(),dim.end(),size_t(0))==dim.end() &&
	    	    dim.size() == res.size());

		_dim = dim;
		_res = res;

        Allocate();
		
	}

    
    /**
     * @brief           Construct square 2D matrix
     *
     * Usage:
     * @code{.cpp}
     *   Matrix<double> m (5); // Empty 5x5 matrix
     * @endcode
     *
     * @param  n        Rows & Columns
     */
    inline explicit
    Matrix (const size_t n) NOEXCEPT {

	    assert (n);

		_dim.resize(2,n);
	    _res.resize(2,1.0);

        Allocate();
        
	}



    
    
    /**
     * @brief           Construct 2D matrix
     *
     * Usage:
     * @code{.cpp}
     *   Matrix<double> m (5,4); // Empty 5x4 matrix
     * @endcode
     *
     * @param  m        Rows
     * @param  n        Columns
     */
    inline
    Matrix              (const size_t m, const size_t n) NOEXCEPT {

    	assert (m && n);

    	_dim.resize(2); _dim[0] = m; _dim[1] = n;
		_res.resize(2,1.0);

        Allocate();

    }

    
    
    /**
     * @brief           Construct 3D volume
     *
     * Usage:
     * @code{.cpp}
     *   Matrix<double> m (5,4,6); // Empty 5x4x6 matrix
     * @endcode
     *
     * @param  m        Rows
     * @param  n        Columns
     * @param  k        Slices
     */
    inline
    Matrix (const size_t m, const size_t n, const size_t k) NOEXCEPT {

	    assert (m && n && k);

    	_dim.resize(3); _dim[0] = m; _dim[1] = n; _dim[2] = k;
		_res.resize(3,1.0);

        Allocate();

    }
    

    /**
     * @brief           Construct 4D volume
     *
     * @param  col      Scan
     * @param  lin      Phase encoding lines
     * @param  cha      Channels
     * @param  set      Sets
     * @param  eco      Echoes
     * @param  phs      Phases
     * @param  rep      Repetitions
     * @param  seg      Segments
     * @param  par      Partitions
     * @param  slc      Slices
     * @param  ida      IDA
     * @param  idb      IDB
     * @param  idc      IDC
     * @param  idd      IDD
     * @param  ide      IDE
     * @param  ave      Averages
     */
    inline
    Matrix              (const size_t col,     const size_t lin,     const size_t cha,     const size_t set,
                         const size_t eco = 1, const size_t phs = 1, const size_t rep = 1, const size_t seg = 1,
                         const size_t par = 1, const size_t slc = 1, const size_t ida = 1, const size_t idb = 1,
                         const size_t idc = 1, const size_t idd = 1, const size_t ide = 1, const size_t ave = 1) NOEXCEPT {

		assert (col && lin && cha && set && eco && phs && rep && seg &&
				par && slc && ida && idb && idc && idd && ide && ave );

	    size_t nd = 16, n = nd, i;

	    _dim.resize(n);
	    _dim[ 0] = col; _dim[ 1] = lin; _dim[ 2] = cha; _dim[ 3] = set;
	    _dim[ 4] = eco; _dim[ 5] = phs; _dim[ 6] = rep; _dim[ 7] = seg;
	    _dim[ 8] = par; _dim[ 9] = slc; _dim[10] = ida; _dim[11] = idb;
	    _dim[12] = idc; _dim[13] = idd; _dim[14] = ide; _dim[15] = ave;

        // Remove trailing singleton dimensions
        for (i = 0; i < nd; i++)
            if (_dim[i] == 1)
                n--;
            else
                n = nd;
        
        // Resize skeleton
        _dim.resize(n);
		_res.resize(n,1.0);

        Allocate();

	}


#ifdef HAVE_CXX11_RVALUE_REFERENCES
    /**
     * @brief           Default copy constructor
     *
     * Usage:
     * @code{.cpp}
     *   Matrix<cxfl> m (n); // Copy n into m
     * @endcode
     *
     * @param  M        Right hand side
     */
    inline
    Matrix             (const Matrix<T,P> &M) NOEXCEPT {
    	if (this != &M)
    		*this = M;
    }
    /**
     * @brief           Default move constructor
     *
     * Usage:
     * @code{.cpp}
     *   Matrix<cxfl> m (n); // Copy n into m
     * @endcode
     *
     * @param  M        Right hand side
     */
    inline
    Matrix             (Matrix<T,P>&& M) NOEXCEPT {
    	if (this != &M)
    		*this = M;
    }
#endif

    //@}



    /**
     * @name            Import export functions for ICE access specifiers.<br/>
     *                  Ice access specifiers can be handled in one of the following ways.<br/>
     *                  It is crucial to understand that the
     */

    //{@

    // Only if compiled within IDEA we know of access specifiers.
#ifdef PARC_MODULE_NAME
    #include "ICE.hpp"
#endif

    //@}

    
    /**
     * @name            Elementwise access
     *                  Elementwise access
     */
    
    //@{
    
    
    /**
     * @brief           Get copy of p-th element.
     *
     * Usage:
     * @code{.cpp}
     *   Matrix<float> m = rand<float> (10,1);
     *   float f = m[6]; // right hand side
     * @endcode
     *
     * @param  p        Requested position.
     * @return          Value at _M[p].
     */
    inline T
    operator[]          (const size_t p) const NOEXCEPT {
        assert(p <  Size());
        return _M[p];
    }
    
    
    /**
     * @brief           Get reference to p-th element.
     *
     * Usage:
     * @code{.cpp}
     *   Matrix<float> m = rand<float> (10,1);
     *   m[6] = 1.8; // left hand side
     * @endcode
     *
     * @param  p        Requested position.
     * @return          Reference to _M[p].
     */
    inline T&
    operator[] (const size_t p) NOEXCEPT {
        assert(p <  Size());
        return _M[p];
    }


	//#include "AssignmentHandler.hpp"
    
    //inline AssignmentHandler<T> operator[] (const int, const int) {

    //}

    /**
     * @brief           Get pointer to memory starting at p-th (default:0) element.
     *  
     * @param  p        Position
     *
     * @return          Data 
     */
    inline const T*            
    Ptr             (const size_t p = 0)  const NOEXCEPT {
        assert (p < Size());
        return _M.ptr(p);
    }

    /**
     * @brief           Get pointer to memory starting at p-th (default:0) element.
     *
     * @param  p        Position
     *
     * @return          Data
     */
    inline T*
    Ptr             (const size_t p = 0) NOEXCEPT {
        assert (p < Size());
        return _M.ptr(p);
    }


    
    /**
     * @brief           Data container (lhs)
     *  
     * @return          Data container
     */
    inline Vector<T>&
    Container           ()  NOEXCEPT {
        return _M;
    }

    
    /**
     * @brief           Data container (rhs)
     *  
     * @return          Data container
     */
    inline Vector<T>
    Container           ()  const NOEXCEPT {
        return _M;
    }


    /**
     * @brief           Container iterator to first element (lhs)
     *
     * @return          Container iterator
     */
    inline typename Vector<T>::iterator

    Begin               () NOEXCEPT {
    	return _M.begin ();
    }


    /**
     * @brief           Container const iterator to first element (rhs)
     *
     * @return          Container const iterator
     */
    inline typename Vector<T>::const_iterator
    Begin               ()  const NOEXCEPT {
    	return _M.begin ();
    }

    
    /**
     * @brief           Container iterator to last element (lhs)
     *
     * @return          Container iterator
     */
    inline typename Vector<T>::iterator
    End                 () NOEXCEPT {
    	return _M.end ();
    }


    /**
     * @brief           Container const iterator to last element (rhs)
     *
     * @return          Container const iterator
     */
    inline typename Vector<T>::const_iterator
    End                 ()  const NOEXCEPT {
    	return _M.end ();
    }


    /**
     * @brief           Element at position p (rhs)
     *  
     * @param  p        Position
     * @return          Value at _M[p]
     */
    inline const T&
    At                  (const size_t p) const NOEXCEPT {
        assert (p < Size());
        return _M[p];
    }


    /**
     * @brief            Element at position (lhs)
     *  
     * @param  pos       Position
     * @return           Reference to _M[p]
     */
    inline T&           
    At                  (const size_t pos) NOEXCEPT {
        assert (pos < Size());
        return _M[pos];
    }


    
    /**
     * @brief           Get element in (first) slice (rhs)
     *  
     * @param  x        Column
     * @param  y        Line
     *
     * @return          Value
     */
    inline const T& At (const size_t x, const size_t y) const NOEXCEPT {
    	assert ((!x || x < _dim[0]) && (!y || y < _dim[1]));
        return _M[x + _dim[0]*y];
    }
    inline T&       At (const size_t x, const size_t y) NOEXCEPT {
    	assert ((!x || x < _dim[0]) && (!y || y < _dim[1]));
        return _M[x + _dim[0]*y];
    }

    
    /**
     * @brief          Get element in (first) volume (lhs)
     *  
     * @param  x       Column
     * @param  y       Line
     * @param  z       Slice
     *
     * @return         Value
     */
    inline const T& At (const size_t x, const size_t y, const size_t z) const NOEXCEPT {
    	assert ((!x || x < _dim[0]) && (!y || y < _dim[1]) && (!z || z < _dim[2]));
        return _M[x + _dsz[1]*y + _dsz[2]*z];
    }
    inline T&       At (const size_t x, const size_t y, const size_t z) NOEXCEPT {
    	assert ((!x || x < _dim[0]) && (!y || y < _dim[1]) && (!z || z < _dim[2]));
        return _M[x + _dsz[1]*y + _dsz[2]*z];
    }
    

    /**
     * @brief            Reference to element in (first) volume (lhs)
     *
     * @param  x         Column
     * @param  y         Line
     * @param  z         Slice
     *
     * @return           Reference
     */
    inline const T&
    At                   (const size_t x, const size_t y, const size_t z, const size_t w) const NOEXCEPT {
    	assert ((!x || x < _dim[0]) && (!y || y < _dim[1]) && (!z || z < _dim[2]) && (!w || w < _dim[3]));
        return _M[x + _dsz[1]*y + _dsz[2]*z + _dsz[3]*w];
    }
    inline T&
    At                   (const size_t x, const size_t y, const size_t z, const size_t w) NOEXCEPT {
    	assert ((!x || x < _dim[0]) && (!y || y < _dim[1]) && (!z || z < _dim[2]) && (!w || w < _dim[3]));
        return _M[x + _dsz[1]*y + _dsz[2]*z + _dsz[3]*w];
    }

    

    /**
     * @brief            Get value at position (lhs)
     *  
     * @param  col       Column
     * @param  lin       Line
     * @param  cha       Channel
     * @param  set       Set
     * @param  eco       Echo
     * @param  phs       Phase
     * @param  rep       Repetition
     * @param  seg       Segment
     * @param  par       Partition
     * @param  slc       Slice
     * @param  ida       Free index A
     * @param  idb       Free index B
     * @param  idc       Free index C
     * @param  idd       Free index D
     * @param  ide       Free index E
     * @param  ave       Average
     * @return           Value at position
     */
    inline const T&
    At                   (const size_t col,     const size_t lin,     const size_t cha,     const size_t set,
                          const size_t eco,     const size_t phs = 0, const size_t rep = 0, const size_t seg = 0,
                          const size_t par = 0, const size_t slc = 0, const size_t ida = 0, const size_t idb = 0,
                          const size_t idc = 0, const size_t idd = 0, const size_t ide = 0, const size_t ave = 0) const NOEXCEPT {
    	assert ((!col || col < _dim[ 0])&& (!lin || lin < _dim[ 1])&& (!cha || cha < _dim[ 2])&& (!set || set < _dim[ 3])&&
    	        (!eco || eco < _dim[ 4])&& (!phs || phs < _dim[ 5])&& (!rep || rep < _dim[ 6])&& (!seg || seg < _dim[ 7])&&
    	        (!par || par < _dim[ 8])&& (!slc || slc < _dim[ 9])&& (!ida || ida < _dim[10])&& (!idb || idb < _dim[11])&&
    	        (!idc || idc < _dim[12])&& (!idd || idd < _dim[13])&& (!ide || ide < _dim[14])&& (!ave || ave < _dim[15]));
        return _M [col + lin*_dsz[ 1] + cha*_dsz[ 2] + set*_dsz[ 3] + eco*_dsz[ 4] + phs*_dsz[ 5] + rep*_dsz[ 6] +
   	               seg*_dsz[ 7] + par*_dsz[ 8] + slc*_dsz[ 9] + ida*_dsz[10] + idb*_dsz[11] + idc*_dsz[12] +
   	               idd*_dsz[13] + ide*_dsz[14] + ave*_dsz[15]];
    }
    inline T&
    At                   (const size_t col,     const size_t lin,     const size_t cha,     const size_t set,
                          const size_t eco,     const size_t phs = 0, const size_t rep = 0, const size_t seg = 0,
                          const size_t par = 0, const size_t slc = 0, const size_t ida = 0, const size_t idb = 0,
                          const size_t idc = 0, const size_t idd = 0, const size_t ide = 0, const size_t ave = 0) NOEXCEPT {
    	assert ((!col || col < _dim[ 0])&& (!lin || lin < _dim[ 1])&& (!cha || cha < _dim[ 2])&& (!set || set < _dim[ 3])&&
    	        (!eco || eco < _dim[ 4])&& (!phs || phs < _dim[ 5])&& (!rep || rep < _dim[ 6])&& (!seg || seg < _dim[ 7])&&
    	        (!par || par < _dim[ 8])&& (!slc || slc < _dim[ 9])&& (!ida || ida < _dim[10])&& (!idb || idb < _dim[11])&&
    	        (!idc || idc < _dim[12])&& (!idd || idd < _dim[13])&& (!ide || ide < _dim[14])&& (!ave || ave < _dim[15]));
        return _M [col + lin*_dsz[ 1] + cha*_dsz[ 2] + set*_dsz[ 3] + eco*_dsz[ 4] + phs*_dsz[ 5] + rep*_dsz[ 6] +
   	               seg*_dsz[ 7] + par*_dsz[ 8] + slc*_dsz[ 9] + ida*_dsz[10] + idb*_dsz[11] + idc*_dsz[12] +
   	               idd*_dsz[13] + ide*_dsz[14] + ave*_dsz[15]];
    }
    

    /**
     * @brief          Cast operator
     *
     * @return         Casted copy
     */
    template<class S>
    inline operator Matrix<S,P> () const NOEXCEPT {
		Matrix<S,P> m (_dim);
		for (size_t i = 0; i < Size(); ++i)
			m[i] = (S)_M[i];
		return m;
	}

    /**
     * @brief Deliver range of values with indices
     *
     * @param indices List of indices
     * @return        Matrix containing values at indices
     */
    inline Matrix<T> operator() (const Vector<size_t>& indices, size_t col = 0) const NOEXCEPT {
    	size_t indices_size = indices.size();
    	assert (indices_size);
    	assert (col < _dim[1]);
    	assert (indices_size < _dim[0]);
    	assert (mmax(indices) < _dim[0]);
    	Matrix<T> ret (indices_size,1);
    	for (size_t i = 0; i < indices_size; ++i)
    		ret[i] = At(indices[i],col);
    	return ret;
    }

    /**
     * @brief Deliver range of values with indices
     *
     * @param indices List of indices
     * @return        Matrix containing values at indices
     */
    inline Matrix<T> operator() (size_t row, const Vector<size_t>& indices) const NOEXCEPT {
    	size_t indices_size = indices.size();
    	assert (indices_size);
    	assert (row < _dim[0]);
    	assert (indices_size < _dim[1]);
    	assert (mmax(indices) < _dim[1]);
    	Matrix<T> ret (1,indices_size);
    	for (size_t i = 0; i < indices_size; ++i)
    		ret[i] = At(row,indices[i]);
    	return ret;
    }

    /**
     * @brief Deliver range of values with indices
     *
     * @param indices List of indices
     * @return        Matrix containing values at indices
     */
    inline Matrix<T> operator() (const Vector<size_t>& row_inds, const Vector<size_t>& col_inds) const NOEXCEPT {
    	size_t row_inds_size = row_inds.size();
    	size_t col_inds_size = col_inds.size();
    	assert (row_inds_size);
    	assert (col_inds_size);
    	assert (row_inds_size < _dim[0]);
    	assert (col_inds_size < _dim[1]);
    	assert (mmax(row_inds) < _dim[0]);
    	assert (mmax(col_inds) < _dim[1]);
    	Matrix<T> ret (row_inds_size, col_inds_size);
    	for (size_t j = 0; j < col_inds_size; ++j)
    		for (size_t i = 0; i < row_inds_size; ++i)
    			ret(i,j) = At(row_inds[i],col_inds[j]);
    	return ret;
    }




    /**
     * @brief           @see At(const size_t)
     *
     * @param  p        Requested position.
     * @return          Requested scalar value.
     */
    inline const T&
    operator()          (const size_t p) const NOEXCEPT {
        return this->At(p);
    }

    
    /**
     * @brief           Get value of pth element of repository.
     *
     * @param  p        Requested position.
     * @return          Requested scalar value.
     */
    inline T&
    operator()          (const size_t p) NOEXCEPT {
        return this->At(p);
    }

    
    /**
     * @brief           Get value in slice
     *
     * @param  x        Column
     * @param  y        Line
     * @return          Value
     */
    inline const T&
    operator()          (const size_t x, const size_t y) const NOEXCEPT {
        return this->At(x,y);
    }
    

    /**
     * @brief           Reference to value in slice
     *
     * @param  x        Column
     * @param  y        Line
     * @return          Reference
     */
    inline T&
    operator()           (const size_t x, const size_t y) NOEXCEPT {
        return this->At(x,y);
    }
    
    
    /**
     * @brief            Get value in volume
     *
     * @param  x         Column
     * @param  y         Line
     * @param  z         Slice
     *
     * @return           Value
     */
    inline const T&
    operator()           (const size_t x, const size_t y, const size_t z) const NOEXCEPT {
        return this->At(x,y,z);
    }
    
    
    /**
     * @brief            Reference to value in volume
     *
     * @param  x         Column
     * @param  y         Line
     * @param  z         Slice
     *
     * @return           Reference to _M[col + _dim[COL]*lin + _dim[COL]*_dim[LIN]*slc]
     */
    inline T&
    operator()           (const size_t x, const size_t y, const size_t z) NOEXCEPT {
        return this->At(x,y,z);
    }
    
    
    /**
     * @brief            Get value in volume
     *
     * @param  x         Column
     * @param  y         Line
     * @param  z         Slice
     *
     * @return           Value
     */
    inline const T&
    operator()           (const size_t x, const size_t y, const size_t z, const size_t w) const NOEXCEPT {
        return this->At(x,y,z,w);
    }


    /**
     * @brief            Reference to value in volume
     *
     * @param  x         Column
     * @param  y         Line
     * @param  z         Slice
     *
     * @return           Reference to _M[col + _dim[COL]*lin + _dim[COL]*_dim[LIN]*slc]
     */
    inline T&
    operator()           (const size_t x, const size_t y, const size_t z, const size_t w) NOEXCEPT {
        return this->At(x,y,z,w);
    }


    /** 
     * @brief            Reference to element
     *
     * @param  col      Column
     * @param  lin      Line
     * @param  cha      Channel
     * @param  set      Set
     * @param  eco      Echo
     * @param  phs      Phase
     * @param  rep      Repetition
     * @param  seg      Segment
     * @param  par      Partition
     * @param  slc      Slice
     * @param  ida      Free index A
     * @param  idb      Free index B
     * @param  idc      Free index C
     * @param  idd      Free index D
     * @param  ide      Free index E
     * @param  ave      Average
     * @return          Reference to position
     */
    inline T&                 
    operator()     (const size_t col,     const size_t lin,     const size_t cha,     const size_t set,
					const size_t eco,     const size_t phs = 0, const size_t rep = 0, const size_t seg = 0,
					const size_t par = 0, const size_t slc = 0, const size_t ida = 0, const size_t idb = 0,
					const size_t idc = 0, const size_t idd = 0, const size_t ide = 0, const size_t ave = 0) NOEXCEPT {

		assert ((!col || col < _dim[ 0])&& (!lin || lin < _dim[ 1])&& (!cha || cha < _dim[ 2])&& (!set || set < _dim[ 3])&&
		  (!eco || eco < _dim[ 4])&& (!phs || phs < _dim[ 5])&& (!rep || rep < _dim[ 6])&& (!seg || seg < _dim[ 7])&&
		  (!par || par < _dim[ 8])&& (!slc || slc < _dim[ 9])&& (!ida || ida < _dim[10])&& (!idb || idb < _dim[11])&&
		  (!idc || idc < _dim[12])&& (!idd || idd < _dim[13])&& (!ide || ide < _dim[14])&& (!ave || ave < _dim[15]));

		return _M [col + lin*_dsz[ 1] + cha*_dsz[ 2] + set*_dsz[ 3] + eco*_dsz[ 4] + phs*_dsz[ 5] + rep*_dsz[ 6] +
				seg*_dsz[ 7] + par*_dsz[ 8] + slc*_dsz[ 9] + ida*_dsz[10] + idb*_dsz[11] + idc*_dsz[12] +
				idd*_dsz[13] + ide*_dsz[14] + ave*_dsz[15]];

	}

    /** 
     * @brief            Value at ...
     *
     * @param  col      Column
     * @param  lin      Line
     * @param  cha      Channel
     * @param  set      Set
     * @param  eco      Echo
     * @param  phs      Phase
     * @param  rep      Repetition
     * @param  seg      Segment
     * @param  par      Partition
     * @param  slc      Slice
     * @param  ida      Free index A
     * @param  idb      Free index B
     * @param  idc      Free index C
     * @param  idd      Free index D
     * @param  ide      Free index E
     * @param  ave      Average
     * @return          Value
     */
    inline const T&
    operator()           (const size_t col,     const size_t lin,     const size_t cha,     const size_t set,
                          const size_t eco,     const size_t phs = 0, const size_t rep = 0, const size_t seg = 0,
                          const size_t par = 0, const size_t slc = 0, const size_t ida = 0, const size_t idb = 0,
                          const size_t idc = 0, const size_t idd = 0, const size_t ide = 0, const size_t ave = 0) const NOEXCEPT {

    	assert ((!col || col < _dim[ 0])&& (!lin || lin < _dim[ 1])&& (!cha || cha < _dim[ 2])&& (!set || set < _dim[ 3])&&
    	        (!eco || eco < _dim[ 4])&& (!phs || phs < _dim[ 5])&& (!rep || rep < _dim[ 6])&& (!seg || seg < _dim[ 7])&&
    	        (!par || par < _dim[ 8])&& (!slc || slc < _dim[ 9])&& (!ida || ida < _dim[10])&& (!idb || idb < _dim[11])&&
    	        (!idc || idc < _dim[12])&& (!idd || idd < _dim[13])&& (!ide || ide < _dim[14])&& (!ave || ave < _dim[15]));

        return _M [col + lin*_dsz[ 1] + cha*_dsz[ 2] + set*_dsz[ 3] + eco*_dsz[ 4] + phs*_dsz[ 5] + rep*_dsz[ 6] +
   	               seg*_dsz[ 7] + par*_dsz[ 8] + slc*_dsz[ 9] + ida*_dsz[10] + idb*_dsz[11] + idc*_dsz[12] +
   	               idd*_dsz[13] + ide*_dsz[14] + ave*_dsz[15]];

    }


    //@}
    

    
    
#ifndef NO_LAPACK
    /**
     * @brief           Matrix product. i.e. this * M.
     *
     * @param  M        The factor.
     */
    inline Matrix<T,P>
    operator->*         (const Matrix<T,P>& M) const;

    /**
     * @brief           Matrix Product.
     *
     * @param   M       The factor
     * @param   transa  Transpose ('T') / Conjugate transpose ('C') the left matrix. Default: No transposition'N'
     * @param   transb  Transpose ('T') / Conjugate transpose ('C') the right matrix. Default: No transposition 'N'
     * @return          Product of this and M.
     */
    Matrix<T,P>
    prod                (const Matrix<T,P> &M, const char transa = 'N', const char transb = 'N') const;



    /**
     * @brief           Complex conjugate left and multiply with right.
     *
     * @param   M       Factor
     * @return          Product of conj(this) and M.
     */
    Matrix<T,P>
    prodt               (const Matrix<T,P> &M) const;


    /**
     * @brief           Scalar product (complex: conjugate first vector) using <a href="http://www.netlib.org/blas/">BLAS</a> routines XDOTC and XDOT
     *
     * @param  M        Factor
     * @return          Scalar product
     */
    inline T
    dotc (const Matrix<T,P>& M) const;


    /**
     * @brief           Scalar product using <a href="http://www.netlib.org/blas/">BLAS</a> routines XDOTU and XDOT
     *
     * @param  M        Factor
     * @return          Scalar product
     */
    inline T
    dot (const Matrix<T,P>& M) const;
    
#endif
    /**
     * @name            Dimensions
     *                  Some convenience functions to access dimensionality
     */

    //@{

    /**
     * @brief           Get number of rows, i.e. tmp = size(this); tmp(1).
     *
     * @return          Number of rows.
     */
    inline size_t
    Height              () const NOEXCEPT {
        return _dim[0];
    }


    /**
     * @brief           Get number of columns, i.e. tmp = size(this); tmp(2).
     *
     * @return          Number of columns.
     */
    inline size_t
    Width               () const NOEXCEPT {
        return (_dim.size()>1) ? _dim[1] : 1;
    }

#ifdef HAVE_SCALAPACK

    /**
     * @brief           Get number of rows, i.e. tmp = size(this); tmp(1).
     *
     * @return          Number of rows.
     */
    inline size_t
    GHeight              () const NOEXCEPT {
        return _gdim[0];
    }


    /**
     * @brief           Get number of columns, i.e. tmp = size(this); tmp(2).
     *
     * @return          Number of columns.
     */
    inline size_t
    GWidth               () const NOEXCEPT {
        return _gdim[1];
    }


    /**
     * @brief           Get number of columns, i.e. tmp = size(this); tmp(2).
     *
     * @return          Number of columns.
     */
    inline const int*
    Desc               () const NOEXCEPT {
        return _desc;
    }
#endif


    /**
     * @brief           Get resolution of a given dimension.
     *
     * @param   i       Dimension
     * @return          Resolution .
     */
    inline float
    Res                 (const size_t i) const NOEXCEPT {
        assert (i < _res.size());
        return _res[i];
    }


    /**
     * @brief           Resolution of a given dimension.
     *
     * @param   i       Dimension
     * @return          Resolution
     */
    inline float&
    Res                 (const size_t i)       NOEXCEPT {
        assert (i < _res.size());
        return _res[i];
    }



    /**
     * @brief           Resolution array
     *
     * @return          All resolutions
     */
    inline const Vector<float>&
    Res                 () const NOEXCEPT {
        return _res;
    }



    /**
     * @brief           Get size a given dimension.
     *
     * @param   i       Dimension
     * @return          Dimension
     */
    inline size_t
    Dim                 (const size_t i)  const NOEXCEPT {
        return (i < _dim.size()) ? _dim[i]: 1;
    }


    /**
     * @brief           Get dimension array
     *
     * @return          All dimensions
     */
    inline const Vector<size_t>&
    Dim                 ()                  const NOEXCEPT {
        return _dim;
    }


    inline size_t
    NDim() const NOEXCEPT {
    	return _dim.size();
    }


    /**
     * @brief           Get dimension sizes
     *
     * @return          All dimensions
     */
    inline const Vector<size_t>&
    Dsz                 ()                  const NOEXCEPT {
        return _dsz;
    }


    /**
     * @brief           Purge data and free RAM.
     */
    inline void
    Clear               ()                                      NOEXCEPT {
    	_dim.Clear();
        _dsz.Clear();
        _res.Clear();
        _M.Clear();
    }


    //@}





    /**
     * @name            Other functions.
     *                  Other functions.
     */
    
    //@{
    

    /**
     * @brief           Who are we?
     *
     * @return          Class name
     */ 
    inline const char*
    GetClassName        () const NOEXCEPT { 
        return _name.c_str(); 
    }


    /**
     * @brief           Who are we?
     *
     * @return          Class name
     */ 
    inline void
    SetClassName        (const char* name) NOEXCEPT { 
        _name = name; 
    }


    /**
     * @brief           Number of elements
     *
     * @return          Size
     */
    inline size_t
    Size () const NOEXCEPT {        
        return _M.size();
    }

    //@}


#include <functional>

/**
     * @name            Some operators
     *                  Operator definitions. Needs big expansion still.
     */

    //@{

#ifdef HAVE_CXX11_RVALUE_REFERENCES
	/**
	 * @brief  Move assignment operator
	 * @param  rhs  Right hand side reference
	 * @return      Left hand side
	 */
    inline Matrix<T,P>&
    operator= (Matrix<T,P>&& rhs) NOEXCEPT {
        _M    = std::move(rhs._M);
        _name = std::move(rhs._name);
        _res  = std::move(rhs._res);
        _dsz  = std::move(rhs._dsz);
        _dim  = std::move(rhs._dim);
        return *this;
    }
	/**
	 * @brief  Copy assignment operator
	 * @param  rhs  Right hand side
	 * @return      Left hand side
	 */
    inline Matrix<T,P>&
    operator= (const Matrix<T,P>& rhs) NOEXCEPT {
        _M    = rhs._M;
        _name = rhs._name;
        _res  = rhs._res;
        _dsz  = rhs._dsz;
        _dim  = rhs._dim;
        return *this;
    }
#endif
    /**
     * @brief           Assignment data
     *
     * @param  v        Data vector (size must match numel(M)).
     */
    inline Matrix<T,P>&
    operator=           (const Vector<T>& v) NOEXCEPT {

    	assert (_M.size() == v.size());

        if (&_M != &v)
            _M = v;

        return *this;

    }



    /**
     * @brief           Assignment operator. Sets all elements s.
     *
     * @param  s        The assigned scalar.
     */
    inline const Matrix<T,P>&
    operator=           (const T s) NOEXCEPT {
        T t = T(s);
        std::fill(_M.begin(), _M.end(), t);
        return *this;
    }


    /**
     * @brief           Unary minus (additive inverse)
     *
     * @return          Negation
     */
    inline Matrix<T,P>
    operator-           () const NOEXCEPT {
        Matrix<T,P> res (_dim);
        std::transform (_M.begin(), _M.end(), res.Begin(), std::negate<T>());
        return res;
    }


    /**
     * @brief           Unary plus
     *
     * @return          Identity
     */
    inline Matrix<T,P>
    operator+           () const NOEXCEPT {
        return *this;
    }


    /**
     * @brief           Transposition / Complex conjugation. i.e. this'.
     *
     * @return          Matrix::tr()
     */
    inline Matrix<T,P>
    operator!           () const NOEXCEPT {
    	assert(_dim.size() ==2);
    	Matrix<T,P> res = *this;
		for (size_t i = 0; i < _dim[1]; ++i)
			for (size_t j = 0; j < i; j++)
				swapd(res(j,i),res(i,j));
        return res;
    }



    /**
     * @brief           Return a matrix with result[i] = (m[i] ? this[i] : 0).
     *
     * @param  M        The operand
     * @return          Cross-section or zero
     */
    //inline Matrix<T,P>
    //operator&           (const Matrix<cbool>& M) const ;


     /**
     * @brief           Scalar inequality. result[i] = (this[i] != m). i.e. this ~= m
     *
     * @param  s        Comparing scalar.
     * @return          Matrix of false where elements are equal s and true else.
     */
    inline Matrix<cbool>
    operator!=          (const T s) const NOEXCEPT {
        Matrix<cbool> res(_dim);
        for (size_t i = 0; i < Size(); ++i)
        	res[i] = (_M[i] != s) ? 1 : 0;
        return res;
    }



    /**
     * @brief           Scalar greater comaprison, result[i] = (this[i] > m). i.e. this > m
     *
     * @param  s        Comparing scalar.
     * @return          Hit list
     */
    inline Matrix<cbool>
    operator>           (const T s) const NOEXCEPT {
        Matrix<cbool> res(_dim);
        for (size_t i = 0; i < Size(); ++i)
        	res[i] = CompTraits<T>::greater(_M[i], s);
        return res;

    }



    /**
     * @brief           Scalar greater or equal comparison. result[i] = (this[i] >= m). i.e. this >= m
     *
     * @param  s        Comparing scalar.
     * @return          Hit list
     */
    inline Matrix<cbool>
    operator>=          (const T s) const NOEXCEPT {
		Matrix<cbool> res(_dim);
        for (size_t i = 0; i < Size(); ++i)
        	res[i] = CompTraits<T>::greater_or_equal(_M[i], s);
        return res;
	}


    /**
     * @brief           Scalar minor or equal comparison. result[i] = (this[i] <= m). i.e. this <= m
     *
     * @param  s        Comparing scalar.
     * @return          Hit list
     */
    inline Matrix<cbool>
    operator<=          (const T s) const NOEXCEPT {
        Matrix<cbool> res(_dim);
        for (size_t i = 0; i < Size(); ++i)
        	res[i] = CompTraits<T>::less_or_equal(_M[i], s);
        return res;
    }


    /**
     * @brief           Scalar minor or equal comparison. result[i] = (this[i] < m). i.e. this < m
     *
     * @param  s        Comparing scalar.
     * @return          Hit list
     */
    inline Matrix<cbool>
    operator<           (const T s) const NOEXCEPT {
        Matrix<cbool> res(_dim);
        for (size_t i = 0; i < Size(); ++i)
        	res[i] = CompTraits<T>::less(_M[i], s);
        return res;
    }


    /**
     * @brief           Elementwise equality, result[i] = (this[i] != m[i]). i.e. this ~= m
     *
     * @param  M        Comparing matrix.
     * @return          Hit list
     */
    inline Matrix<cbool>
    operator!=          (const Matrix<T,P>& M) const NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        Matrix<cbool> res(_dim,_res);
        for (size_t i = 0; i < Size(); ++i)
        	res[i] = (_M[i]!= M[i]) ? 1 : 0;
        return res;
    }


    /**
     * @brief           Matrix comparison, result[i] = (this[i] >= m[i]). i.e. this >= m
     *
     * @param  M        Comparing matrix.
     * @return          Hit list
     */
    inline Matrix<cbool>
    operator>=          (const Matrix<T,P>& M) const NOEXCEPT {
    	op_assert (_dim == M.Dim(), *this, M);
        Matrix<cbool> res(_dim);
        for (size_t i = 0; i < Size(); ++i)
        	res[i] = CompTraits<T>::greater_or_equal(_M[i], M[i]);
        return res;
    }


    /**
     * @brief           Matrix comparison, result[i] = (this[i] <= m[i]). i.e. this <= m.
     *
     * @param  M        Comparing matrix.
     * @return          Hit list
     */
    inline Matrix<cbool>
    operator<=          (const Matrix<T,P>& M) const NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        Matrix<cbool> res(_dim);
        for (size_t i = 0; i < Size(); ++i)
        	res[i] = CompTraits<T>::less_or_equal(_M[i], M[i]);
        return res;
    }


    /**
     * @brief           Matrix comparison, result[i] = (this[i] > m[i]). i.e. this > m.
     *
     * @param  M        Comparing matrix.
     * @return          Hit list
     */
    inline Matrix<cbool>
    operator>           (const Matrix<T,P>& M) const NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        Matrix<cbool> res(_dim,_res);
        for (size_t i = 0; i < Size(); ++i)
        	res[i] = CompTraits<T>::greater(_M[i], M[i]);
        return res;
    }


    /**
     * @brief           Matrix comparison, result[i] = (this[i] < m[i]). i.e. this < m.
     *
     * @param  M        Comparing matrix.
     * @return          Hit list
     */
    inline Matrix<cbool>
    operator<           (const Matrix<T,P>& M) const NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        Matrix<cbool> res(_dim,_res);
        for (size_t i = 0; i < Size(); ++i)
        	res[i] = CompTraits<T>::less(_M[i], M[i]);
        return res;
    }


    /**
     * @brief           Matrix comparison, result[i] = (m[i] || this[i] ? 1 : 0). i.e. this | m.
     *
     * @param  M        Comparing matrix.
     * @return          Hit list
     */
    inline Matrix<cbool>
    operator||          (const Matrix<T,P>& M) const NOEXCEPT {
        Matrix<cbool> res(_dim);
        for (size_t i = 0; i < Size(); ++i)
        	res[i] = CompTraits<T>::logical_or(_M[i], M[i]);
        return res;
    }


    /**
     * @brief           Matrix comparison, result[i] = (m[i] || this[i] ? 1 : 0). i.e. this | m.
     *
     * @param  M        Comparing matrix.
     * @return          Hit list
     */
    inline Matrix<short>
    operator|          (const Matrix<T,P>& rhs) const NOEXCEPT {
        Matrix<cbool> ret(_dim);
        for (size_t i = 0; i < Size(); ++i)
        	ret[i] = (short)CompTraits<T>::logical_or(_M[i], ret[i]);
        return ret;
    }


    /**
     * @brief           Matrix comparison, result[i] = (m[i] && this[i] ? 1 : 0). i.e. this & m.
     *
     * @param  M        Comparing matrix.
     * @return          Hit list
     */
    inline Matrix<cbool>
    operator&&          (const Matrix<T,P>& M) const NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        Matrix<cbool> res(_dim);
        for (size_t i = 0; i < Size(); ++i)
        	res[i] = CompTraits<T>::logical_and(_M[i], M[i]);
        return res;
    }


    /**
     * @brief           Elementwise raise of power. i.e. this .^ p.
     *
     * @param  p        Power.
     * @return          Result
     */
    inline Matrix<T,P>
    operator^           (const float p) const NOEXCEPT {
    	Matrix<T,P> res = *this;
		for (size_t i = 0; i < Size(); ++i)
			res[i] = (p == 0) ? T(1) : TypeTraits<T>::Pow(res[i],  p);
        return res;
    }


    /**
     * @brief           Elementwise raise of power. i.e. this .^ p.
     *
     * @param  p        Power.
     * @return          Result
     */
    inline Matrix<T,P>&
    operator^=          (const float p) NOEXCEPT {
		for (size_t i = 0; i < Size(); ++i)
			_M[i] = TypeTraits<T>::Pow(_M[i],  p);
        return *this;
    }


    /**
     * @brief           Elementwise addition iof all elements with a scalar
     *
     * @param  s        Scalar additive.
     */
    template <class S>
    inline Matrix<T,P>
    operator+           (const S s) const NOEXCEPT {
        Matrix<T,P> res = *this;
        return res += s;
    }


    /**
     * @brief           ELementwise multiplication and assignment operator. i.e. this = this .* M.
     *
     * @param  M        Factor matrix.
     * @return          Result
     */
    inline Matrix<T,P>&
    operator+=         (const Matrix<T,P>& M) NOEXCEPT {
    	op_assert (_dim == M.Dim(), *this, M);
   		std::transform (_M.begin(), _M.end(), M.Begin(), _M.begin(), std::plus<T>());
        return *this;
    }


	/**
	 * @brief           ELementwise multiplication and assignment operator. i.e. this = m.
	 *
	 * @param  M        Added matrix.
	 * @return          Result
	 */
    template <class S> inline Matrix<T,P>&
    operator+=         (const Matrix<S,P>& M) NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        std::transform (_M.begin(), _M.end(), M.Begin(), _M.begin(), std::plus<T>());
		return *this;
    }


    /**
     * @brief           ELementwise addition with scalar and assignment operator. i.e. this = m.
     *
     * @param  s        Added scalar.
     * @return          Result
     */
    template <class S > inline Matrix<T,P>&
    operator+=          (const S s) NOEXCEPT {
        std::transform (_M.begin(), _M.end(), _M.begin(), std::bind2nd(std::plus<T>(),(T)s));
        return *this;
    }

    /**
     * @brief           Elementwise subtraction all elements by a scalar
     *
     * @param  s        Scalar substruent.
     */
    template <class S> inline Matrix<T,P>
    operator-           (const S s) const NOEXCEPT {
		Matrix<T,P> res = *this;
		return res -= s;
    }



    /**
     * @brief           ELementwise multiplication and assignment operator. i.e. this = this .* M.
     *
     * @param  M        Factor matrix.
     * @return          Result
     */
    inline Matrix<T,P>&
    operator-=         (const Matrix<T,P>& M) NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        std::transform (_M.begin(), _M.end(), M.Begin(), _M.begin(), std::minus<T>());
        return *this;
    }


    /**
     * @brief           ELementwise multiplication and assignment operator. i.e. this = m.
     *
     * @param  M        Added matrix.
     * @return          Result
     */
    template <class S> inline Matrix<T,P>&
    operator-=          (const Matrix<S,P>& M) NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        std::transform (_M.begin(), _M.end(), M.Begin(), _M.begin(), std::minus<T>());
        return *this;
    }

    /**
     * @brief           ELementwise substration with scalar and assignment operator. i.e. this = m.
     *
     * @param  s        Added scalar.
     * @return          Result
     */
    template <class S> inline Matrix<T,P>&
    operator-=          (const S s) NOEXCEPT {
        std::transform (_M.begin(), _M.end(), _M.begin(), std::bind2nd(std::minus<T>(),(T)s));
		return *this;
    }

    /**
     * @brief           Elementwise multiplication. i.e. this .* M.
     *
     * @param  M        Factor matrix.
     * @return          Result
     */
    /*inline Matrix<T,P>
    operator*          (const Matrix<T,P> &M) const NOEXCEPT {
		Matrix<T,P> res = *this;
		return res *= M;
    }*/


    /**
     * @brief           Elementwise multiplication. i.e. this .* M.
     *
     * @param  M        Factor matrix.
     * @return          Result
     */
    /*template <class S>
    inline Matrix<T,P>
    operator*          (const Matrix<S,P> &M) const NOEXCEPT {
		Matrix<T,P> res = *this;
		return res *= M;
    }*/


    /**
     * @brief           Elementwise multiplication with a scalar. i.e. this * m.
     *
     * @param  s        Factor scalar
     * @return          Result
     */
    template <class S>
    inline Matrix<T,P>
    operator*          (const S s) const  NOEXCEPT {
        Matrix<T,P> res = *this;
        return res *= s;
    }



    /**
     * @brief           ELementwise multiplication and assignment operator. i.e. this = this .* M.
     *
     * @param  M        Factor matrix.
     * @return          Result
     */
    inline Matrix<T,P>&
    operator*=         (const Matrix<T,P>& M) NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        std::transform (_M.begin(), _M.end(), M.Begin(), _M.begin(), std::multiplies<T>());
        return *this;
    }


    /**
     * @brief           ELementwise multiplication and assignment operator. i.e. this = this .* M.
     *
     * @param  M        Factor matrix.
     * @return          Result
     */
    template <class S> inline Matrix<T,P>&
    operator*=         (const Matrix<S,P>& M) NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        std::transform (_M.begin(), _M.end(), M.Begin(), _M.begin(), std::multiplies<T>());
        return *this;
    }


    /**
     * @brief           ELementwise multiplication with scalar and assignment operator. i.e. this *= s.
     *
     * @param  s        Factor scalar.
     * @return          Result
     */
    template <class S> inline Matrix<T,P>&
    operator*=         (const S s) NOEXCEPT {
        std::transform (_M.begin(), _M.end(), _M.begin(), std::bind2nd(std::multiplies<T>(),(T)s));
		return *this;
    }


    /**
     * @brief           Elementwise division by scalar. i.e. this * m.
     *
     * @param  s        The divisor.
     * @return          Result
     */
    template <class S>
    inline Matrix<T,P>
    operator/           (const S s) const NOEXCEPT {
		Matrix<T,P> res = *this;
		return res /= s;
	}


    /**
     * @brief           ELementwise multiplication and assignment operator. i.e. this = this .* M.
     *
     * @param  M        Factor matrix.
     * @return          Result
     */
    inline Matrix<T,P>&
    operator/=         (const Matrix<T,P>& M) NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        std::transform (_M.begin(), _M.end(), M.Begin(), _M.begin(), std::divides<T>());
        return *this;
    }


    /**
     * @brief           ELementwise division and assignment operator. i.e. this = this ./ M.
     *
     * @param  M        Divisor matrix.
     * @return          Result
     */
    template <class S> inline Matrix<T,P>&
    operator/=         (const Matrix<S,P> &M) NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        std::transform (_M.begin(), _M.end(), M.Begin(), _M.begin(), std::divides<T>());
        return *this;
    }



    /**
     * @brief           ELementwise multiplication with scalar and assignment operator. i.e. this = m.
     *
     * @param  s        Divisor scalar.
     * @return          Result
     */
    template <class S>
    inline Matrix<T,P>&
    operator/=         (const S s) NOEXCEPT {
        std::transform (_M.begin(), _M.end(), _M.begin(), std::bind2nd(std::divides<T>(),(T)s));
        return *this;
    }


    //@}


    /**
     * @name            Friend operators
     *                  Who doesn't need friends
     */

    //@{


    //--
    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator*  (const double s, const Matrix<T,P>& m) NOEXCEPT {
        return   m * s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator*  (const float s, const Matrix<T,P> &m) NOEXCEPT {
        return   m * s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator*  (const short s, const Matrix<T,P> &m) NOEXCEPT {
        return   m * s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator*  (const int s, const Matrix<T,P> &m) NOEXCEPT {
        return   m * s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator*  (const long s, const Matrix<T,P> &m) NOEXCEPT {
        return   m * s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator*  (const cxfl s, const Matrix<T,P> &m) NOEXCEPT {
        return   m * s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator*  (const cxdb s, const Matrix<T,P> &m) NOEXCEPT {
        return   m * s;
    }


    //--
    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator+  (const double s, const Matrix<T,P> &m) NOEXCEPT {
        return   m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator+  (const float s, const Matrix<T,P> &m) NOEXCEPT {
        return   m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator+  (const short s, const Matrix<T,P> &m) NOEXCEPT {
        return   m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator+  (const int s, const Matrix<T,P> &m) NOEXCEPT {
        return   m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator+  (const long s, const Matrix<T,P> &m) NOEXCEPT {
        return   m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator+  (const cxfl s, const Matrix<T,P> &m) NOEXCEPT {
        return   m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator+  (const cxdb s, const Matrix<T,P> &m) NOEXCEPT {
        return   m + s;
    }


    //--
    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator-  (const double s, const Matrix<T,P> &m) NOEXCEPT {
        return -m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator-  (const float s, const Matrix<T,P> &m) NOEXCEPT {
        return -m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator-  (const short s, const Matrix<T,P> &m) NOEXCEPT {
        return -m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator-  (const int s, const Matrix<T,P> &m) NOEXCEPT {
        return -m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator-  (const long s, const Matrix<T,P> &m) NOEXCEPT {
        return   -m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator-  (const cxfl s, const Matrix<T,P> &m) NOEXCEPT {
        return   -m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator-  (const cxdb s, const Matrix<T,P> &m) NOEXCEPT {
        return   -m + s;
    }


    /**
     * @brief           Elementwise multiplication with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m * s
     */
    inline friend Matrix<T,P>
    operator/  (const T s, const Matrix<T,P> &m) NOEXCEPT {
        Matrix<T,P> res = m;
        for (size_t i = 0; i < m.Size(); ++i)
            res[i] = s / res[i];
        return res;
    }

    /**
     * @brief           Elementwise equality with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m == s
     */
    inline friend Matrix<cbool>
    operator== (const T s, const Matrix<T,P>& m) NOEXCEPT {
        return   m == s;
    }


    /**
     * @brief           Elementwise >= with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          m <= t
     */
    inline friend Matrix<cbool>
    operator>= (const T s, const Matrix<T,P>& m) NOEXCEPT {
        return   m <= s;
    }


    /**
     * @brief           Elementwise <= with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          T<=M
     */
    inline friend Matrix<cbool>
    operator<= (const T s, const Matrix<T,P>& m) NOEXCEPT {
        return   m >= s;
    }


    /**
     * @brief           Elementwise unequality with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          T!=M
     */
    inline friend Matrix<cbool>
    operator!= (const T s, const Matrix<T,P>& m) NOEXCEPT {
        return   m != s;
    }


    /**
     * @brief           Elementwise equality with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          T+M
     */
    inline friend Matrix<cbool>
    operator>  (const T s, const Matrix<T,P>& m) NOEXCEPT {
        return   m <  s;
    }


    /**
     * @brief           Elementwise < with scalar (lhs)
     *
     * @param  s        Scalar lhs
     * @param  m        Matrix rhs
     * @return          T+M
     */
    inline friend Matrix<cbool>
    operator<  (const T s, const Matrix<T,P>& m) NOEXCEPT {
        return   m >  s;
    }


    /**
     * @brief           Elementwise equality with scalar (lhs)
     *
     * @param  mb       Scalar lhs
     * @param  m        Matrix rhs
     * @return          T+M
     */
    //inline friend Matrix<T,P>
    //operator&  (const Matrix<cbool>& mb, const Matrix<T,P>& m) {
    //    return   m & mb;
// }

    //@}


    /**
     * @brief           Elementwise equality, result[i] = (this[i] == m[i]). i.e. this == m
     *
     * @param  M        Comparing matrix.
     * @return          Hit list
     */
    inline Matrix<cbool>
    operator==          (const Matrix<T,P>& M) const NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
        Matrix<cbool> res(_dim);
		for (size_t i = 0; i < Size(); ++i)
			res[i] = (_M[i] == M[i]) ? 1 : 0;
        return res;

    }


    /**
	 * @brief           Elementwise equality, result[i] = (this[i] == m[i]). i.e. this == m
	 *
	 * @param  M        Comparing matrix.
	 * @return          Hit list
	 */
    template<class S>
	inline Matrix<cbool>
	operator==          (const Matrix<S,P>& M) const NOEXCEPT {
        op_assert (_dim == M.Dim(), *this, M);
		Matrix<cbool> res (_dim);
		for (size_t i = 0; i < Size(); ++i)
			res[i] = (_M[i] == (T)M[i]) ? 1 : 0;
		return res;
     }

    /**
     * @brief           Scalar equality. result[i] = (this[i] == m).
     *
     * @param  s        Comparing scalar.
     * @return          Matrix of true where elements are equal s and false else.
     */
    inline Matrix<cbool>
    operator==          (const T s) const NOEXCEPT {
    	T t = (T) s;
        Matrix<cbool> res (_dim);
		for (size_t i = 0; i < Size(); ++i)
			res[i] =  (_M[i] == s) ? 1 : 0;
        return res;
    }

    inline Matrix<T>
    operator() (const std::string& range) const {
    	Matrix<T> ret;
    	Vector<Vector<size_t> > view;

    	try {
    		view = RangeParser (range, _dim);
    	} catch (const RangeParseException&) {
    		std::cout << "  ** Bailing out **" << std::endl;
    		assert (false);
    	}

    	if (view.size() == 1) {
    		ret = Matrix<T>(view[0].size(),1);
    		for (size_t i = 0; i < view[0].size(); ++i)
    			ret[i] = _M[i];
    	} else if (view.size() == 2) {
            ret = Matrix<T>(view[0].size(),view[1].size());
    		for (size_t j = 0; j < view[1].size(); ++j)
                for (size_t i = 0; i < view[0].size(); ++i)
                    ret (i,j) = (*this)(view[0][i],view[1][j]);
        } else if (view.size() == 3) {
            ret = Matrix<T>(view[0].size(),view[1].size(),view[2].size());
    		for (size_t k = 0; k < view[2].size(); ++k)
                for (size_t j = 0; j < view[1].size(); ++j)
                    for (size_t i = 0; i < view[0].size(); ++i)
                        ret (i,j,k) = (*this)(view[0][i],view[1][j],view[2][k]);
        } else if (view.size() == 4) {
            ret = Matrix<T>(view[0].size(),view[1].size(),view[2].size(),view[3].size());
    		for (size_t l = 0; l < view[3].size(); ++l)
                for (size_t k = 0; k < view[2].size(); ++k)
                    for (size_t j = 0; j < view[1].size(); ++j)
                        for (size_t i = 0; i < view[0].size(); ++i)
                            ret (i,j,k,l) = (*this)(view[0][i],view[1][j],view[2][k],view[3][l]);
        } else if (view.size() == 5) {
            ret = Matrix<T>(view[0].size(),view[1].size(),view[2].size(),view[3].size(),view[4].size());
    		for (size_t m = 0; m < view[4].size(); ++m)
                for (size_t l = 0; l < view[3].size(); ++l)
                    for (size_t k = 0; k < view[2].size(); ++k)
                        for (size_t j = 0; j < view[1].size(); ++j)
                            for (size_t i = 0; i < view[0].size(); ++i)
                                ret (i,j,k,l,m) = (*this)(view[0][i],view[1][j],view[2][k],view[3][l],view[4][l]);
        } else if (view.size() == 6) {
            ret = Matrix<T>(view[0].size(),view[1].size(),view[2].size(),view[3].size(),view[4].size(),view[5].size());
    		for (size_t n = 0; n < view[5].size(); ++n)
                for (size_t m = 0; m < view[4].size(); ++m)
                    for (size_t l = 0; l < view[3].size(); ++l)
                        for (size_t k = 0; k < view[2].size(); ++k)
                            for (size_t j = 0; j < view[1].size(); ++j)
                                for (size_t i = 0; i < view[0].size(); ++i)
                                    ret (i,j,k,l,m,n) = (*this)(view[0][i],view[1][j],view[2][k],view[3][l],view[4][l],view[5][n]);
        }
        
    	return ret;
        
    }


protected:
	
    /**
     * @brief           Number of elements
     *
     * @return          Size
     */
    inline size_t
    DimProd () const NOEXCEPT {
      return std::accumulate(_dim.begin(), _dim.end(), size_t(1), c_multiply<size_t>);
    }


    /**
     * @brief          Allocate RAM
     */
    inline void
    Allocate () NOEXCEPT {
        size_t ds = _dim.size(), i;
		_dsz.resize(ds,1);
	    for (i = 1; i < ds; ++i)
	        _dsz[i] = _dsz[i-1]*_dim[i-1];
        size_t  n = DimProd(); 
        if (n != _M.size())
            _M.resize(n);
    }


    // Structure
    Vector<size_t> _dim; /// Dimensions
    Vector<size_t> _dsz; /// Dimension size.
    Vector<float>  _res; /// Resolutions
    

    //Data
    Vector<T>        _M; /// Data container


    // Name
    std::string         _name; /// Name
    
#ifdef HAVE_SCALAPACK
    // BLACS 
	int              _bs;
	int              _desc[9]; /**< @brief matrix grid vector */
	int              _gdim[2]; /**< @brief Global dimensions */
#endif
    
};

/*
template<class T>
class MatrixProxy {
public:
    MatrixProxy (Matrix<T>& M) : _Mptr(&M) {}
    virtual ~MatrixProxy () { _Mptr = 0; }
    //friend operator= (const Matrix<T>& M) { *_Mptr = M; }
private:
    MatrixProxy () : _Mptr(0) {}
    Matrix<T>* _Mptr;
}
*/
#endif // __MATRIX_H__


