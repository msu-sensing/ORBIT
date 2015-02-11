#ifndef __PROTOCOLDEFS_H__
#define __PROTOCOLDEFS_H__

#include "Arduino.h"
#include "Time.h"

#ifndef PACKED
#define PACKED __attribute__ ((packed))
#endif
//
//	Define the message type identifiers
//

typedef enum {
  UNUSED0				  = 0x00,		// unused
  GET_PARAMS	          = 0x01,		// request parameters
  REPORT_PARAMS			  = 0x01,		// response parameter values
  SET_PARAMS			  = 0x02,		// request set parameter values
  SET_PARAMS_STATUS		  = 0x02,		//response - set parameter value
  GET_SAMPLES			  = 0x03,		// request n samples
  SEND_SAMPLES			  = 0x03,		// send specified number of samples
  GET_GPS_POSITION		  = 0x04,		// request position
  REPORT_GPS_POSITION	  = 0x04,		// report position
  SEND_TASK_LIST		  = 0x05,		// send a task list to board incoming only
  TASK_OPERATION		  = 0x06,		// perform a task operation  incoming only
  SET_TASK_ARG			  = 0x07,		// set a task input argument incoming only
  SEND_TASK_RESULT		  = 0x08,		// send results of task to phone 
  ROBOT_CONTROL			  = 0x09,		// Robot control
  GET_SENSOR_STATUS		  = 0x0A,		// Request sensor status
  REPORT_SENSOR_STATUS    = 0x0A,		// Response sensor status
  RADIO_MESSAGE			  = 0x0B,		// message to/from XBEE Radio
  SD_OPERATION			  = 0x0C,		// SD Card Operations
  
  //
  //	Seismic messages
  //
  PEVENT_LIST			= 0x0D,
 
   
  //
  //	Special messages
  //
  LOG_MESG				  = 0xE,		// log Message
  TEXT_MESG 			  = 0xF,		// Text Message
  MONITOR_SENSOR		  = 0x10,
   

  // BOOKMARK(add_feature): Add new message type to enum.
  MESSAGE_TYPE_LIMIT,
  SYNC					  = 0x7F,		// Special synchronize message
  RESPONSE_FLAG			  = 0x80
} MESSAGE_TYPE;


//
//	The following structures define the format of the messages
//	The INCOMING_MESSAGE and OUTGOING_MESSAGE structures define the
//	message structure. The first byte is always the message type.
//
//	The following structures define the format after the type byte.
//

// reserved
typedef struct PACKED {
  // for future use
} RESERVED_ARGS;

//
//	Get/Set node parameters
//
//	Define the parameter types
//
typedef enum {
    
	SUBSYS_NODE 				= 0x0000,	// readonly
		NODE_BATTERY 			= 0x0001,	// readonly
		NODE_SOLAR				= 0x0002,	// readonly
		NODE_CURRENT			= 0x0003,	// readonly
		NODE_LOGGINGLEVEL       = 0x0004,	// levels are defined by the log class
		NODE_LOGSERIAL			= 0x0005,
		NODE_CONFIG				= 0x0006,	// config operation
		NODE_STATUSPERIOD		= 0x0007,	// node status polling period
		
	SUBSYS_EVENT 				= 0x0100,
		EVENT_SAMPLING_STATE  	= 0x0101,
		EVENT_SAMPLE_RATE		= 0x0102,	// readonly
		//
		//	STA/LTA event detection algorithm parameters
		//
		EVENT_LTADURATION 		= 0x0103,
		EVENT_STADURATION		= 0x0104,
		EVENT_HIGHTHRESHOLD		= 0x0105,
		EVENT_LOWTHRESHOLD		= 0x0106,
		EVENT_MINDURATION		= 0x0107,
		EVENT_SKIPDURATION		= 0x0108,
		//
		EVENT_SETSAMPLELEVEL  	= 0x0109,	// set the level to notify samples are available
		EVENT_SAMPLESAVAILABLE	= 0x010A,	// number of samples available
		EVENT_SIMULATE			= 0x010B,	// use simulated input
	
	SUBSYS_GPS					= 0x0200,
		GPS_GETSTATUS			= 0x0201	// readonly
		
} PARAMIDENTIFIERS;

