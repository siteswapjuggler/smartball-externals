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
	class_addmethod(c, (method)sbSend_saveImu,	   "saveImu",	  0);
	class_addmethod(c, (method)sbSend_saveFactory, "saveFactory", 0);
	class_addmethod(c, (method)sbSend_saveGeneral, "saveGeneral", 0);
	
	// ARGS MESSAGES METHODS
	class_addmethod(c, (method)sbSend_infrared,    "ir",		 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_vibration,   "vibe",		 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_master,	   "master",	 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_strobe,	   "strobe",	 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_color1,	   "color",		 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_color2,	   "background", A_GIMME, 0);
	class_addmethod(c, (method)sbSend_stream,	   "stream",	 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_setImu,	   "setImu",	 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_accRange,    "accRange",	 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_gyrRange,    "gyrRange",	 A_GIMME, 0);
	class_addmethod(c, (method)sbSend_setFactory,  "setFactory", A_GIMME, 0);
	class_addmethod(c, (method)sbSend_setGeneral,  "setGeneral", A_GIMME, 0);
	
	// ATTR CONFIGURATION

	CLASS_ATTR_LONG(c, "stream_offset", 0, t_sbSend, stream_offset);
	CLASS_ATTR_SAVE(c, "stream_offset", 1);
	CLASS_ATTR_ORDER(c, "stream_offset", 0, "10"); 
	CLASS_ATTR_CATEGORY(c, "stream_offset", 0, "Stream");
	CLASS_ATTR_FILTER_CLIP(c, "stream_offset", 0, 65535);
	CLASS_ATTR_LABEL(c, "stream_offset", 0, "Offset");
	CLASS_ATTR_ACCESSORS(c, "stream_offset", NULL, (method)stream_offset_set);

	CLASS_ATTR_CHAR(c, "stream_loop", 0, t_sbSend, stream_loop);
	CLASS_ATTR_SAVE(c, "stream_loop", 1);
	CLASS_ATTR_ORDER(c, "stream_loop", 0, "20"); 
	CLASS_ATTR_CATEGORY(c, "stream_loop", 0, "Stream");
	CLASS_ATTR_STYLE_LABEL(c, "stream_loop", 0, "onoff", "Loop");
	CLASS_ATTR_ACCESSORS(c, "stream_loop", NULL, (method)stream_loop_set);

	CLASS_ATTR_CHAR(c, "stream_col1", 0, t_sbSend, stream_col1);
	CLASS_ATTR_SAVE(c, "stream_col1", 1);
	CLASS_ATTR_ORDER(c, "stream_col1", 0, "30"); 
	CLASS_ATTR_CATEGORY(c, "stream_col1", 0, "Stream");
	CLASS_ATTR_ENUM(c, "stream_col1", 0, "0 1 2 3 6");
	CLASS_ATTR_STYLE_LABEL(c, "stream_col1", 0, "enum", "Foreground Color");
	CLASS_ATTR_ACCESSORS(c, "stream_col1", NULL, (method)stream_col1_set);


	CLASS_ATTR_CHAR(c, "stream_col2", 0, t_sbSend, stream_col2);
	CLASS_ATTR_SAVE(c, "stream_col2", 1);
	CLASS_ATTR_ORDER(c, "stream_col2", 0, "40");
	CLASS_ATTR_CATEGORY(c, "stream_col2", 0, "Stream");
	CLASS_ATTR_ENUM(c, "stream_col2", 0, "0 1 2 3 6");
	CLASS_ATTR_STYLE_LABEL(c, "stream_col2", 0, "enum", "Background Color");
	CLASS_ATTR_ACCESSORS(c, "stream_col2", NULL, (method)stream_col2_set);

	CLASS_ATTR_CHAR(c, "stream_mst", 0, t_sbSend, stream_mst);
	CLASS_ATTR_SAVE(c, "stream_mst", 1);
	CLASS_ATTR_ORDER(c, "stream_mst", 0, "50"); 
	CLASS_ATTR_CATEGORY(c, "stream_mst", 0, "Stream");
	CLASS_ATTR_STYLE_LABEL(c, "stream_mst", 0, "onoff", "Master");
	CLASS_ATTR_ACCESSORS(c, "stream_mst", NULL, (method)stream_mst_set);

	CLASS_ATTR_CHAR(c, "stream_stb", 0, t_sbSend, stream_stb);
	CLASS_ATTR_SAVE(c, "stream_stb", 1);
	CLASS_ATTR_ORDER(c, "stream_stb", 0, "60"); 
	CLASS_ATTR_CATEGORY(c, "stream_stb", 0, "Stream");
	CLASS_ATTR_STYLE_LABEL(c, "stream_stb", 0, "onoff", "Strobe");
	CLASS_ATTR_ACCESSORS(c, "stream_stb", NULL, (method)stream_stb_set);

	CLASS_ATTR_CHAR(c, "stream_mot", 0, t_sbSend, stream_mot);
	CLASS_ATTR_SAVE(c, "stream_mot", 1);
	CLASS_ATTR_ORDER(c, "stream_mot", 0, "70");
	CLASS_ATTR_CATEGORY(c, "stream_mot", 0, "Stream");
	CLASS_ATTR_STYLE_LABEL(c, "stream_mot", 0, "onoff", "Motor");
	CLASS_ATTR_ACCESSORS(c, "stream_mot", NULL, (method)stream_mot_set);

	CLASS_ATTR_CHAR(c, "stream_irl", 0, t_sbSend, stream_irl);
	CLASS_ATTR_SAVE(c, "stream_irl", 1);
	CLASS_ATTR_ORDER(c, "stream_irl", 0, "80");
	CLASS_ATTR_CATEGORY(c, "stream_irl", 0, "Stream");
	CLASS_ATTR_STYLE_LABEL(c, "stream_irl", 0, "onoff", "Infrared");
	CLASS_ATTR_ACCESSORS(c, "stream_irl", NULL, (method)stream_irl_set);

	class_register(CLASS_BOX, c);
	sbSend_class = c;
	post("sb.send v0.31 - 18.05.2019");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// ATTRIBUTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------

