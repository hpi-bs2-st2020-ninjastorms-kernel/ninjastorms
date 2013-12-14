
#include <stddef.h>

#define GPIO_BASE ((void*)0x01E26000)
#define SYSCFG_BASE ((void*)0x01C14000)
#define NO_OF_LEDS 4

#define REGUnlock \
  {\
    *((unsigned int*)(SYSCFG_BASE + 0x38)) = 0x83E70B13;\
    *((unsigned int*)(SYSCFG_BASE + 0x3C)) = 0x95A4F1E0;\
  }

#define REGLock \
  {\
    *((unsigned int*)SYSCFG_BASE + 0x38) = 0x0;\
    *((unsigned int*)SYSCFG_BASE + 0x3C) = 0x0;\
  }

#define   DIODEInit(D) \
  {\
    (*UiLedPin[D].pGpio).clr_data  =  UiLedPin[D].Mask;\
    (*UiLedPin[D].pGpio).dir      &= ~UiLedPin[D].Mask;\
  }

#define   DIODEOn(D) \
  {\
    (*UiLedPin[D].pGpio).set_data  =  UiLedPin[D].Mask;\
  }

#define   DIODEOff(D) \
  {\
    (*UiLedPin[D].pGpio).clr_data  =  UiLedPin[D].Mask;\
  }


enum UiLedPins
{
  DIODE0,
  DIODE1,
  DIODE2,
  DIODE3,
  DIODE4,
  DIODE5,
  LED_PINS
};

typedef   struct
{
  int               Pin;
  unsigned short    MuxReg;
  unsigned int      Mask;
  unsigned int      Mode;
}
MRM;

enum
{
  GP0_0,GP0_1,GP0_2,GP0_3,GP0_4,GP0_5,GP0_6,GP0_7,GP0_8,GP0_9,GP0_10,GP0_11,GP0_12,GP0_13,GP0_14,GP0_15,
  GP1_0,GP1_1,GP1_2,GP1_3,GP1_4,GP1_5,GP1_6,GP1_7,GP1_8,GP1_9,GP1_10,GP1_11,GP1_12,GP1_13,GP1_14,GP1_15,
  GP2_0,GP2_1,GP2_2,GP2_3,GP2_4,GP2_5,GP2_6,GP2_7,GP2_8,GP2_9,GP2_10,GP2_11,GP2_12,GP2_13,GP2_14,GP2_15,
  GP3_0,GP3_1,GP3_2,GP3_3,GP3_4,GP3_5,GP3_6,GP3_7,GP3_8,GP3_9,GP3_10,GP3_11,GP3_12,GP3_13,GP3_14,GP3_15,
  GP4_0,GP4_1,GP4_2,GP4_3,GP4_4,GP4_5,GP4_6,GP4_7,GP4_8,GP4_9,GP4_10,GP4_11,GP4_12,GP4_13,GP4_14,GP4_15,
  GP5_0,GP5_1,GP5_2,GP5_3,GP5_4,GP5_5,GP5_6,GP5_7,GP5_8,GP5_9,GP5_10,GP5_11,GP5_12,GP5_13,GP5_14,GP5_15,
  GP6_0,GP6_1,GP6_2,GP6_3,GP6_4,GP6_5,GP6_6,GP6_7,GP6_8,GP6_9,GP6_10,GP6_11,GP6_12,GP6_13,GP6_14,GP6_15,
  GP7_0,GP7_1,GP7_2,GP7_3,GP7_4,GP7_5,GP7_6,GP7_7,GP7_8,GP7_9,GP7_10,GP7_11,GP7_12,GP7_13,GP7_14,GP7_15,
  GP8_0,GP8_1,GP8_2,GP8_3,GP8_4,GP8_5,GP8_6,GP8_7,GP8_8,GP8_9,GP8_10,GP8_11,GP8_12,GP8_13,GP8_14,GP8_15,
  NO_OF_GPIOS,
  UART0_TXD,UART0_RXD,UART1_TXD,UART1_RXD,
  SPI0_MOSI,SPI0_MISO,SPI0_SCL,SPI0_CS,
  SPI1_MOSI,SPI1_MISO,SPI1_SCL,SPI1_CS,
  EPWM1A,EPWM1B,APWM0,APWM1,EPWM0B,AXR3,AXR4
};

