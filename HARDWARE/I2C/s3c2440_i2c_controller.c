#include "i2c_controller.h"
#include "s3c2440_soc.h"
#include "function.h"
#include "my_printf.h"
#include "string_utils.h"
#include "interrupt.h"


static PSI2cMsg g_psCurMsg;

int isLastData(void)
{
	if (g_psCurMsg->iCntTransferred == g_psCurMsg->iLen - 1)
		return 1;  /* 正要开始传输最后一个数据 */
	else 
		return 0;
}

void ResumeIicWithAck(void)
{
	unsigned int iiccon = IICCON;
	iiccon |= (1<<7); /* 回应ACK */
	iiccon &= ~(1<<4); /* 恢复IIC操作 */
	IICCON =  iiccon;
}

void ResumeIicWithoutAck(void)
{
	unsigned int iiccon = IICCON;
	iiccon &= ~((1<<7) | (1<<4)); /* 不回应ACK, 恢复IIC操作 */
	IICCON =  iiccon;
}


void I2cInterruptFunc(int irq)
{
	int index;
	Delay(1000);
	unsigned int iicstat = IICSTAT;
	//unsigned int iiccon;

	//printf("i2c_interrupt_func! flags = %d\n\r", g_psCurMsg->iFlags);
	
	g_psCurMsg->iCntTransferred++;
	
	//printf("i2c_interrupt_func! g_psCurMsg->iCntTransferred = %d\n\r", g_psCurMsg->iCntTransferred);
	/* 每传输完一个数据将产生一个中断 */

	/* 对于每次传输, 第1个中断是"已经发出了设备地址" */

	if (g_psCurMsg->iFlags == 0)	/* write */
	{
		/* 对于第1个中断, 它是发送出设备地址后产生的
		 * 需要判断是否有ACK
		 * 有ACK : 设备存在
		 * 无ACK : 无设备, 出错, 直接结束传输
		 */
		if (g_psCurMsg->iCntTransferred == 0)  /* 第1次中断 */
		{
			if (iicstat & (1<<0))
			{ /* no ack */
				/* 停止传输 */
				IICSTAT = 0xd0;
				IICCON &= ~(1<<4);
				g_psCurMsg->iErr = -1;
				printf("tx err, no ack\n\r");
				return;
			}
		}

		if (g_psCurMsg->iCntTransferred < g_psCurMsg->iLen)
		{
			/* 对于其他中断, 要继续发送下一个数据
			 */
			IICDS = g_psCurMsg->pchBuf[g_psCurMsg->iCntTransferred];
			IICCON &= ~(1<<4);
		}
		else
		{
			/* 停止传输 */
			IICSTAT = 0xd0;
			IICCON &= ~(1<<4);
			Delay(1000);
		}
	}
	else /* read */
	{
		/* 对于第1个中断, 它是发送出设备地址后产生的
		 * 需要判断是否有ACK
		 * 有ACK : 设备存在, 恢复I2C传输, 这样在下一个中断才可以得到第1个数据
		 * 无ACK : 无设备, 出错, 直接结束传输
		 */
		if (g_psCurMsg->iCntTransferred == 0)  /* 第1次中断 */
		{
			if (iicstat & (1<<0))
			{ /* no ack */
				/* 停止传输 */
				IICSTAT = 0x90;
				IICCON &= ~(1<<4);
				g_psCurMsg->iErr = -1;
				printf("rx err, no ack\n\r");
				return;
			}
			else  /* ack */
			{
				/* 如果是最后一个数据, 启动传输时要设置为不回应ACK */
				/* 恢复I2C传输 */
				if (isLastData())
				{
					ResumeIicWithoutAck();
				}
				else
				{
					ResumeIicWithAck();
				}
				return;
			}
		}

		/* 非第1个中断, 表示得到了一个新数据
		 * 从IICDS读出、保存
		 */
		if (g_psCurMsg->iCntTransferred < g_psCurMsg->iLen)
		{
			index = g_psCurMsg->iCntTransferred - 1;
			g_psCurMsg->pchBuf[index] = IICDS;

			/* 如果是最后一个数据, 启动传输时要设置为不回应ACK */
			/* 恢复I2C传输 */
			if (isLastData())
			{
				ResumeIicWithoutAck();
			}
			else
			{
				ResumeIicWithAck();
			}
		}
		else
		{
			/* 发出停止信号 */
			IICSTAT = 0x90;
			IICCON &= ~(1<<4);
			Delay(1000);
		}
	}
}


