#include <wiringPi.h>
#include <stdio.h>

typedef unsigned          char uint8_t;

#define NRF_CE          3
#define NRF_IRQ	        2

#define RF24L01_SET_CE_HIGH()       digitalWrite(NRF_CE,  HIGH)
#define RF24L01_SET_CE_LOW()        digitalWrite(NRF_CE,  LOW) 

#define RF24L01_GET_IRQ_STATUS()    digitalRead(NRF_IRQ)

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


/** NRF24L01���� */
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//�Ĵ�����������
#define NRF_READ_REG    0x00	//�����üĴ�������5λΪ�Ĵ�����ַ
#define NRF_WRITE_REG   0x20	//д���üĴ�������5λΪ�Ĵ�����ַ
#define RD_RX_PLOAD     0x61	//��RX��Ч���ݣ�1~32�ֽ�
#define WR_TX_PLOAD     0xA0	//дTX��Ч���ݣ�1~32�ֽ�
#define FLUSH_TX        0xE1	//���TX FIFO�Ĵ���������ģʽ��ʹ��
#define FLUSH_RX        0xE2	//���RX FIFO�Ĵ���������ģʽ��ʹ��
#define REUSE_TX_PL     0xE3	//����ʹ����һ�����ݣ�CEΪ�ߣ����ݰ������Ϸ���
#define R_RX_PL_WID     0x60
#define NOP             0xFF	//�ղ���������������״̬�Ĵ���
#define W_ACK_PLOAD		0xA8
#define WR_TX_PLOAD_NACK 0xB0
//SPI(NRF24L01)�Ĵ�����ַ
#define CONFIG          0x00	//���üĴ�����ַ��bit0:1����ģʽ,0����ģʽ;bit1:��ѡ��;bit2:CRCģʽ;bit3:CRCʹ��;
							    //bit4:�ж�MAX_RT(�ﵽ����ط������ж�)ʹ��;bit5:�ж�TX_DSʹ��;bit6:�ж�RX_DRʹ��	
#define EN_AA           0x01	//ʹ���Զ�Ӧ���� bit0~5 ��Ӧͨ��0~5
#define EN_RXADDR       0x02	//���յ�ַ���� bit0~5 ��Ӧͨ��0~5
#define SETUP_AW        0x03	//���õ�ַ����(��������ͨ��) bit0~1: 00,3�ֽ� 01,4�ֽ�, 02,5�ֽ�
#define SETUP_RETR      0x04	//�����Զ��ط�;bit0~3:�Զ��ط�������;bit4~7:�Զ��ط���ʱ 250*x+86us
#define RF_CH           0x05	//RFͨ��,bit0~6����ͨ��Ƶ��
#define RF_SETUP        0x06	//RF�Ĵ�����bit3:��������( 0:1M 1:2M);bit1~2:���书��;bit0:�����Ŵ�������
#define STATUS          0x07	//״̬�Ĵ���;bit0:TX FIFO����־;bit1~3:��������ͨ����(���:6);bit4:�ﵽ�����ط�����
								//bit5:���ݷ�������ж�;bit6:���������ж�
#define MAX_TX  		0x10	//�ﵽ����ʹ����ж�
#define TX_OK   		0x20	//TX��������ж�
#define RX_OK   		0x40	//���յ������ж�

