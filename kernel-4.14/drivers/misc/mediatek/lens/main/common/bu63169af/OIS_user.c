/////////////////////////////////////////////////////////////////////////////
// File Name	: OIS_user.c
// Function		: User defined function.
// 				  These functions depend on user's circumstance.
// 				  
// Rule         : Use TAB 4
// 
// Copyright(c)	Rohm Co.,Ltd. All rights reserved 
// 
/***** ROHM Confidential ***************************************************/
#ifndef OIS_USER_C
#define OIS_USER_C
#endif

#include "OIS_head.h"


#define OIS_DEBUG

#ifdef OIS_DEBUG
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#endif

#ifdef OIS_DEBUG
#define OIS_DRVNAME "BU63165AF_OIS"
#define LOG_INF(format, args...)                                               \
	pr_info(OIS_DRVNAME " [%s] " format, __func__, ##args)
#endif

// Following Variables that depend on user's environment			RHM_HT 2013.03.13	add
OIS_UWORD			FOCUS_VAL	= 0x0200;				// Focus Value


// /////////////////////////////////////////////////////////
// VCOSET function
// ---------------------------------------------------------
// <Function>
//		To use external clock at CLK/PS, it need to set PLL.
//		After enabling PLL, more than 30ms wait time is required to change clock source.
//		So the below sequence has to be used:
// 		Input CLK/PS --> Call VCOSET0 --> Download Program/Coed --> Call VCOSET1
//
// <Input>
//		none
//
// <Output>
//		none
//
// =========================================================
void	VCOSET0( void )
{
    // !!!  Depend on user system   !!!
    OIS_UWORD 	CLK_PS = 24000;            					    // Input Frequency [kHz] of CLK/PS terminal (Depend on your system)
    // !!!  Depend on user system   !!!

    OIS_UWORD 	FVCO_1 = 36000;                				    // Target Frequency [kHz]
    OIS_UWORD 	FREF   = 25;             						// Reference Clock Frequency [kHz]
 
    OIS_UWORD	DIV_N  = CLK_PS / FREF - 1;         			// calc DIV_N
    OIS_UWORD	DIV_M  = FVCO_1 / FREF - 1;         			// calc DIV_M
 
    I2C_OIS_per_write( 0x62, DIV_N  ); 							// Divider for internal reference clock
    I2C_OIS_per_write( 0x63, DIV_M  ); 							// Divider for internal PLL clock
    I2C_OIS_per_write( 0x64, 0x4060 ); 							// Loop Filter

    I2C_OIS_per_write( 0x60, 0x3011 ); 							// PLL
    I2C_OIS_per_write( 0x65, 0x0080 ); 							// 
    I2C_OIS_per_write( 0x61, 0x8002 ); 							// VCOON 
    I2C_OIS_per_write( 0x61, 0x8003 ); 							// Circuit ON 
    I2C_OIS_per_write( 0x61, 0x8809 ); 							// PLL ON
}


void	VCOSET1( void )
{
    I2C_OIS_per_write( 0x05, 0x000C ); 							// Prepare for PLL clock as master clock
    I2C_OIS_per_write( 0x05, 0x000D ); 							// Change to PLL clock
}


void	WR_I2C( OIS_UBYTE slvadr, OIS_UBYTE size, OIS_UBYTE *dat )
{
    // /////////////////////////////////////////////////
    // Please write your own code for I2C write access.
    // /////////////////////////////////////////////////

	// OIS_UWORD       addr = dat[0] << 8 | dat[1];
	// OIS_UBYTE	*data_wr   = dat + 2;
	/*unsigned char output[128];
	int i;

	for (i = 0; i < size; i++)
	{
		if (i == 0)
			sprintf(output, "%02x |", dat[0]);
		else
			sprintf(output, "%s %02x", output, dat[i]);
	}

	LOG_INF("%s", output);*/

	s4AF_WriteReg_BU63169AF(slvadr << 1, dat, size);
}



OIS_UWORD	RD_I2C( OIS_UBYTE slvadr, OIS_UBYTE size, OIS_UBYTE *dat )
{
    // /////////////////////////////////////////////////
    // Please write your own code for I2C read access.  
    // /////////////////////////////////////////////////

	unsigned short int read_data = 0;
	unsigned short int read_data_h = 0;

	if (size == 1) {
		dat[1] = 0;
		s4AF_ReadReg_BU63169AF(slvadr << 1, dat, 2,
				       (unsigned char *)&read_data, 2);
	} else if (size == 2) {
		s4AF_ReadReg_BU63169AF(slvadr << 1, dat, 2,
				       (unsigned char *)&read_data, 2);
	} 

	read_data_h = read_data >> 8;
	read_data = read_data << 8;
	read_data = read_data | read_data_h;

	return read_data;
}


// *********************************************************
// Write Factory Adjusted data to the non-volatile memory
// ---------------------------------------------------------
// <Function>
//		Factory adjusted data are sotred somewhere
//		non-volatile memory.
//
// <Input>
//		_FACT_ADJ	Factory Adjusted data
//
// <Output>
//		none
//
// <Description>
//		You have to port your own system.
//
// *********************************************************
void	store_FADJ_MEM_to_non_volatile_memory( _FACT_ADJ param )
{
	/* 	Write to the non-vollatile memory such as EEPROM or internal of the CMOS sensor... */	
}


// *********************************************************
// Read Factory Adjusted data from the non-volatile memory
// ---------------------------------------------------------
// <Function>
//		Factory adjusted data are sotred somewhere
//		non-volatile memory.  I2C master has to read these
//		data and store the data to the OIS controller.
//
// <Input>
//		none
//
// <Output>
//		_FACT_ADJ	Factory Adjusted data
//
// <Description>
//		You have to port your own system.
//
// *********************************************************
_FACT_ADJ	get_FADJ_MEM_from_non_volatile_memory( void )
{
	unsigned short ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24C2, &ReadData);
	FADJ_MEM.gl_CURDAT = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24C4, &ReadData);
	FADJ_MEM.gl_HALOFS_X = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24C6, &ReadData);
	FADJ_MEM.gl_HALOFS_Y = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24C8, &ReadData);
	FADJ_MEM.gl_HX_OFS = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24CA, &ReadData);
	FADJ_MEM.gl_HY_OFS = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24CC, &ReadData);
	FADJ_MEM.gl_PSTXOF = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24CE, &ReadData);
	FADJ_MEM.gl_PSTYOF = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24D0, &ReadData);
	FADJ_MEM.gl_GX_OFS = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24D2, &ReadData);
	FADJ_MEM.gl_GY_OFS = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24D4, &ReadData);
	FADJ_MEM.gl_KgxHG = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24D6, &ReadData);
	FADJ_MEM.gl_KgyHG = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24D8, &ReadData);
	FADJ_MEM.gl_KGXG = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24DA, &ReadData);
	FADJ_MEM.gl_KGYG = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24DC, &ReadData);
	FADJ_MEM.gl_SFTHAL_X = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24DE, &ReadData);
	FADJ_MEM.gl_SFTHAL_Y = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24E0, &ReadData);
	FADJ_MEM.gl_TMP_X_ = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24E2, &ReadData);
	FADJ_MEM.gl_TMP_Y_ = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24E4, &ReadData);
	FADJ_MEM.gl_KgxH0 = (unsigned short int)ReadData;

	s4EEPROM_ReadReg_BU63169AF(0x24E6, &ReadData);
	FADJ_MEM.gl_KgyH0 = (unsigned short int)ReadData;

	return FADJ_MEM;		// Note: This return data is for DEBUG.
}