typedef enum {
	CONFIG_LOAD		= 0x01,		// load the current config from file
	CONFIG_SAVE		= 0x02,		// Save the current node configuration to file
};

typedef struct PACKED {		// request/get parameters message
	uint8_t num_params;
	uint16_t paramids[];
} PARAM_LIST_ARGS;

typedef struct PACKED {
	uint8_t tracking :1;
	uint8_t fixInd : 1;
	uint8_t numSat;
} GPS_STATUS;

typedef struct PACKED {
	uint16_t paramid;
	union PACKED {
		//
		//	Generic parameters
		//
		byte b[4];
		uint32_t up32;
		int32_t ip32;
		uint32_t status; 
		float flt;
		//
		// Specialized parameter values
		GPS_STATUS gStatus;
		
	} pvalue;
} PARAMS;

typedef enum {
	PCODES_SUCCESS				= -1,
	PCODES_UNKNOWNPARAM			= 1,
	PCODES_INVALIDPARAMVALUE	= 2,
	PCODES_READONLY				= 3
} SETPARAMERRORCODES;

typedef struct PACKED {		// response/set parameters message
	uint8_t num_params;
	PARAMS params[];
} GETSET_PARAM_ARGS;

//
// ADC Reading
//
typedef struct PACKED {
	uint16_t number;
} GET_SAMPLES_ARGS;

#define MAX_READINGS 1
typedef struct PACKED {
	UNIXTIMEMS tm;
	int32_t reading[MAX_READINGS];
} SAMPLE_READING_ARGS;

#define MAX_MESSAGE_SAMPLES 20
// ADC multiple readings
typedef struct PACKED {
	uint8_t num;
	SAMPLE_READING_ARGS sample[];
} SEND_SAMPLES_ARGS;

typedef struct PACKED {
	char latInd;		//1
	uint32_t latitude;  //2
	char longInd;		// 6
	uint32_t longitude;	// 7
	int32_t alt;		// 11
} REPORT_POSITION_ARGS;

typedef struct PACKED {
	uint16_t length;	// total number of bytes in entire task list
	uint8_t id;			// task list id
	uint8_t num_msg;	// total number of messages in task list
	uint8_t seq;		// number of this message, starts at 1,
	uint8_t num_list;	// number of bytes in list for this message
	uint8_t list[];
} TASK_LIST_ARGS;

typedef struct PACKED {
	uint8_t id;
	uint8_t seq;
	uint8_t status;
	
} TASK_LIST_STATUS;

typedef enum {
	TASKLIST_SUCCESS = 0,		// complete list successfully received
	TASKLIST_MISSEDPACKET = 1,	// Packet dropped.
	TASKLIST_TOOMUCHDATA = 2	// send more bytes than in length
}	TASK_LIST_STATUSCODES;

//
//	TASK_RESULT_ARGS is used for sending args between the phone and
//	external board in both directions
//
typedef struct PACKED {
	uint8_t task_id;
	uint8_t arg_id;
	uint8_t num_msg;	// total number of messages needed to send results
	uint8_t seq;		// number of this message
	uint16_t max_result;		// total number of elements in this result
	uint8_t type;				// result type
	uint8_t num_result;			// number of elements of type in result for this message
	uint8_t result[];
} TASK_RESULT_ARGS;

typedef struct PACKED {
	uint8_t id;			// task List id
	uint8_t op;			// Operation
} TASK_OPERATION_ARGS;

typedef struct PACKED {
	uint8_t id;
	uint8_t op;
	uint8_t status;
} TASK_OPERATION_STATUS;

typedef enum {
	TASKOP_EXECUTE = 0x01,
	TASKOP_DELETE = 0x02,
	TASKOP_STOP = 0x03
}	TASK_OPERATIONS;

typedef enum {
	TASKSTATUS_SUCCESS = 0,
	TASKSTATUS_NOLISTS = 1,
	TASKSTATUS_UNDEFLIST = 2,
	TASKSTATUS_WRONGSTATE = 3,
	TASKSTATUS_INVALIDTASKID = 4,
	TASKSTATUS_UNKNOWNFUNCTION = 5
}	TASK_OP_STATUS;

