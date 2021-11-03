#ifndef __spi_da_ad_H_
#define __spi_da_ad_H_
#define AD_CH0 0X0000
#define AD_CH1 0X0800

#define SPI3_CLK_HIGH()         GPIO_SetValue(1, (1<<3))
#define SPI3_CLK_LOW()          GPIO_ClearValue(1, (1<<3))
#define SPI3_MOSI_HIGH()		GPIO_SetValue(3, (1<<19))
#define	SPI3_MOSI_LOW()			GPIO_ClearValue(3, (1<<19))

#define SPI_DA_CS_LOW()			GPIO_ClearValue(1, (1<<12))
#define	SPI_DA_CS_HIGH()		GPIO_SetValue(1, (1<<12))

#define	SPI_U_CS_HIGH()			GPIO_SetValue(0, (1<<9))
#define	SPI_U_CS_LOW()			GPIO_ClearValue(0, (1<<9))

#define SPI_I_CS_HIGH()			GPIO_SetValue(0, (1<<8))
#define SPI_I_CS_LOW()			GPIO_ClearValue(0, (1<<8))

#define SPI3_ADTA_IN()			GPIO_ReadValue(0)


#define _SPI_4094_STROL

void SPI_ADDA_GPIO(void);
uint8_t SPI3_Receivedata(void);
void Send_Vout(uint32_t value);
void Start_HV_Out(void);
void Stop_Vout_vot(void);//高压停止输出;
void Start_Vout_vot(uint16_t da);

void Discharge(void);//放电
void Close_dischange(void);
void HV_Out_Gpioint(void);


uint8_t SPI_ADDA_SendByte(uint8_t byte);
uint32_t Read_AD_7192(uint8_t value);
void AD_7192_Rest(uint8_t chip);
void AD_7129_IO(uint8_t port);
void AD_7192Config(uint32_t value);
void U_adconfig(void);
void V_DA_out(u16 date);
void Out_4094(u8 *buff);
void Range_Control(u8 dat);
void Read_Ad(void);
void Ad_Filter(void);

#endif
