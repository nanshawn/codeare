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

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "ReconContext.hpp"

using namespace RRStrategy;

/**
 * @brief Entry in reconstruction queue
 */
struct QEntry {
    std::string name;
    ReconContext* context;
    QEntry() : name(""), context(0) {}
    QEntry(const std::string& n, ReconContext* c) : name(n), context(c) {}
};

/**
 * @brief Container for data and reconstructions
 */
class Queue {

public:
	
	/**
	 * @brief      Default constructor
	 */
	Queue () : m_config(0) {}

	/**
	 * @brief      Destructor
	 */
	virtual	~Queue ();
	
	/**
	 * @brief      Process startegy (Needs initialisation @see Init)
	 * @param name Name of library
	 * @return     Sucess
	 */
	virtual short Process (const char* name);
	
	/**
	 * @brief      Prepare startegy (Needs initialisation @see Init)
	 * @param name Name of library
	 * @return     Sucess
	 */
	virtual short Prepare (const char* name);
	
	/**
	 * @brief      Initialise strategy (Configuration document needs to be set first @see config)
	 * @param name Name of processing library
	 * @return     success
	 */
	virtual short Init (const char* name, const char* config, const char* client_id = "");
	
	/**
	 * @brief      Finalise algorithm
	 * @param name Name of processing library
	 */
	virtual short Finalise (const char* name = 0);
	
	/**
	 * @brief      Clean up left over objects
	 * @return     Success
	 */
	virtual short CleanUp ();
	
	/**
	 * @brief      Set configuration
	 * @param c    Configuration string
	 */
	virtual void 
	config         (const char* c);
	
protected:

	char*               m_config;   /**< Serialised XML document  */
	std::vector<QEntry> m_contexts; /**< Reconstruction contexts (Abstraction layer to algorithms)*/

};

#endif //__QUEUE_H__
