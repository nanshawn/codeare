/*
 *  codeare Copyright (C) 2007-2010 Kaveh Vahedipour
 *                               Forschungszentrum Juelich, Germany
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

#ifndef __DFT_HPP__
#define __DFT_HPP__

#include "Matrix.hpp"
#include "Algos.hpp"
#include "FT.hpp"
#include "FFTWTraits.hpp"

#include <iterator>
#include "Access.hpp"

template<class T> inline static Matrix<T> fftshift (const Matrix<T>& in, const size_t& dim,
		bool fwd) NOEXCEPT {

	typedef typename TypeTraits<T>::RT RT;
	size_t howmany;
	Matrix<T> ret, tmp;
	Vector<size_t> order;
	size_t cent, ndims, n;

	if (dim > 0) {
		Vector<size_t> in_dims = size(in);
		ndims = numel(in_dims);
		assert(ndims>=dim);
		for (size_t i = 0; i < ndims; ++i)
			order.push_back(i);
		order.erase(order.begin()+dim);
		order.insert(order.begin(), dim);
		ret = permute (in, order);
		in_dims.erase(in_dims.begin()+dim);
	} else {
		ret = in;
	}

	howmany = numel(in)/size(in,0);
	n = size(ret,0);
	cent = (fwd) ? round((RT)n/2) : floor((RT)n/2);
	for (auto i = ret.Begin(); i < ret.End(); i += size(ret,0))
		std::rotate(i, i+cent, i+n);

	if (dim > 0) {
		for (size_t i = 0; i < ndims; ++i)
			order[i] = i;
		order.erase(order.begin());
		order.insert(order.begin()+dim, 0);
		return permute (ret, order);
	} else {
		return ret;
	}

}

template<class T> inline static Matrix<T>
fftshift (const Matrix<T>& in, const size_t& dim = 0) NOEXCEPT {
	return fftshift(in, dim, true);
}
template<class T> inline static Matrix<T>
ifftshift (const Matrix<T>& in, const size_t& dim = 0) NOEXCEPT {
	return fftshift(in, dim, false);
}
template<class T> inline static Matrix<T>
fftshift (const View<T,true>& in, const size_t& dim = 0) NOEXCEPT {
	Matrix<T> inn = in;
	return fftshift(inn, dim, true);
}
template<class T> inline static Matrix<T>
ifftshift (const View<T,true>& in, const size_t& dim = 0) NOEXCEPT {
	Matrix<T> inn = in;
	return fftshift(inn, dim, false);
}


template<class T> inline static Matrix<T> fft (const Matrix<T>& in, size_t dim, bool shift, bool fwd) NOEXCEPT {

	typedef typename TypeTraits<T>::RT RT;
	Matrix<T> ret, tmp;
	Vector<size_t> order;
	size_t ndims;

	if (dim > 0) {
		Vector<size_t> in_dims = size(in);
		size_t cent = floor((float)in_dims[dim]/2);
		ndims = numel(in_dims);
		assert(ndims>=dim);

		for (size_t i = 0; i < ndims; ++i)
			order.push_back(i);
		order.erase(order.begin()+dim);
		order.insert(order.begin(), dim);
		ret = permute (in, order);
		in_dims.erase(in_dims.begin()+dim);
	} else {
		ret = in;
	}

	int n = static_cast<int>(size(ret,0));
	int n2 = floor((RT)n/2);
	int howmany = numel(ret)/n;

	if (shift)
		for (auto os = ret.Begin(); os < ret.End(); os += n)
			std::rotate(os, os+n2, os+n);

	// 1d-fft
	typedef typename FTTraits<T>::Plan FTPlan;
	typedef typename FTTraits<T>::T FTType;
    FTPlan cp = FTTraits<T>::DFTPlanMany (1, &n, howmany,
          (FTType*)ret.Ptr(), (FTType*)ret.Ptr(), (fwd) ? FFTW_FORWARD : FFTW_BACKWARD);
    FTTraits<T>::Execute(cp);
    FTTraits<T>::Destroy(cp);
    ret /= n;
    
    n2 = round((RT)n/2);
    if (shift)
    	for (auto os = ret.Begin(); os < ret.End(); os += n)
    		std::rotate(os, os+n2, os+n);

	if (dim > 0) {
		for (size_t i = 0; i < ndims; ++i)
			order[i] = i;
		order.erase(order.begin());
		order.insert(order.begin()+dim, 0);
		if (fwd)
			return permute (ret, order)*sqrt((RT)numel(in));
		else
			return permute (ret, order);
	} else {
		if (fwd)
			return ret*sqrt((RT)numel(in));
		else
			return ret;
	}


}

template<class T> inline static Matrix<typename TypeTraits<T>::CT>
fft (const Matrix<T>& in, size_t dim = 0, bool shift = true) NOEXCEPT {
	typedef typename TypeTraits<T>::CT CT;
	return fft<CT>(in, dim, shift, true);
}
template<class T> inline static Matrix<typename TypeTraits<T>::CT>
ifft (const Matrix<T>& in, size_t dim = 0, bool shift = true) NOEXCEPT {
	typedef typename TypeTraits<T>::CT CT;
	return fft<CT>(in, dim, shift, false);
}
template<class T> inline static Matrix<typename TypeTraits<T>::CT>
fft (const View<T,true>& in, size_t dim = 0, bool shift = true) NOEXCEPT {
	typedef typename TypeTraits<T>::CT CT;
	Matrix<CT> inn(size(in));
	for (size_t i =0; i < numel(inn); ++ i)
		inn[i] = in[i];
	return fft(inn, dim, shift, true);
}
template<class T> inline static Matrix<typename TypeTraits<T>::CT>
ifft (const View<T,true>& in, size_t dim = 0, bool shift = true) NOEXCEPT {
	typedef typename TypeTraits<T>::CT CT;
	Matrix<CT> inn(size(in));
	for (size_t i =0; i < numel(inn); ++ i)
		inn[i] = in[i];
	return fft(inn, dim, shift, false);
}


/**
 * @brief         Hann window
 * 
 * @param   size  Side lengths
 * @param   t     Scaling factor
 * @return        Window
 */