int InitS3c2440I2cCon(void)
{
	/* 配置引脚用于I2C*/
	GPECON &= ~((3<<28) | (3<<30));
	GPECON |= ((2<<28) | (2<<30));
	
	/* 设置时钟 */
	/* [7] : IIC-bus acknowledge enable bit, 1-enable in rx mode
	 * [6] : 时钟源, 0: IICCLK = fPCLK /16; 1: IICCLK = fPCLK /512
	 * [5] : 1-enable interrupt
	 * [4] : 读出为1时表示中断发生了, 写入0来清除并恢复I2C操作
	 * [3:0] : Tx clock = IICCLK/(IICCON[3:0]+1).
	 * Tx Clock = 100khz = 50Mhz/16/(IICCON[3:0]+1)
	 */
	IICCON = (1<<7) | (0<<6) | (1<<5) | (30<<0);

	/* 注册中断处理函数 */
	RegisterIrq(27, I2cInterruptFunc);

	return 0;
}

int DoMasterTx(PSI2cMsg msg)
{
	g_psCurMsg = msg;
	
	msg->iCntTransferred = -1;
	msg->iErr = 0;
	
	/* 设置寄存器启动传输 */
	/* 1. 配置为 master tx mode */
	IICCON |= (1<<7); /* TX mode, 在ACK周期释放SDA */
	IICSTAT = (1<<4);
		
	/* 2. 把从设备地址写入IICDS */
	IICDS = msg->iAddr<<1;

	printf("msg->iAddr = %d\n\r",msg->iAddr);
	
	/* 3. IICSTAT = 0xf0 , 数据即被发送出去, 将导致中断产生 */
	IICSTAT = 0xf0;
	
	/* 后续的传输由中断驱动 */

	/* 循环等待中断处理完毕 */
	while (!msg->iErr && msg->iCntTransferred != msg->iLen);

	if (msg->iErr)
	{
		printf("msg->iErr = %d\n\r",msg->iErr);
		return -1;
	}
	else
	{
		printf("msg->iErr = %d\n\r",msg->iErr);
		return 0;
	}
}

int DoMasterRx(PSI2cMsg msg)
{
	g_psCurMsg = msg;

	msg->iCntTransferred = -1;
	msg->iErr = 0;
	
	/* 设置寄存器启动传输 */
	/* 1. 配置为 Master Rx mode */
	IICCON |= (1<<7); /* RX mode, 在ACK周期回应ACK */
	IICSTAT = (1<<4);
		
	/* 2. 把从设备地址写入IICDS */
	IICDS = (msg->iAddr<<1)|(1<<0);

	printf("msg->iAddr = %d\n\r",msg->iAddr);
	
	/* 3. IICSTAT = 0xb0 , 从设备地址即被发送出去, 将导致中断产生 */
	IICSTAT = 0xb0;
	

	/* 后续的传输由中断驱动 */

	/* 循环等待中断处理完毕 */
	while (!msg->iErr && msg->iCntTransferred != msg->iLen);

	if (msg->iErr)
		return -1;
	else
		return 0;
}

int S3c2440MasterXfer(PSI2cMsg msgs, int num)
{
	int i;
	int err;
	
	for (i = 0; i < num; i++)	
	{
		if (msgs[i].iFlags == 0)/* write */
			err = DoMasterTx(&msgs[i]);
		else
			err = DoMasterRx(&msgs[i]);
		if (err)
			return err;
	}
	return 0;
}

/* 实现i2c_controller
          .init
          .master_xfer
          .name
 */

static SI2cController sS3c2440I2cCon = {
	.pchName = "s3c2440",
	.Init = InitS3c2440I2cCon,
	.MasterXfer = S3c2440MasterXfer,
};

void AddS3c2440I2cCon(void)
{
	RegisterI2cController(&sS3c2440I2cCon);
}

