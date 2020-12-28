/////////////////////////////////////////////////////////////////////////////
// File Name	: OIS_func.c
// Function		: Various function for OIS control
// Rule         : Use TAB 4
// 
// Copyright(c)	Rohm Co.,Ltd. All rights reserved 
// 
/***** ROHM Confidential ***************************************************/
#include "OIS_head.h"
#include "OIS_prog.h"
#include "OIS_coef.h"
#include "OIS_defi.h"


OIS_UWORD	INTG__INPUT;
OIS_UWORD	KGNTG_VALUE;
OIS_UWORD	GYRSNS;


//  *****************************************************
//  **** Program Download Function
//  *****************************************************
ADJ_STS		func_PROGRAM_DOWNLOAD( void )
{
	OIS_UWORD	sts;
	
	download( 0, 0 );														// Program Download
	sts = I2C_OIS_mem__read( _M_OIS_STS );									// Check Status

	if ( ( sts & 0x0004 ) == 0x0004 ){
		OIS_UWORD u16_dat;

		u16_dat = I2C_OIS_mem__read( _M_FIRMVER );
		return ADJ_OK;														// Success
	}
	else{
		return PROG_DL_ERR;													// FAIL
	}
}


//  *****************************************************
//  **** COEF Download function
//  *****************************************************
void	func_COEF_DOWNLOAD( OIS_UWORD u16_coef_type )
{
	download( 1, u16_coef_type );											// COEF Download

	// Get default Integ_input, KgnTG value and GYRSNS
	INTG__INPUT = I2C_OIS_mem__read( 0x38 );
	KGNTG_VALUE = I2C_OIS_mem__read( _M_KgxTG );
	GYRSNS = I2C_OIS_mem__read(_M_GYRSNS);
}


//  *****************************************************
//  **** Download the data
//  *****************************************************
void	download( OIS_UWORD u16_type, OIS_UWORD u16_coef_type ){

	// Data Transfer Size per one I2C access
	#define		DWNLD_TRNS_SIZE		(32)

	OIS_UBYTE	temp[DWNLD_TRNS_SIZE+1];
	OIS_UWORD	block_cnt;
	OIS_UWORD	total_cnt;
	OIS_UWORD	lp;
	OIS_UWORD	n;
	OIS_UWORD	u16_i;

	if	( u16_type == 0 ){
		n = DOWNLOAD_BIN_LEN;
	}
	else{
		n = DOWNLOAD_COEF_LEN;
	}
	block_cnt	= n / DWNLD_TRNS_SIZE + 1;
	total_cnt	= block_cnt;
	
	while( 1 ){
		// Residual Number Check
		if( block_cnt == 1 ){
			lp = n % DWNLD_TRNS_SIZE;
		}
		else{
			lp = DWNLD_TRNS_SIZE;
		}

		// Transfer Data set
		if( lp != 0 ){
			if(	u16_type == 0 ){
				temp[0] = _OP_FIRM_DWNLD;
				for( u16_i = 1; u16_i <= lp; u16_i += 1 ){
					temp[ u16_i ] = DOWNLOAD_BIN[ ( total_cnt - block_cnt ) * DWNLD_TRNS_SIZE + u16_i - 1 ];
				}
			}
			else{
				temp[0] = _OP_COEF_DWNLD;
				for( u16_i = 1; u16_i <= lp; u16_i += 1 ){
					temp[u16_i] = DOWNLOAD_COEF[(total_cnt - block_cnt) * DWNLD_TRNS_SIZE + u16_i -1];
				}
			}
			// Data Transfer
			WR_I2C(_SLV_OIS_, lp + 1, temp);

		}
		
		// Block Counter Decrement
		block_cnt = block_cnt - 1;
		if( block_cnt == 0 ){
			break;
		}
	}
}