template <class T> inline Matrix< std::complex<T> >
hannwindow (const Matrix<size_t>& size, const T& t) NOEXCEPT {
	
	size_t dim = size.Dim(0);
	assert (dim > 1 && dim < 4);
	
	Matrix<double> res;
	
	if      (dim == 1) res = Matrix<double> (size[0], 1);
	else if (dim == 2) res = Matrix<double> (size[0], size[1]);
	else               res = Matrix<double> (size[0], size[1], size[2]);
	
	float          h, d;
	float          m[3];
	
	if (isvec(res)) {
		
		m[0] = 0.5 * size[0];
		m[1] = 0.0;
		m[2] = 0.0;
		
	} else if (is2d(res)) {
		
		m[0] = 0.5 * size[0];
		m[1] = 0.5 * size[1];
		m[2] = 0.0;
		
	} else {
		
		m[0] = 0.5 * size[0];
		m[1] = 0.5 * size[1];
		m[2] = 0.5 * size[2];
		
	}
	
	res = squeeze(res);
	
	for (size_t s = 0; s < res.Dim(2); s++)
		for (size_t r = 0; r < res.Dim(1); r++)
			for (size_t c = 0; c < res.Dim(0); c++) {
				d = pow( (float)pow(((float)c-m[0])/m[0],2) + pow(((float)r-m[1])/m[1],2) + pow(((float)s-m[2])/m[2],2) , (float)0.5);
				h = (d < 1) ? (0.5 + 0.5 * cos (PI * d)) : 0.0;
				res(c,r,s) = t * h;
			}
	
	return res;
	
}


/**
 * @brief Matrix templated 1-3D Discrete Cartesian Fourier transform
 */
template <class T=std::complex<float> >
class DFT : public FT<T> {

	typedef typename FTTraits<T>::Plan Plan;
	typedef typename FTTraits<T>::T FTT;
	typedef typename FTTraits<T>::RT RT;
	
public:
	
