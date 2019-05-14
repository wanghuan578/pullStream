
#ifndef _BASIC_TYPE_H_
#define _BASIC_TYPE_H_


#define MAX_STRING_SIZE			256
#define MEDIUM_STRING_SIZE			128
#define SMALL_STRING_SIZE			64
#define MIN_STRING_SIZE			32
#define MINIUM_STRING_SIZE			16



#define SUCCESS 							0
#define FAILURE 							~0
#define RETRY								1

#define __WORD_SIZE 64
	//////////////////////////////////  type definition ///////////////////////////////////
#if	__WORD_SIZE == 32
	typedef char 								s8;
	typedef unsigned char 						u8;
	typedef signed short int 					s16;
	typedef unsigned short int 					u16;
	typedef signed int 							s32;
	typedef unsigned int 						u32;
	typedef float								f32;
	typedef u8 									boolean;
	typedef double								d64;
	typedef signed long 						sl32;
	typedef unsigned long 						ul32;
	typedef signed long long 					sl64;
	typedef unsigned long long 					ul64;
	typedef long double							ld96;
#endif

#if	__WORD_SIZE == 64
	typedef char 								s8;
	typedef unsigned char 						u8;
	typedef signed short int 					s16;
	typedef unsigned short int 					u16;
	typedef signed int 							s32;
	typedef unsigned int 						u32;
	typedef float								f32;
	typedef u8 									boolean;
	typedef double								d64;
	typedef signed long 						sl64;
	typedef unsigned long						ul64;
	typedef signed long long 					sll64;
	typedef unsigned long long 					ull64;
	typedef long double							ld128;
#endif

	/////////////////////////// struct definition ////////////////////////////
	typedef struct p_buf_s
	{
		s8		*buf;	
		u32		l;		
	}P_BUF_S;

	//////////////////////////////////  boolean definition  ///////////////////////////////
//#define true									(1 == 1)
//#define false   								!true
	//////////////////////////////////  point definition  ////////////////////////////////
#if	__WORD_SIZE == 32
	typedef s8*									pS8;
	typedef u8*									pU8;
	typedef s16*								pS16;
	typedef u16*								pU16;
	typedef s32*								pS32;
	typedef u32*								pU32;
	typedef f32*								pF32;
	typedef d64*								pD64;
	typedef sl32*								pSl32;
	typedef ul32*								pUl32;
	typedef sl64*								pSl64;
	typedef ul64*								pUl64;
	typedef ld96								pLd96;
	typedef void*								pVoid;
	typedef boolean*							pBoolean;
#endif

#if	__WORD_SIZE == 64

	typedef s8*									pS8;
	typedef u8*									pU8;
	typedef s16*								pS16;
	typedef u16*								pU16;
	typedef s32*								pS32;
	typedef u32*								pU32;
	typedef f32*								pF32;
	typedef d64*								pD64;
	typedef sl64*								pSl64;
	typedef ul64*								pUl64;
	typedef sll64*								pSll64;
	typedef ull64*								pUll64;
	typedef ld128								pLd128;
	typedef void*								pVoid;
	typedef boolean*							pBoolean;
#endif
///////////////////////////////////  end  ///////////////////////////////////////////////


#ifdef TRACE

//#define TRACE_IN(a,b) (a)->trace_in((b));
//#define TRACE_OUT(a)	(a)->trace_out();
//#define DEBUG(a,b,...)	(a)->prt(PURE_LOG_TYPE_TERM,"File:%s, Line:%d, Function:%s\n\t"b,__FILE__,__LINE__,__FUNCTION__,##__VA_ARGS__);
#define PDEBUG(b,...)	printf("File:%s, Line:%d, Function:%s\t"b"\n",__FILE__,__LINE__,__FUNCTION__,##__VA_ARGS__);
#else

//#define TRACE_IN(a,b)
//#define TRACE_OUT(a)
//#define DEBUG(a,b,...)
#define PDEBUG(b,...)

#endif

#endif /* _BASIC_TYPE_H_ */
