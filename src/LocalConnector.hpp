/*
 *  jrrs Copyright (C) 2007-2010 Kaveh Vahedipour
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

#ifndef __LOCAL_CONNECTOR_H__
#define __LOCAL_CONNECTOR_H__

#ifdef __WIN32__ 
    #include "RRSModule.h"
#else
    #include "RRSModule.hh"
#endif

#include <complex>
#include <vector>

#include "Configurable.hpp"
#include "ReconContext.hpp"
#include "FunctorContainer.hpp"
#include "DataBase.hpp"

using namespace RRSModule;
using namespace RRStrategy;

/**
 * @brief Remote recon client
 */
namespace RRClient {
	
class RemoteConnector;
	/**
	 * @brief               Remote reconstruction client 
	 */
	class LocalConnector : public Configurable, 
						   public FunctorContainer {
		
		
	public:
		

		LocalConnector () {};
		LocalConnector (const char* name, const char* debug) {};


		/**
		 * @brief           Destroy ORB
		 */
		virtual ~LocalConnector     ();
		
		
		/**
		 * @brief      Process startegy (Needs initialisation @see Init)
		 *
		 * @param name Name of processing library
		 * @return     Sucess
		 */
		virtual error_code
		Process        (const char* name);

		
		/**
		 * @brief      Prepare startegy (Needs initialisation @see Init)
		 *
		 * @param name Name of processing library
		 * @return     Sucess
		 */
		virtual error_code
		Prepare        (const char* name);

		
		/**
		 * @brief      Initialise strategy (Configuration document needs to be set first @see config)
		 * 
		 * @param name Name of processing library
		 * @return     success
		 */
		virtual error_code
		Init           (const char* name);
		

		/**
		 * @brief      Finalise algorithm
		 *
		 * @param name Name of processing library
		 */
		virtual error_code
		Finalise       (const char* name = 0);
		

		/**
		 * @brief      Clean up left over objects
		 *
		 * @return     Success
		 */
		virtual error_code
		CleanUp        ();
		

		/**
		 * @brief Transmit measurement data to remote service
		 *
		 * @param  name     Name
		 * @param  m        Complex data
		 */
		template <class T> void 
		SetMatrix           (const std::string& name, Matrix<T>& m) const {

			DataBase::Instance()->SetMatrix(name, m);
			
		}
		
		
		/**
		 * @brief           Retrieve manipulated data from remote service
		 *
		 * @param  name     Name
		 * @param  m        Receive storage
		 */
		template <class T> void 
		GetMatrix           (const std::string& name, Matrix<T>& m) const {
			
			DataBase::Instance()->GetMatrix(name, m);
			
		}
		
		
		
	private:
		
		std::vector<short>  m_rstrats;    /**< Remote reconstruction strategies    */
		
		/**
		 * @brief           Get size from dimensions (Needed internally)
		 *
		 * @param  dims     Dimension array from the CORBA types 
		 * @return          Size
		 */
		long
		GetSize             (const longs dims) const {};
		
		char*                                m_config;   /**< Serialised XML document  */
		std::map<std::string, ReconContext*> m_contexts; /**< Reconstruction contexts (Abstraction layer to algorithms)*/
		
		
	};
	
}


#endif // __LOCAL_CONNECTOR_H__