#ifndef sb_send_h
#define sb_send_h

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// INCLUDES (see http://www.zachburlingame.com/2011/05/resolving-redefinition-errors-betwen-ws2def-h-and-winsock-h/)
//---------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef WIN_VERSION
#define MAXAPI_USE_MSCRT
#endif

#include "ext_syssock.h"	    // CNMAT crossplatform socket library - should be first to avoid winsock2 redeclaration problem
#include "ext_smartball.h"		// Smartball definitions
#include "ext.h"				// you must include this - it contains the external object's link to available Max functions
#include "ext_obex.h"			// this is required for all objects using the newer style for writing objects.

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// DEFINES
//---------------------------------------------------------------------------------------------------------------------------------------------------------

#define DEFAULT_IP	 "239.0.0.50"
#define DEFAULT_PORT 8000

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTURE
//---------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct _sbSend {        // defines our object's internal variables for each instance in a patch
	t_object    s_ob;			// object header - ALL objects MUST begin with this...
	t_syssocket fd;				// UDP socket
	t_sysaddr   addr;			// Address structure
} t_sbSend;

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// PROTOTYPES
//---------------------------------------------------------------------------------------------------------------------------------------------------------

void *sbSend_new(t_symbol *s, long argc, t_atom *argv);
void sbSend_free(t_sbSend*);

void sbSend_send(t_sbSend*, enum sb_cmd, t_uint16, t_uint8*);

void sbSend_ping(t_sbSend*);
void sbSend_reboot(t_sbSend*);
void sbSend_sleep(t_sbSend*);
void sbSend_saveImu(t_sbSend*);
void sbSend_saveFactory(t_sbSend*);

void sbSend_infrared(t_sbSend*, t_symbol*, long, t_atom*);
void sbSend_vibration(t_sbSend*, t_symbol*, long, t_atom*);
void sbSend_color1(t_sbSend*, t_symbol*, long, t_atom*);
void sbSend_color2(t_sbSend*, t_symbol*, long, t_atom*);
void sbSend_color(t_sbSend*, t_symbol*, long, t_atom*, enum sb_cmd);
void sbSend_strobe(t_sbSend*, t_symbol*, long, t_atom*);
void sbSend_master(t_sbSend*, t_symbol*, long, t_atom*);
void sbSend_stream(t_sbSend*, t_symbol*, long, t_atom*);
void sbSend_setImu(t_sbSend*, t_symbol*, long, t_atom*);
void sbSend_accRange(t_sbSend*, t_symbol*, long, t_atom*);
void sbSend_gyrRange(t_sbSend*, t_symbol*, long, t_atom*);
void sbSend_setFactory(t_sbSend*, t_symbol*, long, t_atom*);

void sbSend_assist(t_sbSend*, void*, long, long, char*);

#endif /* sb_send_h */