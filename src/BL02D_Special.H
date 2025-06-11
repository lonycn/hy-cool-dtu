 

/*----------------------------------------------
Function:BL02D_Special_Temp(int16_t  temp)
Input:  int16_t  temp:读取传感器,经过偏差计算后,得到温度值
Output:  VarList[0...3]:处理后的温湿度放置,该变量为外部变量
Description:通过计算,判断温度是否在温度拉伸范围内,如果是正常模式,直接退出,
                                                  如果是拉伸模式,根据设定的值进行拉伸
                                                  如果是特定值模式,根据设定的特定值进行填充
------------------------------------------------*/   
extern void  BL02D_Special_Temp(int16_t  temp);
	
/*----------------------------------------------
Function:void  BL02D_Special_Humi(int16_t  temp)
Input:  int16_t  temp:读取传感器,经过偏差计算后,得到湿度度值
Output:    VarList[0...3]:处理后的温湿度放置,该变量为外部变量
Description:通过计算,判断温度是否在湿度度拉伸范围内,如果是正常模式,直接退出,
                                                  如果是拉伸模式,根据设定的值进行拉伸
                                                  如果是特定值模式,根据设定的特定值进行填充
------------------------------------------------*/   
extern void  BL02D_Special_Humi(int16_t  temp);

/*----------------------------------------------
Function:void BL02D_Special_Initial()
Input:  None
Output:    None
Description:特定情况下的参数初始化
------------------------------------------------*/    
extern void BL02D_Special_Initial();

/*----------------------------------------------
Function:void BL02D_Special_Update()
Input:  None
Output:    None
Description:更新特定情况下的参数
------------------------------------------------*/    
extern void BL02D_Special_Update()  ;  
    