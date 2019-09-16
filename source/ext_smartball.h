//---------------------------------------------------------------------------------------------------------------------------------------------------------
// CONSTANTS
//---------------------------------------------------------------------------------------------------------------------------------------------------------

#define ID_MSB			0
#define ID_LSB			1
#define SN_MSB			2
#define SN_LSB			3
#define CMD_ADDR		4
#define DATA_ADDR  		5
#define MIN_PACKET 		5

#define IMU_MASK   		0b11111111
#define RANGE_MASK 		0b00000011

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// STREAM FLAG ADDRESSES (16bit)
//---------------------------------------------------------------------------------------------------------------------------------------------------------

#define COL_STREAM_MASK	 0b111
#define COL1_STREAM_ADDR 0
#define COL2_STREAM_ADDR 3
#define MST_STREAM_BIT   6
#define STB_STREAM_BIT   7
#define IRL_STREAM_BIT   8
#define MOT_STREAM_BIT   9
#define LOOP_STREAM_BIT	 15

//---------------------------------------------------------------
// IMU FLAG ADDRESSES (8bit)
//---------------------------------------------------------------

#define ACC_BIT  0
#define GYR_BIT  1
#define MAG_BIT  2
#define TMP_BIT  3
#define VEC_BIT  4
#define QUA_BIT  5
#define WLD_BIT  6
#define STA_BIT	 7

//---------------------------------------------------------------------------------------------------------------------------------------------------------
// ENUMERATIONS
//---------------------------------------------------------------------------------------------------------------------------------------------------------

enum sb_cmd {
	CMD_BAT      = 0x00,
	CMD_PING     = 0x01,
	CMD_REBOOT   = 0x02,
	CMD_FACTORY  = 0x10,
	SAVE_FACTORY = 0x11,
	CMD_GENERAL  = 0x12,
	SAVE_GENERAL = 0x13,
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