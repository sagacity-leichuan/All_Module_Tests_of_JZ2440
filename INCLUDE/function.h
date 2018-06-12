/**********************************************************************************
  * @brief      : 	JZ2440v2开发板全测试主要功能函数代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
***********************************************************************************/

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

#define SPIGPIO

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

/**********************************************************************************
  * @brief       : 	在测试主页面的指定位置显示进入各个测试子项目的触摸屏按钮
  * @param[in]   : 	x	显示触摸屏按钮的x坐标
  					y	显示触摸屏按钮的y坐标
  					name	显示触摸屏按钮的名字
  					color	显示触摸屏按钮名字的颜色
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void DisplayTestItemIcon(int x,int y,char * name,unsigned int color);

/**********************************************************************************
  * @brief       : 	在测试子页面的指定位置显示返回主测试页面的触摸屏按钮
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void DisplayReturnButton(void);

/**********************************************************************************
  * @brief       : 	当有触摸屏按钮被点击时改变按钮的颜色标识按钮被点击
  * @param[in]   : 	x	被点击的按钮的x坐标
  					y	被点击的按钮的y坐标
  					name	被点击的按钮的名字
  					color	被点击的按钮的名字的颜色
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void ClickedButton(int x,int y,char * name,unsigned int color);

/**********************************************************************************
  * @brief       : 	程序启动欢迎界面及相关信息显示
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void BootScreen(void);

/**********************************************************************************
  * @brief       : 	程序启动的一系列初始化
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void BootInit(void);

/**********************************************************************************
  * @brief       : 	初始化触摸屏校准
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void InitCalibration(void);

/**********************************************************************************
  * @brief       : 	初始化测试所需要的触摸屏按钮，共13个
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void InitButton(void);

/**********************************************************************************
  * @brief       : 	程序运行的主页面显示函数
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void MainPage(void);

/**********************************************************************************
  * @brief       : 	在主程序运行时用以获取测试项，通过检测主页面中的哪一个按钮被点击实现
  * @param[in]   : 	无
  * @param[out]  : 	value	用以存放测试项的id
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void GetTestItem(int* value);

/**********************************************************************************
  * @brief       : 	判断测试子页面的返回按钮是否被点击
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	1	测试子页面的返回按钮被点击
  					0	测试子页面的返回按钮未被点击
  * @others      : 	无
***********************************************************************************/
int isClickReturn(void);

/**********************************************************************************
  * @brief       : 	jz2440开发板spi相关的模块测试函数程序，包括spi flash 和 oled
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void TestSPI(void);

#endif

