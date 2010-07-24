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

#include "ReconServant.h"
#include "ReconContext.h"


/**************************************************************************************************/
ReconServant::ReconServant  ()               {

	m_have_raw    = false;
	m_have_helper = false;
	m_have_pixel  = false;
	m_have_labels = false;
}


/**************************************************************************************************/
ReconServant::~ReconServant ()               {

}


/**************************************************************************************************/
error_code
ReconServant::process_data  (method m)       {

	error_code e = OK;

	ReconContext* context = new ReconContext();
	context->Strategy(m);

	if (m_have_raw)
		context->Strategy()->SetRaw(&m_raw);
	if (m_have_helper)
		context->Strategy()->SetHelper(&m_helper);
	if (m_have_pixel)
		context->Strategy()->SetPixel(&m_pixel);
	if (m_have_labels)
		context->Strategy()->SetLabels(m_labels);

	e = context->ProcessData();
	
	cout << "Finished processing. Getting results ..." << endl;

	if (m_have_raw)
		context->Strategy()->GetRaw(&m_raw);
	if (m_have_helper)
		context->Strategy()->GetHelper(&m_helper);
	if (m_have_pixel)
		context->Strategy()->GetPixel(&m_pixel);
	if (m_have_labels)
		context->Strategy()->GetLabels(m_labels);

	cout << "... done. Will handle control back to client." << endl;
	delete context;

	return e;

	
}


/**************************************************************************************************/
error_code
ReconServant::Process  (const char* name) {

	error_code e = OK;

	ReconContext* context = new ReconContext();

	context->Strategy(name);

	cout << "Setting data for processing ... " << endl;

	if (m_have_raw)
		context->Strategy()->SetRaw(&m_raw);
	if (m_have_helper)
		context->Strategy()->SetHelper(&m_helper);
	if (m_have_pixel)
		context->Strategy()->SetPixel(&m_pixel);
	if (m_have_labels)
		context->Strategy()->SetLabels(m_labels);

	cout << "Done. Processing now ..." << endl;

	e = context->ProcessData();
	
	cout << "Finished processing. Getting results ..." << endl;

	if (m_have_raw)
		context->Strategy()->GetRaw(&m_raw);
	if (m_have_helper)
		context->Strategy()->GetHelper(&m_helper);
	if (m_have_pixel)
		context->Strategy()->GetPixel(&m_pixel);
	if (m_have_labels)
		context->Strategy()->GetLabels(m_labels);

	cout << "... done. Will handle control back to client." << endl;
	delete context;

	return e;
	
}


/**************************************************************************************************/
void
ReconServant::raw          (const raw_data& d)   {
	m_raw = d;
	m_have_raw = true;
}


/**************************************************************************************************/
raw_data*
ReconServant::raw          ()                    {
	return new raw_data (m_raw);
}


/**************************************************************************************************/
void
ReconServant::helper       (const raw_data& d)   {
	m_helper = d;
	m_have_helper = true;
}


/**************************************************************************************************/
raw_data*
ReconServant::helper       ()                    {
	return new raw_data (m_helper);
}


/**************************************************************************************************/
void
ReconServant::pixel        (const pixel_data& d) {
	m_pixel = d;
	m_have_pixel = true;
}


/**************************************************************************************************/
pixel_data*
ReconServant::pixel        ()                    {
	return new pixel_data (m_pixel);
}


/**************************************************************************************************/
void 
ReconServant::labels       (const strings& d)    {
	m_labels = d;
	m_have_labels = true;
}


/**************************************************************************************************/
strings* 
ReconServant::labels       ()                    {
		return new strings (m_labels);
}
