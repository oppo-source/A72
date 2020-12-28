/////////////////////////////////////////////////////////////////////////////
// File Name	: OIS_main.c
// Function		: Main control function runnning on ISP.
//                ( But Just for example )
// Rule         : Use TAB 4
//
// Copyright(c)	Rohm Co.,Ltd. All rights reserved
//
/***** ROHM Confidential ***************************************************/
#ifndef OIS_MAIN_C
#define OIS_MAIN_C
#endif

#include "OIS_head.h"
#ifdef VENDOR_EDIT
//Feiping.Li@Cam.Drv, 20200115, add for ois engineer calibration
#include "BU63169.h"
#endif

////////////////////////////////////////////////////////////////////////////////
ADJ_STS		OIS_MAIN_STS  = ADJ_ERR;		// Status register of this main routine.

#ifdef VENDOR_EDIT
// Feiping.Li@Cam.Drv, 20200115, add for fix ois init error
void setOISMode(int Disable)
{
  if (Disable == 1)
		I2C_OIS_mem_write( 0x7F, 0x0C0C );
  else
		I2C_OIS_mem_write( 0x7F, 0x0D0D );
}
#else
void setOISMode(int Disable)
{
	if (Disable == 1)
		func_SET_SCENE_PARAM_for_NewGYRO_Fil(_SCENE_SPORT_3, 0,
						     &fadj);
	else
		func_SET_SCENE_PARAM_for_NewGYRO_Fil(_SCENE_SPORT_3, 1,
						     &fadj);
}
#endif

void OIS_Standby(void)
{
	I2C_OIS_F0123_wr_(0x90, 0x00, 0x0200);
	I2C_OIS_mem_write(0x7F, 0x0080);

	I2C_OIS_per_write(0x72, 0x0000);
	I2C_OIS_per_write(0x73, 0x0000);

	I2C_OIS_per_write(0x18, 0x000F);
	I2C_OIS_per_write(0x1B, 0x0B02);
	I2C_OIS_per_write(0x1C, 0x0B02);

	I2C_OIS_per_write(0x3D, 0x0000);
	I2C_OIS_per_write(0x22, 0x0300);
	I2C_OIS_per_write(0x59, 0x0000);
}

// MAIN
////////////////////////////////////////////////////////////////////////////////
int Main_OIS( void )
{
	_FACT_ADJ		fadj;									// Factory Adjustment data for OIS
//	_FACT_ADJ_AF	fadj_af;								// Factory Adjustment data for CLAF

	//------------------------------------------------------
	// Get Factory adjusted data from non volatile memory
	//	(function is example)
	//------------------------------------------------------
	fadj 	= get_FADJ_MEM_from_non_volatile_memory();		// for OIS
//	fadj_af = get_FADJ_AF_MEM_from_non_volatile_memory();	// for CLAF

	//------------------------------------------------------
	// Enable Source Power and Input external clock to CLK/PS pin.
	//------------------------------------------------------
	/* Please write your source code here. */

	//------------------------------------------------------
	// PLL setting to use external CLK
	//------------------------------------------------------
	VCOSET0();

	//------------------------------------------------------
	// Download Program and Coefficient
	//------------------------------------------------------
	OIS_MAIN_STS = func_PROGRAM_DOWNLOAD( );				// Program Download
	if ( OIS_MAIN_STS <= ADJ_ERR ) return OIS_MAIN_STS;		// If success OIS_MAIN_STS is zero.

	func_COEF_DOWNLOAD( 0 );								// Download Coefficient
	I2C_OIS_mem__read( _M_CEFTYP );
	//------------------------------------------------------
	// Change Clock to external pin CLK_PS
	//------------------------------------------------------
	VCOSET1();

	//------------------------------------------------------
	// Set calibration data
	//------------------------------------------------------
	SET_FADJ_PARAM( &fadj );								// for OIS
	//SET_FADJ_PARAM_CLAF( &fadj_af );// for CLAF
	#ifdef VENDOR_EDIT
	//Feiping.Li@Cam.Drv, 20200111, add for ois engineer calibration
	readOisCalDataFromFile();
	#endif

	//------------------------------------------------------
	// Issue DSP start command.
	//------------------------------------------------------
	I2C_OIS_spcl_cmnd( 1, _cmd_8C_EI );						// DSP calculation START

	//------------------------------------------------------
	// Set default AF dac and scene parameter for OIS
	//------------------------------------------------------

	#ifdef VENDOR_EDIT
	//Feiping.Li@Cam.Drv, 20200115, add for fix ois init error
	// Set default SCENE ( Mode is example )
	I2C_OIS_mem_write( 0x7F, 0x0C0C );
	I2C_OIS_mem_write( 0x1B, 0x0080 );
	I2C_OIS_mem_write( 0x9B, 0x0080 );
	I2C_OIS_mem_write( 0x36, 0x7FFD );
	I2C_OIS_mem_write( 0xB6, 0x7FFD );
	I2C_OIS_mem_write( 0x40, 0x3FF0 );
	I2C_OIS_mem_write( 0xC0, 0x3FF0 );
	I2C_OIS_mem_write( 0x43, 0x23A0 );
	I2C_OIS_mem_write( 0xC3, 0x23A0 );
	I2C_OIS_mem_write( 0x38, 0x14E0 );
	I2C_OIS_mem_write( 0xB8, 0x14E0 );
	I2C_OIS_mem_write( 0x47, 0x20A4 );
	I2C_OIS_mem_write( 0xC7, 0x20A4 );
	I2C_OIS_per_write( 0x96, 0x0180 );
	I2C_OIS_per_write( 0x91, 0x0005 );
	I2C_OIS_per_write( 0x92, 0x0002 );
	I2C_OIS_per_write( 0x99, 0x0480 );
	I2C_OIS_mem_write( 0x7F, 0x0D0D );
	#else
	//I2C_OIS_F0123_wr_( 0x90,0x00, 0x0130 );					// AF Control ( Value is example )
	func_SET_SCENE_PARAM_for_NewGYRO_Fil( _SCENE_SPORT_3, 1, &fadj );
	#endif

	return ADJ_OK;
}
