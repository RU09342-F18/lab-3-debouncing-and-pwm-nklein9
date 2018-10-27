/*Nicholas Klein
 *Created 10/27/18   Last Edit: 10/27/18
 *Button Debouncing on an MSP430F5529
 */

#include <msp430.h> 

int setFreq(int hz)
{
    //counts pos edge of 12kHz clock to the ratio, giving a response at the desired frequency
    int new = 2000/hz;

    return new;
}

void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;               //Unlocks GPIO pins

    //LED Setup
    P1SEL &= ~BIT0;                    //Sets P1.0 as an I/O pin
    P1OUT &= ~BIT0;                    //Turn off LED
    P1DIR |= BIT0;                     //Sets P1.0 as an output

    // Button setup
    P2SEL &= ~BIT1;                     //Sets P2.1 as an I/O pin
    P2DIR &= ~BIT1;                     // button is an input
    P2OUT |= BIT1;                      // pull-up resistor
    P2REN |= BIT1;                      // resistor enabled

    //Clock Setup
    TA0CTL = TASSEL_1 +  ID_3 + MC_1 + TACLR + TAIE;    //ACLK, /8, UP Mode, clear TAR, enable interrupt

    //Interrupt flag timing frequency setup
    TA0CCR0 = setFreq(10);              //LED 1 interrupt set for 2Hz

    //Interrupt Setup
    P2IFG &= ~BIT1;                     //No Interrupt Pending
    P2IE |= BIT1;                       //interrupt is enabled
    P2IES |= BIT1;                      //interrupt is set with high-to-low transition

    _BIS_SR(LPM3_bits + GIE);           //Enter Low Power Mode
}

//interrupt service routine
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_B1(void)
{
    P1OUT ^= BIT0;
    TA0CTL = TASSEL_1 + ID_3 + MC_1 + TACLR + TAIE;  // ACLK, /8, up, clear TBR, enable interrupt
    TA0CTL = TACLR;
    P1IFG &= ~BIT1;                     // Clear the P1.1 interrupt
    P1IE |= BIT1;                       //interrupt is enabled
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IE &= ~BIT1;                       //interrupt is disabled
    TA0CCR0 = setFreq(8);              //LED 1 interrupt set for 8Hz
    TA0CTL = TASSEL_1 + ID_3 + MC_1 + TACLR + TAIE;  // ACLK, /8, up, clear TBR, enable interrupt

}
