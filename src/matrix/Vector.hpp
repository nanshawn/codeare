/*
 * Vector.hpp
 *
 *  Created on: May 28, 2013
 *      Author: kvahed
 */

#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <iostream>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <numeric>
#include <vector>

#if (_MSC_VER >= 1300) && (WINVER < 0x0500) && !defined(_ftol)
#ifndef _ftol
extern "C" inline long _ftol( double d) { return (long) d;}
#endif
#endif

#if defined (_MSC_VER) && _MSC_VER<1300
#    define VECTOR_TYPE(A) std::vector<A>
#    define VECTOR_CONSTR(A,B) std::vector<A>(B)
#    define VECTOR_CONSTR_VAL(A,B,C) std::vector<A>(B,C)
#else
#    if defined USE_VALARRAY
#        include <valarray>
#        define VECTOR_TYPE(A) std::valarray<A>
#        define VECTOR_CONSTR(A,B) std::valarray<A>(B)
#    else
#        include "simd/AlignmentAllocator.hpp"
#        if defined __AVX__
#            define ALIGNEMENT 32
#        else
#            define ALIGNEMENT 16
#        endif
#        define VECTOR_TYPE(A) std::vector<A,AlignmentAllocator<A,ALIGNEMENT> >
#        define VECTOR_CONSTR(A,B) std::vector<A,AlignmentAllocator<A,ALIGNEMENT> >(B)
#        define VECTOR_CONSTR_VAL(A,B,C) std::vector<A,AlignmentAllocator<A,ALIGNEMENT> >(B,C)
#    endif
#endif
template<class T> inline static std::ostream&
operator<< (std::ostream& os, const std::vector<T> v) {
    for (size_t i = 0; i < v.size(); ++i)
        os << v[i] << " ";
    return os;
}

typedef unsigned char cbool;

/**
 * @brief   Memory paradigm (share, opencl or message passing)
 */
enum    paradigm {

  SHM, /**< @brief Shared memory (Local RAM) */
  OCL, /**< @brief Open CL GPU RAM */
  MPI  /**< @brief Distributed memory */

};


/**
 * @brief Alligned data Vector for Matrix<T>
 */
template <class T, paradigm P=SHM> class Vector {
public:

    /**
     * @brief convenience typedefs 
     */
	typedef typename VECTOR_TYPE(T)::iterator iterator;
	typedef typename VECTOR_TYPE(T)::const_iterator const_iterator;

    /**
     * @brief Default constructor
     */
	explicit inline Vector () {}

    /**
     * @brief Construct with size
     * @bparam  n  New size
     */
	explicit inline Vector (const size_t n) { _data = VECTOR_CONSTR (T,n); }

    /**
     * @brief Construct with size and preset value
     * @param  n  New size
     * @param  val Preset value
     */
	explicit inline Vector (const size_t n, const T& val) { _data = VECTOR_CONSTR_VAL(T,n,val); }

    /**
     * @brief Copy constructor from different type
     * @param  cs  To copy
     */
	template<class S> inline Vector (const Vector<S>& cs) {
		_data.resize(cs.size());
		for (size_t i = 0; i < _data.size(); ++i)
			_data[i] = (T)cs[i];
	}

#ifdef HAVE_CXX11_RVALUE_REFERENCES
	inline Vector (const Vector<T>& other) : _data(other._data) {}
	inline Vector (Vector<T>&& other) : _data(std::move(other._data)) {}
	inline Vector& operator= (const Vector<T>& other) {
		if (this != &other)
			_data = other._data;
		return *this;
	}
	inline Vector& operator= (Vector<T>&& other) {
		if (this != &other)
			_data = std::move(other._data);
		return *this;
	}
#endif

	/**
     * @brief Elementwise access (lhs)
     * @param  n  n-th element 
     */
	inline T& operator[] (const size_t n) { return _data[n]; }
    /**
     * @brief Elementwise access (rhs)
     * @param  n  n-th element 
     */
	inline const T& operator[] (const size_t n) const { return _data[n]; }

    /**
     * @brief Access last element (lhs)
     */
	inline T& back () { return _data.back(); }
    /**
     * @brief Access last element (rhs)
     */
	inline const T& back () const { return _data.back; }

    /**
     * @brief Access first element (lhs)
     */
	inline T& front () { return _data.front(); }
    /**
     * @brief Access first element (rhs)
     */
	inline const T& front () const { return _data.front; }

    /**
     * @brief Access RAM address (lhs)
     * @param  n  at n-th element (default 0)
     */
	inline T* ptr (const size_t n = 0) { return &_data[n]; }
    /**
     * @brief Access RAM address (rhs)
     * @param  n  at n-th element (default 0)
     */
	inline const T* ptr (const size_t n = 0) const { return &_data[n]; }

    /**
     * @brief Access data vector (lhs)
     */
	inline VECTOR_TYPE(T)& data() { return _data; }
    /**
     * @brief Access data vector (rhs)
     */
	inline const VECTOR_TYPE(T)& data() const { return _data; }

    /**
     * @brief Vector size
     */
	inline size_t size() const { return _data.size(); }

    /**
     * @brief Iterator at start of vector (lhs)
     */
	inline iterator begin() { return _data.begin(); }
    /**
     * @brief Iterator at start of vector (rhs)
     */
	inline const_iterator begin() const { return _data.begin(); }

    /**
     * @brief Iterator at end of vector (lhs)
     */
	inline iterator end() { return _data.end(); }
    /**
     * @brief Iterator at end of vector (rhs)
     */
	inline const_iterator end() const { return _data.end(); }

    /**
     * @brief resize data storage
     */
	inline void resize (const size_t n) {
		if (!(n==_data.size()))
			_data.resize(n);
	}

    /**
     * @brief resize data storage
     */
	inline void resize (const size_t n, const T val) {
		if (!(n==_data.size()))
			_data.resize(n,val);
		else
			_data.assign(n,val);
	}

    /**
     * @brief Add elemet at end
     * @param t  Element to be added
     */
	inline void push_back (const T& t) { _data.push_back(t);}

	inline void Clear() {_data.clear();}

	inline bool Empty() const {return _data.empty();}

	inline bool operator== (const Vector<T>& other) const {return _data == other._data;}

	inline void PopBack () {_data.pop_back();}
	inline void PushBack (const T& t) {_data.push_back(t);}
	template<class S> inline Vector<T>& operator/= (const S& s) {
		std::transform (_data.begin(), _data.end(), _data.begin(), std::bind2nd(std::divides<T>(),(T)s));
		return *this;
	}
	template<class S> inline Vector<T>& operator/= (const Vector<S>& v) {
		std::transform (_data.begin(), _data.end(), v.begin(), _data.begin(), std::divides<T>());
		return *this;
	}
	template<class S> inline Vector<T> operator/ (const S& s) const {
		Vector<T> ret = *this;
		return ret/=s;
	}

private:
	VECTOR_TYPE(T) _data;
};

