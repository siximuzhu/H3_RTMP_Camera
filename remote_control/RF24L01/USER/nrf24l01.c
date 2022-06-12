#include <stdint.h>
#include "stm32f10x.h"

/*
ce 		--- 	A3	
irq 	---		A2
cs		---		A4
clk		---		A5
mosi	---		A7
miso	---		A6
*/




/** ??????? */
#define DYNAMIC_PACKET      1 		//1:?????, 0:??
#define FIXED_PACKET_LEN    32		//???
#define REPEAT_CNT          15		//????
#define INIT_ADDR           0x34,0x43,0x10,0x10,0x01

typedef enum ModeType
{
	MODE_TX = 0,
	MODE_RX
}nRf24l01ModeType;

typedef enum SpeedType
{
	SPEED_250K = 0,
	SPEED_1M,
	SPEED_2M
}nRf24l01SpeedType;

typedef enum PowerType
{
	POWER_F18DBM = 0,
	POWER_F12DBM,
	POWER_F6DBM,
	POWER_0DBM
}nRf24l01PowerType;


/** NRF24L01?? */
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//???????
#define NRF_READ_REG    0x00	//??????,?5???????
#define NRF_WRITE_REG   0x20	//??????,?5???????
#define RD_RX_PLOAD     0x61	//?RX????,1~32??
#define WR_TX_PLOAD     0xA0	//?TX????,1~32??
#define FLUSH_TX        0xE1	//??TX FIFO???,???????
#define FLUSH_RX        0xE2	//??RX FIFO???,???????
#define REUSE_TX_PL     0xE3	//?????????,CE??,????????
#define R_RX_PL_WID     0x60
#define NOP             0xFF	//???,??????????
#define W_ACK_PLOAD		0xA8
#define WR_TX_PLOAD_NACK 0xB0
//SPI(NRF24L01)?????
#define CONFIG          0x00	//???????,bit0:1????,0????;bit1:???;bit2:CRC??;bit3:CRC??;
							    //bit4:??MAX_RT(??????????)??;bit5:??TX_DS??;bit6:??RX_DR??	
#define EN_AA           0x01	//???????? bit0~5 ????0~5
#define EN_RXADDR       0x02	//?????? bit0~5 ????0~5
#define SETUP_AW        0x03	//??????(??????) bit0~1: 00,3?? 01,4??, 02,5??
#define SETUP_RETR      0x04	//??????;bit0~3:???????;bit4~7:?????? 250*x+86us
#define RF_CH           0x05	//RF??,bit0~6??????
#define RF_SETUP        0x06	//RF???,bit3:????( 0:1M 1:2M);bit1~2:????;bit0:???????
#define STATUS          0x07	//?????;bit0:TX FIFO???;bit1~3:???????(??:6);bit4:?????????
								//bit5:????????;bit6:??????
#define MAX_TX  		0x10	//??????????
#define TX_OK   		0x20	//TX??????
#define RX_OK   		0x40	//???????

#define OBSERVE_TX      0x08	//???????,bit7~4:????????;bit3~0:?????
#define CD              0x09	//???????,bit0:????
#define RX_ADDR_P0      0x0A	//????0????,????5???,?????
#define RX_ADDR_P1      0x0B	//????1????,????5???,?????
#define RX_ADDR_P2      0x0C	//????2????,???????,???,???RX_ADDR_P1[39:8]??
#define RX_ADDR_P3      0x0D	//????3????,???????,???,???RX_ADDR_P1[39:8]??
#define RX_ADDR_P4      0x0E	//????4????,???????,???,???RX_ADDR_P1[39:8]??
#define RX_ADDR_P5      0x0F	//????5????,???????,???,???RX_ADDR_P1[39:8]??
#define TX_ADDR         0x10	//????(?????),ShockBurstTM???,RX_ADDR_P0?????
#define RX_PW_P0        0x11	//??????0??????(1~32??),???0???
#define RX_PW_P1        0x12	//??????1??????(1~32??),???0???
#define RX_PW_P2        0x13	//??????2??????(1~32??),???0???
#define RX_PW_P3        0x14	//??????3??????(1~32??),???0???
#define RX_PW_P4        0x15	//??????4??????(1~32??),???0???
#define RX_PW_P5        0x16	//??????5??????(1~32??),???0???
#define NRF_FIFO_STATUS 0x17	//FIFO?????;bit0:RX FIFO??????;bit1:RX FIFO???;bit2~3??
								//bit4:TX FIFO ???;bit5:TX FIFO???;bit6:1,?????????.0,???								