void sbSend_updateStreamFlag(t_sbSend *x)
{
	x->stream_flag = (x->stream_col1 << COL1_STREAM_ADDR) | (x->stream_col2 << COL2_STREAM_ADDR);
	x->stream_flag |= (x->stream_mst << MST_STREAM_BIT) | (x->stream_stb << STB_STREAM_BIT);
	x->stream_flag |= (x->stream_mot << MOT_STREAM_BIT) | (x->stream_irl << IRL_STREAM_BIT);
	x->stream_flag |= (x->stream_loop << LOOP_STREAM_BIT);

	x->stream_col1_addr = 0;
	x->stream_col2_addr = x->stream_col1 * 3;
	x->stream_mst_addr  = x->stream_col2_addr + x->stream_col2 * 3;
	x->stream_stb_addr  = x->stream_mst_addr  + x->stream_mst  * 2;
	x->stream_irl_addr  = x->stream_stb_addr  + x->stream_stb  * 2;
	x->stream_mot_addr  = x->stream_irl_addr  + x->stream_irl  * 2;
	x->stream_bytes     = x->stream_mot_addr  + x->stream_mot  * 2;

	x->stream_col1_args = 0;
	x->stream_col2_args = x->stream_col1 * 3;
	x->stream_mst_args = x->stream_col2_args + x->stream_col2 * 3;
	x->stream_stb_args = x->stream_mst_args + x->stream_mst;
	x->stream_irl_args = x->stream_stb_args + x->stream_stb;
	x->stream_mot_args = x->stream_irl_args + x->stream_irl;
	x->stream_args = x->stream_mot_args + x->stream_mot;
}

t_max_err stream_offset_set(t_sbSend *x, void *attr, long ac, t_atom *av)
{
	if (ac&&av) {
		long val = atom_getlong(av);
		if (val > 65535) val = 65535;
		else if (val < 0) val = 0;
		x->stream_offset = val;
	}
	else x->stream_offset = STREAM_OFFSET_DEF;
	return MAX_ERR_NONE;
}

t_max_err stream_col1_set(t_sbSend *x, void *attr, long ac, t_atom *av)
{
	if (ac&&av) {
		char val = atom_getcharfix(av);
		switch (val) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 6:
			x->stream_col1 = val;
			break;
		default:
			x->stream_col1 = STREAM_COL1_DEF;
		}
	}
	else x->stream_col1 = STREAM_COL1_DEF;
	sbSend_updateStreamFlag(x);
	return MAX_ERR_NONE;
}

t_max_err stream_col2_set(t_sbSend *x, void *attr, long ac, t_atom *av)
{
	if (ac&&av) {
		char val = atom_getcharfix(av);
		switch (val) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 6:
			x->stream_col2 = val;
			break;
		default:
			x->stream_col2 = STREAM_COL2_DEF;
		}
	}
	else x->stream_col2 = STREAM_COL2_DEF;
	sbSend_updateStreamFlag(x);
	return MAX_ERR_NONE;
}

