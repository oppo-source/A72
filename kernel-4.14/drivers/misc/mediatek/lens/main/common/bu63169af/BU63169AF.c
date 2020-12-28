/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/*
 * BU63169AF voice coil motor driver
 * BU63169 : OIS driver
 * dw9839  : VCM driver be the same as DW9839AF
 */

#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>

/* kernel standard for PMIC*/
#if !defined(CONFIG_MTK_LEGACY)
#include <linux/regulator/consumer.h>
#endif

#ifdef VENDOR_EDIT
//Feiping.Li@Cam.Drv, 20200111, add for ois engineer calibration
#ifdef CONFIG_COMPAT
#include <linux/compat.h>
#endif
#include "BU63169.h"
#endif

#include "OIS_head.h"
#include "lens_info.h"

#define AF_DRVNAME "BU63169AF_DRV"
#define AF_I2C_SLAVE_ADDR 0x1c
#define EEPROM_I2C_SLAVE_ADDR 0xa0

#define DW9839AF_I2C_SLAVE_ADDR 0x18

#define AF_DEBUG
#ifdef AF_DEBUG
#define LOG_INF(format, args...)                                               \
	pr_info(AF_DRVNAME " [%s] " format, __func__, ##args)
#else
#define LOG_INF(format, args...)
#endif

static struct i2c_client *g_pstAF_I2Cclient;
static int *g_pAF_Opened;
static spinlock_t *g_pAF_SpinLock;
static DEFINE_MUTEX(i2c_mutex);

static unsigned long g_u4AF_INF;
static unsigned long g_u4AF_MACRO = 1023;
static unsigned long g_u4CurrPosition;

/* PMIC */
#if !defined(CONFIG_MTK_LEGACY)
static struct regulator *regVCAMAF;
static struct device *lens_device;
#endif

#ifdef VENDOR_EDIT
//Feiping.Li@Cam.Drv, 20200111, add for  ois engineer calibration
static unsigned int g_oisInitDone = 0;
#endif

static int s4DW9839AF_WriteReg(unsigned short a_u2Addr, unsigned short a_u2Data)
{
	int i4RetValue = 0;

	char puSendCmd[2];

	mutex_lock(&i2c_mutex);

	puSendCmd[0] = (char)a_u2Addr;
	puSendCmd[1] = (char)a_u2Data;

	g_pstAF_I2Cclient->addr = DW9839AF_I2C_SLAVE_ADDR;

	g_pstAF_I2Cclient->addr = g_pstAF_I2Cclient->addr >> 1;

	i4RetValue = i2c_master_send(g_pstAF_I2Cclient, puSendCmd, 2);

	mutex_unlock(&i2c_mutex);

	if (i4RetValue < 0) {
		LOG_INF("I2C write failed!!\n");
		return -1;
	}

	return 0;
}

static int s4DW9839AF_ReadReg(u8 a_uAddr)
{
	int i4RetValue = 0;
	char pBuff;
	char puSendCmd[1];

	mutex_lock(&i2c_mutex);

	puSendCmd[0] = a_uAddr;

	g_pstAF_I2Cclient->addr = DW9839AF_I2C_SLAVE_ADDR;

	g_pstAF_I2Cclient->addr = g_pstAF_I2Cclient->addr >> 1;

	i4RetValue = i2c_master_send(g_pstAF_I2Cclient, puSendCmd, 1);

	if (i4RetValue < 0) {
		LOG_INF("I2C read - send failed!!\n");
		/* return -1; */
	}

	i4RetValue = i2c_master_recv(g_pstAF_I2Cclient, &pBuff, 1);

	mutex_unlock(&i2c_mutex);

	if (i4RetValue < 0) {
		LOG_INF("I2C read - recv failed!!\n");
		return -1;
	}

	return   pBuff;
}

static inline int setDW9839AFPos(unsigned long a_u4Position)
{
	int i4RetValue = 0;

	i4RetValue = s4DW9839AF_WriteReg(
		0x0, (unsigned short)((a_u4Position >> 2) & 0xff));

	if (i4RetValue < 0)
		return -1;

	i4RetValue = s4DW9839AF_WriteReg(
		0x1, (unsigned short)((a_u4Position & 0x3) << 6));

	return i4RetValue;
}

int s4EEPROM_ReadReg_BU63169AF(unsigned short addr, unsigned short *data)
{
	int i4RetValue = 0;

	unsigned char u8data[2];
	unsigned char pu_send_cmd[2] = {(unsigned char)(addr >> 8),
					(unsigned char)(addr & 0xFF)};

	*data = 0;
	g_pstAF_I2Cclient->addr = (EEPROM_I2C_SLAVE_ADDR) >> 1;
	if (i2c_master_send(g_pstAF_I2Cclient, pu_send_cmd, 2) < 0) {
		LOG_INF("read I2C send failed!!\n");
		return -1;
	}
	if (i2c_master_recv(g_pstAF_I2Cclient, u8data, 2) < 0) {
		LOG_INF("EEPROM_ReadReg failed!!\n");
		return -1;
	}
	LOG_INF("u8data[0] = 0x%x\n", u8data[0]);
	LOG_INF("u8data[1] = 0x%x\n", u8data[1]);

	*data = u8data[1] << 8 | u8data[0];

	LOG_INF("s4EEPROM_ReadReg2 0x%x, 0x%x\n", addr, *data);

	return i4RetValue;
}

int s4AF_WriteReg_BU63169AF(unsigned short i2c_id, unsigned char *a_pSendData,
			    unsigned short a_sizeSendData)
{
	int i4RetValue = 0;

	mutex_lock(&i2c_mutex);

	g_pstAF_I2Cclient->addr = i2c_id >> 1;

	i4RetValue =
		i2c_master_send(g_pstAF_I2Cclient, a_pSendData, a_sizeSendData);

	mutex_unlock(&i2c_mutex);

	if (i4RetValue != a_sizeSendData) {
		LOG_INF("I2C send failed!!, Addr = 0x%x, Data = 0x%x\n",
			a_pSendData[0], a_pSendData[1]);
		return -1;
	}

	return 0;
}

int s4AF_ReadReg_BU63169AF(unsigned short i2c_id, unsigned char *a_pSendData,
			   unsigned short a_sizeSendData,
			   unsigned char *a_pRecvData,
			   unsigned short a_sizeRecvData)
{
	int i4RetValue;
	struct i2c_msg msg[2];

	mutex_lock(&i2c_mutex);

	g_pstAF_I2Cclient->addr = i2c_id >> 1;

	msg[0].addr = g_pstAF_I2Cclient->addr;
	msg[0].flags = 0;
	msg[0].len = a_sizeSendData;
	msg[0].buf = a_pSendData;

	msg[1].addr = g_pstAF_I2Cclient->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len = a_sizeRecvData;
	msg[1].buf = a_pRecvData;

	i4RetValue =
		i2c_transfer(g_pstAF_I2Cclient->adapter, msg, ARRAY_SIZE(msg));

	mutex_unlock(&i2c_mutex);

	if (i4RetValue != 2) {
		LOG_INF("I2C Read failed!!\n");
		return -1;
	}
	return 0;
}

static inline int getAFInfo(__user struct stAF_MotorInfo *pstMotorInfo)
{
	struct stAF_MotorInfo stMotorInfo;

	stMotorInfo.u4MacroPosition = g_u4AF_MACRO;
	stMotorInfo.u4InfPosition = g_u4AF_INF;
	stMotorInfo.u4CurrentPosition = g_u4CurrPosition;
	stMotorInfo.bIsSupportSR = 1;

	stMotorInfo.bIsMotorMoving = 1;

	if (*g_pAF_Opened >= 1)
		stMotorInfo.bIsMotorOpen = 1;
	else
		stMotorInfo.bIsMotorOpen = 0;

	if (copy_to_user(pstMotorInfo, &stMotorInfo,
			 sizeof(struct stAF_MotorInfo)))
		LOG_INF("copy to user failed when getting motor information\n");

	return 0;
}

/* initAF include driver initialization and standby mode */
static int initAF(void)
{
	unsigned short data = 0;
	signed int ois_init_state;

	LOG_INF("+\n");

	if (*g_pAF_Opened == 1) {

		s4DW9839AF_WriteReg(0x03, 0x11);  //dw9839

		msleep(5);

		data = s4DW9839AF_ReadReg(0x03);

		if ((data & 0x10) == 0) {
			pr_err("init dw9839 error");
			return -1;
		}
		//add init register
		s4DW9839AF_WriteReg(0x04, 0x00); /* from Standby mode to Active mode */

		ois_init_state = Main_OIS();
		if (ois_init_state == 0) {
			g_oisInitDone = 1;
		} else {
			LOG_INF("ois init error! errorno: %d", ois_init_state);
		}
		//setOISMode(1);

		spin_lock(g_pAF_SpinLock);
		*g_pAF_Opened = 2;
		spin_unlock(g_pAF_SpinLock);
	}

	LOG_INF("-\n");

	return 0;
}

/* moveAF only use to control moving the motor */
static inline int moveAF(unsigned long a_u4Position)
{
	int ret = 0;

	//LOG_INF("AF_POS(%d), OIS_POS(%d, %d)\n", (int)(a_u4Position), (short int)(I2C_OIS_mem__read(0x3F)), (short int)(I2C_OIS_mem__read(0xBF)));
	//LOG_INF("gyro pos(%d, %d)\n", (short int)(I2C_OIS_mem__read(0x55)), (short int)(I2C_OIS_mem__read(0x56)));

	if (setDW9839AFPos(a_u4Position) == 0) {
		g_u4CurrPosition = a_u4Position;
		ret = 0;
	} else {
		LOG_INF("set I2C failed when moving the motor\n");
		ret = -1;
	}

	return ret;
}

static inline int setAFInf(unsigned long a_u4Position)
{
	spin_lock(g_pAF_SpinLock);
	g_u4AF_INF = a_u4Position;
	spin_unlock(g_pAF_SpinLock);
	return 0;
}

static inline int setAFMacro(unsigned long a_u4Position)
{
	spin_lock(g_pAF_SpinLock);
	g_u4AF_MACRO = a_u4Position;
	spin_unlock(g_pAF_SpinLock);
	return 0;
}

int BU63169AF_OisGetHallPos(int *PosX, int *PosY)
{
	if (*g_pAF_Opened == 2) {
		*PosX = 0;
		*PosY = 0;
	}

	return 0;
}

static inline int64_t getCurNS(void)
{
	int64_t ns;
	struct timespec time;

	time.tv_sec = time.tv_nsec = 0;
	get_monotonic_boottime(&time);
	ns = time.tv_sec * 1000000000LL + time.tv_nsec;

	return ns;
}

int BU63169AF_OisGetHallInfo(void *a_pOisPosInfo)
{
	struct stAF_OisPosInfo *pOisPosInfo;
	#define MAX_VALUE_OF_N 12
	unsigned char addr;
	unsigned char data[52];
	unsigned char valueN;
	unsigned char pos_x_msb;
	unsigned char pos_x_lsb;
	unsigned char pos_y_msb;
	unsigned char pos_y_lsb;
	unsigned char delay_count_msb;
	unsigned char delay_count_lsb;
	int delay_count;
	int ois_pos_x[MAX_VALUE_OF_N];
	int ois_pos_y[MAX_VALUE_OF_N];
	int64_t ois_pos_ts[MAX_VALUE_OF_N];
	int64_t timestamp;
	int cnt, i, j;


	pOisPosInfo = (struct stAF_OisPosInfo *)a_pOisPosInfo;

    if (*g_pAF_Opened != 2) {
        LOG_INF("*g_pAF_Opened != 2\n");
        return 0;
    }

	cnt = 0;
	addr = 0x8A;
	s4AF_ReadReg_BU63169AF(0x0E << 1, &addr, 1, data, 52);
	timestamp = getCurNS();

	/*
	//add for debug to print regs
	unsigned char output[512];
	for (i = 0; i < 52; i++)
	{
		if (i == 0) {
			sprintf(output, "OIS Pos => %02x |", data[0]);
		} else {
			sprintf(output, "%s %02x", output, data[i]);
		}
	}
	LOG_INF("%s\n", output);
	*/

	//LOG_INF("read 0x8A, data transfering timing : %lld ns\n", timestamp);
	valueN = data[cnt];

	if (valueN == 0 || valueN > MAX_VALUE_OF_N)
		return 0;

	cnt++;
	for (i = 0; i < valueN; i++)
	{
		pos_x_msb = data[cnt];
		cnt++;
		pos_x_lsb = data[cnt];
		cnt++;
		pos_y_msb = data[cnt];
		cnt++;
		pos_y_lsb = data[cnt];
		cnt++;
		ois_pos_x[i] = (short)(pos_x_msb << 8 | pos_x_lsb);
		ois_pos_y[i] = (short)(pos_y_msb << 8 | pos_y_lsb);
	}
	delay_count_msb = data[cnt];
	cnt++;
	delay_count_lsb = data[cnt];
	cnt++;
	delay_count = (short)(delay_count_msb << 8 | delay_count_lsb);


	cnt = valueN - 1;
	timestamp -= (delay_count * 17780);
	ois_pos_ts[cnt] = timestamp;
	cnt--;
	while(cnt >= 0)
	{
		timestamp -= 4000000;
		ois_pos_ts[cnt] = timestamp;
		cnt--;
	}

	cnt = valueN - 1;
	j = 0;
	for (i = 0; i < OIS_DATA_NUM; i++)
	{
		pOisPosInfo->i4OISHallPosX[i] = ois_pos_x[j];
		pOisPosInfo->i4OISHallPosY[i] = ois_pos_y[j];
		pOisPosInfo->TimeStamp[i]     = ois_pos_ts[j];
		//LOG_INF("[%d] Pos(%d, %d) Ts(%lld ns) ", cnt, ois_pos_x[cnt], ois_pos_y[cnt], ois_pos_ts[cnt]);
		j++;
		if (j > cnt)
			break;
	}

#if 0
	LOG_INF("delay count : %d, delta time : %lld ns\n", delay_count, delay_count * 17780);

	sprintf(output, "OIS Pos => %d \n", valueN);
	for (i = 0; i < valueN; i++)
	{
		LOG_INF("[%d] Pos(%d, %d) Ts(%lld ns) ", i, ois_pos_x[i], ois_pos_y[i], ois_pos_ts[i]);
	}

	for (i = 0; i < 52; i++)
	{
		if (i == 0) {
			sprintf(output, "OIS Pos => %02x |", data[0]);
		} else {
			sprintf(output, "%s %02x", output, data[i]);
		}
	}
	LOG_INF("%s\n", output);
#endif
	return 0;
}

#ifdef VENDOR_EDIT
//Feiping.Li@Cam.Drv, 20200113, add for ois still calibration
#define STILL_CAL 0x03
#define MOTION_CAL 0x04
#define SR_CAL 0x05
#define STILL_CAL_DATA_NUM  20

static int g_stillGyroDataCnt = 0;
static signed int g_stillGyroPosXSum = 0;
static signed int g_stillGyroPosYSum = 0;
static signed int g_stillGyroPosXAvg = 0;
static signed int g_stillGyroPosYAvg = 0;
static unsigned char g_oisCalMode = 0;
static unsigned char g_OisSRCalOnOffOis = 1;
static unsigned char g_OisStillCalOnOffOis = 1;
static unsigned char g_OisStillCalDone = 0;

#define OIS_STILL_CAL_RESULT  "/mnt/vendor/persist/camera/ois_still_cal.bin"

//static DEFINE_MUTEX(gyro_offet_mutex);

static void stillOisCalibration(void)
{
	if (g_OisStillCalOnOffOis == 1) {  //need off ois first
		setOISMode(1);
		g_OisStillCalOnOffOis = 0;
	}

	if (g_stillGyroDataCnt < STILL_CAL_DATA_NUM) {
		//mutex_lock(&gyro_offet_mutex);
		g_stillGyroPosXSum += (short int)(I2C_OIS_mem__read(0x55));
		g_stillGyroPosYSum += (short int)(I2C_OIS_mem__read(0x56));
		g_stillGyroDataCnt ++;
		//mutex_unlock(&gyro_offet_mutex);
		LOG_INF("gyro (%d) pos(%d, %d)\n",g_stillGyroDataCnt, g_stillGyroPosXSum, g_stillGyroPosYSum);
	}
}

static int saveOisCalDataToFile(void)
{
	struct file *fp;
	mm_segment_t fs;
	loff_t pos;
	//signed int reverse_x,reverse_y;

	char buf[8];

	fp = filp_open(OIS_STILL_CAL_RESULT, O_RDWR | O_CREAT,0644);
	if (IS_ERR(fp)) {
		LOG_INF("create OisCalData file error");
		return -1;
	}

	//int to char
	buf[0] = g_stillGyroPosXAvg & 0xFF;
	buf[1] = (g_stillGyroPosXAvg>>8) & 0xFF;
	buf[2] = (g_stillGyroPosXAvg>>16) & 0xFF;
	buf[3] = (g_stillGyroPosXAvg>>24) & 0xFF;
	buf[4] = g_stillGyroPosYAvg & 0xFF;
	buf[5] = (g_stillGyroPosYAvg>>8) & 0xFF;
	buf[6] = (g_stillGyroPosYAvg>>16) & 0xFF;
	buf[7] = (g_stillGyroPosYAvg>>24) & 0xFF;

	/*reverse_x = ((g_stillGyroPosXAvg & 0xFF) <<24) | (((g_stillGyroPosXAvg>>8) & 0xFF) << 16)
		| (((g_stillGyroPosXAvg>>16) & 0xFF) << 8) | ((g_stillGyroPosXAvg>>24) & 0xFF);
	reverse_y = ((g_stillGyroPosYAvg & 0xFF) <<24) | (((g_stillGyroPosYAvg>>8) & 0xFF) << 16)
		| (((g_stillGyroPosYAvg>>16) & 0xFF) << 8) | ((g_stillGyroPosYAvg>>24) & 0xFF);*/


	I2C_OIS_mem_write(0x06, g_stillGyroPosXAvg);
	I2C_OIS_mem_write(0x86, g_stillGyroPosYAvg);

	/*I2C_OIS_mem_write(0x06, reverse_x);
	I2C_OIS_mem_write(0x86, reverse_y);*/

	fs = get_fs();
	set_fs(KERNEL_DS);
	pos =0;
	vfs_write(fp, buf, sizeof(buf), &pos);
	set_fs(fs);

	filp_close(fp,NULL);

	LOG_INF("save to file success");

	return 0;
}

int readOisCalDataFromFile(void)
{
	struct file *fp;
	mm_segment_t fs;
	loff_t pos;

	char buf[8];
	signed int gyro_offset_x,gyro_offset_y;

	fp = filp_open(OIS_STILL_CAL_RESULT, O_RDWR | O_CREAT, 0644);
	if (IS_ERR(fp)) {
		LOG_INF("open OisCalData file error");
		return -1;
	}

	fs = get_fs();
	set_fs(KERNEL_DS);
	pos =0;
	vfs_read(fp, buf, sizeof(buf), &pos);
	set_fs(fs);

	gyro_offset_x = buf[0] | (buf[1]<<8) | (buf[2]<<16) | (buf[3]<<24);
	gyro_offset_y = buf[4] | (buf[5]<<8) | (buf[6]<<16) | (buf[7]<<24);

	I2C_OIS_mem_write(0x06, gyro_offset_x);
	I2C_OIS_mem_write(0x86, gyro_offset_y);

	LOG_INF("read from file offset: (%d %d)", gyro_offset_x, gyro_offset_y);

	filp_close(fp,NULL);

	LOG_INF("write caldata success");

	return 0;

}

static int getOisCalResult(void *a_u4Param)
{
	struct stAF_GyroOffsetResult stGyroResult = {0,0,0};
	if (g_oisCalMode == STILL_CAL  && (g_stillGyroDataCnt >= STILL_CAL_DATA_NUM)) {
		g_stillGyroPosXAvg = g_stillGyroPosXSum / STILL_CAL_DATA_NUM;
		g_stillGyroPosYAvg = g_stillGyroPosYSum / STILL_CAL_DATA_NUM;
		LOG_INF("gyro avg pos(%d, %d)\n",g_stillGyroPosXAvg, g_stillGyroPosYAvg);
		stGyroResult.result = 1;
		stGyroResult.gyro_x_offset = g_stillGyroPosXAvg;
		stGyroResult.gyro_y_offset = g_stillGyroPosYAvg;

		//save to persit file
		if (g_OisStillCalDone == 0) {
			if (!saveOisCalDataToFile()) {
				LOG_INF("gyro cal result pos(%d, %d)\n", (short int)(I2C_OIS_mem__read(0x07)), (short int)(I2C_OIS_mem__read(0x87)));
				g_OisStillCalDone = 1;
			}
		}
	}

	if (copy_to_user((__user struct stAF_GyroOffsetResult*)(a_u4Param), &stGyroResult, sizeof(struct stAF_GyroOffsetResult))) {
		LOG_INF("get ois cal result fail");
		return -1;
	}
	return 0;
}
#endif

static inline int setAFPara(__user struct stAF_MotorCmd *pstMotorCmd)
{
	#ifdef VENDOR_EDIT
	//Feiping.Li@Cam.Drv, 20200113, add for ois calibration
	struct stAF_MotorCmd stMotorCmd;

	if (copy_from_user(&stMotorCmd, pstMotorCmd, sizeof(stMotorCmd)))
		LOG_INF("copy to user failed when getting motor command\n");

	LOG_INF("Motor CmdID : %x, Param : %x\n", stMotorCmd.u4CmdID, stMotorCmd.u4Param);

	switch (stMotorCmd.u4CmdID) {
	case MCU_CMD_OIS_CAL_DISABLE:
		if (g_OisSRCalOnOffOis) {
			setOISMode(stMotorCmd.u4Param); /* 1 : disable */
			g_OisSRCalOnOffOis = 0;
			LOG_INF("SR cal disable ois");
		}
		break;
	case MCU_CMD_OIS_CAL_MODE:
		g_oisCalMode = stMotorCmd.u4Param;
		if (STILL_CAL == stMotorCmd.u4Param && g_oisInitDone) {
			LOG_INF("STILL CAL MODE");
			stillOisCalibration();
		}
		/*else if (MOTION_CAL == stMotorCmd.u4Param) */
		break;
	/*case MCU_CMD_GET_CAL_RESULT:
			getOisCalResult();
		break;*/
	default:
	    break;
	}
	#endif

	return 0;

}

/* ////////////////////////////////////////////////////////////// */
long BU63169AF_Ioctl(struct file *a_pstFile, unsigned int a_u4Command,
		     unsigned long a_u4Param)
{
	long i4RetValue = 0;

	switch (a_u4Command) {
	case AFIOC_G_MOTORINFO:
		i4RetValue =
			getAFInfo((__user struct stAF_MotorInfo *)(a_u4Param));
		break;

	case AFIOC_T_MOVETO:
		i4RetValue = moveAF(a_u4Param);
		break;

	case AFIOC_T_SETINFPOS:
		i4RetValue = setAFInf(a_u4Param);
		break;

	case AFIOC_T_SETMACROPOS:
		i4RetValue = setAFMacro(a_u4Param);
		break;

	case AFIOC_S_SETPARA:
		i4RetValue =
			setAFPara((__user struct stAF_MotorCmd *)(a_u4Param));
		break;

	/*case AFIOC_G_MOTOROISINFO:
		i4RetValue = getOISInfo(
			(__user struct stAF_MotorOisInfo *)(a_u4Param));
		break;*/
	#ifdef VENDOR_EDIT
	//Feiping.Li@Cam.Drv, 20200111, add for ois engineer calibration
	case AFIOC_X_GETGYROOFFSET:
		i4RetValue = getOisCalResult((__user struct stAF_GyroOffsetResult*)(a_u4Param));
		break;
	#endif

	default:
		LOG_INF("No CMD\n");
		i4RetValue = -EPERM;
		break;
	}

	return i4RetValue;
}

/* Main jobs: */
/* 1.Deallocate anything that "open" allocated in private_data. */
/* 2.Shut down the device on last close. */
/* 3.Only called once on last time. */
/* Q1 : Try release multiple times. */
int BU63169AF_Release(struct inode *a_pstInode, struct file *a_pstFile)
{
	LOG_INF("Start\n");

	if (*g_pAF_Opened == 2) {
		LOG_INF("Wait\n");
		OIS_Standby();
		msleep(20);
	}

	if (*g_pAF_Opened) {
		LOG_INF("Free\n");

		spin_lock(g_pAF_SpinLock);
		*g_pAF_Opened = 0;
		spin_unlock(g_pAF_SpinLock);
	}

	#ifdef VENDOR_EDIT
	//Feiping.Li@Cam.Drv, 20200113, add for ois calibration
	{   //release ois calibration data
		//mutex_lock(&gyro_offet_mutex);
		g_stillGyroDataCnt = 0;
		g_stillGyroPosXSum = 0;
		g_stillGyroPosYSum = 0;
		g_OisSRCalOnOffOis = 1;
		g_OisStillCalOnOffOis = 1;
		g_OisStillCalDone = 0;
		g_oisInitDone = 0;
		//mutex_unlock(&gyro_offet_mutex);
	}
	#endif

	LOG_INF("End\n");

	return 0;
}

int BU63169AF_PowerDown(struct i2c_client *pstAF_I2Cclient,
			int *pAF_Opened)
{
	g_pstAF_I2Cclient = pstAF_I2Cclient;
	g_pAF_Opened = pAF_Opened;

	LOG_INF("+\n");
	if (*g_pAF_Opened == 0) {
		unsigned short data = 0;
		int cnt = 0;

		while (1) {
			data = 0;

			s4DW9839AF_WriteReg(0x02, 0x20);

			data = s4DW9839AF_ReadReg(0x02);

			LOG_INF("Addr : 0x02 , Data : %x\n", data);

			OIS_Standby();

			if (data == 0x20 || cnt == 1)
				break;

			cnt++;
		}
	}
	LOG_INF("-\n");

	return 0;
}

int BU63169AF_SetI2Cclient(struct i2c_client *pstAF_I2Cclient,
			   spinlock_t *pAF_SpinLock, int *pAF_Opened)
{
	g_pstAF_I2Cclient = pstAF_I2Cclient;
	g_pAF_SpinLock = pAF_SpinLock;
	g_pAF_Opened = pAF_Opened;
#if !defined(CONFIG_MTK_LEGACY)
	regVCAMAF = NULL;
	lens_device = NULL;
#endif

	LOG_INF("SetI2Cclient\n");

	initAF();

	return 1;
}

int BU63169AF_GetFileName(unsigned char *pFileName)
{
	#if SUPPORT_GETTING_LENS_FOLDER_NAME
	char FilePath[256];
	char *FileString;

	sprintf(FilePath, "%s", __FILE__);
	FileString = strrchr(FilePath, '/');
	*FileString = '\0';
	FileString = (strrchr(FilePath, '/') + 1);
	strncpy(pFileName, FileString, AF_MOTOR_NAME);
	LOG_INF("FileName : %s\n", pFileName);
	#else
	pFileName[0] = '\0';
	#endif
	return 1;
}