#define DYNPD			0x1C
#define FEATRUE			0x1D
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//???
#define MASK_RX_DR   	6 
#define MASK_TX_DS   	5 
#define MASK_MAX_RT  	4 
#define EN_CRC       	3 
#define CRCO         	2 
#define PWR_UP       	1 
#define PRIM_RX      	0 

#define ENAA_P5      	5 
#define ENAA_P4      	4 
#define ENAA_P3      	3 
#define ENAA_P2      	2 
#define ENAA_P1      	1 
#define ENAA_P0      	0 

#define ERX_P5       	5 
#define ERX_P4       	4 
#define ERX_P3       	3 
#define ERX_P2      	2 
#define ERX_P1       	1 
#define ERX_P0       	0 

#define AW_RERSERVED 	0x0 
#define AW_3BYTES    	0x1
#define AW_4BYTES    	0x2
#define AW_5BYTES    	0x3

#define ARD_250US    	(0x00<<4)
#define ARD_500US    	(0x01<<4)
#define ARD_750US    	(0x02<<4)
#define ARD_1000US   	(0x03<<4)
#define ARD_2000US   	(0x07<<4)
#define ARD_4000US   	(0x0F<<4)
#define ARC_DISABLE   	0x00
#define ARC_15        	0x0F

#define CONT_WAVE     	7 
#define RF_DR_LOW     	5 
#define PLL_LOCK      	4 
#define RF_DR_HIGH    	3 
//bit2-bit1:
#define PWR_18DB  		(0x00<<1)
#define PWR_12DB  		(0x01<<1)
#define PWR_6DB   		(0x02<<1)
#define PWR_0DB   		(0x03<<1)

#define RX_DR         	6 
#define TX_DS         	5 
#define MAX_RT        	4 
//for bit3-bit1, 
#define TX_FULL_0     	0 

#define RPD           	0 

#define TX_REUSE      	6 
#define TX_FULL_1     	5 
#define TX_EMPTY      	4 
//bit3-bit2, reserved, only '00'
#define RX_FULL       	1 
#define RX_EMPTY      	0 

#define DPL_P5        	5 
#define DPL_P4        	4 
#define DPL_P3        	3 
#define DPL_P2        	2 
#define DPL_P1        	1 
#define DPL_P0        	0 

#define EN_DPL        	2 
#define EN_ACK_PAY    	1 
#define EN_DYN_ACK    	0 
#define IRQ_ALL  ( (1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT) )

#define RF24L01_SET_CS_HIGH()			GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define RF24L01_SET_CS_LOW()			GPIO_ResetBits(GPIOA,GPIO_Pin_4)

#define RF24L01_SET_CE_HIGH()		GPIO_SetBits(GPIOA,GPIO_Pin_3)
#define RF24L01_SET_CE_LOW()		GPIO_ResetBits(GPIOA,GPIO_Pin_3)

#define RF24L01_GET_IRQ_STATUS( )		(( GPIOA->IDR & (uint32_t)GPIO_Pin_2) != GPIO_Pin_2 ) ? 0 : 1	//IRQ??

void delay_ms(int a)
{
	int i,j;
	for(i = 0;i < 100000;i++)
		for(j = 0;j < a;j++);
}

void spi_init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE);
  /* SPI1 configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//SPI_CPOL_High; //must be mode 0!!!!!mode 3 is not ok  for nrl24l01
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;//SPI_BaudRatePrescaler_4; //clk cannot too fast,16M is not ok.  4M is ok
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(SPI1, ENABLE);
}


void gpio_init(void)
{		  
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);
	/* Configure SPI1 pins: SCK, MISO and MOSI ---------------------------------*/ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  | GPIO_Pin_6 | GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	/* CS and CE */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*irq*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_2);

	RF24L01_SET_CS_HIGH();