void SET_FADJ_PARAM( const _FACT_ADJ *param )
{
	//*********************
	// HALL ADJUST
	//*********************
	// Set Hall Current DAC   value that is FACTORY ADJUSTED
	I2C_OIS_per_write( _P_30_ADC_CH0, param->gl_CURDAT );
	// Set Hall     PreAmp Offset   that is FACTORY ADJUSTED
	I2C_OIS_per_write( _P_31_ADC_CH1, param->gl_HALOFS_X );
	I2C_OIS_per_write( _P_32_ADC_CH2, param->gl_HALOFS_Y );
	// Set Hall-X/Y PostAmp Offset  that is FACTORY ADJUSTED
	I2C_OIS_mem_write( _M_X_H_ofs, param->gl_HX_OFS );
	I2C_OIS_mem_write( _M_Y_H_ofs, param->gl_HY_OFS );
	// Set Residual Offset          that is FACTORY ADJUSTED
	I2C_OIS_per_write( _P_39_Ch3_VAL_1, param->gl_PSTXOF );
	I2C_OIS_per_write( _P_3B_Ch3_VAL_3, param->gl_PSTYOF );

	//*********************
	// DIGITAL GYRO OFFSET
	//*********************
	I2C_OIS_mem_write( _M_Kgx00, param->gl_GX_OFS );
	I2C_OIS_mem_write( _M_Kgy00, param->gl_GY_OFS );
	I2C_OIS_mem_write( _M_TMP_X_, param->gl_TMP_X_ );
	I2C_OIS_mem_write( _M_TMP_Y_, param->gl_TMP_Y_ );
	
	//*********************
	// HALL SENSE
	//*********************
	// Set Hall Gain   value that is FACTORY ADJUSTED
	I2C_OIS_mem_write( _M_KgxHG, param->gl_KgxHG );
	I2C_OIS_mem_write( _M_KgyHG, param->gl_KgyHG );
	// Set Cross Talk Canceller
	I2C_OIS_mem_write( _M_KgxH0, param->gl_KgxH0 );
	I2C_OIS_mem_write( _M_KgyH0, param->gl_KgyH0 );
	
	//*********************
	// LOOPGAIN
	//*********************
	I2C_OIS_mem_write( _M_KgxG, param->gl_KGXG );
	I2C_OIS_mem_write( _M_KgyG, param->gl_KGYG );

	// Position Servo ON ( OIS OFF )
	I2C_OIS_mem_write( _M_EQCTL, 0x0C0C );
}

void SET_FADJ_PARAM_CLAF( const _FACT_ADJ_AF *param )
{
	I2C_OIS_per_write( _P_37_ADC_CH7,	param->gl_CURDAZ    );				// Hall Bias
	I2C_OIS_per_write( _P_36_ADC_CH6,	param->gl_HALOFS_Z  );				// Pre-amp offset
	I2C_OIS_per_write( _P_38_Ch3_VAL_0,	param->gl_PSTZOF    );				// Post-amp offset
	I2C_OIS_per_write( _P_M_HZOFS, 		param->gl_P_M_HZOFS );				// Digital offst
	I2C_OIS_per_write( _P_M_KzHG,  		param->gl_P_M_KzHG  );				// Hall Normalized gain
}


//  *****************************************************
//  **** Scence parameter
//  *****************************************************
    // !!!  Depend on user system   !!!
#define	ANGLE_LIMIT_DEG	1       											// Angle limit[deg]
#define	G_SENSE			114												// [LSB/dps] for ICG20660
    // !!!  Depend on user system   !!!

