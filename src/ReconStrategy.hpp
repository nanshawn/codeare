/*
 *  jrrs Copyright (C) 2007-2010 Kaveh Vahedipour
 *                               Forschungszentrum Jülich, Germany
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

#ifndef __RECON_STRATEGY_HPP__
#define __RECON_STRATEGY_HPP__

#include "Matrix.hpp"
#include "Configurable.hpp"

#include "DllExport.h"

#ifdef __WIN32__ 
    #include "RRSModule.h"
#else
    #include "RRSModule.hh"
#endif

#include <cstdlib>
#include <complex>
#include <stdint.h>

#include "cycle.h"            // FFTW cycle implementation

using namespace RRSModule;

namespace RRServer {

	/**
	 * @brief Strategy for reconstruction strategies
	 *        Derive hereof to expand the reconstruction toolbox
	 *
	 */
	class ReconStrategy : public Configurable {
		
		
	public:
		

		/**
		 * @brief       Default constructor
		 */ 
		ReconStrategy   () {};
		

		/**
		 * @brief       Default destructor
		 */ 
		virtual
		~ReconStrategy  () {};
		
		
		/**
		 * @brief       Mandatory implementation of actual data procession
		 *
		 * @return      Success
		 */ 
		virtual error_code
		Process         () = 0;
		

		/**
		 * @brief       Mandatory implementation of initialiser
		 *
		 * @return      Success
		 */ 
		virtual error_code
		Init            () = 0;
		

		/**
		 * @brief       Mandatory implementation of finaliser
		 *
		 * @return      Success
		 */ 
		virtual error_code
		Finalise        () = 0;
		
		
		/**
		 * @brief       Add a complex matrix to complex matrix container
		 *
		 * @param  name Name
		 * @param  m    The added matrix
		 * @return      Success
		 */
		bool
		AddCplx         (const std::string name, Matrix<cplx>* m) {

			if (m_cplx.find (name) == m_cplx.end())
				m_cplx.insert (std::pair<std::string, Matrix<cplx>*> (name, m));
			else 
				return false;

			return true;
			
		}


		/**
		 * @brief       Add a real matrix to real matrix container
		 *
		 * @param  name Name
		 * @param  m    The added matrix
		 * @return      Success
		 */
		bool
		AddReal         (const std::string name, Matrix<double>* m) {

			if (m_real.find (name) == m_real.end())
				m_real.insert (std::pair<std::string, Matrix<double>*> (name, m));
			else 
				return false;

			return true;
			
		}


		/**
		 * @brief       Add a pixel matrix to pixel matrix container
		 *
		 * @param  name Name
		 * @param  m    The added matrix
		 * @return      Success
		 */
		bool
		AddPixel        (const std::string name, Matrix<short>* m) {

			if (m_pixel.find (name) == m_pixel.end())
				m_pixel.insert (std::pair<std::string, Matrix<short>*> (name, m));
			else 
				return false;

			return true;
			
		}

		/**
		 * @brief        Get data from recon (Remote access)
		 *
		 * @param  r     Raw data storage 
		 */
		void
		GetCplx       (const std::string name, cplx_data* c)   {
			
			if (m_cplx.find (name) == m_cplx.end())
				return;
			
			Matrix<cplx>* tmp = m_cplx[name];
			
			for (int j = 0; j < INVALID_DIM; j++)
				c->dims[j] = tmp->Dim(j);
			
			c->dreal.length(tmp->Size()); 
			c->dimag.length(tmp->Size());
			
			for (int i = 0; i < tmp->Size(); i++) {
				c->dreal[i] = tmp->At(i).real();
				c->dimag[i] = tmp->At(i).imag(); 
			}
			
			tmp->Clear();
			
		}
		
		
		/**
		 * @brief        Set data for recon (Remote access)
		 *
		 * @param name   Name
		 * @param r      Cplx data
		 */
		void 
		SetCplx       (const std::string name, const cplx_data* c)   {

			Matrix<cplx>* tmp;
				
			if (m_cplx.find (name) == m_cplx.end())
				m_cplx.insert (std::pair<std::string, Matrix<cplx>*> (name, tmp = new Matrix<cplx>()));
			else
			    tmp = m_cplx[name];
			
			for (int i = 0; i < INVALID_DIM; i++)
				tmp->Dim(i) = c->dims[i];
			
			tmp->Reset ();
			
			for (int j = 0; j < tmp->Size(); j++)
				tmp->At(j) =  std::complex<float> (c->dreal[j], c->dimag[j]);
			
		}
		

		/**
		 * @brief        Get data from recon (Local access)
		 *
		 * @param  m     Cplx data storage 
		 */
		void
		GetCplx       (const std::string name, Matrix<cplx>* m)   {

			if (m_cplx.find (name) == m_cplx.end())
				return;
			
			m = m_cplx[name];
			
		}
		
		
		/**
		 * @brief        Set data for recon (Local access)
		 *
		 * @param m      Raw data
		 */
		void 
		SetCplx       (const std::string name, Matrix<cplx>* m)   {
			
			if (m_cplx.find (name) == m_cplx.end())
				m_cplx.insert (std::pair<std::string, Matrix<cplx>*> (name, new Matrix<cplx>()));
			
			m_cplx[name] = m;
			
		}
		

		/**
		 * @brief        Get data from recon (Remote access)
		 *
		 * @param  helper Raw data storage
		 */
		void 
		GetReal        (const std::string name, real_data* r)   {
			
			if (m_real.find (name) == m_real.end())
				return;
			
			Matrix<double>* tmp = m_real[name];
			
			for (int j = 0; j < INVALID_DIM; j++)
				r->dims[j] = tmp->Dim(j);
			
			r->vals.length(tmp->Size()); 
			
			for (int i = 0; i < tmp->Size(); i++)
				r->vals[i] = tmp->At(i);
			
			tmp->Clear();
			
		}
		

		/**
		 * @brief         Set data for recon (Local access)
		 * 
		 * @param  helper Real data
		 */
		void 
		SetReal        (const std::string name, const real_data* r)   {
			
			Matrix<double>* tmp;

			if (m_real.find (name) == m_real.end())
				m_real.insert (std::pair<std::string, Matrix<double>*> (name, tmp = new Matrix<double>()));
			else
				tmp = m_real[name];
			
			for (int i = 0; i < INVALID_DIM; i++)
				tmp->Dim(i) = r->dims[i];
			
			tmp->Reset ();
			
			for (int j = 0; j < tmp->Size(); j++)
				tmp->At(j) =  r->vals[j];
			
		}
		
		
		/**
		 * @brief         Get data from recon
		 *
		 * @param  m      Real data
		 */
		void
		GetReal         (const std::string name, Matrix<double>* m)   {
			
			if (m_real.find (name) == m_real.end())
				return;
			
			m = m_real[name];
			
		}
		
	

		/**
		 * @brief         Set data for recon
		 *
		 * @param  m      Real data storage
		 */
		void 
		SetReal         (const std::string name, Matrix<double>* m)   {
			
			if (m_real.find (name) == m_real.end())
				m_real.insert (std::pair<std::string, Matrix<double>*> (name, new Matrix<double>()));
			
			m_real[name] = m;
			
		}
		

		/**
		 * @brief         Get data from recon
		 *
		 * @param  pixel  Pixel data storage
		 */
		void 
		GetPixel          (const std::string name, pixel_data* p)   {
			
			if (m_pixel.find (name) == m_pixel.end())
				return;
			
			Matrix<short>* tmp = m_pixel[name];

			for (int j = 0; j < INVALID_DIM; j++)
				p->dims[j] = tmp->Dim(j);
			
			p->vals.length(tmp->Size()); 
			
			for (int i = 0; i < tmp->Size(); i++)
				p->vals[i] = tmp->At(i);
			
			tmp->Clear();
			
		}
		
		
		/**
		 * @brief        Set data for recon
		 *
		 * @param  pixel Pixel data
		 */
		void 
		SetPixel         (const std::string name, const pixel_data* p)   {
			
			Matrix<short>* tmp;

			if (m_pixel.find (name) == m_pixel.end())
				m_pixel.insert (std::pair<std::string, Matrix<short>*> (name, tmp = new Matrix<short>()));
			else
				tmp = m_pixel[name];

			for (int i = 0; i < INVALID_DIM; i++)
				tmp->Dim(i) = p->dims[i];
			
			tmp->Reset ();
			
			for (int j = 0; j < tmp->Size(); j++) 
				tmp->At(j) =  p->vals[j];
			
		}

		
		/**
		 * @brief        Get data from recon
		 *
		 * @param  m     Pixel data storage
		 */
		void
		GetPixel         (const std::string name, Matrix<short>* m)   {
			
			if (m_pixel.find (name) == m_pixel.end())
				return;
			
			 m = m_pixel[name];
			
		}

		
		/**
		 * @brief        Set data for recon
		 *
		 * @param  m     Pixel data
		 */
		void 
		SetPixel         (const std::string name, Matrix<short>* m)   {
			
			if (m_pixel.find (name) == m_pixel.end())
				m_pixel.insert (std::pair<std::string, Matrix<short>*> (name, new Matrix<short>()));
			
			m_pixel[name] = m;
			
		}
	
		
		/**
		 * @brief          Attach a name to the algorithm
		 *
		 * @param  name    Name
		 */
		void Name (const char* name) { 

			m_name = std::string(name);
		
		}


		/**
		 * @brief          Get given name
		 *
		 * @return         Name
		 */
		const char* Name () {
			
			return m_name.c_str();
		
		}


	
	protected:
		
		std::map < std::string, Matrix< std::complex<float> >* >   m_cplx;
		std::map < std::string, Matrix<double>* >                  m_real;
		std::map < std::string, Matrix<short>* >                   m_pixel;
		
		std::string     m_name;        /*!< Name                               */
		
		bool            m_initialised; /*!< Reco is initialised                */
		
	};
	

}
#endif /* __RECON_STRATEGY_HPP__ */
	


/**
 * @brief              Dynamic constructor
 */
typedef RRServer::ReconStrategy* create_t  ();


/**
 *@brief               Dynamic destructor
 */
typedef void           destroy_t (RRServer::ReconStrategy*);


