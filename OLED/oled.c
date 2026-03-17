#include "oled.h"
#include "oledfont.h"   //只能在oled.c里调用，不能放到oled.h中


#define oled_adr 0x3C

#define OLED_MAX_DELAY      50
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
/**
 * -X---------------------------  设置X坐标 n*8+1
 *      0-------------------------127
 * - 0  ***************************
 * - 1  ***************************
 * Y 2  ***************************
 * - 3  ***************************
 * - 4  ***************************
 * - 5  ***************************
 * - 6  ***************************
 * - 7  ***************************
 * @brief	OLED写入命令
 * @param cmd - 待写入命令
 * @note	移植时，请使用自己的底层API实现 
*/
static void OLED_Write_Cmd(uint8_t cmd)
{
	uint8_t buf[2];
	buf[0] = 0x00;	//control byte
	buf[1] = cmd;
	
	//使用HAL库的API实现
	HAL_I2C_Master_Transmit(&hi2c1, oled_adr << 1, buf, 2, OLED_MAX_DELAY);

}
/**
 * @brief	OLED写入数据
 * @param cmd - 待写入数据
 * @note	移植时，请使用自己的底层API实现 
*/
static void OLED_Write_Dat(uint8_t dat)
{
	uint8_t buf[2];
	buf[0] = 0x40; //control byte
	buf[1] = dat;
	
	//使用HAL库的API实现
	HAL_I2C_Master_Transmit(&hi2c1, oled_adr << 1, buf, 2, OLED_MAX_DELAY);
}

//static void IR219_Write_Dat(uint8_t dat)
//{
//	uint8_t buf[2];
//	uint8_t re_buf[2];
//	buf[0] = 0x00; //control byte
//	buf[1] = dat;
//	
//	//使用HAL库的API实现
//	HAL_I2C_Master_Transmit(&hi2c1, IR219_adr, buf, 2, 0xFFFF);
//	HAL_I2C_Master_Receive(&hi2c1, IR219_adr,re_buf, 2,0xFFFF);
//}

/** 8个字节占据一个x列(可以优化内存占用)
 * @brief	OLED设置显示位置
 * @param x - X方向位置  0~127
 * @param y - Y方向位置  0~63
*/
//void OLED_Draw_Pixel(uint8_t x, uint8_t y)
//{
//	uint8_t dot=0;
//	OLED_Set_Pos(x,y);
//	OLED_Write_Dat(dot);	
//}

/**
 * @brief	OLED设置显示位置
 * @param x - X方向位置
 * @param y - Y方向位置
*/
void OLED_Set_Pos(uint8_t x, uint8_t y)
{ 	
	OLED_Write_Cmd(0xb0+y);
	OLED_Write_Cmd(((x&0xf0)>>4)|0x10);
	OLED_Write_Cmd((x&0x0f)|0x01);
}   	  
/**
 * @brief	OLED开启显示
*/ 
void OLED_Display_On(void)
{
	OLED_Write_Cmd(0X8D);  //SET DCDC命令
	OLED_Write_Cmd(0X14);  //DCDC ON
	OLED_Write_Cmd(0XAF);  //DISPLAY ON
}
/**
 * @brief	OLED关闭显示
*/   
void OLED_Display_Off(void)
{
	OLED_Write_Cmd(0X8D);  //SET DCDC命令
	OLED_Write_Cmd(0X10);  //DCDC OFF
	OLED_Write_Cmd(0XAE);  //DISPLAY OFF
}		   			 
/**
 * @brief	OLED清屏函数（清屏之后屏幕全为黑色）
*/ 
void OLED_Clear(void)
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_Write_Cmd(0xb0+i);    //设置页地址（0~7）
		OLED_Write_Cmd(0x00);      //设置显示位置—列低地址
		OLED_Write_Cmd(0x10);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
		{
			OLED_Write_Dat(0);
		}			
	}
}
/**
 * @brief	OLED显示全开（所有像素点全亮）
*/
void OLED_On(void)
{  
	uint8_t i,n;
	for(i=0;i<8;i++)
	{  
		OLED_Write_Cmd(0xb0+i);    //设置页地址（0~7）
		OLED_Write_Cmd(0x00);      //设置显示位置—列低地址
		OLED_Write_Cmd(0x10);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)
		{
			OLED_Write_Dat(1);
		}			
	}
}
/**
 * @brief	在指定位置显示一个ASCII字符
 * @param x - 0 - 127
 * @param y - 0 - 7
 * @param chr  - 待显示的ASCII字符
 * @param size - ASCII字符大小
 * 				字符大小有12(6*8)/16(8*16)两种大小
*/
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size)
{   
	uint8_t c=0,i=0;
	
	c = chr-' ';	
	if(x > 128-1)
	{
		x=0;
		y++;
	}
	
	if(size ==16)
	{
		OLED_Set_Pos(x,y);	
		for(i=0;i<8;i++)
		{
			OLED_Write_Dat(F8X16[c*16+i]);
		}
		OLED_Set_Pos(x,y+1);
		for(i=0;i<8;i++)
		{
			OLED_Write_Dat(F8X16[c*16+i+8]);
		}
	}
	else
	{	
		OLED_Set_Pos(x,y);
		for(i=0;i<6;i++)
		{
			OLED_Write_Dat(F6x8[c][i]);
		}
	}
}
/**
 * @brief	OLED 专用pow函数
 * @param m - 底数
 * @param n - 指数
*/
static uint32_t oled_pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}	
/**
 * @brief	在指定位置显示一个整数
 * @param x - 0 - 127
 * @param y - 0 - 7
 * @param num - 待显示的整数(0-4294967295)
 * @param	len - 数字的位数 
 * @param size - ASCII字符大小
 * 				字符大小有12(6*8)/16(8*16)两种大小
*/
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size); 
	}
} 
/**
 * @brief	在指定位置显示一个字符串
 * @param x - 0 - 127
 * @param y - 0 - 7
 * @param chr - 待显示的字符串指针
 * @param size - ASCII字符大小
 * 				字符大小有12(6*8)/16(8*16)两种大小
*/
void OLED_ShowString(uint8_t x,uint8_t y,char *chr,uint8_t size)
{
	uint8_t j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j],size);
			x+=8;
			if(x>120){x=0;y+=2;}
			j++;
	}
}
/**
 * @brief	在指定位置显示一个汉字
 * @param x  - 0 - 127
 * @param y  - 0 - 7
 * @param no - 汉字在中文字库数组中的索引（下标）
 * @note	中文字库在oledfont.h文件中的Hzk数组中，需要提前使用软件对汉字取模
*/
//void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no)
//{      			    
//	uint8_t t,adder=0;
//	OLED_Set_Pos(x,y);	
//    for(t=0;t<16;t++)
//		{
//				OLED_Write_Dat(Hzk[2*no][t]);
//				adder+=1;
//     }	
//		OLED_Set_Pos(x,y+1);	
//    for(t=0;t<16;t++)
//			{	
//				OLED_Write_Dat(Hzk[2*no+1][t]);
//				adder+=1;
//      }					
//}
/**
 * @brief	在指定位置显示一幅图片
 * @param x1,x2  - 0 - 127
 * @param y1,y2  - 0 - 7(8表示全屏显示)
 * @param BMP - 图片数组地址
 * @note	图像数组BMP存放在bmp.h文件中
*/
//void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,uint8_t BMP[])
//{ 	
// uint16_t j=0;
// uint8_t x,y;
//  
//  if(y1%8==0)
//	{
//		y=y1/8;
//	}		
//  else
//	{
//		y=y1/8+1;
//	}
//	for(y=y0;y<y1;y++)
//	{
//		OLED_Set_Pos(x0,y);
//    for(x=x0;x<x1;x++)
//	  {      
//	    	OLED_Write_Dat(BMP[j++]);	    	
//	  }
//	}
//} 

