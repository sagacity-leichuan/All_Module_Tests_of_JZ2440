
#ifndef _I2C_CONTROLLER_H
#define _I2C_CONTROLLER_H

typedef struct SI2cMsg {
	unsigned int iAddr;  /* 7bits */
	int iFlags;  /* 0 - write, 1 - read */
	int iLen;
	int iCntTransferred;
	int iErr;
	unsigned char *pchBuf;
}SI2cMsg, *PSI2cMsg;

typedef struct SI2cController {
	int (*Init)(void);
	int (*MasterXfer)(PSI2cMsg msgs, int num);
	char *pchName;
}SI2cController, *PSI2cController;

void InitI2c(void);

void RegisterI2cController(PSI2cController p);
int TransferI2c(PSI2cMsg msgs, int num);


#endif /* _I2C_CONTROLLER_H */


