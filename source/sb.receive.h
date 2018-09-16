#ifndef sb_receive_h
#define sb_receive_h

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// INCLUDES (see http://www.zachburlingame.com/2011/05/resolving-redefinition-errors-betwen-ws2def-h-and-winsock-h/)
//---------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef WIN_VERSION
#define MAXAPI_USE_MSCRT
#endif

#ifndef SB_RECEIVE
#define SB_RECEIVE
#endif

#include "ext_syssock.h"	    // CNMAT crossplatform socket library - should be first to avoid winsock2 redeclaration problem
#include "ext_smartball.h"		// Smartball definitions
#include "ext.h"				// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"			// this is required for all objects using the newer style for writing objects.


//---------------------------------------------------------------------------------------------------------------------------------------------------------
// DEFINES
//---------------------------------------------------------------------------------------------------------------------------------------------------------

#define DEFAULT_MULTI	"239.0.0.51"
#define DEFAULT_PORT	9000

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURE
//---------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct _sbReceive {     // defines our object's internal variables for each instance in a patch
	t_object    s_ob;			// object header - ALL objects MUST begin with this...
	t_syssocket fd;				// UDP socket
	t_sysaddr   addr;			// Address structure
	int listening;				// listening state
	t_systhread listener;		// thread
	char* multicast;			// multicast address
	int port;					// UDP reveiving port
	void* outlet;				// outlet creation - inlets are automatic, but objects must "own" their own outlets
} t_sbReceive;

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// PROTOTYPES
//---------------------------------------------------------------------------------------------------------------------------------------------------------

void *sbReceive_new(t_symbol*, long, t_atom*);
void sbReceive_free(t_sbReceive*);
void sbReceive_assist(t_sbReceive*, void*, long, long, char*);
void sbReceive_receive(t_sbReceive*);
void sbReceive_parse(t_sbReceive*, t_uint8*);

int syssock_set(t_sbReceive*);
int syssock_addmulticast(t_syssocket, char*);
int syssock_dropmulticast(t_syssocket, char*);

#endif /* sb_send_h */