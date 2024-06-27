


/*
  2023, 04, 23
  송명규
  010-2402-4398
  mgsong@hanmail.net 

  V1.0 == 2023, 04, 23 = 최초작성
  V1.2 == 2023, 08, 16 = d_in 추가
  V1.5 == 2023, 11, 16 = byte in arduino 함수 추가  = 더조은컴퓨터학원 에서
  V1.6 == 2024, 05, 28 == define, Enum 추가 == 한국직업능력교육원 IoT 8기 과정에서 추가
*/

#ifndef __My_Arduino_LIB__
#define __My_Arduino_LIB__

#define  ON  1
#define  OFF 0
#define _ON  0
#define _OFF 1

// V1.6 추가 == IoT 8기 == 2024, 5, 28추가
#define LED_0  A0 // 14
#define LED_1  12
#define LED_2  2
#define LED_3  3
#define LED_4  4
#define LED_5  5
#define LED_6  6
#define LED_7  7

const enum LED_
{
  led0 = A0,
  led1 = 12,
  led2 = 2,
  led3 = 3,
  led4 = 4,
  led5 = 5,
  led6 = 6,
  led7 = 7
}LED;
 
const int led_0 = A0;
const int led_1 = 12;
const int led_2 = 2;
const int led_3 = 3;
const int led_4 = 4;
const int led_5 = 5;
const int led_6 = 6;
const int led_7 = 7;

// 출력 핀 정의 = 핀번호는 PRj에 맞게 수정하여 사용하세요
const struct GPIO_OUT
{
  const char OUT_Bit_0;
  const char OUT_Bit_1;
  const char OUT_Bit_2;
  const char OUT_Bit_3;
  const char OUT_Bit_4;
  const char OUT_Bit_5;
  const char OUT_Bit_6;
  const char OUT_Bit_7; 
}Out = {10, 11, 12}; // 핀 번호는 프로젝트에 맞게 수정하여 사용하세요

// 입력 핀 정의 = 핀번호는 PRj에 맞게 수정하여 사용하세요
const struct GPIO_IN
{
  const char IN_Bit_0;
  const char IN_Bit_1;
  const char IN_Bit_2;
  const char IN_Bit_3;
  const char IN_Bit_4;
  const char IN_Bit_5;
  const char IN_Bit_6;
  const char IN_Bit_7;
}In = {0, 1, 2, 3, 4, 5, 6, 7}; // 핀 번호는 프로젝트에 맞게 수정하여 사용하세요

extern char gpio_in_flag = 0;  // GPIO 입력이 있으면 1로 설정된다. = 외부 입력시. Key 압력체크용

// 구조체 포인터 선언
struct GPIO_OUT *Gpio_Out = &Out;
struct GPIO_IN  *Gpio_in = &In;

// MCU Mode Bit Control
#define bit_set(ADDRESS, BIT) (ADDRESS |= (1<<BIT)) // bit hi 
#define bit_clr(ADDRESS, BIT) (ADDRESS &= ~(1<<BIT)) // bit low
#define bit_chk(ADDRESS, BIT) (ADDRESS & (1<<BIT))  // bit chak
#define bit_tg(PORT, BIT) (PORT ^= (1<<BIT))  //bit 토글

// Macro
#define d_out(pin, value) digitalWrite(pin, value)
#define d_in(pin) digitalRead(pin)

// Arduino API 사용시
void Byte_Out(unsigned char data)
{
 d_out(Gpio_Out -> OUT_Bit_0, data%2);     // bit 0 = lsb
 d_out(Gpio_Out -> OUT_Bit_1, data/2%2);   // bit 1 
 d_out(Gpio_Out -> OUT_Bit_2, data/4%2);   // bit 2 
 d_out(Gpio_Out -> OUT_Bit_3, data/8%2);   // bit 3 
 d_out(Gpio_Out -> OUT_Bit_4, data/16%2);  // bit 4  
 d_out(Gpio_Out -> OUT_Bit_5, data/32%2);  // bit 5
 d_out(Gpio_Out -> OUT_Bit_6, data/64%2);  // bit 6
 d_out(Gpio_Out -> OUT_Bit_7, data/128%2); // bit 7 = msb
}

// Arduino API 사용시
char Byte_In()
{
  char buf = 0x0e;

 buf =  d_in(Gpio_in -> IN_Bit_0);     // bit 0 = lsb
 buf |= d_in(Gpio_in -> IN_Bit_1)*2;   // bit 1 
 buf |= d_in(Gpio_in -> IN_Bit_2)*4;   // bit 2 
 buf |= d_in(Gpio_in -> IN_Bit_3)*8;   // bit 3 
 buf |= d_in(Gpio_in -> IN_Bit_4)*16;  // bit 4  
 buf |= d_in(Gpio_in -> IN_Bit_5)*32;  // bit 5
 buf |= d_in(Gpio_in -> IN_Bit_6)*64;  // bit 6
 buf |= d_in(Gpio_in -> IN_Bit_7)*128; // bit 7 = msb
/*
 buf &= 0x0e; 
 while((PINC & 0x0e) != 0x0e)   
  {
    gpio_in_flag = 1;
  }
*/
 return buf;
}

// 2023, 11, 20 추가 = 멀티펑션보드 
#define Bz_Off(Pin, Value)  d_out(Pin, Value) // bz off 
#define Bz_On(Pin, Value)   d_out(Pin, Value) // bz on 
 

#endif

// 사용법
// d_out(LED1, ON);
// bit_set(PORTD, 5);
// bit_clr(PORTD, 3);
// if(bit_chk(PIND, 3) == 0){  }
// bit_tg(PORTD, 0);
// delay(400);