//	RF24L01_SET_CE_LOW();

	spi_init();
}

 uint8_t drv_spi_read_write_byte( uint8_t TxData )
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, TxData);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}


uint8_t NRF24L01_Read_Reg( uint8_t RegAddr )
{
	uint8_t btmp;
	
	RF24L01_SET_CS_LOW(); 
	drv_spi_read_write_byte( NRF_READ_REG | RegAddr );	
    btmp = drv_spi_read_write_byte( 0xFF );				
	RF24L01_SET_CS_HIGH();
	return btmp;
}

void NRF24L01_Read_Buf( uint8_t RegAddr, uint8_t *pBuf, uint8_t len )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );			//??
	
    drv_spi_read_write_byte( NRF_READ_REG | RegAddr );	//??? ??
    for( btmp = 0; btmp < len; btmp ++ )
    {
        *( pBuf + btmp ) = drv_spi_read_write_byte( 0xff );	//???
    }
    RF24L01_SET_CS_HIGH( );		//????
}

void NRF24L01_Write_Reg( uint8_t RegAddr, uint8_t Value )
{
    RF24L01_SET_CS_LOW( );		//??
	
    drv_spi_read_write_byte( NRF_WRITE_REG | RegAddr );	//??? ??
    drv_spi_read_write_byte( Value );			//???
	
    RF24L01_SET_CS_HIGH( );		//????
}

void NRF24L01_Write_Buf( uint8_t RegAddr, uint8_t *pBuf, uint8_t len )
{
    uint8_t i;
	
    RF24L01_SET_CS_LOW( );		//??
	
    drv_spi_read_write_byte( NRF_WRITE_REG | RegAddr );	//??? ??
    for( i = 0; i < len; i ++ )
    {
        drv_spi_read_write_byte( *( pBuf + i ) );		//???
    }
	
    RF24L01_SET_CS_HIGH( );		//????
}

void NRF24L01_Flush_Tx_Fifo ( void )
{
    RF24L01_SET_CS_LOW( );		//??
	
    drv_spi_read_write_byte( FLUSH_TX );	//?TX FIFO??
	
    RF24L01_SET_CS_HIGH( );		//????
}


void NRF24L01_Flush_Rx_Fifo( void )
{
    RF24L01_SET_CS_LOW( );		//??
	
    drv_spi_read_write_byte( FLUSH_RX );	//?RX FIFO??
	
    RF24L01_SET_CS_HIGH( );		//????
}

void NRF24L01_Reuse_Tx_Payload( void )
{
    RF24L01_SET_CS_LOW( );		//??
	
    drv_spi_read_write_byte( REUSE_TX_PL );		//?????????
	
    RF24L01_SET_CS_HIGH( );		//????
}

void NRF24L01_Nop( void )
{
    RF24L01_SET_CS_LOW( );		//??
	
    drv_spi_read_write_byte( NOP );		//?????
	
    RF24L01_SET_CS_HIGH( );		//????
}

uint8_t NRF24L01_Read_Status_Register( void )
{
    uint8_t Status;
	
    RF24L01_SET_CS_LOW( );		//??
	
    drv_spi_read_write_byte( NRF_READ_REG + STATUS );	//??????
	
	Status = drv_spi_read_write_byte( 0xff );
	
    RF24L01_SET_CS_HIGH( );		//????
	
    return Status;
}

uint8_t NRF24L01_Clear_IRQ_Flag( uint8_t IRQ_Source )
{
    uint8_t btmp = 0;

    IRQ_Source &= ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT );	//??????
    btmp = NRF24L01_Read_Status_Register( );			//??????
			
    RF24L01_SET_CS_LOW( );			//??
    drv_spi_read_write_byte( NRF_WRITE_REG + STATUS );	//????????
    drv_spi_read_write_byte( IRQ_Source | btmp );		//???????
    RF24L01_SET_CS_HIGH( );			//????
	
    return ( NRF24L01_Read_Status_Register( ));			//?????????
}

