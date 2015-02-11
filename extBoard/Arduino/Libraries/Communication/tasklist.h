//++
//
//  FACILITY:    Orbit
//
//  ABSTRACT:
//		The definitions contained in this file define the in memory representation of
//		a sequence of tasks which should be executed on the Orbit extension board. A task
//		structure has three main areas: 1) tasks to be executed, 2) Input/output arguments
//		for the various task 3) storage for vector arguments.
//
//		+-----------------+
//		|       Task      |
//      |     Entries     |
//      +-----------------+
//		|       Task      |
//		|     Arguments   |
//      +-----------------+
//		|      Vector     |
//      |     Storage     |
//	    +-----------------+
//
//		Task entry area and Task Arguments have a header describing the area. The first 16 bits is the length in bytes
//		of the area. This length added to the start of the area gives the start of the next area. The Vector Storage area
//		does not have a header. That is the first vector begins at offset 0 within that area.
//
//		The TASK_LIST structure defines the overall structure of the Task Entries area while the TASK_ENTRY structure
//		defines the structure of the individual tasks.
//
//		The TASK_ARGLIST structure defines the overall structure of the Task Arguments area while the TASK_ARG structure
//		defines the structure of the individual Input/Output arguments for the tasks. An identifier between 0 and 255 is
//		assigned to each task argument. When an argument is an output from one task than then is an input to a subsequent
//		task the tasks will reference the same argument ID. These ID should be assigned sequentially beginning at 0 to the
//		maximum number of unique arguments in the task structure. ID numbers should not be skipped.
//
//  AUTHORS: Dennis Phillips, Michigan State University, Department of Computer Science
//
//  CREATION DATE:   Jan 30, 2013
//
//--

#ifndef TASKLIST_H
#define TASKLIST_H

#include "time.h"			// used to define UNIXTIMEMS and other time related argument types
#include "sample.h"			// used to define sample argument type
#include "gps.h"			// used to define position arg type

#ifndef PACKED
#define PACKED __attribute__ ((packed))
#endif


typedef struct PACKED {
	byte argID;
	byte argType;
	union PACKED {
		int16_t	int16arg;		// 16 bits
		int32_t int32arg;		// 32 bits
		float floatarg;			// 32 bits
		uint32_t vectOffset;	// 32 bits
	} arg;
	
} TASK_ARG;

typedef struct PACKED {
	UNIXTIMEMS tm;
	byte num;
	uint32_t timings[];
} TIMINGS_TYPE;

typedef enum {
	INT16TYPE = 0x01,
	INT32TYPE = 0x02,
	FLOATTYPE = 0x03,
	VECTORINT16TYPE = 0x11,
	VECTORINT32TYPE = 0x12,
	VECTORFLOATTYPE = 0x13,
	VECTORSAMPLETYPE = 0x14,
	VECTORPOSITIONTYPE = 0x15,
	VECTORDATETIMESHORTTYPE = 0x16,		// does not include milliseconds TIMESHORT
	VECTORTIMINGSTYPE = 0x17,
	VECTORUNIXTIMETYPE = 0x18,
	VECTORCOMPLEX32TYPE = 0x19,
	VECTORUSERTYPE = 0xFF
} ARG_TYPES;

typedef struct PACKED {
	byte taskID;			//Task number 0, 1, 2, 3, 4
	byte functionID;
	byte numInArgs;
	byte numOutArgs;
	byte argIDs[];	
} TASK_ENTRY;

typedef struct PACKED {
	uint16_t length;			// number of bytes in this structure including length
	byte listID;
	byte numTasks;
	TASK_ENTRY tasks[];
} TASK_LIST;