ADJ_STS	func_SET_SCENE_PARAM_for_NewGYRO_Fil(OIS_UBYTE u16_scene, OIS_UBYTE u16_mode, const _FACT_ADJ *param)
{
	OIS_UWORD u16_i;
	OIS_UWORD u16_dat;

	OIS_UBYTE u16_adr_target[4]        = { _M_Kgxdr, _M_X_LMT, 	_M_X_TGT, _M_Kgx10,  };

    // !!!  Each _M_Kgxdr data needs to be evaluated by user   !!!
	OIS_UWORD u16_dat_SCENE_NIGHT_1[4] = { 0x7FFE,   0x3FF0,   	0x06AE,   0x0200,  };
	OIS_UWORD u16_dat_SCENE_D_A_Y_1[4] = { 0x7FFE,   0x3FF0,   	0x11D0,   0x0300,  };
	OIS_UWORD u16_dat_SCENE_SPORT_1[4] = { 0x7FFD,   0x3FF0,   	0x23A0,   0x0080,  };
	OIS_UWORD u16_dat_SCENE_TEST___[4] = { 0x7FFF,   0x3FFF,   	0x7FFF,   0x0080,  };	// Limmiter OFF
    // !!!  Each _M_Kgxdr data needs to be evaluated by user   !!!
	
	OIS_UWORD *u16_dat_SCENE_;
	
	OIS_UBYTE	size_SCENE_tbl = sizeof( u16_dat_SCENE_NIGHT_1 ) / sizeof(OIS_UWORD);

	// Disable OIS ( position Servo is not disable )
	u16_dat = I2C_OIS_mem__read( _M_EQCTL );
	u16_dat = ( u16_dat &  0xFEFE );
	I2C_OIS_mem_write( _M_EQCTL, u16_dat );

	// Scene parameter select
	switch( u16_scene ){
		case _SCENE_NIGHT_1 : u16_dat_SCENE_ = u16_dat_SCENE_NIGHT_1;	break;
		case _SCENE_D_A_Y_1 : u16_dat_SCENE_ = u16_dat_SCENE_D_A_Y_1;	break;
		case _SCENE_SPORT_1 : u16_dat_SCENE_ = u16_dat_SCENE_SPORT_1;	break;
		case _SCENE_TEST___ : u16_dat_SCENE_ = u16_dat_SCENE_TEST___;	break;
		default             : u16_dat_SCENE_ = u16_dat_SCENE_TEST___;	break;
	}
	
	// Set parameter to the OIS controller
	for( u16_i = 0; u16_i < size_SCENE_tbl; u16_i += 1 ){
		I2C_OIS_mem_write( u16_adr_target[u16_i],          	u16_dat_SCENE_[u16_i]   );
	}
	for( u16_i = 0; u16_i < size_SCENE_tbl; u16_i += 1 ){
		I2C_OIS_mem_write( u16_adr_target[u16_i] + 0x80,	u16_dat_SCENE_[u16_i] );
	}
	
	// 1. Read out Limitter(X1).
	// 2. Read out the input of Integral(X2).
	// 3. Set X2 * 7FFFh/(X1)/2(Poast amp. gain) = (X2) * 4000h/(X1) as the input of Integral.
	// 4. Read Kg*TGX3), and then set X3=X3/(7FFFh/X1/2) => X3=X3/(4000h/X1)) => X3=X3*X1/4000h
	{
 		OIS_ULONG	temp;
 		OIS_UWORD	ANGLE_LIMIT = (OIS_UWORD)(GYRSNS * ANGLE_LIMIT_DEG);

		temp = ( INTG__INPUT * 16384 );										// X2 * 4000h / X1
		u16_dat = temp / ANGLE_LIMIT;

		I2C_OIS_mem_write( 0x38, u16_dat );
		I2C_OIS_mem_write( 0xB8, u16_dat );

		//----------------------------------------------

		temp = ( (unsigned int)KGNTG_VALUE * (unsigned int)ANGLE_LIMIT );								// X3 * X1 / 4000h
		u16_dat = temp / 16384;

		I2C_OIS_mem_write( _M_KgxTG, u16_dat );
		I2C_OIS_mem_write( _M_KgyTG, u16_dat );

		//----------------------------------------------

		//I2C_OIS_mem_write( _M_X_LMT, 0x3FFF );									// 
		//I2C_OIS_mem_write( _M_Y_LMT, 0x3FFF );									// 
		I2C_OIS_per_write( 0x96, 0x0180 );
		I2C_OIS_per_write( 0x91, 0x0005 );
		I2C_OIS_per_write( 0x92, 0x0002 );
		I2C_OIS_per_write( 0x99, 0x0480 );
		
	}

	// Enable OIS (if u16_mode = 1)
	if(u16_mode == 1){	// OIS ON
		u16_dat = I2C_OIS_mem__read( _M_EQCTL );
		u16_dat = ( u16_dat |  0x0101 );
		I2C_OIS_mem_write( _M_EQCTL, u16_dat );
	}
	else{
		u16_dat = I2C_OIS_mem__read( _M_EQCTL );
		u16_dat = ( u16_dat &  0xFEFE );
		I2C_OIS_mem_write( _M_EQCTL, u16_dat );
	}
	
	return ADJ_OK;
}

