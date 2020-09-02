#include "stm32L476xx.h"

//**************************************************************************************************
//º¯ÊýÉùÃ÷
//**************************************************************************************************
#define SW_ON    0
#define SW_OFF   1

void delayms(unsigned int i);			
void Init(void);
void AllLedFlashOnce(void);
int  Finite_Machine_State(void);
int  SW1_State(void);
int  SW4_State(void);
void LightLed(int nCurCount);
void Warning(void);
void wrong_coin_Warning(void);
void Print_Ticket(void);

//**************************************************************************************************
//definition
//**************************************************************************************************

int Finite_Machine_State(void)//Determine State.
{
	int nSW1,nSW4,x=0;
  nSW1 = SW1_State();
	nSW4 = SW4_State();
	if((SW_OFF==nSW4)&&(SW_ON==nSW1))
	{
		x= 1;
	}
	else if((SW_ON==nSW4)&&(SW_ON==nSW1))
	{
		x= 2;
	}
	else if((SW_OFF==nSW4)&&(SW_OFF==nSW1))
	{
		x= 3;
	}

	return x;
}

void delayms(unsigned int i)
{
	int j=0;
  while(i!=0)
	{
		  i--;
		  for(j=1000;j>0;j--);  //delay 1ms
	}
}

unsigned int tttt;
void Init(void)
{

  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;//GPIOA clock
  GPIOA->MODER &= ~(0xFF<<(10));
  GPIOA->MODER |=0x55<<(10);//output
  GPIOA->ODR &=~(0xF<<5);
	GPIOA->MODER &=~(0x3C000C);
	GPIOA->PUPDR &=~(0x3C000C);
	GPIOA->PUPDR |=0x140004;
	
//	GPIOA->MODER &=0xFFFFFF3F;
//	GPIOA->PUPDR &=(0xFFFFFF3F);//
//	GPIOA->PUPDR |=0X00000040;//
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;//GPIOB clock
	GPIOB->MODER &=0xFFFFFFFC;
	GPIOB->MODER |=0x1;
  GPIOB->ODR &=~(0x1);
	GPIOB->MODER &=0xFFFFFF3F;
	GPIOB->PUPDR &=(0xFFFFFF3F);//
	GPIOB->PUPDR |=0X40;//
	
}

void AllLedFlashOnce(void)
{
	GPIOA->ODR &=~(0xF<<5);
	delayms(200);
	GPIOA->ODR |=0xF<<5;	// LED flashes once,prove that SW1 was pressed
	delayms(200);
	GPIOA->ODR &=~(0xF<<5);
}

int SW1_State(void)
{
	 int flag=1;
	
	 flag = GPIOA->IDR;//Determine if SW1 is being pressed.
	 flag=flag&0x0200; 
	 if(0==flag)// 
	 {
		 return 0; 
	 }
	 else
	 {
	   return 1; 
	 }
}

int SW4_State(void)
{
	 int flag=1;
	
	 flag = GPIOA->IDR;//Determine if SW4 is being pressed.
	 flag=flag&0x0400; 
	 if(0==flag)// If SW4 is being pressed, 
	 {
		 return 0; 
	 }
	 else
	 {
	   return 1; 
	 }
}

void LightLed(int nCurCount)
{
	GPIOA->ODR &=~(0xF<<5);
	GPIOA->ODR |=nCurCount<<5;	
	delayms(100);	//Control the LED to count
}

void Warning(void)//Warning for exceeding the coin limit
{
	for(int i=0;i<5;i++)
	{
		GPIOA->ODR &=~(0xF<<5);
		delayms(200);
		GPIOA->ODR |=1<<5;	
		delayms(200);
	}
}

void wrong_coin_Warning(void)//Wrong coin warning
{
	for(int i=0;i<5;i++)
	{
		GPIOB->ODR &=~(0x1);
		delayms(200);
		GPIOB->ODR |=(0x1);
		delayms(200);
	}
	GPIOB->ODR &=~(0x1);
}

void Print_Ticket(void)//Print a receipt
{
	for(int i=0;i<5;i++)
	{
		GPIOA->ODR &=~(0xF<<5);
		delayms(200);
		GPIOA->ODR |=0xF<<5;	
		delayms(200);
	}
}

int main(void)
{
  int nCount=0;
	int nMachine=0;
  int flag=0;   
	
  Init();
  while(1)
	{
		nMachine = Finite_Machine_State();
		switch(nMachine)
		{
			case 1: 
		  	if(nCount==0)
				{
				   AllLedFlashOnce();
				}
				
				while(GPIOA->IDR &=1<<1)// Waiting Push the "push SW", enter 50penny.
				{
					//delayms(10)
					if(1!=Finite_Machine_State())
					{
						break;
					}
					
					flag = (GPIOB->IDR &=(1<<3));
					if(flag == 0)
				  {
					  break;
				  }							
				}
				if(SW_OFF==SW4_State())
				{	
          			
					delayms(200);		// Avoid shaking
          if(!(GPIOB->IDR &=(1<<3)))
          { 
            wrong_coin_Warning();
          }
          else if(GPIOA->IDR &=~(1<<1))
					{
						if(nCount==0xF)
						{
							 Warning();						
						}
						else 
						{	 
							 nCount++;//enter 50penny.				
							 LightLed(nCount);             	 
						}							
					}
			  }
	  
				break;
				
			case 2:
				Print_Ticket();
				break;
			
			case 3:
				nCount=0;
			  Init();
			  delayms(200);
				break;
			default:
				break;
		}
	}
}