typedef struct PACKED {
	uint8_t opcode;			// control function id
	union PACKED {
		uint8_t sensorid;
		int8_t speed;
		int8_t speeds[2];
		int16_t angle;
	} params;		// control function parameters
} ROBOT_CONTROL_ARGS;

typedef enum {
	//
	// operations with NO response message
	//
	ROBOT_MOVE	= 0x01,		// move both wheels at the same speed, param speed (byte) -100 to 100
	ROBOT_SPIN	= 0x02,		// move right wheel as speed, left wheel at -speed, param speed (byte) -100 to 100
	ROBOT_TURN	= 0x03,		// move wheels at different speeds, params speeds[2] (2 bytes) right speed + left speed
	ROBOT_PAN	= 0x04,		// pan to specified angle, params angle (int16_t) 0 to 180
	ROBOT_TILT	= 0x05,		// tilt to specified angle, params angle (int16_t) 0 to 180
	//
	//	Operations with a response message
	//
	
	ROBOT_STOP = 0xFF
} ROBOT_OPCODES;

typedef struct PACKED {
	uint8_t sensor;
} GET_SENSOR_STATUS_ARGS;

typedef struct PACKED {		// sensor status message
	uint8_t sensor;			// sensor id
	union PACKED {
		uint8_t status;		// number of bytes in status depends on the sensor
							// on/off sensors send only 1 byte of status
		long reading;
		long readings[3];	//later for e.g. 3-axis sensors
	};
							//	range sensors send 4 bytes of status
} SENSOR_STATUS_ARGS;

typedef enum {
	SENSOR_TOUCHLEFT	= 0x11,	// status (byte) 0 = false, 0xff = true
	SENSOR_TOUCHRIGHT	= 0x12,	// status (byte) 0 = false, 0xff = true
	SENSOR_PROXIMITY	= 0x13,	// status (byte) 0 = false, 0xff = true
	//
	//	Put other touch (on/off) sensors here
	//
	SENSOR_MINONEBYTE,
	//
	//	Start of sensors that return more than 1 byte if status information
	//
	SENSOR_RANGESENSOR	= 0x44,	// Reading (long(4bytes)) distance in cm, 0xFFFFFFFF if too far
	
	SENSOR_COMPASS		= 0xC5,
	SENSOR_GYRO			= 0xC6,
	SENSOR_ACCEL		= 0xC7,
	
	SENSOR_MAXID					//internal to robot
} SENSOR_IDS;

typedef struct PACKED {
	uint8_t msglen;
	uint8_t msg[];
} RADIO_MESSAGE_ARGS;

typedef struct PACKED {
	uint32_t	nodeaddr_lsb;
	uint32_t	nodeaddr_msb;
	UNIXTIMEMS	tm;
} PEVENT;

typedef enum {
	PEVENT_UNKNOWN = 0,
	PEVENT_LOCAL = 1,
	PEVENT_REGIONAL = 2,
	PEVENT_GLOBAL = 3
} PEVENT_TYPES;

typedef struct PACKED {
	uint8_t pevent_type;
	uint8_t num_events;
	PEVENT events[];
} PEVENT_ARGS;

typedef struct PACKED {
	uint8_t op;
	uint32_t param32;
	uint8_t len;
	uint8_t data[];
} SDOP_ARGS;

typedef enum {
	SD_LS = 1,
	SD_REM = 2,
	SD_GET = 3,
	SD_DONE = 0x80
} SD_OPERATIONS;

typedef struct PACKED {
	uint8_t num;
	byte text[];
} TEXT_MESSAGE;

typedef struct PACKED {
	uint8_t csum;
} SYNC_MESSAGE;
	
#define MAXLOGMESG 80

typedef struct PACKED {
	UNIXTIMEMS tm;
	uint8_t level;
	uint8_t length;
	char msg[];
} LOGGING_ARGS;

typedef struct PACKED {			// Sensor monitoring request
	int8_t channel;				//channel -1 stops any monitoring
	uint8_t downSample;			// number of samples to average together before including in output
} MONITOR_ARGS;