uint8_t RF24L01_Read_IRQ_Status( void )
{
    return ( NRF24L01_Read_Status_Register( ) & (( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT )));	//??????
}

uint8_t NRF24L01_Read_Top_Fifo_Width( void )
{
    uint8_t btmp;
	
    RF24L01_SET_CS_LOW( );		//??
	
    drv_spi_read_write_byte( R_RX_PL_WID );	//?FIFO???????
    btmp = drv_spi_read_write_byte( 0xFF );	//???
	
    RF24L01_SET_CS_HIGH( );		//????
	
    return btmp;
}

uint8_t NRF24L01_Read_Rx_Payload( uint8_t *pRxBuf )
{
    uint8_t Width, PipeNum;
	
    PipeNum = ( NRF24L01_Read_Reg( STATUS ) >> 1 ) & 0x07;	//?????
    Width = NRF24L01_Read_Top_Fifo_Width( );		//???????

    RF24L01_SET_CS_LOW( );		//??
    drv_spi_read_write_byte( RD_RX_PLOAD );			//???????
	
    for( PipeNum = 0; PipeNum < Width; PipeNum ++ )
    {
        *( pRxBuf + PipeNum ) = drv_spi_read_write_byte( 0xFF );		//???
    }
    RF24L01_SET_CS_HIGH( );		//????
    NRF24L01_Flush_Rx_Fifo( );	//??RX FIFO
	
    return Width;
}

void NRF24L01_Write_Tx_Payload_Ack( uint8_t *pTxBuf, uint8_t len )
{
    uint8_t btmp;
    uint8_t length = ( len > 32 ) ? 32 : len;		//??????32 ????32?

    NRF24L01_Flush_Tx_Fifo( );		//?TX FIFO
	
    RF24L01_SET_CS_LOW( );			//??
    drv_spi_read_write_byte( WR_TX_PLOAD );	//????
	
    for( btmp = 0; btmp < length; btmp ++ )
    {
        drv_spi_read_write_byte( *( pTxBuf + btmp ) );	//????
    }
    RF24L01_SET_CS_HIGH( );			//????
}

void NRF24L01_Write_Tx_Payload_NoAck( uint8_t *pTxBuf, uint8_t len )
{
    if( len > 32 || len == 0 )
    {
        return ;		//??????32 ????0 ???
    }
	
    RF24L01_SET_CS_LOW( );	//??
    drv_spi_read_write_byte( WR_TX_PLOAD_NACK );	//????
    while( len-- )
    {
        drv_spi_read_write_byte( *pTxBuf );			//????
		pTxBuf++;
    }
    RF24L01_SET_CS_HIGH( );		//????
}

void NRF24L01_Write_Tx_Payload_InAck( uint8_t *pData, uint8_t len )
{
    uint8_t btmp;
	
	len = ( len > 32 ) ? 32 : len;		//??????32????32???

    RF24L01_SET_CS_LOW( );			//??
    drv_spi_read_write_byte( W_ACK_PLOAD );		//??
    for( btmp = 0; btmp < len; btmp ++ )
    {
        drv_spi_read_write_byte( *( pData + btmp ) );	//???
    }
    RF24L01_SET_CS_HIGH( );			//????
}

void NRF24L01_Set_TxAddr( uint8_t *pAddr, uint8_t len )
{
	len = ( len > 5 ) ? 5 : len;					//??????5???
    NRF24L01_Write_Buf( TX_ADDR, pAddr, len );	//???
}


void NRF24L01_Set_RxAddr( uint8_t PipeNum, uint8_t *pAddr, uint8_t Len )
{
    Len = ( Len > 5 ) ? 5 : Len;
    PipeNum = ( PipeNum > 5 ) ? 5 : PipeNum;		//?????5 ???????5???

    NRF24L01_Write_Buf( RX_ADDR_P0 + PipeNum, pAddr, Len );	//????
}