//  *****************************************************
//  **** Write to the Peripheral register < 82h >
//  **** ------------------------------------------------
//  **** OIS_UBYTE	adr	Peripheral Address
//  **** OIS_UWORD	dat	Write data
//  *****************************************************
void	I2C_OIS_per_write( OIS_UBYTE u08_adr, OIS_UWORD u16_dat ){

	OIS_UBYTE	out[4];
	
	out[0] = _OP_Periphe_RW;
	out[1] = u08_adr;
	out[2] = ( u16_dat      ) & 0xFF;
	out[3] = ( u16_dat >> 8 ) & 0xFF;
		
	WR_I2C( _SLV_OIS_, 4, out );
}

//  *****************************************************
//  **** Write to the Memory register < 84h >
//  **** ------------------------------------------------
//  **** OIS_UBYTE	adr	Memory Address
//  **** OIS_UWORD	dat	Write data
//  *****************************************************
void	I2C_OIS_mem_write( OIS_UBYTE u08_adr, OIS_UWORD u16_dat){

	OIS_UBYTE	out[4];
	
	out[0] = _OP_Memory__RW;
	out[1] = u08_adr;
	out[2] = ( u16_dat      ) & 0xFF;
	out[3] = ( u16_dat >> 8 ) & 0xFF;
		
	WR_I2C( _SLV_OIS_, 4, out );
}

//  *****************************************************
//  **** Read from the Peripheral register < 82h >
//  **** ------------------------------------------------
//  **** OIS_UBYTE	adr	Peripheral Address
//  **** OIS_UWORD	dat	Read data
//  *****************************************************
OIS_UWORD	I2C_OIS_per__read( OIS_UBYTE u08_adr ){

	OIS_UBYTE	u08_dat[2];
	
	u08_dat[0] = _OP_Periphe_RW;											// Op-code
	u08_dat[1] = u08_adr;													// target address
		
	return RD_I2C( _SLV_OIS_, 2, u08_dat );
}


//  *****************************************************
//  **** Read from the Memory register < 84h >
//  **** ------------------------------------------------
//  **** OIS_UBYTE	adr	Memory Address
//  **** OIS_UWORD	dat	Read data
//  *****************************************************
OIS_UWORD	I2C_OIS_mem__read( OIS_UBYTE u08_adr)
{
	OIS_UBYTE	u08_dat[2];
	
	u08_dat[0] = _OP_Memory__RW;											// Op-code
	u08_dat[1] = u08_adr;													// target address
		
	return RD_I2C( _SLV_OIS_, 2, u08_dat );
}


//  *****************************************************
//  **** Special Command 8Ah
// 		_cmd_8C_EI			0	// 0x0001
// 		_cmd_8C_DI			1	// 0x0002
//  *****************************************************
void	I2C_OIS_spcl_cmnd( OIS_UBYTE u08_on, OIS_UBYTE u08_dat )
{
	if( ( u08_dat == _cmd_8C_EI ) ||
		( u08_dat == _cmd_8C_DI )    ){

		OIS_UBYTE out[2];
		
		out[0] = _OP_SpecialCMD;
		out[1] = u08_dat;
		
		WR_I2C( _SLV_OIS_, 2, out );
	}
}


//  *****************************************************
//  **** F0-F3h Command NonAssertClockStretch Function
//  *****************************************************
void	I2C_OIS_F0123_wr_( OIS_UBYTE u08_dat0, OIS_UBYTE u08_dat1, OIS_UWORD u16_dat2 )
{
	OIS_UBYTE out[5];
		
	out[0] = 0xF0;
	out[1] = u08_dat0;
	out[2] = u08_dat1;
	out[3] = u16_dat2 / 256;
	out[4] = u16_dat2 % 256;
		
	WR_I2C( _SLV_OIS_, 5, out );
}

/*
OIS_UWORD	I2C_OIS_F0123__rd( void )
{
	OIS_UBYTE	u08_dat;

	u08_dat = 0xF0;				// Op-code

	return RD_I2C( _SLV_OIS_, 1, &u08_dat );
}
*/


// -----------------------------------------------------------
// -----------------------------------------------------------
// -----------------------------------------------------------