	/**
	 * @brief        Construct FFTW plans for forward and backward FT with credentials
	 *
	 * @param  sl    Matrix of side length of the FT range
	 * @param  mask  K-Space mask (if left empty no mask is applied)
	 * @param  pc    Phase correction (or target phase)
	 * @param  b0    Field distortion
	 */
	explicit DFT (const Vector<size_t>& sl, const Matrix<RT>& mask = Matrix<RT>(1),
				 const Matrix<T>& pc = Matrix<T>(1), const Matrix<RT>& b0 = Matrix<RT>(1)) NOEXCEPT :
				 	 m_N(1), m_have_mask (false), m_have_pc (false), m_threads(1) {

		size_t rank = numel(sl);

		Vector<int> n (rank);

		if (numel(mask) > 1) {
			m_have_mask = true;	m_mask = mask;
		}

		if (numel(pc)   > 1) {
			m_have_pc = true; m_pc = pc; m_cpc = conj(pc);
		}

		for (size_t i = 0; i < rank; i++)
			n[i]  = (int) sl [rank-1-i];

		m_N = std::accumulate(n.begin(), n.end(), 1, std::multiplies<int>());

		Vector<float> tmp = sl;
		tmp.resize(3);
		for (size_t i = 0; i < 3; ++i)
			tmp[i] = (tmp[i] > 0) ? tmp[i] : 1;

		d = tmp; // data side lengths
		c = d; 
        for (size_t i = 0; i < c.size(); ++i)
            c[i] /= 2;

 		Allocate (rank, &n[0]);

		m_initialised = true;

	}

	DFT        (const Params& p) NOEXCEPT :
		FT<T>::FT(p), m_cs(0), m_N(0), m_in(0), m_have_pc(false), m_zpad(false),
		m_initialised(false), m_have_mask(false), m_threads(1) {

		size_t rank;
		Vector<int> n;
        
		if (p.exists("dims")) {
			try {
				n = (Vector<int>)p.Get<Vector<size_t> >("dims");
				rank = n.size();
			} catch (const boost::bad_any_cast& e){
				printf("**ERROR - DFT: cannot interpret dimensions vector (Vector<size_t>)\n%s\n", e.what());
			}
		} else if (p.exists("rank") && p.exists("dim")) {
			int dim;
			try {
				rank = unsigned_cast (p["rank"]);
			} catch (const boost::bad_any_cast& e) {
				printf ("**ERROR - DFT: cannot interpret FT rank.\n%s\n", e.what());
				assert (false);
			}
			try {
				dim = unsigned_cast (p["dim"]);
			} catch (const boost::bad_any_cast& e) {
				printf ("**ERROR - DFT: cannot interpret FT dim.\n%s\n", e.what());
				assert (false);
			}
			n = Vector<int>(rank,dim);
		} else {
			printf ("**ERROR - DFT: either vector with FT dimensions or rank and single dimension must be specified.\n");
			assert (false);
		}

        try {
            m_threads = unsigned_cast (p["threads"]);
        } catch (const boost::bad_any_cast& e) {
            printf ("**WARNING - DFT: cannot interpret FT threads.\n%s\n", e.what());
        }


		d = n;
		c = n;
		for (size_t i = 0; i < n.size(); ++i)
			c[i] /= 2;

		m_N = prod(n);
		std::reverse(n.begin(),n.end());
		Allocate (rank, &n[0]);

		m_initialised = true;

	}


	DFT (const DFT<T>& ft) NOEXCEPT {
		*this = ft;
	}

	
	DFT<T>& operator= (const DFT<T>& ft) NOEXCEPT {

		m_mask = ft.m_mask;

		m_pc = ft.m_pc;
		m_cpc = ft.m_cpc;


		m_N = ft.m_N;
		m_cs = ft.m_cs;

		m_sn = ft.m_sn;
        m_threads = ft.m_threads;

		m_have_mask=ft.m_have_mask;
		m_have_pc=ft.m_have_pc;
		m_zpad=ft.m_zpad;

		m_in=ft.m_in;
		d=ft.d;
		c=ft.c;

		Vector<int> n (d);
		std::reverse(n.begin(),n.end());
		int rank = d.size();

		Allocate (rank, &n[0]);

		m_initialised = ft.m_initialised;
		return *this;

	}

