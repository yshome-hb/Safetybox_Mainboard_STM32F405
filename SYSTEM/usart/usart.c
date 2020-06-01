#include "stdio.h"
#include "usart.h"	

static Receive_Parse8_f uart1_parse = NULL;
static Receive_Parse8_f uart3_parse = NULL;
static Receive_Parse8_f uart4_parse = NULL;
static Receive_Parse8_f uart5_parse = NULL;

void uart1_init(u32 buad, Receive_Parse8_f receive_parse)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	uart1_parse = receive_parse;	
	
	USART_InitStructure.USART_BaudRate = buad;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	if(uart1_parse != NULL)
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	else
		USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);	
	
	if(uart1_parse != NULL)
	{
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=8;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
}

void uart3_init(u32 buad, Receive_Parse8_f receive_parse)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOB时钟	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //使能USART3时钟	

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10复用为USART3_TX
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11复用为USART3_RX
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	uart3_parse = receive_parse;

	USART_InitStructure.USART_BaudRate = buad;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	if(uart3_parse != NULL)
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	else
		USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	USART_Cmd(USART3, ENABLE);

	//USART_ClearFlag(USART3, USART_FLAG_TC);
	
	if(uart3_parse != NULL)
	{		
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
		NVIC_Init(&NVIC_InitStructure);	
	}
}

void uart4_init(u32 buad, Receive_Parse8_f receive_parse)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOC时钟	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); //使能UART4时钟	

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOC10复用为UART4_TX
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOC11复用为UART4_RX
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	uart4_parse = receive_parse;

	USART_InitStructure.USART_BaudRate = buad;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	if(uart4_parse != NULL)
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	else
		USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);

	USART_Cmd(UART4, ENABLE);

	//USART_ClearFlag(UART4, USART_FLAG_TC);
	
	if(uart4_parse != NULL)
	{		
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
		NVIC_Init(&NVIC_InitStructure);	
	}
}

void uart5_init(u32 buad, Receive_Parse8_f receive_parse)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOC时钟	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE); //使能USART5时钟	

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOC12复用为UART5_TX
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5);  //GPIOA10复用为UART5_RX
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);

	uart5_parse = receive_parse;

	USART_InitStructure.USART_BaudRate = buad;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	if(uart5_parse != NULL)
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	else
		USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);

	USART_Cmd(UART5, ENABLE);

	//USART_ClearFlag(UART5, USART_FLAG_TC);
	
	if(uart5_parse != NULL)
	{		
		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);

		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
		NVIC_Init(&NVIC_InitStructure);	
	}
}


void uart1_send(uint8_t c)
{
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	USART_SendData(USART1, c);
}

void uart3_send(uint8_t c)
{
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);
	USART_SendData(USART3, c);
}

void uart4_send(uint8_t c)
{
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET);
	USART_SendData(UART4, c);
}

void uart5_send(uint8_t c)
{
	while(USART_GetFlagStatus(UART5,USART_FLAG_TC)!=SET);
	USART_SendData(UART5, c);
}


void USART1_IRQHandler(void)
{
	uint8_t recv;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		recv =USART_ReceiveData(USART1);
 		if(uart1_parse != NULL)
			uart1_parse(recv);
	} 
} 

void USART3_IRQHandler(void)
{
	uint8_t recv;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		recv =USART_ReceiveData(USART3);
 		if(uart3_parse != NULL)
			uart3_parse(recv);
	} 
} 

void UART4_IRQHandler(void)
{
	uint8_t recv;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		recv =USART_ReceiveData(UART4);
 		if(uart4_parse != NULL)
			uart4_parse(recv);
	} 
} 

void UART5_IRQHandler(void)
{
	uint8_t recv;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		recv =USART_ReceiveData(UART5);
 		if(uart5_parse != NULL)
			uart5_parse(recv);
	} 
} 


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式   
//__use_no_semihosting was requested, but _ttywrch was 
void _ttywrch(int ch) 
{ 
ch = ch; 
} 

void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	uart1_send(ch);     
	return ch;
}
#endif 

