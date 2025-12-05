/**
功能：sd8563读addr
修改：sjm
参数：reg_addr-寄存器地址，*read_buf-保存读取的数据，length-长度，bus-iic总线
返回值：RTC_OK-成功
**/




/**
功能：sd8563写addr
修改：sjm
参数：reg_addr-寄存器地址，*write_buf-写数据，length-长度，bus-iic总线
返回值：RTC_OK-成功
**/



/**
功能：sd8563读时间数据
修改：sjm
参数：*time-结构体指针，IIC_x-iic总线
返回值：RTC_OK-成功
**/




/**
功能：sd8563写时间数据
修改：sjm
参数：*time-结构体指针，IIC_x-iic总线
返回值：RTC_OK-成功
**/



/**
功能：sd8563读id
修改：sjm
参数：*Data-保存读取的数据，IIC_x-iic总线
返回值：TRUE-成功
**/



/**
功能：sd8563读取温度
修改：sjm
参数：IIC_x-iic总线
返回值：sd8563_temp-读取到的温度
**/



/**
功能：sd8563读id数据oled显示
修改：sjm
参数：*Data-保存读取的数据，IIC_x-iic总线
返回值：0-成功
**/



/**
功能：sd8563读取寄存器，返回读取到的数据
修改：sjm
参数：addr-寄存器地址，bus-iic总线
返回值：mdata-返回读到的数值
**/




/**
功能：sd8563读取vbat电压
修改：sjm
参数：IIC_x-iic总线
返回值：sd8200_vbat-读取到vbat电压
**/




/**
功能：sd8563读取vbat电压和上位机使用
修改：sjm
参数：IIC_x-iic总线
返回值：sd8200_vbat-读取到vbat电压
**/






/**
功能：sd8563输出1Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/




/**
功能：sd8563关闭输出1Hz频率
修改：sjm
参数：IIC_x-iic总线
返回值：RTC_OK-成功
**/





/**
功能：sd8563和标准模块时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/





/**
功能：sd8563时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/






/**
功能：sd8563复制时间oled显示
修改：sjm
参数：-
返回值：TRUE-成功
**/





/**
功能：与上位机交互读取sd8563数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度，IIC_x-iic总线
返回值：SD8563_read_addr(addr,data,len,IIC_x)-读取到的数据
**/





/**
功能：与上位机交互写sd8563数据
修改：sjm
参数：addr-寄存器地址，*data-读取的数据，len-长度，IIC_x-iic总线
返回值：TRUE-成功
**/





/**
功能：sd8563打开充电功能
修改：sjm
参数：-
返回值：-
**/





/**
功能：sd8563关闭充电功能
修改：sjm
参数：-
返回值：-
**/



