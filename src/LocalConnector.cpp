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

#include "LocalConnector.hpp"

namespace RRClient {

	LocalConnector::~LocalConnector ()               {
		
		FunctorContainer::Finalise ();
		
	}
	
	
	short
	LocalConnector::CleanUp () {
		
		return FunctorContainer::Finalise ();
		
	}
	
	short
	LocalConnector::Init (const char* name) {
		
		std::stringstream  temp;
		temp << GetConfig();
		
		FunctorContainer::config  (temp.str().c_str());
		
		return FunctorContainer::Init (name);
		
	}
	
	
	short
	LocalConnector::Finalise (const char* name) {
		
		return FunctorContainer::Finalise (name);
		
	}
	
	
	short
	LocalConnector::Process  (const char* name)       {
		
		return FunctorContainer::Process (name);
		
	}
	
	
	short
	LocalConnector::Prepare  (const char* name)       {
		
		return FunctorContainer::Prepare (name);
		
	}
	
	
}
