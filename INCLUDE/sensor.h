#ifndef _SENSOR_H
#define _SENSOR_H


void InitADC(int channel);
int ReadADC(int channel);

void TestPhotoresistor(void);
void TestDs18b20(void);

/**********************************************************************************
  * @brief       : 	Dht11测试函数
  * @param[in]   : 	无
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
void TestDht11(void);
void TestIrdaNec(void);


#endif