MRM       MuxRegMap[] =
{ //  Pin     MuxReg  Mask        Mode

    { GP0_1 ,      1,     0xF0FFFFFF, 0x08000000 },
    { GP0_2 ,      1,     0xFF0FFFFF, 0x00800000 },
    { GP0_3 ,      1,     0xFFF0FFFF, 0x00080000 },
    { GP0_4 ,      1,     0xFFFF0FFF, 0x00008000 },
    { GP0_5 ,      1,     0xFFFFF0FF, 0x00000800 },
    { GP0_6 ,      1,     0xFFFFFF0F, 0x00000080 },
    { GP0_7 ,      1,     0xFFFFFFF0, 0x00000008 },

    { GP0_11,      0,     0xFFF0FFFF, 0x00080000 },
    { GP0_12,      0,     0xFFFF0FFF, 0x00008000 },
    { GP0_13,      0,     0xFFFFF0FF, 0x00000800 },
    { GP0_14,      0,     0xFFFFFF0F, 0x00000080 },
    { GP0_15,      0,     0xFFFFFFF0, 0x00000008 },

    { GP1_0 ,      4,     0x0FFFFFFF, 0x80000000 },
    { GP1_8 ,      3,     0xFFFFFFF0, 0x00000004 },

    { GP1_9,       2,     0xF0FFFFFF, 0x04000000 },
    { GP1_10,      2,     0xFF0FFFFF, 0x00400000 },
    { GP1_11,      2,     0xFFF0FFFF, 0x00040000 },
    { GP1_12,      2,     0xFFFF0FFF, 0x00004000 },
    { GP1_13,      2,     0xFFFFF0FF, 0x00000400 },
    { GP1_14,      2,     0xFFFFFF0F, 0x00000040 },
    { GP1_15,      2,     0xFFFFFFF0, 0x00000008 },

    { GP2_0,       6,     0x0FFFFFFF, 0x80000000 },
    { GP2_1,       6,     0xF0FFFFFF, 0x08000000 },
    { GP2_2,       6,     0xFF0FFFFF, 0x00800000 },
    { GP2_3,       6,     0xFFF0FFFF, 0x00080000 },
    { GP2_4,       6,     0xFFFF0FFF, 0x00008000 },
    { GP2_5,       6,     0xFFFFF0FF, 0x00000800 },
    { GP2_6,       6,     0xFFFFFF0F, 0x00000080 },
    { GP2_7,       6,     0xFFFFFFF0, 0x00000008 },

    { GP2_8,       5,     0x0FFFFFFF, 0x80000000 },
    { GP2_9,       5,     0xF0FFFFFF, 0x08000000 },
    { GP2_10,      5,     0xFF0FFFFF, 0x00800000 },
    { GP2_11,      5,     0xFFF0FFFF, 0x00080000 },
    { GP2_12,      5,     0xFFFF0FFF, 0x00008000 },
    { GP2_13,      5,     0xFFFFF0FF, 0x00000800 },

    { GP3_0,       8,     0x0FFFFFFF, 0x80000000 },
    { GP3_1 ,      8,     0xF0FFFFFF, 0x08000000 },
    { GP3_2,       8,     0xFF0FFFFF, 0x00800000 },
    { GP3_3,       8,     0xFFF0FFFF, 0x00080000 },
    { GP3_4,       8,     0xFFFF0FFF, 0x00008000 },
    { GP3_5,       8,     0xFFFFF0FF, 0x00000800 },
    { GP3_6,       8,     0xFFFFFF0F, 0x00000080 },
    { GP3_7,       8,     0xFFFFFFF0, 0x00000008 },

    { GP3_8,       7,     0x0FFFFFFF, 0x80000000 },
    { GP3_9,       7,     0xF0FFFFFF, 0x08000000 },
    { GP3_10,      7,     0xFF0FFFFF, 0x00800000 },
    { GP3_11,      7,     0xFFF0FFFF, 0x00080000 },
    { GP3_12,      7,     0xFFFF0FFF, 0x00008000 },
    { GP3_13,      7,     0xFFFFF0FF, 0x00000800 },
    { GP3_14,      7,     0xFFFFFF0F, 0x00000080 },
    { GP3_15,      7,     0xFFFFFFF0, 0x00000008 },

    { GP4_1,      10,     0xF0FFFFFF, 0x08000000 },

    { GP4_8,       9,     0x0FFFFFFF, 0x80000000 },
    { GP4_9,       9,     0xF0FFFFFF, 0x08000000 },
    { GP4_10,      9,     0xFF0FFFFF, 0x00800000 },

    { GP4_12,      9,     0xFFFF0FFF, 0x00008000 },

    { GP4_14,      9,     0xFFFFFF0F, 0x00000080 },

    { GP5_0,      12,     0x0FFFFFFF, 0x80000000 },
    { GP5_1,      12,     0xF0FFFFFF, 0x08000000 },
    { GP5_2,      12,     0xFF0FFFFF, 0x00800000 },
    { GP5_3,      12,     0xFFF0FFFF, 0x00080000 },
    { GP5_4,      12,     0xFFFF0FFF, 0x00008000 },
    { GP5_5,      12,     0xFFFFF0FF, 0x00000800 },
    { GP5_6,      12,     0xFFFFFF0F, 0x00000080 },
    { GP5_7,      12,     0xFFFFFFF0, 0x00000008 },

    { GP5_8,      11,     0x0FFFFFFF, 0x80000000 },
    { GP5_9,      11,     0xF0FFFFFF, 0x08000000 },
    { GP5_10,     11,     0xFF0FFFFF, 0x00800000 },
    { GP5_11,     11,     0xFFF0FFFF, 0x00080000 },
    { GP5_12,     11,     0xFFFF0FFF, 0x00008000 },
    { GP5_13,     11,     0xFFFFF0FF, 0x00000800 },
    { GP5_14,     11,     0xFFFFFF0F, 0x00000080 },
    { GP5_15,     11,     0xFFFFFFF0, 0x00000008 },

    { GP6_0 ,     19,     0xF0FFFFFF, 0x08000000 },
    { GP6_1,      19,     0xFF0FFFFF, 0x00800000 },
    { GP6_2,      19,     0xFFF0FFFF, 0x00080000 },
    { GP6_3,      19,     0xFFFF0FFF, 0x00008000 },
    { GP6_4,      19,     0xFFFFF0FF, 0x00000800 },
    { GP6_5,      16,     0xFFFFFF0F, 0x00000080 },

    { GP6_6,      14,     0xFFFFFF0F, 0x00000080 },
    { GP6_7,      14,     0xFFFFFFF0, 0x00000008 },

    { GP6_8,      13,     0x0FFFFFFF, 0x80000000 },
    { GP6_9,      13,     0xF0FFFFFF, 0x08000000 },
    { GP6_10,     13,     0xFF0FFFFF, 0x00800000 },
    { GP6_11,     13,     0xFFF0FFFF, 0x00080000 },
    { GP6_12,     13,     0xFFFF0FFF, 0x00008000 },
    { GP6_13,     13,     0xFFFFF0FF, 0x00000800 },
    { GP6_14,     13,     0xFFFFFF0F, 0x00000080 },
    { GP6_15,     13,     0xFFFFFFF0, 0x00000008 },

    { GP7_4,      17,     0xFF0FFFFF, 0x00800000 },
    { GP7_8,      17,     0xFFFFFF0F, 0x00000080 },
    { GP7_9,      17,     0xFFFFFFF0, 0x00000008 },
    { GP7_10,     16,     0x0FFFFFFF, 0x80000000 },
    { GP7_11,     16,     0xF0FFFFFF, 0x08000000 },
    { GP7_12,     16,     0xFF0FFFFF, 0x00800000 },
    { GP7_13,     16,     0xFFF0FFFF, 0x00080000 },
    { GP7_14,     16,     0xFFFF0FFF, 0x00008000 },
    { GP7_15,     16,     0xFFFFF0FF, 0x00000800 },

    { GP8_2 ,     3 ,     0xF0FFFFFF, 0x04000000 },
    { GP8_3 ,     3 ,     0xFF0FFFFF, 0x00400000 },
    { GP8_5 ,     3 ,     0xFFFF0FFF, 0x00004000 },
    { GP8_6 ,     3 ,     0xFFFFF0FF, 0x00000400 },
    { GP8_8 ,     19,     0xFFFFFF0F, 0x00000080 },
    { GP8_9 ,     19,     0xFFFFFFF0, 0x00000008 },
    { GP8_10,     18,     0x0FFFFFFF, 0x80000000 },
    { GP8_11,     18,     0xF0FFFFFF, 0x08000000 },
    { GP8_12,     18,     0xFF0FFFFF, 0x00800000 },
    { GP8_13,     18,     0xFFF0FFFF, 0x00080000 },
    { GP8_14,     18,     0xFFFF0FFF, 0x00008000 },
    { GP8_15,     18,     0xFFFFF0FF, 0x00000800 },


    { UART0_TXD,   3,     0xFF0FFFFF, 0x00200000 },
    { UART0_RXD,   3,     0xFFF0FFFF, 0x00020000 },

    { UART1_TXD,   4,     0x0FFFFFFF, 0x20000000 },
    { UART1_RXD,   4,     0xF0FFFFFF, 0x02000000 },

    { SPI0_MOSI,   3,     0xFFFF0FFF, 0x00001000 },
    { SPI0_MISO,   3,     0xFFFFF0FF, 0x00000100 },
    { SPI0_SCL,    3,     0xFFFFFFF0, 0x00000001 },
    { SPI0_CS,     3,     0xF0FFFFFF, 0x01000000 },

    { SPI1_MOSI,   5,     0xFF0FFFFF, 0x00100000 },
    { SPI1_MISO,   5,     0xFFF0FFFF, 0x00010000 },
    { SPI1_SCL,    5,     0xFFFFF0FF, 0x00000100 },
    { SPI1_CS,     5,     0xFFFF0FFF, 0x00008000 },

    { EPWM1A,      5,     0xFFFFFFF0, 0x00000002 },
    { EPWM1B,      5,     0xFFFFFF0F, 0x00000020 },
    { APWM0,       2,     0x0FFFFFFF, 0x20000000 },
    { APWM1,       1,     0x0FFFFFFF, 0x40000000 },
    { EPWM0B,      3,     0xFFFFFF0F, 0x00000020 },

    { AXR3,        2,     0xFFF0FFFF, 0x00010000 },
    { AXR4,        2,     0xFFFF0FFF, 0x00001000 },

    { -1,          0,     0,          0 }
};