void NRF24L01_Set_Speed( nRf24l01SpeedType Speed )
{
	uint8_t btmp = 0;
	
	btmp = NRF24L01_Read_Reg( RF_SETUP );
	btmp &= ~( ( 1<<5 ) | ( 1<<3 ) );
	
	if( Speed == SPEED_250K )		//250K
	{
		btmp |= ( 1<<5 );
	}
	else if( Speed == SPEED_1M )   //1M
	{
   		btmp &= ~( ( 1<<5 ) | ( 1<<3 ) );
	}
	else if( Speed == SPEED_2M )   //2M
	{
		btmp |= ( 1<<3 );
	}

	NRF24L01_Write_Reg( RF_SETUP, btmp );
}

void NRF24L01_Set_Power( nRf24l01PowerType Power )
{
    uint8_t btmp;
	
	btmp = NRF24L01_Read_Reg( RF_SETUP ) & ~0x07;
    switch( Power )
    {
        case POWER_F18DBM:
            btmp |= PWR_18DB;
            break;
        case POWER_F12DBM:
            btmp |= PWR_12DB;
            break;
        case POWER_F6DBM:
            btmp |= PWR_6DB;
            break;
        case POWER_0DBM:
            btmp |= PWR_0DB;
            break;
        default:
            break;
    }
    NRF24L01_Write_Reg( RF_SETUP, btmp );
}

void RF24LL01_Write_Hopping_Point( uint8_t FreqPoint )
{
    NRF24L01_Write_Reg(  RF_CH, FreqPoint & 0x7F );
}

void NRF24L01_check( void )
{
	uint8_t i;
	uint8_t buf[5]={ 0XA5, 0XA5, 0XA5, 0XA5, 0XA5 };
	uint8_t read_buf[ 5 ] = { 0 };
	 
	while( 1 )
	{
        memset(buf,0xA5,5);
        memset(read_buf,0,5);
		//RF24L01_SET_CE_HIGH();  //HIGH or LOW is ok
		NRF24L01_Write_Buf( TX_ADDR, buf, 5 );			//??5??????
     //   delay_ms(1);
		NRF24L01_Read_Buf( TX_ADDR, read_buf, 5 );		//???????  
 //       printf("send data:0XA5, 0XA5, 0XA5, 0XA5, 0XA5");
 //       printf("read data:");
		for( i = 0; i < 5; i++ )
		{
    //        printf("0x%02x ",read_buf[i]);
        }
   //     printf("\n");
		
        memset(buf,0xA5,5);
		if( memcmp(buf,read_buf,5) == 0 )
		{
    //        printf("NRF24L01 CHECK OK!\n");
			break;
		}
		else
		{
	//		printf("NRF24L01 CHECK FAIL!\n");
		}
		delay_ms(10);
	}
}

void RF24L01_Set_Mode( nRf24l01ModeType Mode )
{
    uint8_t controlreg = 0;
	controlreg = NRF24L01_Read_Reg( CONFIG );
	
    if( Mode == MODE_TX )       
	{
		controlreg &= ~( 1<< PRIM_RX );
	}
    else 
	{
		if( Mode == MODE_RX )  
		{ 
			controlreg |= ( 1<< PRIM_RX ); 
		}
	}

    NRF24L01_Write_Reg( CONFIG, controlreg );
}

uint8_t NRF24L01_TxPacket( uint8_t *txbuf, uint8_t Length )
{
	uint8_t l_Status = 0;
	
	RF24L01_SET_CS_LOW( );		//??
	drv_spi_read_write_byte( FLUSH_TX );
	RF24L01_SET_CS_HIGH( );
	
	RF24L01_SET_CE_HIGH( );		//modify by luozhu!!!!!!!!!!
	NRF24L01_Write_Buf( WR_TX_PLOAD, txbuf, Length );	//????TX BUF 32??  TX_PLOAD_WIDTH
//	RF24L01_SET_CE_HIGH( );			//????
	while( 0 != RF24L01_GET_IRQ_STATUS( ))
	{
		delay_ms(5);
	}
	l_Status = NRF24L01_Read_Reg(STATUS);						//??????
	NRF24L01_Write_Reg( STATUS, l_Status );						//??TX_DS?MAX_RT????
	
	if( l_Status & MAX_TX )	//????????
	{
		NRF24L01_Write_Reg( FLUSH_TX,0xff );	//??TX FIFO???
		return MAX_TX; 
	}
	if( l_Status & TX_OK )	//????
	{
		return TX_OK;
	}
	
	return 0xFF;	//????????
}

