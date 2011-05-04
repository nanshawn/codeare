#include "Toolbox.hpp"

inline bool fexists (const char* fname) {
	
	std::ifstream fs (fname);
	
	if (fs)
		return true;
	else
		return false;

}

template<> inline std::ostream&  
Matrix<short>::Print     (std::ostream &os) const {

	for (int i = 0; i < _dim[LIN]; i++) {
		for(int j = 0; j < _dim[COL]; j++)
			printf ("%4i ", _M [i * _dim[COL] + j]);
		printf("\n");
	}
	
	return os;
	
}


template<> inline std::ostream&  
Matrix<double>::Print     (std::ostream &os) const {

	for (int i = 0; i < _dim[LIN]; i++) {
		for(int j = 0; j < _dim[COL]; j++)
			printf ("%+.4f ", _M [i * _dim[COL] + j]);
		printf("\n");
	}
	
	return os;
	
}


template<> inline std::ostream&  
Matrix<raw>::Print       (std::ostream& os) const {
	
	for (int i = 0; i < _dim[LIN]; i++) {
		for(int j = 0; j < _dim[COL]; j++)
			printf ("%+.4f+%+.4fi ", _M [i*_dim[COL]+j].real(), _M [i*_dim[COL]+j].imag());
		printf("\n");
	}
	
	return os;

}


template <class T> std::ostream& 
operator<<    (std::ostream& os, Matrix<T>& M) {

	M.Print(os);
	return os;

}


template <class T>
bool Matrix<T>::dump (std::string fname, std::string dname, std::string dloc) {

	int i = 0;

	if (fname != "") {
		
#ifdef HAVE_H5CPP_H
		try {
			
#ifndef VERBOSE
			Exception::dontPrint();
#endif
			
			H5File        file; 
			
			try {
				file = H5File  (fname, H5F_ACC_RDWR);
#ifdef VERBOSE
				printf ("File %s opened for RW\n", fname.c_str());
#endif
			} catch (Exception e) {
				file = H5File  (fname, H5F_ACC_TRUNC);
#ifdef VERBOSE
				printf ("File %s created for RW\n", fname.c_str());
#endif
			}
			
			Group group, *tmp;
			
			try {
				
				group = file.openGroup(dloc);
#ifdef VERBOSE
				printf ("Group %s opened for writing\n", dloc.c_str()) ;
#endif
				
			} catch (Exception e) {
				
				int    depth   = 0;

				std::vector<std::string> sv;

				Toolbox tb;
				tb.split(sv, dloc, "/");

				for (int i = 0; i < sv.size(); i++) {
					
					try {
						if (depth)
							group = (*tmp).openGroup(sv.at(i));
						else
							group = file.openGroup(sv.at(i));
					} catch (Exception e) {
						if (depth)
							group = (*tmp).createGroup(sv.at(i));
						else
							group = file.createGroup(sv.at(i));
					}
					
					tmp = &group;
					depth++;
					
				}
				
				group = (*tmp);
				
			}
			
			// One more field for complex numbers
			int tmpdim = (typeid(T) == typeid(raw)) ? INVALID_DIM+1 : INVALID_DIM;
			hsize_t* dims = new hsize_t[tmpdim];

			for (i = 0; i < INVALID_DIM; i++)
				dims[i] = _dim[INVALID_DIM-1-i];
			
			if (typeid(T) == typeid(raw))
				dims[INVALID_DIM] = 2;

			DataSpace space (tmpdim, dims);
			PredType*  type;
			
			delete [] dims;

			if (typeid(T) == typeid(raw)) {
				type = (PredType*) new FloatType (PredType::NATIVE_FLOAT);
				if (dname == "") 
					dname = "raw";
			} else if (typeid(T) == typeid(double)) {
				type = (PredType*) new FloatType (PredType::NATIVE_DOUBLE);
				if (dname == "") 
					dname = "double";
			} else {
				type = (PredType*) new IntType   (PredType::NATIVE_SHORT);
				if (dname == "") 
					dname = "pixel";
			}
				
			DataSet set = group.createDataSet(dname, (*type), space);
				
			set.write   (_M, (*type));
			set.close   ();
			space.close ();
			file.close  ();
			
		} catch(FileIException      e) {
			e.printError();
			return false;
		} catch(DataSetIException   e) {
			e.printError();
			return false;
		} catch(DataSpaceIException e) {
			e.printError();
			return false;
		} catch(DataTypeIException  e) {
			e.printError();
			return false;
		}
		
#else // HAVE_H5CPP_H
		
		std::ofstream fout(fname.c_str() , std::ios::out | std::ios::binary);
		
		for (i = 0; i < Size(); i++) {
			std::cout << _M[i] << std::endl;
			fout.write ((char*)(&(_M[i])), sizeof(double));
		}
		
		fout.close();
		
#endif // HAVE_H5CPP_H

	}
	
	return true;
	
}

template <class T>
bool Matrix<T>::read (std::string fname, std::string dname, std::string dloc) {
	
    if (fname != "") {
		
#ifdef HAVE_H5CPP_H
		
#ifndef VERBOSE
		Exception::dontPrint();
#endif
		
		try {
			
			
			H5File    file (fname, H5F_ACC_RDONLY);
			DataSet   dataset = file.openDataSet(dloc+"/"+dname);
			DataSpace space   = dataset.getSpace();
			
			hsize_t   dims [space.getSimpleExtentNdims()];
			int       ndim    = space.getSimpleExtentDims(dims, NULL);

			
			for (int i = 0; i < INVALID_DIM; i++)
				_dim[INVALID_DIM-i] = dims[i];
			
			Reset();
			
#ifdef VERBOSE
			if (!read) {
				std::cout << "rank: " << ndim << ", dimensions: ";
				for (int i = 0; i < ndim; i++) {
					std::cout << (unsigned long)(dims[i]);
					if (i == ndim - 1)
						std::cout << std::endl;
					else
						std::cout << " x ";
				}
			}
#endif
			
			PredType*  type;
			
			if (typeid(T) == typeid(raw))
				type = (PredType*) new FloatType (PredType::NATIVE_FLOAT);
			else if (typeid(T) == typeid(double))
				type = (PredType*) new FloatType (PredType::NATIVE_DOUBLE);
			else
				type = (PredType*) new IntType   (PredType::NATIVE_SHORT);
				
			dataset.read (_M, (*type));
			
			space.close();
			dataset.close();
			file.close();
			
		} catch(FileIException      e) {
			e.printError();
			return false;
		} catch(DataSetIException   e) {
			e.printError();
			return false;
		} catch(DataSpaceIException e) {
			e.printError();
			return false;
		} catch(DataTypeIException  e) {
			e.printError();
			return false;
		}
		
#else // HAVE_H5CPP_H
		
		std::ifstream fin  (fname.c_str() , std::ios::in | std::ios::binary);
		
		for (int i = 0; i < Size(); i++)
			fin.read  ((char*)(&(_M[i])), sizeof(T));
		
		fin.close();
		
#endif // HAVE_H5CPP_H
		
	}
	
	return true;
	
}

	