t_max_err stream_loop_set(t_sbSend *x, void *attr, long ac, t_atom *av)
{
	if (ac&&av) x->stream_loop = atom_getcharfix(av) > 0;
	else		x->stream_loop = STREAM_LOOP_DEF;
	sbSend_updateStreamFlag(x);
	return MAX_ERR_NONE;
}


t_max_err stream_mst_set(t_sbSend *x, void *attr, long ac, t_atom *av)
{
	if (ac&&av) x->stream_mst = atom_getcharfix(av) > 0;
	else		x->stream_mst = STREAM_MST_DEF;
	sbSend_updateStreamFlag(x);
	return MAX_ERR_NONE;
}

t_max_err stream_stb_set(t_sbSend *x, void *attr, long ac, t_atom *av)
{
	if (ac&&av) x->stream_stb = atom_getcharfix(av) > 0;
	else		x->stream_stb = STREAM_STB_DEF;
	sbSend_updateStreamFlag(x);
	return MAX_ERR_NONE;
}

t_max_err stream_mot_set(t_sbSend *x, void *attr, long ac, t_atom *av)
{
	if (ac&&av) x->stream_mot = atom_getcharfix(av) > 0;
	else		x->stream_mot = STREAM_MOT_DEF;
	sbSend_updateStreamFlag(x);
	return MAX_ERR_NONE;
}

