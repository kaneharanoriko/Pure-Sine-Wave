#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"



#define PWM_FREQUENCY 30000

int main(void)
{
    volatile uint32_t ui32Load;
    volatile uint32_t ui32PWMClock;
    volatile uint8_t ui8Adjust;
    ui8Adjust = 10;

    //Set bus clock to 40 MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    //Set PWM clock to 10 MHz
    SysCtlPWMClockSet(SYSCTL_PWMDIV_4); //40 MHz/4  = 10 MHz by setting divider 4

    //Enable, Turn on clock to PWM1, GPIOD, GPIOF //Pin PDO
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //Configure pin PDO for PWM output
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinConfigure(GPIO_PD0_M1PWM0);

    //Configure PORTF switches
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;  //unlock PF0
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    //Set up PWM Clock
    ui32PWMClock = SysCtlClockGet() / 4;  //PWM clock is  10 MHz
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;  //PWM period
    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);  //Count down mode
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);  //Set the PWM period

    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 1000);
    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);  //Enable PWM output
    PWMGenEnable(PWM1_BASE, PWM_GEN_0);  //Turn PWM1 on

    //uint32_t duty[8] = {1, 38, 71, 92, 99, 92, 71,38};
    uint32_t duty[12] = {63, 85, 98, 98, 85, 63, 37,15,2,2,15,37};
    uint32_t i;//set up for which one in array
    uint32_t j;

    while(1)
    {
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0,  ui32Load*duty[i] / 100);// Setup for % duty cycle
        i++;
        if(i > 11)
        {
            i = 0; //once max set back to min
        }

        //SysCtlDelay((41680/3));//waiting for a mili-second
        SysCtlDelay((41680*8)/18);//waiting for a mili-second

        //Delay Loop 1ms
        //for(j = 0; j<16000; j++){}

     }

}


