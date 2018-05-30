#ifndef _FUNCTION_H
#define _FUNCTION_H

#define TEST_LED	0
#define TEST_KEY	1
#define TEST_LCD	2
#define TEST_TS		3
#define TEST_I2C	4
#define TEST_SPI	5
#define TEST_IDR	6
#define TEST_DHT1	7
#define TEST_DS18	8
#define TEST_0038T	9
#define TEST_NOR	10
#define TEST_NAND	11

#define TEST_RETURN	12


#define RETURN_BUTTON_X		170
#define RETURN_BUTTON_Y		187

#define BUTTON_LENGTH	140
#define BUTTON_HIGH		45

#define ORIGINAL	100




typedef struct STsParameters
{
	int iFlag;
	int iA_ts_x;
	int iA_ts_y;
	int iB_ts_x;
	int iB_ts_y;
	int iC_ts_x;
	int iC_ts_y;
	int iD_ts_x;
	int iD_ts_y;
	int iE_ts_x;
	int iE_ts_y;
	int iTsXYSwap;
}STsParameters,*PSTsParameters;

typedef struct SButton
{
	int iID;
	int iX;
	int iY;
	int iLength;
	int iHigh;
	char *sName;
}SButton,*PSButton;


void Delay(volatile int d);
void DisplayTestItemIcon(int x,int y,char * name,unsigned int color);
void ClickedButton(int x,int y,char * name,unsigned int color);
void SystemInit(void);
void BootScreen(void);
void BootInit(void);
void InitCalibration(void);
void InitButton(void);
void MainPage(void);
void GetTestItem(int* value);
int isClickReturn(void);

void TestNandFlash(void);



#endif

