//---------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------

#define SOP 			0xE7
#define SOP_ADDR		0
#ifndef SB_RECEIVE
	#define CMD_ADDR		1
	#define LEN_MSB_ADDR	2
	#define LEN_LSB_ADDR	3
	#define DATA_ADDR  		4
	#define MIN_PACKET 		5
#else
	#define ID_MSB			1
	#define ID_LSB			2
	#define CMD_ADDR		3
	#define LEN_MSB_ADDR	4
	#define LEN_LSB_ADDR	5
	#define DATA_ADDR  		6
	#define MIN_PACKET 		7
#endif
#define IMU_MASK   		0b00001111
#define RANGE_MASK 		0b00000011

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// ENUMERATIONS
//---------------------------------------------------------------------------------------------------------------------------------------------------------

enum sb_cmd {
	CMD_BAT		 = 0x00,
	CMD_PING	 = 0x01,
	CMD_REBOOT	 = 0x02,
	CMD_SLEEP	 = 0x03,
	CMD_FACTORY  = 0x10,
	SAVE_FACTORY = 0x11,
	CMD_COLOR1   = 0x20,
	CMD_STREAM   = 0x21,
	CMD_COLOR2	 = 0x22,
	CMD_IMU      = 0x30,
	SAVE_IMU	 = 0x31,
	CMD_ACCRANGE = 0x32,
	CMD_GYRRANGE = 0x33,
	CMD_IRL      = 0x40,
	CMD_MOT      = 0x50,
	CMD_STB		 = 0x60,
	CMD_MST		 = 0x70
};