struct gpio_controller {
        unsigned int     dir;
        unsigned int     out_data;
        unsigned int     set_data;
        unsigned int     clr_data;
        unsigned int     in_data;
        unsigned int     set_rising;
        unsigned int     clr_rising;
        unsigned int     set_falling;
        unsigned int     clr_falling;
        unsigned int     intstat;
};

typedef   struct gpio_controller *     GPIOC;

typedef struct
{
  int               Pin;      // GPIO pin number
  GPIOC             pGpio;    // GPIO bank base address
  unsigned int      Mask;     // GPIO pin mask
}
INPIN;

INPIN UiLedPin[LED_PINS] =
{
  { GP6_7 ,  NULL, 0 },  // DIODE0
  { GP6_13,  NULL, 0 },  // DIODE1
  { GP2_1 ,  NULL, 0 },  // DIODE2
  { GP5_7 ,  NULL, 0 },  // DIODE3
};

typedef   enum
{
  LED_BLACK                     = 0,
  LED_GREEN                     = 1,
  LED_RED                       = 2,
  LED_ORANGE                    = 3,
  LED_GREEN_FLASH               = 4,
  LED_RED_FLASH                 = 5,
  LED_ORANGE_FLASH              = 6,
  LED_GREEN_PULSE               = 7,
  LED_RED_PULSE                 = 8,
  LED_ORANGE_PULSE              = 9,

  LEDPATTERNS
}
LEDPATTERN;

