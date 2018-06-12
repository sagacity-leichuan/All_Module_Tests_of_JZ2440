/**********************************************************************************
  * @brief      : 	JZ2440v2开发板LCD绘制几何图像代码源文件
  * @version    : 	V0.0
  * @note       : 	无
  * @history    : 	无
***********************************************************************************/

#include "framebuffer.h"
#include "geometry.h"

 //-------------画圆函数。参数：圆心，半径，颜色----------	
 // 	   画1/8圆 然后其他7/8对称画  
 // 		 ---------------->X  
 // 		 |(0,0)   0  
 // 		 |	   7	 1	
 // 		 |	  6 	  2  
 // 		 |	   5	 3	
 // 	  (Y)V		  4  
 //  
 // 	 L = x^2 + y^2 - r^2  
 /**********************************************************************************
  * @brief       : 	在LCD中指定位置绘制一个特定半径特定颜色的圆
  * @param[in]   : 	x	绘制的圆的x坐标
  					y	绘制的圆的y坐标
  					r	绘制的圆的半径
  					color	绘制的圆的颜色
  * @param[out]  : 	无
  * @return      : 	无
  * @others      : 	无
***********************************************************************************/
 void DrawCircle(int x, int y, int r, int color)  
 {	
	 int a, b, num;  
	 a = 0;  
	 b = r;  
	 while(22 * b * b >= r * r) 		 // 1/8圆即可  
	 {	
		 PutFbPixel(x + a, y - b,color); // 0~1  
		 PutFbPixel(x - a, y - b,color); // 0~7  
		 PutFbPixel(x - a, y + b,color); // 4~5  
		 PutFbPixel(x + a, y + b,color); // 4~3  
   
		 PutFbPixel(x + b, y + a,color); // 2~3  
		 PutFbPixel(x + b, y - a,color); // 2~1  
		 PutFbPixel(x - b, y - a,color); // 6~7  
		 PutFbPixel(x - b, y + a,color); // 6~5  
		   
		 a++;  
		 num = (a * a + b * b) - r*r;  
		 if(num > 0)  
		 {	
			 b--;  
			 a--;  
		 }	
	 }	
 }	
   
 /**********************************************************************************
   * @brief 	  :  在LCD中绘制一条指定起始点的特定颜色的线条
   * @param[in]   :  x1		线条起点的x坐标
   					 y1		线条起点的y坐标
   					 x2		线条结束点的x坐标
   					 y2		线条结束点的y坐标
   					 color	线条的颜色
   * @param[out]  :  无
   * @return	  :  无
   * @others	  :  无
 ***********************************************************************************/
 void DrawLine(int x1,int y1,int x2,int y2,int color)	
 {	
	 int dx,dy,e;  
	 dx=x2-x1;	 
	 dy=y2-y1;	
	 if(dx>=0)	
	 {	
		 if(dy >= 0) // dy>=0  
		 {	
			 if(dx>=dy) // 1/8 octant  
			 {	
				 e=dy-dx/2;  
				 while(x1<=x2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){y1+=1;e-=dx;}	   
					 x1+=1;  
					 e+=dy;  
				 }	
			 }	
			 else		 // 2/8 octant	
			 {	
				 e=dx-dy/2;  
				 while(y1<=y2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){x1+=1;e-=dy;}	   
					 y1+=1;  
					 e+=dx;  
				 }	
			 }	
		 }	
		 else			// dy<0  
		 {	
			 dy=-dy;   // dy=abs(dy)  
			 if(dx>=dy) // 8/8 octant  
			 {	
				 e=dy-dx/2;  
				 while(x1<=x2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){y1-=1;e-=dx;}	   
					 x1+=1;  
					 e+=dy;  
				 }	
			 }	
			 else		 // 7/8 octant	
			 {	
				 e=dx-dy/2;  
				 while(y1>=y2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){x1+=1;e-=dy;}	   
					 y1-=1;  
					 e+=dx;  
				 }	
			 }	
		 }	   
	 }	
	 else //dx<0  
	 {	
		 dx=-dx;	 //dx=abs(dx)  
		 if(dy >= 0) // dy>=0  
		 {	
			 if(dx>=dy) // 4/8 octant  
			 {	
				 e=dy-dx/2;  
				 while(x1>=x2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){y1+=1;e-=dx;}	   
					 x1-=1;  
					 e+=dy;  
				 }	
			 }	
			 else		 // 3/8 octant	
			 {	
				 e=dx-dy/2;  
				 while(y1<=y2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){x1-=1;e-=dy;}	   
					 y1+=1;  
					 e+=dx;  
				 }	
			 }	
		 }	
		 else			// dy<0  
		 {	
			 dy=-dy;   // dy=abs(dy)  
			 if(dx>=dy) // 5/8 octant  
			 {	
				 e=dy-dx/2;  
				 while(x1>=x2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){y1-=1;e-=dx;}	   
					 x1-=1;  
					 e+=dy;  
				 }	
			 }	
			 else		 // 6/8 octant	
			 {	
				 e=dx-dy/2;  
				 while(y1>=y2)	
				 {	
					 PutFbPixel(x1,y1,color);  
					 if(e>0){x1-=1;e-=dy;}	   
					 y1-=1;  
					 e+=dx;  
				 }	
			 }	
		 }	   
	 }	
 }	

 /**********************************************************************************
   * @brief 	  :  绘制校准触摸屏所用的用来点击的十字架
   * @param[in]   :  x	十字架的x坐标
   					 y	十字架的y坐标
   					 color	十字架的颜色
   * @param[out]  :  无
   * @return	  :  无
   * @others	  :  无
 ***********************************************************************************/
void FbDispCross(int x, int y, unsigned int color)
{
	DrawLine(x-10, y, x+10, y, color);
	DrawLine(x, y-10, x, y+10, color);
}

/********************************************************************************************
  * @brief		 :	在LCD的指定位置绘制一个指定大小，指定颜色的矩形
  * @param[in]	 :	x  矩形的x坐标
					y  矩形的y坐标
					color  矩形的颜色
  * @param[out]  :	无
  * @return 	 :	无
  * @others 	 :	无
*********************************************************************************************/
void DrawRectangle(int x,int y,int length,int high,int color)
{
	int i;
	for(i = 0; i < 3 ; i++)
	{
		DrawLine(x,y+i,x+length,y+i,color);
		DrawLine(x+length-i,y,x+length-i,y+high,color);
		DrawLine(x+length,y+high-i,x,y+high-i,color);
		DrawLine(x+i,y+high,x+i,y,color);
	}	
}