#define OBSERVE_TX      0x08	//���ͼ��Ĵ���,bit7~4:���ݰ���ʧ������;bit3~0:�ط�������
#define CD              0x09	//�ز����Ĵ���,bit0:�ز����
#define RX_ADDR_P0      0x0A	//����ͨ��0���յ�ַ����󳤶�5���ֽڣ����ֽ���ǰ
#define RX_ADDR_P1      0x0B	//����ͨ��1���յ�ַ����󳤶�5���ֽڣ����ֽ���ǰ
#define RX_ADDR_P2      0x0C	//����ͨ��2���յ�ַ������ֽڿ����ã����ֽڣ�����ͬRX_ADDR_P1[39:8]���
#define RX_ADDR_P3      0x0D	//����ͨ��3���յ�ַ������ֽڿ����ã����ֽڣ�����ͬRX_ADDR_P1[39:8]���
#define RX_ADDR_P4      0x0E	//����ͨ��4���յ�ַ������ֽڿ����ã����ֽڣ�����ͬRX_ADDR_P1[39:8]���
#define RX_ADDR_P5      0x0F	//����ͨ��5���յ�ַ������ֽڿ����ã����ֽڣ�����ͬRX_ADDR_P1[39:8]���
#define TX_ADDR         0x10	//���͵�ַ(���ֽ���ǰ),ShockBurstTMģʽ�£�RX_ADDR_P0���ַ���
#define RX_PW_P0        0x11	//��������ͨ��0��Ч���ݿ���(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P1        0x12	//��������ͨ��1��Ч���ݿ���(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P2        0x13	//��������ͨ��2��Ч���ݿ���(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P3        0x14	//��������ͨ��3��Ч���ݿ���(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P4        0x15	//��������ͨ��4��Ч���ݿ���(1~32�ֽ�),����Ϊ0��Ƿ�
#define RX_PW_P5        0x16	//��������ͨ��5��Ч���ݿ���(1~32�ֽ�),����Ϊ0��Ƿ�
#define NRF_FIFO_STATUS 0x17	//FIFO״̬�Ĵ���;bit0:RX FIFO�Ĵ����ձ�־;bit1:RX FIFO����־;bit2~3����
								//bit4:TX FIFO �ձ�־;bit5:TX FIFO����־;bit6:1,ѭ��������һ���ݰ�.0,��ѭ��								
#define DYNPD			0x1C
#define FEATRUE			0x1D
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//λ����
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

uint8_t NRF24L01_Read_Reg( uint8_t RegAddr )
{
    uint8_t bytes[2] = {0x0,0x0};

    bytes[0] = NRF_READ_REG | RegAddr;

    wiringPiSPIDataRW(0,bytes,2);

    return bytes[1];
}

int NRF24L01_Read_Buf( uint8_t RegAddr, uint8_t *pBuf, uint8_t len )
{
    uint8_t *p_data;

    p_data = (uint8_t *)malloc(len + 1);
    if(!p_data)
    {
        printf("read_buf mem error!\n");
        return -1;
    }
    *p_data = NRF_READ_REG | RegAddr;
    memcpy(p_data + 1,pBuf,len);

    wiringPiSPIDataRW(0,p_data,len + 1);
    memcpy(pBuf,p_data + 1,len);

    free(p_data);
    return 0;
}

void NRF24L01_Write_Reg( uint8_t RegAddr, uint8_t Value )
{
    uint8_t bytes[2];

    bytes[0] = NRF_WRITE_REG | RegAddr;
	bytes[1] = Value;
    wiringPiSPIDataRW(0,bytes,2);
}

int NRF24L01_Write_Buf( uint8_t RegAddr, uint8_t *pBuf, uint8_t len )
{
    uint8_t *p_data;

    p_data = (uint8_t *)malloc(len + 1);
    if(!p_data)
    {
        printf("wirte_buf mem error!\n");
        return -1;
    }
    *p_data = NRF_WRITE_REG | RegAddr;
    memcpy(p_data + 1,pBuf,len);

    wiringPiSPIDataRW(0,p_data,len + 1);

    free(p_data);
    return 0;
}


void NRF24L01_Flush_Tx_Fifo ( void )
{

}


void NRF24L01_Flush_Rx_Fifo( void )
{

}

void NRF24L01_Reuse_Tx_Payload( void )
{

}

void NRF24L01_Nop( void )
{

}

uint8_t NRF24L01_Read_Status_Register( void )
{
    uint8_t bytes[2] = {0x0,0x0};

    bytes[0] = NRF_READ_REG + STATUS;

    wiringPiSPIDataRW(0,bytes,2);

    return bytes[1];
}

uint8_t NRF24L01_Clear_IRQ_Flag( uint8_t IRQ_Source )
{
    uint8_t btmp = 0;

    IRQ_Source &= ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT );	//??????
    btmp = NRF24L01_Read_Status_Register( );			//??????
			
    NRF24L01_Write_Reg(STATUS,IRQ_Source | btmp);
	
    return ( NRF24L01_Read_Status_Register( ));			//?????????
}

uint8_t RF24L01_Read_IRQ_Status( void )
{
    return 0;
}

uint8_t NRF24L01_Read_Top_Fifo_Width( void )
{
    return 0;
}

