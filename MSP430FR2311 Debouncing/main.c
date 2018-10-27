/*Nicholas Klein
 *Created 10/3/18   Last Edit: 10/3/18
 *Button Debouncing on an MSP430FR2311
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
    //CSCTL4 = 0x0100;                   // Set ACLK to REFO at 32kHZ, SELA_VLO doesn't exist
    WDTCTL = WDTPW | WDTHOLD;           // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;               //Unlocks GPIO pins

    //LED Setup
    P1SEL0 &= ~BIT0;                    //Sets P1.0 as an I/O pin
    P1SEL1 &= ~BIT0;                    //Sets P1.0 as an I/O pin
    P1DIR |= BIT0;                      //Sets P1.0 as an output

    // Button setup
    P1SEL0 &= ~BIT1;                    //Sets P1.0 as an I/O pin
    P1SEL1 &= ~BIT1;                    //Sets P1.0 as an I/O pin
    P1DIR &= ~BIT1;                     // button is an input
    P1OUT |= BIT1;                      // pull-up resistor
    P1REN |= BIT1;                      // resistor enabled

    //Clock Setup
    TB0CTL = TBSSEL_1 +  ID_3 + MC_1 + TBCLR + TBIE;    //ACLK, /8, UP Mode, clear TBR, enable interrupt

    //Interrupt flag timing frequency setup
    TB0CCR0 = setFreq(10);              //LED 1 interrupt set for 2Hz

    //Interrupt Setup
    P1IFG &= ~BIT1;                     //No Interrupt Pending
    P1IE |= BIT1;                       //interrupt is enabled
    P1IES |= BIT1;                      //interrupt is set with high-to-low transition

    _BIS_SR(LPM3_bits + GIE);           //Enter Low Power Mode
}

//interrupt service routine
#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1(void)
{
    P1OUT ^= BIT0;
    TB0CTL = TBSSEL_1 + ID_3 + MC_1 + TBCLR + TBIE;  // ACLK, /8, up, clear TBR, enable interrupt
    TB0CTL = TBCLR;
    P1IFG &= ~BIT1;                     // Clear the P1.1 interrupt
    P1IE |= BIT1;                       //interrupt is enabled
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
    P1IE &= ~BIT1;                       //interrupt is disabled
    TB0CCR0 = setFreq(8);              //LED 1 interrupt set for 8Hz
    TB0CTL = TBSSEL_1 + ID_3 + MC_1 + TBCLR + TBIE;  // ACLK, /8, up, clear TBR, enable interrupt

}