typedef struct PACKED {
	uint16_t length;			// number of bytes in this structure including length
	byte numArgs;
	TASK_ARG args[];
} TASK_ARGLIST;
//
//	Define the functionIDs for the various functions supported
//
typedef enum {
	FUNC_BRANCH		= 0,	// Branch(IN int32 op, IN int32 arg1, IN int32 arg2, IN int32 TaskID) if arg1 op arg2 is true then execute TaskID else execute next sequential task 
	FUNC_SENDRESULT = 1,	// Send(taskID, argID)	send to phone argID associated with taskID
	FUNC_SAMPLECTL	= 2,	// SampleControl(IN int32 op); op = 0 stop, op = 1 start;
	FUNC_GETSAMPLE  = 3,	// GetSample(OUT Sample result)
	FUNC_FILTER		= 4,	// Filter(IN SAMPLE inSample, IN VectorInt32 coeff, OUT SAMPLE result)
	FUNC_STALTA		= 5,	// STALTA(IN SAMPLE inSample, IN Int32 sampleRate, IN Int32 ltaDuration, 
							//		IN Int32 staDuration, IN Int32 highThreshold, IN Int32 lowThreshold, 
							//		IN Int32 minDuration, IN Int32 skipDuration) 
							//		durations are in terms of the number of samples, Thresholds are thousands i.e. if you want the
							//		threshold set at 2.5 then a value of 2500 should be passed.
							//
	FUNC_MEAN		= 6,	// Mean(IN VectorInt32 x, IN int16 length, OUT int32 result)
	FUNC_VAR		= 7,	// VAR(IN VectorInt32 x, IN int16 length, OUT int32 result)
	FUNC_ABS		= 8,	// ABS(IN int32 x, OUT int32 result)
	FUNC_SQRT		= 9,	// SQRT(IN int32 x, OUT int32 result)
	FUNC_MCOPY		= 10,	// Copy(IN VectorInt32 A, int n, int m, OUT VectorInt32 B);
	FUNC_MMULT		= 11,	// Multiply(IN VectorInt32 A, IN VectorInt32 B, int m, int p, int n, OUT VectorInt32  C);
	FUNC_MADD		= 12,	// Add(IN VectorInt32 A, IN VectorInt32 B, int m, int n, OUT VectorInt32 C);
	FUNC_MSUB		= 13,	// Subtract(IN VectorInt32 A, IN VectorInt32 B, int m, int n, OUT VectorInt32 C);
	FUNC_MTRANS		= 14,	// Transpose(IN VectorInt32 A, int m, int n, OUT VectorInt32 C);
	FUNC_MSCALE		= 15,	// Scale(IN VectorInt32 A, int m, int n, Int32 k);
	FUNC_MINVERT	= 16,	// Invert(IN VectorInt32 A, int n, OUT Int32 status);
	FUNC_FFT		= 17,	// FFT(IN VectorInt32 A, OUT VectorComplex32 result);
	FUNC_CONVOLVE   = 18,	// convolve(IN VectorInt32 Signal[/* SignalLen */], Int32 SignalLen,
							// 		IN VectorInt32 Kernel[/* KernelLen */], Int32 KernelLen,
							//		OUT VectorInt32 Result[/* SignalLen + KernelLen - 1 */])
	FUNC_AUTOCORR	= 19,	// autocorrelation(IN VectorInt32 *x, OUT VectorInt32 *R, Int32 order)
	FUNC_CROSSCORR  = 20	// crossCorrelation(IN VectorInt_t *x, IN VectorInt32 *y, OUT VectorInt32 *r, IN Int32 n, IN Int32 maxdelay)

	
} FUNCTION_IDS;

typedef enum {
	BRANCH_GOTO = 0,
	BRANCH_EQ = 1,
	BRANCH_NE = 2,
	BRANCH_LT = 3,
	BRANCH_LE = 4,
	BRANCH_GT = 5,
	BRANCH_GE = 6
} BRANCH_OPS;

#define getArg(x, argID)  (byte *)&x->args[argID]
#define getArgStart(x) (TASK_ARGLIST *)( (byte *)(x)[x->length] )
#define getVectorStart(x) (byte *)(&x->args[x->numArgs])
#define nextTaskEntry(x)	(TASK_ENTRY *)&x->argIDs[x->numInArgs + x->numOutArgs]

#endif