uint8_t NRF24L01_Read_Rx_Payload( uint8_t *pRxBuf )
{
    return 0;
}

void NRF24L01_Write_Tx_Payload_Ack( uint8_t *pTxBuf, uint8_t len )
{

}

void NRF24L01_Write_Tx_Payload_NoAck( uint8_t *pTxBuf, uint8_t len )
{

}

void NRF24L01_Write_Tx_Payload_InAck( uint8_t *pData, uint8_t len )
{

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
	
	// RF24L01_SET_CS_LOW( );		//??
	// drv_spi_read_write_byte( FLUSH_TX );
	// RF24L01_SET_CS_HIGH( );
	
	RF24L01_SET_CE_HIGH( );		//modify by luozhu!!!!!!!!!!
	NRF24L01_Write_Buf( WR_TX_PLOAD, txbuf, Length );	//????TX BUF 32??  TX_PLOAD_WIDTH
//	RF24L01_SET_CE_HIGH( );			//????
	while( 0 != RF24L01_GET_IRQ_STATUS( ))
	{
		delay(5);
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
int NRF24L01_RxPacket( uint8_t *rxbuf )
{
	uint8_t l_Status = 0, l_RxLength = 0, l_100MsTimes = 0;
	int times = 0;

	RF24L01_Set_Mode(MODE_RX);
	
	// RF24L01_SET_CS_LOW( );		//??
	// drv_spi_read_write_byte( FLUSH_RX );
	// RF24L01_SET_CS_HIGH( );
	
	while( 0 != RF24L01_GET_IRQ_STATUS())
	{
		delay(5);
		if(times++ == 10){
			return -1;
		}
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




void NRF24L01_check( void )
{
	uint8_t i;
	uint8_t buf[5]={ 0XA5, 0XA5, 0XA5, 0XA5, 0XA5 };
	uint8_t read_buf[ 5 ] = { 0 };
	 
	while( 1 )
	{
        memset(buf,0xA5,5);
        memset(read_buf,0,5);
		NRF24L01_Write_Buf( TX_ADDR, buf, 5 );			//д��5���ֽڵĵ�ַ
        delay(20);
		NRF24L01_Read_Buf( TX_ADDR, read_buf, 5 );		//����д��ĵ�ַ  
        printf("send data:0XA5, 0XA5, 0XA5, 0XA5, 0XA5\n");
        printf("read data:");
		for( i = 0; i < 5; i++ )
		{
            printf("0x%02x ",read_buf[i]);
        }
        printf("\n");
		
		if( 5 == i )
		{
            printf("NRF24L01 CHECK OK!\n");
			break;
		}
		else
		{
			printf("NRF24L01 CHECK FAIL!\n");
		}
		delay(1000);
	}
}

void nrf24l01_init()
{
	wiringPiSetup() ;
    wiringPiSPISetup(0,1000000);

    pinMode (NRF_CE, OUTPUT) ;
    pinMode(NRF_IRQ,INPUT);
    digitalWrite(NRF_CE,  LOW) ;

    NRF24L01_check();

    RF24L01_Init();

    	
	NRF24L01_Write_Reg(RF_SETUP,0x26);
	
}

int nrf24l01_test(void)
{
    const char *g_Ashining = "ashining";
    
    char data[16];
	uint8_t read_count = 0;
	uint8_t read_data[128] = {0};

    uint8_t btmp = 0;
    wiringPiSetup() ;
    wiringPiSPISetup(0,1000000);

    pinMode (NRF_CE, OUTPUT) ;
    pinMode(NRF_IRQ,INPUT);
    digitalWrite(NRF_CE,  LOW) ;

    NRF24L01_check();

    RF24L01_Init();

    	
	NRF24L01_Write_Reg(RF_SETUP,0x26);
	
	btmp = NRF24L01_Read_Reg( RF_SETUP );
    printf("RF_SETUP = 0x%02x\n",btmp);

#if 1
    RF24L01_Set_Mode(MODE_TX);
    NRF24L01_TxPacket( (uint8_t *)g_Ashining, 8 );
	printf("send over\n");
	while(1);
#else
	
//	RF24L01_Set_Mode(MODE_RX);
	read_count = NRF24L01_RxPacket(read_data);
	printf("%s",read_data);
	
#endif

}
