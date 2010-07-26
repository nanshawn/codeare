#ifndef __LOADER_H__
#define __LOADER_H__


#include <cstring>
#include <cstdlib>
#include <iostream>

#if defined(_MSC_VER)
    #include <windows.h>
#elif defined(__GNUC__)
    #include <dlfcn.h>
#else
    #error Unknown compiler
#endif

/*
  #define RTLD_LAZY   1
  #define RTLD_NOW    2
  #define RTLD_GLOBAL 4
*/


/**
 * @brief         Load module
 *
 * @param  name   Module name
 * @param  mode   Loader mode
 */ 
void* 
LoadModule        (char *name) {

	void* handle = 0;
	
    #if defined(_MSC_VER) 

	    name = strcat (name, ".dll");
	    handle = (void*)LoadLibrary(name);
		if (!handle)
			std::cerr << "Cannot open " << name << std::endl;
			
    #elif defined(__GNUC__) 

		name = strcat (name, ".so");
		handle = dlopen (name, RTLD_NOW);
		const char* err = dlerror();
		if (err)
			std::cerr << "Cannot open " << err << std::endl;

    #endif

	return handle;

}


/**
 * @brief         Get exported function from module
 *
 * @param  handle Handle on module
 * @param  name   Function name
 */
void*
GetFunction       (void* handle, char* name) {

	void* fnptr = 0;
	
    #if defined(_MSC_VER)

	    fnptr = (void*) GetProcAddress((HINSTANCE)handle, name);

    #elif defined(__GNUC__)

		const char* err = dlerror();
	    fnptr = dlsym (handle, name);
		if (err) 
			std::cerr << "Cannot load symbol " << name << ": " << err << std::endl;

    #endif

	return fnptr;

}


/**
 * @brief         Close module
 *
 * @param  handle Handle on module
 * @return        Success
 */
bool 
CloseModule       (void* handle) {

    #if defined(_MSC_VER)
        return FreeLibrary((HINSTANCE)handle);
    #elif defined(__GNUC__)
        return dlclose(handle);
		const char* err = dlerror();
		if (err) 
			std::cerr << "Cannot unload: " << err << std::endl;

    #endif

};

#endif // __LOADER_H__