unsigned long LEDPATTERNDATA[NO_OF_LEDS + 1][LEDPATTERNS] =
{ //  LED_BLACK   LED_GREEN   LED_RED    LED_ORANGE           LED_GREEN_FLASH                     LED_RED_FLASH                     LED_ORANGE_FLASH                      LED_GREEN_PULSE                       LED_RED_PULSE                      LED_ORANGE_PULSE
  {  0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0b00000000000000000000000000000000, 0b00000000000000000111110000011111, 0b00000000000000000111110000011111, 0b00000000000000000000000000000000, 0b00000000000000000000000001110111, 0b00000000000000000000000001110111 }, // RR
  {  0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0b00000000000000000111110000011111, 0b00000000000000000000000000000000, 0b00000000000000000111110000011111, 0b00000000000000000000000001110111, 0b00000000000000000000000000000000, 0b00000000000000000000000001110111 }, // RG
  {  0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0b00000000000000000000000000000000, 0b00000000000000000111110000011111, 0b00000000000000000111110000011111, 0b00000000000000000000000000000000, 0b00000000000000000000000001110111, 0b00000000000000000000000001110111 }, // LR
  {  0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0b00000000000000000111110000011111, 0b00000000000000000000000000000000, 0b00000000000000000111110000011111, 0b00000000000000000000000001110111, 0b00000000000000000000000000000000, 0b00000000000000000000000001110111 }, // LG
  { 0 }
};


