/* 
 * File:   main.c
 * Author: Allen Hurtig
 *
 * Created on August 13, 2019, 9:20 PM
 */

#define _LEGACY_HEADERS
#include <htc.h>
#include <stdlib.h> //Needed for the rand function.

//----------------DEVICE CONFIGURATION

__CONFIG (INTIO & WDTDIS & PWRTEN & MCLRDIS & UNPROTECT & BORDIS & IESODIS & FCMDIS);
//-------------------PROGRAM MEMORY--------
#define _XTAL_FREQ 4000000 // defined frequency of internal oscillator (4MHz) must have for delay function

#define LED0_BLUE RC3
#define LED0_GREEN RC6
#define LED0_RED RC7

#define LED1_BLUE RC0
#define LED1_GREEN RC1
#define LED1_RED RC2

#define LED2_BLUE RB4
#define LED2_GREEN RB5
#define LED2_RED RB6

void _delay_ms(unsigned long n); // function using a delay of n milliseconds
void _delay_us(unsigned long n); // function using a delay of n microseconds

//Constants
//integer values can be written in 0bxxxxxxxx format.
const int red0 = 0b10000000;
const int green0 = 0b01000000;
const int blue0 = 0b00001000;
const int yellow0 = 0b11000000;
const int cyan0 = 0b01001000;
const int purple0 = 0b10001000;
const int white0 = 0b11001000;
const int led0Colors[] = {128, 64, 8, 192, 72, 136, 200, 128};  //decimal value

const int red1 = 0b00000100;
const int green1 = 0b00000010;
const int blue1 = 0b00000001;
const int yellow1 = 0b00000110;
const int cyan1 = 0b00000011;
const int purple1 = 0b00000101;
const int white1 = 0b00000111;
const int led1Colors[] = {4, 2, 1, 6, 3, 5, 7, 4}; //decimal value

const int red2 = 0b01000000;
const int green2 = 0b00100000;
const int blue2 = 0b00010000;
const int yellow2 = 0b01100000;
const int cyan2 = 0b00110000;
const int purple2 = 0b01010000;
const int white2 = 0b01110000;
const int led2Colors[] = {64, 32, 16, 96, 48, 82, 112, 64}; //decimal value