	/**
	 * @brief        Construct FFTW plans for forward and backward FT with credentials for FT with identical side lengths
	 * 
	 * @param  rank  Rank (i.e. # FT directions)
	 * @param  sl    Side length of the slice, volume ...
	 * @param  mask  K-Space mask (if left empty no mask is applied)
	 * @param  pc    Phase correction (or target phase)
	 * @param  b0    Static field distortion
	 */
	DFT         (const size_t rank, const size_t sl, const Matrix<RT>& mask = Matrix<RT>(),
				 const Matrix<T>& pc = Matrix<T>(), const Matrix<RT>& b0 = Matrix<RT>()) NOEXCEPT :
    m_have_mask (false), m_have_pc (false), m_threads(0) {
        
		std::vector<int> n (rank);
		
		size_t i;

		if (numel(mask) > 1) {
			m_have_mask = true;
			m_mask      = mask;
		}
		
		if (pc.Size() > 1) {
			m_have_pc   = true;
			m_pc   = pc;
			m_cpc  = conj(pc);
		}
		
		for (i = 0; i < rank; i++) {
			n[i]  = sl;
			m_N  *= n[i];
		}
		
		Matrix<float> tmp (3,1);
		for (i = 0; i < rank; ++i)
			tmp[i] = sl;
		for (     ; i < 3;    ++i)
			tmp[i] = 1;

		d = tmp.Container(); // data side lengths
		c = (floor(tmp/2)).Container(); // center coords

		Allocate ((int)rank, (const int*)&n[0]);
		
		m_initialised = true;
	
	}
	
	


    DFT () NOEXCEPT :
    	m_cs(0), m_N(0), m_in(0), m_have_pc(false), m_zpad(false),
    	m_initialised(false), m_have_mask(false), m_threads(8){}
    
	/**
	 * @brief        Clean up RAM, destroy plans
	 */
	virtual 
	~DFT        () NOEXCEPT {

		FTTraits<T>::Destroy (m_fwplan);
		FTTraits<T>::Destroy (m_bwplan);
		//FTTraits<T>::CleanUp();
		
	}
	
	
	/**
	 * @brief    Forward transform
	 *
	 * @param  m To transform
	 * @return   Transform
	 */
	inline virtual Matrix<T>
	Trafo       (const Matrix<T>& m) const NOEXCEPT {
		
		Matrix<T> res = ishift((m_have_pc) ? m * m_pc : m);

		FTTraits<T>::Execute (m_fwplan, (FTT*)&res[0], (FTT*)&res[0]);

		res = shift(res);

		if (m_have_mask)
			res *= m_mask;
		
		return res / m_sn;
		
	}
	
	
	/**
	 * @brief    Backward transform
	 *
	 * @param  m To transform
	 * @return   Transform
	 */
	inline virtual Matrix<T>
	Adjoint     (const Matrix<T>& m) const NOEXCEPT {

		Matrix<T> res = m;
        if (m_have_mask)
            res *= m_mask;

		FTTraits<T>::Execute (m_bwplan, (FTT*)&res[0], (FTT*)&res[0]);

		res = shift(res);

		if (m_have_pc)
			res *= m_cpc;
		
		return res / m_sn;
			
	}
	
	
	/**
	 * @brief   Set k-space mask
	 * @param   mask  k-space mask
	 */
	inline void Mask (const Matrix<RT>& mask) NOEXCEPT {
		m_mask = mask;
		m_have_mask = true;
	}

	/**
	 * @brief    Forward transform
	 *
	 * @param  m To transform
	 * @return   Transform
	 */
	inline virtual Matrix<T>
	operator* (const Matrix<T>& m) const NOEXCEPT {
		return Trafo(m);
	}
	

	/**
	 * @brief    Backward transform
	 *
	 * @param  m To transform
	 * @return   Transform
	 */
	inline virtual Matrix<T>
	operator->* (const Matrix<T>& m) const NOEXCEPT {
		return Adjoint (m);
	}


	std::ostream& Print (std::ostream& os) {
		Operator<T>::Print(os);
		return os;
	}


private:

	inline Matrix<T>
	shift (const Matrix<T>& m, const bool& fw = true) const NOEXCEPT {
		switch (ndims(m)) {
		case  2: return shift2 (m, fw); 
		case  3: return shift3 (m, fw);
		default: return shift2 (m, fw);
		}
	}


