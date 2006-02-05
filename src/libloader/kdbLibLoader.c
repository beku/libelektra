#include <stdio.h>
#include "config.h"
#include "kdb.h"
#include "kdbbackend.h"
#include "kdbLibLoader.h"

#ifdef __STATIC
/* Static case */

extern kdblib_symbol kdb_exported_syms[];

kdbLibHandle kdbLibLoad(const char *module)
{
	kdblib_symbol	*current;
	kdbLibHandle handle;
		
	current = kdb_exported_syms;
	while ( current->name != NULL ) {
		/* Skip symbols, we're searching for
		 * the module name */
		if ( current->function == NULL && strcmp(current->name, module) == 0 ) {
			/* Go to the first symbol for this file */
			current++;
			return current;
		}

		current++;
	}
      
	return NULL;
}

void *kdbLibSym(kdbLibHandle handle, const char *symbol)
{
	kdblib_symbol	*current;

	current = handle;
	/* For each symbol about this module */
	while ( current->function != NULL ) {
		if ( strcmp(current->name, symbol) == 0 )
			return current->function;

		current++;
	}
	
	return NULL;
}

int kdbLibClose(kdbLibHandle handle)
{
	return 0;
}

#else
#ifdef WIN32
/* Windows dynamic case */
dbLibHandle kdbLibLoad(const char *module)
{
  char *modulename = malloc((sizeof(char)*strlen(module))+sizeof(".dll"));
  dbLibHandle handle;
  strcpy(modulename, module);
  strcat(modulename, ".dll");
  handle = LoadLibrary(modulename);
  free(modulename);
  return handle;
}

void *kdbLibSym(kdbLibHandle handle, const char *symbol)
{
  return GetProcAddress(handle, symbol);
}

int kdbLibClose(kdbLibHandle handle)
{
  return FreeLibrary(handle);
}

#else
/* Generic case using libltdl */
int kdbLibInit(void)
{
  return lt_dlinit();
}

kdbLibHandle kdbLibLoad(const char *module)
{
	kdbLibHandle	handle;

	handle = lt_dlopenext(module);
	if ( handle == NULL )
		fprintf(stderr, "kdbLibLoad : %s", lt_dlerror());

	return handle;
}

void *kdbLibSym(kdbLibHandle handle, const char *symbol)
{
  return lt_dlsym(handle, symbol);
}

int kdbLibClose(kdbLibHandle handle)
{
  return lt_dlclose(handle);
}

#endif
#endif