t_max_err stream_irl_set(t_sbSend *x, void *attr, long ac, t_atom *av)
{
	if (ac&&av) x->stream_irl = atom_getcharfix(av) > 0;
	else		x->stream_irl = STREAM_IRL_DEF;
	sbSend_updateStreamFlag(x);
	return MAX_ERR_NONE;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTRUCTION, DESTRUCTION
//---------------------------------------------------------------------------------------------------------------------------------------------------------

void *sbSend_new(t_symbol *s, long argc, t_atom *argv)
{	
	t_sbSend *x = (t_sbSend *)object_alloc(sbSend_class);

	memset(&x->addr, 0, sizeof(x->addr));
	x->addr.sin_family = AF_INET;

	boolean attr1 = argc > 0 ? atom_gettype(argv)   == A_SYM && atom_getsym(argv)->s_name[0]   == '@' : false;
	boolean attr2 = argc > 1 ? atom_gettype(argv+1) == A_SYM && atom_getsym(argv+1)->s_name[0] == '@' : false;

	// HANDLE ARGUMENTS
	if (argc == 0 || attr1) {
		x->addr.sin_addr.s_addr = syssock_inet_addr(DEFAULT_IP);
		x->addr.sin_port = syssock_htons(DEFAULT_PORT);
	}
	else if (argc >= 2 && !attr2) {
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

	// HANDLE ATTRIBUTES
	x->stream_loop   = STREAM_LOOP_DEF;
	x->stream_offset = STREAM_OFFSET_DEF;
	x->stream_col1   = STREAM_COL1_DEF;
	x->stream_col2   = STREAM_COL2_DEF;
	x->stream_mst    = STREAM_MST_DEF;
	x->stream_stb    = STREAM_STB_DEF;
	x->stream_mot    = STREAM_MOT_DEF;
	x->stream_irl    = STREAM_IRL_DEF;
	
	sbSend_updateStreamFlag(x);
	attr_args_process(x, argc, argv);

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

void sbSend_saveImu(t_sbSend *x) {
	sbSend_send(x, SAVE_IMU, 0, NULL);
}

void sbSend_saveFactory(t_sbSend *x) {
	sbSend_send(x, SAVE_FACTORY, 0, NULL);
}

void sbSend_saveGeneral(t_sbSend *x) {
	sbSend_send(x, SAVE_GENERAL, 0, NULL);
}

//---------------------------------------------------------------------------------------

void sbSend_master(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	if (atom_gettype(argv) == A_FLOAT) {
		float value = atom_getlong(argv);
		value = value < 0. ? 0. : value;
		value = value > 100. ? 100. : value;
		t_int16 v = value * 100.;
		t_uint8 data[] = { v >> 8, v & 255 };
		sbSend_send(x, CMD_MST, 2, data);
	}
	else {
		object_warn((t_object*)x, "Master argument not understood");
	}
}

void sbSend_strobe(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	if (atom_gettype(argv) == A_FLOAT) {
		float value = atom_getlong(argv);
		value = value < 0. ? 0. : value;
		value = value > 500. ? 500. : value;
		t_int16 v = value * 100.;
		t_uint8 data[] = { v >> 8, v & 255 };
		sbSend_send(x, CMD_STB, 2, data);
	}
	else {
		object_warn((t_object*)x, "Strobe argument not understood");
	}
}

void sbSend_infrared(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	if (atom_gettype(argv) == A_LONG) {
		long value = atom_getlong(argv);
		value = value < 0 ? 0 : value;
		value = value > 1023 ? 1023: value;
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
		long value = atom_getlong(argv);
		value = value < 0 ? 0 : value;
		value = value > 1023 ? 1023 : value;
		t_uint8 data[] = { (value >> 8) & 255,value & 255 };
		sbSend_send(x, CMD_MOT, 2, data);
	}
	else {
		object_warn((t_object*)x,"Vibration argument not understood");
	}
}

//---------------------------------------------------------------------------------------

void sbSend_color1(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	sbSend_color(x, s, argc, argv, CMD_COLOR1);
}

void sbSend_color2(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	sbSend_color(x, s, argc, argv, CMD_COLOR2);
}

void sbSend_color(t_sbSend *x, t_symbol *s, long argc, t_atom *argv, enum sb_cmd c)
{
	long value;
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
			value = atom_getlong(ap);
			break;
		case A_FLOAT:
			value = (long)atom_getfloat(ap)*255.;
			break;
		default:
			value = 0;
			break;
		}
		value = value < 0 ? 0 : value;
		value = value > 255 ? 255 : value;
		data[i] = (t_uint8)value;
	}

	sbSend_send(x, c, l, data);

END:
	sysmem_freeptr(data);
}

//---------------------------------------------------------------------------------------

void sbSend_stream(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	long value;
	t_atom *ap;
	t_uint8 *data;

	if (argc > 0 && argc % x->stream_args != 0) {
		object_warn((t_object*)x, "According to your parameters arguments number must be greater than 0 and a multiple of %d", x->stream_args);
		return;
	}

	t_uint16 n = argc / x->stream_args;
	t_uint16 l = STREAM_HEADER + n * x->stream_bytes;

	data = (t_uint8 *)sysmem_newptr(sizeof(t_uint8)*l);
	if (data == NULL) {
		object_error((t_object*)x, "Cannot allocate memory for stream data");
		goto END;
	}

	data[0] = (t_uint8)(x->stream_flag >> 8);
	data[1] = (t_uint8)x->stream_flag;
	data[2] = (t_uint8)(x->stream_offset >> 8);
	data[3] = (t_uint8)x->stream_offset;

	if (x->stream_col1 > 0) {
		ap = argv + x->stream_col1_args;
		for (t_uint16 i = 0; i < n; i++) {
			for (t_uint16 c = 0; c < 3 * x->stream_col1; c++) {
				switch (atom_gettype(ap+c)) {
				case A_LONG:
					value = atom_getlong(ap+c);
					break;
				case A_FLOAT:
					value = (long)atom_getfloat(ap+c)*255.;
					break;
				default:
					value = 0;
					break;
				}
				value = value < 0 ? 0 : value;
				value = value > 255 ? 255 : value;
				data[STREAM_HEADER + i*x->stream_bytes+x->stream_col1_addr+c] = (t_uint8)value;
			}
			ap += x->stream_args;
		}
	}

	if (x->stream_col2 > 0) {
		ap = argv + x->stream_col2_args;
		for (t_uint16 i = 0; i < n; i++) {
			for (t_uint16 c = 0; c < 3 * x->stream_col2; c++) {
				switch (atom_gettype(ap + c)) {
				case A_LONG:
					value = atom_getlong(ap + c);
					break;
				case A_FLOAT:
					value = (long)atom_getfloat(ap + c)*255.;
					break;
				default:
					value = 0;
					break;
				}
				value = value < 0 ? 0 : value;
				value = value > 255 ? 255 : value;
				data[STREAM_HEADER + i*x->stream_bytes + x->stream_col2_addr + c] = (t_uint8)value;
			}
			ap += x->stream_args;
		}
	}

	if (x->stream_mst) {
		ap = argv + x->stream_mst_args;
		for (t_uint16 i = 0; i < n; i++) {
			switch (atom_gettype(ap)) {
				case A_LONG:
					value = atom_getlong(ap)*100;
					break;
				case A_FLOAT:
					value = (long)(atom_getfloat(ap)*100.);
					break;
				default:
					value = 0;
					break;
			}
			value = value < 0 ? 0 : value;
			value = value > 10000 ? 10000 : value;
			data[STREAM_HEADER + i*x->stream_bytes + x->stream_mst_addr + 0] = (t_uint8)(value >> 8);
			data[STREAM_HEADER + i*x->stream_bytes + x->stream_mst_addr + 1] = (t_uint8)value;
			ap += x->stream_args;
		}
	}

	if (x->stream_stb) {
		ap = argv + x->stream_stb_args;
		for (t_uint16 i = 0; i < n; i++) {
			switch (atom_gettype(ap)) {
			case A_LONG:
				value = atom_getlong(ap) * 100;
				break;
			case A_FLOAT:
				value = (long)(atom_getfloat(ap)*100.);
				break;
			default:
				value = 0;
				break;
			}
			value = value < 0 ? 0 : value;
			value = value > 50000 ? 50000 : value;
			data[STREAM_HEADER + i*x->stream_bytes + x->stream_stb_addr + 0] = (t_uint8)(value >> 8);
			data[STREAM_HEADER + i*x->stream_bytes + x->stream_stb_addr + 1] = (t_uint8)value;
			ap += x->stream_args;
		}
	}

	if (x->stream_irl) {
		ap = argv + x->stream_irl_args;
		for (t_uint16 i = 0; i < n; i++) {
			switch (atom_gettype(ap)) {
			case A_LONG:
				value = atom_getlong(ap);
				break;
			case A_FLOAT:
				value = (long)(atom_getfloat(ap));
				break;
			default:
				value = 0;
				break;
			}
			value = value < 0 ? 0 : value;
			value = value > 1023 ? 1023: value;
			data[STREAM_HEADER + i*x->stream_bytes + x->stream_irl_addr + 0] = (t_uint8)(value >> 8);
			data[STREAM_HEADER + i*x->stream_bytes + x->stream_irl_addr + 1] = (t_uint8)value;
			ap += x->stream_args;
		}
	}

	if (x->stream_mot) {
		ap = argv + x->stream_mot_args;
		for (t_uint16 i = 0; i < n; i++) {
			switch (atom_gettype(ap)) {
			case A_LONG:
				value = atom_getlong(ap);
				break;
			case A_FLOAT:
				value = (long)(atom_getfloat(ap));
				break;
			default:
				value = 0;
				break;
			}
			value = value < 0 ? 0 : value;
			value = value > 1023 ? 1023 : value;
			data[STREAM_HEADER + i * x->stream_bytes + x->stream_mot_addr + 0] = (t_uint8)(value >> 8);
			data[STREAM_HEADER + i * x->stream_bytes + x->stream_mot_addr + 1] = (t_uint8)value;
			ap += x->stream_args;
		}
	}

	sbSend_send(x, CMD_STREAM, l, data);

END:
	sysmem_freeptr(data);
}

//---------------------------------------------------------------------------------------

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
	if (argc == 3) {
		if (atom_gettype(argv) == A_LONG && atom_gettype(argv + 1) == A_LONG && atom_gettype(argv + 2) == A_FLOAT) {
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

void sbSend_setGeneral(t_sbSend *x, t_symbol *s, long argc, t_atom *argv)
{
	if (argc == 1) {
		if (atom_gettype(argv) == A_LONG) {
			long id = atom_getlong(argv); // ID number
			t_uint8 data[] = {
				(id >> 8) & 255, id & 255
			};
			sbSend_send(x, CMD_GENERAL, 2, data);
		}
		else {
			object_warn((t_object*)x, "Set general settings arguments not understood");
		}
	}
	else {
		object_warn((t_object*)x, "Too few arguments to change general settings");
	}
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------
// MESSAGE HANDLER
//---------------------------------------------------------------------------------------------------------------------------------------------------------

void sbSend_send(t_sbSend* x, enum sb_cmd c, t_uint16 l, t_uint8 *d) {
	// Memory allocation
	t_uint8 *msg;
	t_uint16 length = l + 1;

	msg = (t_uint8 *)sysmem_newptr(sizeof(t_uint8)*(length));

	if (msg == NULL) {
		object_error((t_object*)x, "Cannot allocate memory for data sending");
		goto END;
	}

	// Packet preparation
	msg[0] = c; 
	for (t_uint16 i = 0; i < l; i++) {
		msg[i+1]  = d[i];
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