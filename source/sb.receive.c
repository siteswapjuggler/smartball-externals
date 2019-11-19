/*
	sbReceive.c - receive data from smarball devices

	this object has one inlet and one outlet
	it decodes formatted smartball datagram from udp
	it responds to the 'assistance' message sent by Max when the mouse is positioned over an inlet or outlet

	https://github.com/CNMAT/CNMAT-Externs source code is a great source of inspiration
	https://www.binarytides.com/udp-socket-programming-in-winsock/
	https://gist.github.com/hostilefork/f7cae3dc33e7416f2dd25a402857b6c6
*/

#include "sb.receive.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------
//  INSTANCE DECLARATION
//---------------------------------------------------------------------------------------------------------------------------------------------------------

t_class *sbReceive_class;		// global pointer to the object class - so max can reference the object

void ext_main(void *r)
{
	t_class *c;																											// pointer to a class type
	c = class_new("sb.receive", (method)sbReceive_new, (method)sbReceive_free, sizeof(t_sbReceive), 0L, A_GIMME, 0); 	// class_new() loads our external's class into Max's memory so it can be used in a patch
	class_register(CLASS_BOX, c);																						// register to CLASS_BOX type for max environment
	sbReceive_class = c;
	post("sb.receive v0.31 - 19.05.2019");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTRUCTION, DESTRUCTION
//---------------------------------------------------------------------------------------------------------------------------------------------------------

void *sbReceive_new(t_symbol *s, long argc, t_atom *argv)
{	
	t_sbReceive *x = (t_sbReceive *)object_alloc(sbReceive_class);

	// HANDLE ARGUMENTS
	if (argc == 0) {
		x->port = DEFAULT_PORT;
		x->multicast = DEFAULT_MULTI;
	}
	else if (argc == 1) {
		x->multicast = DEFAULT_MULTI;
		if (atom_gettype(argv) == A_LONG) {
			x->port = atom_getlong(argv);
		}
		else goto ARG_FAULT;
	}
	else if (argc == 2) {
		struct sockaddr_in sa;
		if (inet_pton(AF_INET, atom_getsym(argv)->s_name, &(sa.sin_addr))) {
			x->multicast = atom_getsym(argv)->s_name;
		}
		else {
			struct hostent *he = syssock_gethostbyname(atom_getsym(argv)->s_name);
			if (he != NULL) {
				struct in_addr **addr_list = (struct in_addr *) he->h_addr_list;
				if (addr_list[0] != NULL)
					inet_ntop(AF_INET, addr_list[0], x->multicast, INET_ADDRSTRLEN);
			}
			else goto ARG_FAULT;
		}
		
		if (atom_gettype(argv+1) == A_LONG) {
			x->port = atom_getlong(argv+1);
		}
		else goto ARG_FAULT;
	}
	else {
	ARG_FAULT:
		object_error((t_object*)x, "Cannot start with those arguments ");
		goto NOT_VALID;
	}

	// HANDLE SOCKET INITIATION
	if (syssock_set(x) < 0) goto NOT_VALID;
	
	// CREATE OUTLET
	x->outlet = outlet_new(x, NULL);
	return(x);

NOT_VALID:
	x = NULL;
	return(x);
}

void sbReceive_free(t_sbReceive *x) 
{
	if (x->listener)
	{
		x->listening = false;
		if (x->fd) {
			syssock_dropmulticast(x->fd, x->multicast);
			syssock_close(x->fd);
		}
		systhread_join(x->listener, NULL);
		x->listener = NULL;
	}
}

void sbReceive_assist(t_sbReceive *x, void *b, long m, long a, char *s) {
	if (m == ASSIST_INLET)
		;
	else 
		sprintf(s, "Smartball feedbacks");
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// MESSAGE LISTENER
//---------------------------------------------------------------------------------------------------------------------------------------------------------

void sbReceive_receive(t_sbReceive *x) {
	t_uint8 msgbuf[2048];
	int  nbytes;
	int  addrlen = sizeof(x->addr);

	while (x->listening) {
		nbytes = recvfrom(x->fd, msgbuf, 2048, 0, (struct sockaddr *) &x->addr, &addrlen);
		if (nbytes > 0) sbReceive_parse(x, msgbuf);
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// PARSE MESSAGE | [ID_MSB ID_LSB] [SN_MSB SN_LSB] CMD [DATA]
//---------------------------------------------------------------------------------------------------------------------------------------------------------

// SOP CMD MSB LSB IDMSB IDLSB

void sbReceive_parse(t_sbReceive *x, t_uint8 *msg) {
	
	t_uint8 index = 0;
	t_uint8 imu_flag = 0;
	t_atom  list[20];

	// ADD SERIAL NUMBER
	atom_setlong(list, (t_uint16)(msg[ID_MSB] << 8 | msg[ID_LSB]));

	// ADD DATE ACCORDING ON CMD ID
	switch (msg[CMD_ADDR]) {
	case CMD_PING:
		atom_setsym(list+1, gensym("pong"));
		outlet_list(x->outlet, NULL, 2, &list);
		break;
	case CMD_BAT:
		atom_setsym(list+1, gensym("bat"));
		atom_setfloat(list+2, (float)(t_int16)(msg[DATA_ADDR] << 8 | msg[DATA_ADDR + 1]) / 100.);
		outlet_list(x->outlet, NULL, 3, &list);
		break;
	case CMD_IMU:
		index = DATA_ADDR;
		imu_flag = msg[index];
		atom_setsym(list + 1, gensym("imu"));
		if (imu_flag & 1) {
			atom_setsym(list + 2, gensym("acc"));
			atom_setfloat(list + 3, (float)(t_int16)(msg[index + 1] << 8 | msg[index + 2]) / 100.);
			atom_setfloat(list + 4, (float)(t_int16)(msg[index + 3] << 8 | msg[index + 4]) / 100.);
			atom_setfloat(list + 5, (float)(t_int16)(msg[index + 5] << 8 | msg[index + 6]) / 100.);
			outlet_list(x->outlet, NULL, 6, &list);
			index += 6;
		}
		if (imu_flag & 2) {
			atom_setsym(list + 2, gensym("gyr"));
			atom_setfloat(list + 3, (float)(t_int16)(msg[index + 1] << 8 | msg[index + 2]) / 100.);
			atom_setfloat(list + 4, (float)(t_int16)(msg[index + 3] << 8 | msg[index + 4]) / 100.);
			atom_setfloat(list + 5, (float)(t_int16)(msg[index + 5] << 8 | msg[index + 6]) / 100.);
			outlet_list(x->outlet, NULL, 6, &list);
			index += 6;
		}
		if (imu_flag & 4) {
			atom_setsym(list + 2, gensym("mag"));
			atom_setfloat(list + 3, (float)(t_int16)(msg[index + 1] << 8 | msg[index + 2]) / 100.);
			atom_setfloat(list + 4, (float)(t_int16)(msg[index + 3] << 8 | msg[index + 4]) / 100.);
			atom_setfloat(list + 5, (float)(t_int16)(msg[index + 5] << 8 | msg[index + 6]) / 100.);
			outlet_list(x->outlet, NULL, 6, &list);
			index += 6;
		}
		if (imu_flag & 8) {
			atom_setsym(list + 2, gensym("tmp"));
			atom_setfloat(list + 3, (float)(t_int16)(msg[index + 1] << 8 | msg[index + 2]) / 100.);
			outlet_list(x->outlet, NULL, 4, &list);
			index += 2;
		}
		if (imu_flag & 16) {
			atom_setsym(list + 2, gensym("vec"));
			atom_setfloat(list + 3, (float)(t_int16)(msg[index + 1] << 8 | msg[index + 2]) / 100.);
			atom_setfloat(list + 4, (float)(t_int16)(msg[index + 3] << 8 | msg[index + 4]) / 100.);
			atom_setfloat(list + 5, (float)(t_int16)(msg[index + 5] << 8 | msg[index + 6]) / 100.);
			outlet_list(x->outlet, NULL, 6, &list);
			index += 6;
		}
		if (imu_flag & 32) {
			atom_setsym(list + 2, gensym("qua"));
			atom_setfloat(list + 3, (float)(t_int16)(msg[index + 1] << 8 | msg[index + 2]) / 10000.);
			atom_setfloat(list + 4, (float)(t_int16)(msg[index + 3] << 8 | msg[index + 4]) / 10000.);
			atom_setfloat(list + 5, (float)(t_int16)(msg[index + 5] << 8 | msg[index + 6]) / 10000.);
			atom_setfloat(list + 6, (float)(t_int16)(msg[index + 7] << 8 | msg[index + 8]) / 10000.); 
			outlet_list(x->outlet, NULL, 7, &list);
			index += 8;
		}
		if (imu_flag & 64) {
			atom_setsym(list + 2, gensym("wld"));
			atom_setfloat(list + 3, (float)(t_int16)(msg[index + 1] << 8 | msg[index + 2]) / 100.);
			atom_setfloat(list + 4, (float)(t_int16)(msg[index + 3] << 8 | msg[index + 4]) / 100.);
			atom_setfloat(list + 5, (float)(t_int16)(msg[index + 5] << 8 | msg[index + 6]) / 100.);
			outlet_list(x->outlet, NULL, 6, &list);
			index += 6;
		}
		if (imu_flag & 128) {
			atom_setsym(list + 2, gensym("sta"));
			atom_setlong(list + 3, (t_int16)(msg[index + 1] << 8 | msg[index + 2]));
			outlet_list(x->outlet, NULL, 4, &list);
			index += 2;
		}
		break;
		}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// SYSSOCK EXTENSIONS
//---------------------------------------------------------------------------------------------------------------------------------------------------------

int syssock_set(t_sbReceive *x) {
	if (syssock_init()) {
		object_error((t_object*)x, "Cannot start socket API");
		return -1;
	}

	x->fd = syssock_socket(AF_INET, SOCK_DGRAM, 0);
	if (x->fd < 0) {
		object_error((t_object*)x, "Cannot start UDP socket");
		return -1;
	}

	if (syssock_reuseaddr(x->fd)) {
		object_error((t_object*)x, "Cannot set socket REUSEADDR option");
		return -1;
	}

	memset(&x->addr, 0, sizeof(x->addr));
	x->addr.sin_family = AF_INET;
	x->addr.sin_addr.s_addr = syssock_htonl(INADDR_ANY);
	x->addr.sin_port = htons(x->port);

	if (syssock_bind(x->fd, &x->addr)) {
		object_error((t_object*)x, "Cannot bind to port %d", x->port);
		return -1;
	}

	if (syssock_addmulticast(x->fd, x->multicast)) {
		object_error((t_object*)x, "Cannot join %s group", x->multicast);
		return -1;
	}

	x->listening = true;
	systhread_create((method)sbReceive_receive, x, 0, 32, 0, &(x->listener));

	return 0;
}

int syssock_addmulticast(t_syssocket sockfd, char* ip) {
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = syssock_inet_addr(ip);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	return syssock_setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
}

int syssock_dropmulticast(t_syssocket sockfd, char* ip) {
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = syssock_inet_addr(ip);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	return syssock_setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq, sizeof(mreq));
}