	inline Matrix<T>
	shift3 (const Matrix<T>& m, const bool& fw = true) const NOEXCEPT {

		Matrix<T> res (size(m));

		size_t xi,yi,zi,xs,ys,zs;
		
		for (zi = 0; zi < d[2]; zi++) {
			zs = (zi + c[2]) % d[2];
			for (yi = 0; yi < d[1]; yi++) {
				ys = (yi + c[1]) % d[1];
				for (xi = 0; xi < d[0]; xi++) {
					xs = (xi + c[0]) % d[0];
					if (fw)
						res(xs,ys,zs) = m(xi,yi,zi);
					else
						res(xi,yi,zi) = m(xs,ys,zs);
				}
			}
		}

		return res;

	}

	inline Matrix<T>
	shift2 (const Matrix<T>& m, const bool& fw = true) const NOEXCEPT {

		Matrix<T> res (size(m));
		size_t xi,yi,xs,ys;
		
		for (yi = 0; yi < d[1]; yi++) {
			ys = (yi + c[1]) % d[1];
			for (xi = 0; xi < d[0]; xi++) {
				xs = (xi + c[0]) % d[0];
				if (fw)
					res(xs,ys) = m(xi,yi);
				else
					res(xi,yi) = m(xs,ys);
			}
		}

		return res;

	}

	inline Matrix<T>
	shift1 (const Matrix<T>& m, const bool& fw = true) const NOEXCEPT {

		Matrix<T> res (size(m));
		size_t xi,xs;

		for (xi = 0; xi < d[0]; xi++) {
			xs = (xi + c[0]) % d[0];
			if (fw)
				res(xs) = m(xi);
			else
				res(xi) = m(xs);
		}


		return res;

	}


	inline Matrix<T>
	ishift(const Matrix<T>& m) const NOEXCEPT {

		switch (ndims(m)) {
		case  2: return shift2 (m, false); 
		case  3: return shift3 (m, false);
		default: return shift2 (m, false);
		}

	}


	/**
	 * @brief       Allocate RAM and plans
	 *
	 * @param  rank FT rank
	 * @param  n    Side lengths
	 */
	inline void
	Allocate (const int rank, const int* n) NOEXCEPT {
		
		m_in     = Vector<T> (m_N);

		m_fwplan = FTTraits<T>::DFTPlan (rank, n, (FTT*)&m_in[0], (FTT*)&m_in[0],
                FFTW_FORWARD,  FFTW_ESTIMATE | FFTW_DESTROY_INPUT, m_threads);
		m_bwplan = FTTraits<T>::DFTPlan (rank, n, (FTT*)&m_in[0], (FTT*)&m_in[0],
                FFTW_BACKWARD, FFTW_ESTIMATE | FFTW_DESTROY_INPUT, m_threads);

		m_cs     = m_N * sizeof(FTT);
		m_sn     = sqrt ((T)m_N);

	}


	bool       m_initialised;  /**< @brief Memory allocated / Plans, well, planned! :)*/
	
	Matrix<RT>  m_mask;         /**< @brief K-space mask (applied before inverse and after forward transforms) (double precision)*/
	
	Matrix<T> m_pc;           /**< @brief Phase correction (applied after inverse and before forward trafos) (double precision)*/
	Matrix<T> m_cpc;          /**< @brief Phase correction (applied after inverse and before forward trafos) (double precision)*/
	
	Plan       m_fwplan;       /**< @brief Forward plan (double precision)*/
	Plan       m_bwplan;       /**< @brief Backward plan (double precision)*/

	size_t     m_cs;
	size_t     m_N;            /**< @brief # Nodes */

	T          m_sn;

	Vector<T> m_in;           /**< @brief Aligned fftw input*/
	bool       m_have_mask;    /**< @brief Apply mask?*/
	bool       m_have_pc;      /**< @brief Apply phase correction?*/
	bool       m_zpad;         /**< @brief Zero padding? (!!!NOT OPERATIONAL YET!!!)*/
	

	Vector<size_t> d;
	Vector<size_t> c;

    int m_threads;

	//FTT*      m_in;           /**< @brief Aligned fftw input*/

	
};



#endif