const int timesArray[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                          11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

//outside values.
volatile unsigned int i1 = 0;

void ChangeLED(volatile unsigned char* port, volatile unsigned char* port2,
        int outColor, int inColor,
        int outColor2, int inColor2,
        int outColor3, int inColor3, int loopLimit)
{
    //Change the LED.

    for(int k = 0; k < 21; k++)
    {
        //The time expansion loop.
        for(int j = 0; j < loopLimit; j++)
        {
            *port = outColor | outColor2;
            *port2 = outColor3;

            //The brighness and dimming cycle.
            for(int i = 0; i < (timesArray[20-k]); i++)
            {
                __delay_us(10);
            }

            *port = inColor | inColor2;
            *port2 = inColor3;

            for(int i = 0; i < (timesArray[k]); i++)
            {
                __delay_us(10);
            }

        }
    }
}

void Mode1(void)
{
    //Run through all the colors at 5 seconds a piece, 4 seconds solid
    //with a one second transition.
    //ORDER of colors: red, green, blue, yellow, cyan, magenta, white

    //Set the default color to red.
    PORTC = red0 | red1;
    PORTB = red2;

    //BEGIN MAIN LOOP
    for (int j=0; j<7; j++)
    {
        for(int i=0; i<8; i++)
        {
          __delay_ms(500);
        }
        ChangeLED(&PORTC, &PORTB,
              led0Colors[j], led0Colors[j+1],
              led1Colors[j], led1Colors[j+1],
              led2Colors[j], led2Colors[j+1], 28);
    }
}

void Mode2 (void)
{
    //Display a solid color depending on what value i1 is.
    PORTC=(led0Colors[i1-1]) | (led1Colors[i1-1]);
    PORTB=led2Colors[i1-1];
}

void Mode3 (int string1, int string3)
{
    //Create a twinkling effect.

    //Create random color for string 2.
    int string2 = led1Colors[rand()%6];

    ChangeLED(&PORTC, &PORTB,
                      string1, 0,
                      0, string2,
                      string3, 0, 20);

    //Create random colors for string 1 and string 3.
    int newstring1 = led0Colors[rand()%6];
    int newstring3 = led2Colors[rand()%6];

    ChangeLED(&PORTC, &PORTB,
                      0, newstring1,
                      string2, 0,
                      0, newstring3, 20);

    //Call the function again with the same colors for string 1 and 3.
    Mode3(newstring1, newstring3);
}

void Mode4 (void)
{
    //Cycle through red, green, and white in christmas fashion.
    PORTC=white0 | red1;
    PORTB=white2;
    __delay_ms(500);

    PORTC=red0 | green1;
    PORTB=red2;
    __delay_ms(500);

    PORTC=green0 | white1;
    PORTB=green2;
    __delay_ms(500);
}

void Mode5 (void)
{
    //Random mode.
    ChangeLED(&PORTC, &PORTB,
              led0Colors[rand()%6], led0Colors[rand()%6],
              led1Colors[rand()%6], led1Colors[rand()%6],
              led2Colors[rand()%6], led2Colors[rand()%6], 30);
}

void Mode6 (void)
{
    //Complete random mode.
    ChangeLED(&PORTC, &PORTB,
              rand()%200, rand()%200,
              rand()%7, rand()%7,
              rand()%112, rand()%112, rand()%50);
}

void interrupt ISR(void)
{
    //External interrupt detected.
    if(INTF==1)
    {
        PORTC=0;
        PORTB=0;
        i1 = i1 + 1;
        if (i1==12)
        {
            i1=0;
        }
        __delay_ms(100); //Debounce the push button.
        PORTA;     //Reference PORTA per the data sheet.
        while(RA2==0)
        {
            __delay_ms(100); //Debounce the push button.
        }
        INTF = 0;  //Clear the interrupt flag.
        main();
    }
}

init ()
{
    //Settings for the microcontroller go here.
    CM1CON0=0 ;    //  Turn off Comparators
    CM2CON0=0 ;
    ANSEL = 0;   //  ALL AN (Analog input) pins are digital I/O
    ANSELH = 0 ; //so we are not using the ADC
    OPTION=0b00000000;
    GIE = 1; //Global interrupts.
    PEIE = 1; //Peripheral interrupts.
    INTE = 1; //Enable RA2 as interrupt.
    //INTEDG = 0; //Interrupt on edge (0 falling 1 rising).
    TRISA=0b00000100;     //PortA only RA2 for button input.
    TRISB=0;     //PortB all outputs
    TRISC=0;     //PortC all outputs
    PORTA=0;     //Clear all pins on A
    PORTB=0;     //Clear all pins on B
    PORTC=0;     //Clear all pins on C
}

main()
{
    //Initialize the microcontroller.
    init();

    //Main loop.
    while (1)   //runs continuously until MCU is shut off or interrupted.
    {
        while (i1==0)
        {
            //Main mode that cycles through all colors every 5 seconds.
            Mode1();
        }
        while (i1>=1 && i1<=7)
        {
            //Solid color mode.
            Mode2();
        }
        while (i1==8)
        {
            //Initialize these here instead of in the function argument or the
            //compiler will throw an error.
            int string1 = led0Colors[rand()%6];
            int string3 = led2Colors[rand()%6];

            //Random twinkle mode.
            Mode3(string1, string3);
        }
        while (i1==9)
        {
            //Christmas theme mode.
            Mode4();
        }
        while (i1==10)
        {
            //Random mode.
            Mode5();
        }
        while (i1==11)
        {
            //Utter randomness mode.
            Mode6();
        }
    }
}