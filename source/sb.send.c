/**
	sbSend.c - send data to smarball devices

	this object has one inlet and no outlet
	it responds to specific messages in the left inlet
	it responds to the 'assistance' message sent by Max when the mouse is positioned over an inlet or outlet
	it send formatted smartball datagram over udp
*/

#include "sb.send.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//  INSTANCE DECLARATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------

t_class *sbSend_class;		// global pointer to the object class - so max can reference the object

void ext_main(void *r)
{
	t_class *c;
	c = class_new("sb.send", (method)sbSend_new, (method)sbSend_free, sizeof(t_sbSend), 0L, A_GIMME, 0);
	
	// SIMPLE MESSAGES METHODS
	class_addmethod(c, (method)sbSend_ping,		   "ping",		  0);
	class_addmethod(c, (method)sbSend_reboot,	   "reboot",	  0);
	class_addmethod(c, (method)sbSend_sleep,	   "sleep",		  0);
	class_addmethod(c, (method)sbSend_saveImu,	   "saveImu",	  0);
	class_addmethod(c, (method)sbSend_saveFactory, "saveFactory", 0);
	
	// ARGS MESSAGES METHODS
	class_addmethod(c, (method)sbSend_infrared,    "ir",		 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_vibration,   "vibe",		 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_color,	   "color",		 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_stream,	   "stream",	 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_setImu,	   "setImu",	 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_accRange,    "accRange",	 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_gyrRange,    "gyrRange",	 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_setFactory,  "setFactory", A_GIMME, 0);
	
	
	class_register(CLASS_BOX, c);
	sbSend_class = c;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTRUCTION, DESTRUCTION
//---------------------------------------------------------------------------------------------------------------------------------------------------------

void *sbSend_new(t_symbol *s, long argc, t_atom *argv)
{	
	t_sbSend *x = (t_sbSend *)object_alloc(sbSend_class);

	memset(&x->addr, 0, sizeof(x->addr));
	x->addr.sin_family = AF_INET;

	// HANDLE ARGUMENTS
	if (argc == 0) {
		x->addr.sin_addr.s_addr = syssock_inet_addr(DEFAULT_IP);
		x->addr.sin_port = syssock_htons(DEFAULT_PORT);
	}
	else if (argc == 2) {
		if (atom_gettype(argv) == A_SYM) {
			// TEST FOR VALID IP
			struct sockaddr_in sa;
			if (inet_pton(AF_INET, atom_getsym(argv)->s_name, &(sa.sin_addr))) {
				x->addr.sin_addr.s_addr = syssock_inet_addr(atom_getsym(argv)->s_name);
			}
			// TEST FOR HOSTNAME
			else {
				struct hostent *he = syssock_gethostbyname(atom_getsym(argv)->s_name);
				if (he != NULL) {
					struct in_addr **addr_list = (struct in_addr *) he->h_addr_list;
					if (addr_list[0] != NULL)
						x->addr.sin_addr.s_addr = syssock_inet_addr(inet_ntoa(*addr_list[0]));
				}
				else goto ARG_FAULT;
			}
		}
		else goto ARG_FAULT;

		if (atom_gettype(argv + 1) == A_LONG)
			x->addr.sin_port = syssock_htons((short)atom_getlong(argv + 1));
		else goto ARG_FAULT;
	}
	else {
	ARG_FAULT:
		object_error((t_object*)x, "Cannot start with those arguments ");
		goto NOT_VALID;
	}

	// HANDLE SOCKET INITIATION
	if (syssock_init()) {
		object_error((t_object*)x, "Cannot start socket API");
		goto NOT_VALID;
	}

	x->fd = syssock_socket(AF_INET, SOCK_DGRAM, 0);
	if (x->fd < 0) {
		object_error((t_object*)x, "Cannot start UDP socket");
		goto NOT_VALID;
	}

	// RETURN A VALID OBJECT
	return(x);

NOT_VALID:
	x = NULL;
	return(x);
}

void sbSend_free(t_sbSend *x) 
{
	syssock_close(x->fd);
}

void sbSend_assist(t_sbSend *x, void *b, long m, long a, char *s) {
	if (m == ASSIST_INLET)
		sprintf(s, "Smartball commands");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// MESSAGE HANDLER
//---------------------------------------------------------------------------------------------------------------------------------------------------------

void sbSend_ping(t_sbSend *x) {
	sbSend_send(x, CMD_PING,0,NULL);
}

void sbSend_reboot(t_sbSend *x) {
	sbSend_send(x, CMD_REBOOT, 0, NULL);
}

void sbSend_sleep(t_sbSend *x) {
	sbSend_send(x, CMD_SLEEP, 0, NULL);
}

void sbSend_saveImu(t_sbSend *x) {
	sbSend_send(x, SAVE_IMU, 0, NULL);
}

void sbSend_saveFactory(t_sbSend *x) {
	sbSend_send(x, SAVE_FACTORY, 0, NULL);
}

void sbSend_infrared(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	if (atom_gettype(argv) == A_LONG) {
		long value = atom_getlong(argv) & 1023;
		t_uint8 data[] = { (value >> 8) & 255,value & 255 };
		sbSend_send(x, CMD_IRL, 2, data);
	}
	else {
		object_warn((t_object*)x, "Infrared argument not understood");
	}
}

void sbSend_vibration(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	if (atom_gettype(argv) == A_LONG) {
		long value = atom_getlong(argv) & 1023;
		t_uint8 data[] = { (value >> 8) & 255,value & 255 };
		sbSend_send(x, CMD_MOT, 2, data);
	}
	else {
		object_warn((t_object*)x,"Vibration argument not understood");
	}
}

void sbSend_color(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap; 
	t_uint8 *data;
	t_uint16 i;
	t_uint16 l = (t_uint16)(argc - (argc % 3)); 
	
	data = (t_uint8 *)sysmem_newptr(sizeof(t_uint8)*l);
	if (data == NULL) {
		object_error((t_object*)x,"Cannot allocate memory for color data");
		goto END;
	}

	for (i = 0, ap = argv; i < l; i++, ap++) {
		switch (atom_gettype(ap)) {
		case A_LONG:
			data[i] = atom_getlong(ap) & 255;
			break;
		case A_FLOAT:
			data[i] = (int)(atom_getfloat(ap)*255.) & 255;
			break;
		default:
			data[i] = 0;
			break;
		}
	}

	sbSend_send(x, CMD_COLOR, l, data);

END:
	sysmem_freeptr(data);
}

void sbSend_stream(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	t_atom *ap;
	t_uint8 *data;
	t_uint16 i;
	t_uint16 l = (t_uint16)(argc - (argc % 3) + 1);

	data = (t_uint8 *)sysmem_newptr(sizeof(t_uint8)*l);
	if (data == NULL) {
		object_error((t_object*)x, "Cannot allocate memory for stream data");
		goto END;
	}

	for (i = 0, ap = argv; i < l; i++, ap++) {
		switch (atom_gettype(ap)) {
		case A_LONG:
			data[i] = atom_getlong(ap) & 255;
			break;
		case A_FLOAT:
			data[i] = (int)(atom_getfloat(ap)*255.) & 255;
			break;
		default:
			data[i] = 0;
			break;
		}
	}

	sbSend_send(x, CMD_STREAM, l, data);

END:
	sysmem_freeptr(data);
}

void sbSend_setImu(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	if (atom_gettype(argv) == A_LONG) {
		long value = atom_getlong(argv) & IMU_MASK;
		t_uint8 data[] = { (t_uint8)value };
		sbSend_send(x, CMD_IMU, 1, data);
	}
	else {
		object_warn((t_object*)x, "Set IMU argument not understood");
	}
}

void sbSend_accRange(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	if (atom_gettype(argv) == A_LONG) {
		long value = atom_getlong(argv) & RANGE_MASK;
		t_uint8 data[] = { (t_uint8)value };
		sbSend_send(x, CMD_ACCRANGE, 1, data);
	}
	else {
		object_warn((t_object*)x, "Accelerometer range argument not understood");
	}
}

void sbSend_gyrRange(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	if (atom_gettype(argv) == A_LONG) {
		long value = atom_getlong(argv) & RANGE_MASK;
		t_uint8 data[] = { (t_uint8)value };
		sbSend_send(x, CMD_GYRRANGE, 1, data);
	}
	else {
		object_warn((t_object*)x, "Gyroscope range argument not understood");
	}
}

void sbSend_setFactory(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	if (argc < 3) {
		if (atom_gettype(argv) == A_LONG && atom_gettype(argv + 1) == A_LONG && atom_gettype(argv + 2) == A_LONG) {
			long sn = atom_getlong(argv);						// Serial number
			long df = atom_getlong(argv + 1);					// Device flag
			long bs = (int)(atom_getfloat(argv + 2) * 10.);		// Battery scale
			t_uint8 data[] = {
				(sn >> 8) & 255, sn & 255,
				(df >> 8) & 255, df & 255,
				(bs >> 8) & 255, bs & 255
			};
			sbSend_send(x, CMD_FACTORY, 6, data);
		}
		else {
			object_warn((t_object*)x, "Set factory settings arguments not understood");
		}
	}
	else {
		object_warn((t_object*)x, "Too few arguments to change factory settings");
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// MESSAGE HANDLER
//---------------------------------------------------------------------------------------------------------------------------------------------------------

void sbSend_send(t_sbSend* x, enum sb_cmd c, t_uint16 l, t_uint8 *d) {
	// Memory allocation
	t_uint8 *msg;
	t_uint16 length = MIN_PACKET + l;

	msg = (t_uint8 *)sysmem_newptr(sizeof(t_uint8)*(length));

	if (msg == NULL) {
		object_error((t_object*)x, "Cannot allocate memory for data sending");
		goto END;
	}

	// Packet header
	msg[0] = SOP;
	msg[1] = c;
	msg[2] = (l >> 8) & 255;
	msg[3] = l & 255;

	// Packet datas & checksum
	msg[length - 1] = c + msg[2] + msg[3];
	for (t_uint16 i = 0; i < l; i++) {
		msg[length - 1] += d[i];
		msg[DATA_ADDR+i]  = d[i];
	}

	// Packet sending
	int nbytes = syssock_sendto(x->fd, msg, length, 0, (struct sockaddr*) &x->addr, sizeof(x->addr));
	if (nbytes < 0) {
		object_error((t_object*)x, "Cannot send UDP packet");
	}

	// Free allocated memory
	END:
	sysmem_freeptr(msg);
}