typedef struct PACKED {			// Sensor monitoring response
	UNIXTIMEMS ts;
	int16_t	samplePeriod;			// number of milliseconds between samples
	byte channel;
	byte nSamples;				// number of samples in this message
	int32_t initialReading;		// initial sensor reading
	int16_t sensorDeltas[];
} MONITOR_RESP_ARGS;

// BOOKMARK(add_feature): Add a struct for the new incoming / outgoing message
// arguments.

typedef struct PACKED {
  uint8_t type;
  union PACKED {
	PARAM_LIST_ARGS			param_list;
	GETSET_PARAM_ARGS		params;
	GET_SAMPLES_ARGS		get_samples;
							//
							//	GET GPS status has no arguments
							//
	TASK_LIST_ARGS			send_tasklist;
	TASK_OPERATION_ARGS		op_task;
	TASK_RESULT_ARGS		task_arg;
	ROBOT_CONTROL_ARGS		robot_control;
	GET_SENSOR_STATUS_ARGS  get_sensor;
	RADIO_MESSAGE_ARGS		radio_msg;
	SDOP_ARGS				sd_msg;
	SYNC_MESSAGE			sync_msg;
	TEXT_MESSAGE			text_msg;
	LOGGING_ARGS			log_msg;
	PEVENT_ARGS				pevent_list;
	MONITOR_ARGS			monitor;
	
 // BOOKMARK(add_feature): Add argument struct to the union.
  } args;
  uint8_t __vabuf[64];  // buffer for var args. never access directly!
} INCOMING_MESSAGE;

typedef struct PACKED {
  uint8_t type;
  union PACKED {
	GETSET_PARAM_ARGS						params;
	SEND_SAMPLES_ARGS						send_samples;
	REPORT_POSITION_ARGS					position;
	TASK_OPERATION_STATUS					task_status;
	TASK_LIST_STATUS						list_status;
	TASK_RESULT_ARGS						task_results;
	SENSOR_STATUS_ARGS						sensor_status;
	RADIO_MESSAGE_ARGS						radio_msg;
	SDOP_ARGS								sd_msg;
	SYNC_MESSAGE							sync_msg;
	TEXT_MESSAGE							text_msg;
	LOGGING_ARGS							log_message;
	PEVENT_ARGS								pevent_list;
	MONITOR_RESP_ARGS						monitor;
	
    // BOOKMARK(add_feature): Add argument struct to the union.
  } args;
} OUTGOING_MESSAGE;

const uint8_t outgoing_arg_size[MESSAGE_TYPE_LIMIT] = {
  sizeof(RESERVED_ARGS),			// no message type 0
  sizeof(GETSET_PARAM_ARGS),		// Report Parameters type 1
  sizeof(GETSET_PARAM_ARGS),		// Set param status report type 2
  sizeof(SEND_SAMPLES_ARGS),		// Send samples type 3
  sizeof(REPORT_POSITION_ARGS),		// GPS position type 4
  sizeof(TASK_LIST_STATUS),			// TASK List Status 5
  sizeof(TASK_OPERATION_STATUS),	// TASK operation status
  sizeof(RESERVED_ARGS),			// no message type 7 incoming only
  sizeof(TASK_RESULT_ARGS),			// task results - type 8 outgoing only
  sizeof(RESERVED_ARGS),			// no message type 9
  sizeof(SENSOR_STATUS_ARGS),		// Sensor status report type A
  sizeof(RADIO_MESSAGE_ARGS),		// radio message type B
  sizeof(SDOP_ARGS),				// SDOperation type C
  sizeof(PEVENT_ARGS),				// Seismic event type D
  sizeof(LOGGING_ARGS),				// Log message type E
  sizeof(TEXT_MESSAGE),				// text message type 15 (0x0F)
  sizeof(MONITOR_RESP_ARGS)			// monitor response args
   
  // BOOKMARK(add_feature): Add sizeof (argument for outgoing message).
  // Array is indexed by message type enum.
};

#endif  // __PROTOCOLDEFS_H__