/**
  * @brief :NRF24L01????
  * @param :
  *			@rxbuf:????????
  * @note  :?
  * @retval:???????
  */ 
uint8_t NRF24L01_RxPacket( uint8_t *rxbuf )
{
	uint8_t l_Status = 0, l_RxLength = 0, l_100MsTimes = 0;
	
	RF24L01_SET_CS_LOW( );		//??
	drv_spi_read_write_byte( FLUSH_RX );
	RF24L01_SET_CS_HIGH( );
	
	while( 0 != RF24L01_GET_IRQ_STATUS( ))
	{
		delay_ms(5);
	}
	
	l_Status = NRF24L01_Read_Reg( STATUS );		//??????
	NRF24L01_Write_Reg( STATUS,l_Status );		//?????
	if( l_Status & RX_OK)	//?????
	{
		l_RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//??????????
		NRF24L01_Read_Buf( RD_RX_PLOAD,rxbuf,l_RxLength );	//????? 
		NRF24L01_Write_Reg( FLUSH_RX,0xff );				//??RX FIFO
		return l_RxLength; 
	}	
	
	return 0;				//??????	
}

void RF24L01_Init( void )
{
    uint8_t addr[5] = {INIT_ADDR};

    RF24L01_SET_CE_HIGH( );
    NRF24L01_Clear_IRQ_Flag( IRQ_ALL );
#if DYNAMIC_PACKET == 1

    NRF24L01_Write_Reg( DYNPD, ( 1 << 0 ) ); 	//????1??????
    NRF24L01_Write_Reg( FEATRUE, 0x07 );
    NRF24L01_Read_Reg( DYNPD );
    NRF24L01_Read_Reg( FEATRUE );
	
#elif DYNAMIC_PACKET == 0
    
    L01_WriteSingleReg( L01REG_RX_PW_P0, FIXED_PACKET_LEN );	//??????
	
#endif	//DYNAMIC_PACKET

    NRF24L01_Write_Reg( CONFIG, /*( 1<<MASK_RX_DR ) |*/		//????
                                      ( 1 << EN_CRC ) |     //??CRC 1???
                                      ( 1 << PWR_UP ) );    //????
    NRF24L01_Write_Reg( EN_AA, ( 1 << ENAA_P0 ) );   		//??0????
    NRF24L01_Write_Reg( EN_RXADDR, ( 1 << ERX_P0 ) );		//??0??
    NRF24L01_Write_Reg( SETUP_AW, AW_5BYTES );     			//???? 5???
    NRF24L01_Write_Reg( SETUP_RETR, ARD_4000US |
                        ( REPEAT_CNT & 0x0F ) );         	//?????? 250us
    NRF24L01_Write_Reg( RF_CH, 60 );             			//?????
    NRF24L01_Write_Reg( RF_SETUP, 0x26 );

    NRF24L01_Set_TxAddr( &addr[0], 5 );                      //??TX??
    NRF24L01_Set_RxAddr( 0, &addr[0], 5 );                   //??RX??
}

uint8_t btmp = 0;
void nrf24l01_test(void)
{
    const char *g_Ashining = "ashining";
    
    char data[16];
	uint8_t read_count = 0;
	uint8_t read_data[128] = {0};

    NRF24L01_check();

    RF24L01_Init();
	
#if 0
	btmp = NRF24L01_Read_Reg( RF_SETUP );

    NRF24L01_Write_Reg(RF_SETUP,0x02);

    btmp = NRF24L01_Read_Reg( RF_SETUP );
#endif
	
#if 1
    RF24L01_Set_Mode(MODE_TX);
    NRF24L01_TxPacket( (uint8_t *)g_Ashining, 8 );
	while(1);
#else
	
	RF24L01_Set_Mode(MODE_RX);
	read_count = NRF24L01_RxPacket(read_data);
	printf("%s",read_data);
	
#endif
}