static void
gpio_set (int Pin)
{
  int     Tmp = 0;
  void    *Reg;

  if (Pin >= 0)
  {
    while ((MuxRegMap[Tmp].Pin != -1) && (MuxRegMap[Tmp].Pin != Pin))
    {
      Tmp++;
    }
    if (MuxRegMap[Tmp].Pin == Pin)
    {
      Reg   =  SYSCFG_BASE + 0x120 + (MuxRegMap[Tmp].MuxReg << 2);

      *(unsigned int*)Reg &=  MuxRegMap[Tmp].Mask;
      *(unsigned int*)Reg |=  MuxRegMap[Tmp].Mode;
    }
  }
}

unsigned char     PatternBlock    = 0;          // Block pattern update
unsigned char     PatternBits     = 20;         // Pattern bits
unsigned char     PatternBit      = 0;          // Pattern bit pointer
unsigned long     ActPattern[NO_OF_LEDS];
unsigned long     TmpPattern[NO_OF_LEDS];

void
gpio_init (void)
{
  REGUnlock;
  
  int Pin;
  for (Pin = 0;Pin < NO_OF_LEDS;Pin++)
  {
    if (UiLedPin[Pin].Pin >= 0)
    {
      UiLedPin[Pin].pGpio  =  (struct gpio_controller *)(GPIO_BASE + ((UiLedPin[Pin].Pin >> 5) * 0x28) + 0x10);
      UiLedPin[Pin].Mask   =  (1 << (UiLedPin[Pin].Pin & 0x1F));

      gpio_set(UiLedPin[Pin].Pin);
    }
  }

  int Tmp;
  for (Tmp = 0;Tmp < NO_OF_LEDS;Tmp++)
    {
      DIODEInit(Tmp);
    }

  PatternBlock  =  1;

  PatternBits   =  20;
  PatternBit    =  0;

  for (Tmp = 0;Tmp < NO_OF_LEDS;Tmp++)
    {
      ActPattern[Tmp]  =  LEDPATTERNDATA[Tmp][2];
    }

  PatternBlock  =  0;

  REGLock;
}

void
led_test (void)
{
  while (1)
    {
      int Tmp;
      for (Tmp = 0;Tmp < NO_OF_LEDS;Tmp++)
        {
          if (PatternBit == 0)
            TmpPattern[Tmp]  =  ActPattern[Tmp];
          if ((TmpPattern[Tmp] & 0x00000001))
            DIODEOn(Tmp);
          else
            DIODEOff(Tmp);
          TmpPattern[Tmp] >>=  1;
        }
    
      if (++PatternBit >= PatternBits)
        PatternBit  =  0;
    }
}