template<class T> inline static size_t numel (const Vector<T>& v) {return v.size();}

template <class T> class vector_inserter {
public:
    Vector<T>& _ct;
    vector_inserter (Vector<T>& ct):_ct(ct) {}
    inline vector_inserter& operator, (const T& val) {_ct.push_back(val);return *this;}
};
template <class T> inline vector_inserter<T>& operator+= (Vector<T>& ct,const T& x) {
    return vector_inserter<T>(ct),x;
}


template<class T> inline T ct_real (const std::complex<T> ct) {return ct.real();}
template<class T> inline T ct_imag (const std::complex<T> ct) {return ct.imag();}
template<class T> inline T ct_conj (const T ct) {return std::conj(ct);}

template<class T> inline static Vector<T>
real (const Vector<std::complex<T> >& c) {
	Vector<T> res (c.size());
	std::transform (c.begin(), c.end(), res.begin(), ct_real<T>);
	return res;
}
template<class T> inline static Vector<T>
imag (const Vector<std::complex<T> >& c) {
	Vector<T> res (c.size());
	std::transform (c.begin(), c.end(), res.begin(), ct_imag<T>);
	return res;
}
template<class T> inline static Vector<T>
conj (const Vector<T>& c) {
	Vector<T> res (c.size());
	std::transform (c.begin(), c.end(), res.begin(), ct_conj<T>);
	return res;
}
template<class T> inline std::ostream&
operator<< (std::ostream& os, const Vector<T>& ct) {
    for (typename Vector<T>::const_iterator it = ct.begin(); it != ct.end(); ++it)
        os << *it << " ";
    return os;
}
template<class T> inline static T multiply (const T& a, const T& b) {
    return a*b;
}
template<class T> inline static T prod (const Vector<T>& ct) {
	return std::accumulate(ct.begin(), ct.end(), (T)1, multiply<T>);
}
template<class T> inline static T sum (const Vector<T>& ct) {
	return std::accumulate(ct.begin(), ct.end(), (T)0);
}
template<class T> inline static T mmax (const Vector<T>& ct) {
	return *std::max_element(ct.begin(), ct.end());
}


template<class T> inline static void swapd (T& x,T& y) {T temp=x; x=y; y=temp;}

#endif /* Vector_HPP_ */