/**
 * @brief	OLED初始化
*/			    
void OLED_Init(void)
{ 	

	HAL_Delay(5);
	
	OLED_Write_Cmd(0xAE);//--display off

	OLED_Write_Cmd(0x00);//---set low column address X轴低位，起始X轴为0
	OLED_Write_Cmd(0x10);//---set high column address X轴高位
	OLED_Write_Cmd(0x40);//--set start line address  Y轴，可设区间[0x40,0x7F]，设置为0了
	
	OLED_Write_Cmd(0x81); // contract control 命令头，调节亮度,对比度,变化很小，但是仔细可以观察出来
	OLED_Write_Cmd(0xFF);//--128   可设置区间[0x00,0xFF]

	OLED_Write_Cmd(0xA1);//设置X轴扫描方向，0xa0左右反置 ，0xa1正常（左边为0列）
	OLED_Write_Cmd(0xC8);//设置Y轴扫描方向，0xc0上下反置 ，0xc8正常（上边为0行）
	OLED_Write_Cmd(0xA6);//位值表示的意义，0xa6表示正常，1为点亮，0为关闭，0xa7显示效果相反 --normal / reverse 

	OLED_Write_Cmd(0xA8);//--set multiplex ratio(1 to 64) 命令头，设置多路复用率(1 to 64)
	OLED_Write_Cmd(0x3F);//--1/64 duty

	OLED_Write_Cmd(0xD3);//-set display offset 命令头，设置显示偏移移位映射RAM计数器(0x00~0x3F)
	OLED_Write_Cmd(0x00);//不偏移

	OLED_Write_Cmd(0xD5);//set osc division 命令头，设置显示时钟分频比/振荡器频率
	OLED_Write_Cmd(0x80);//设置分割比率，设置时钟为100帧/秒
	
	OLED_Write_Cmd(0xD9);//命令头，Set Pre-Charge Period
	OLED_Write_Cmd(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock

	OLED_Write_Cmd(0xDA);//命令头，--set com pins hardware configuration
	OLED_Write_Cmd(0x12);//

	OLED_Write_Cmd(0xDB);//命令头，set Vcomh
	OLED_Write_Cmd(0x40);//Set VCOM Deselect Level

	OLED_Write_Cmd(0x20);//命令头，设置寻址模式
	OLED_Write_Cmd(0x02);//页面寻址模式(重置) (0x00/0x01/0x02)
	
	OLED_Write_Cmd(0x8D);//命令头，set charge pump enable
	OLED_Write_Cmd(0x14);//--set(0x10) disable

	OLED_Write_Cmd(0xA4);//恢复到RAM内容显示(重置)

	OLED_Write_Cmd(0xA6);//?

	OLED_Write_Cmd(0xAF);//--turn on oled panel 开启显示
	
	OLED_Clear();
	OLED_Set_Pos(0,0);
}


