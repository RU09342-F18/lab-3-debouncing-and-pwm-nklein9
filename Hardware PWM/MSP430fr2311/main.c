/*Nicholas Klein
 *Created 10/27/18   Last Edit: 10/27/18
 *Hardware PWM on an MSP430FR2311
 */

#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    // LED Setup
    P1SEL0 &= ~BIT7;                    // Connects  P1.7 to the clock
    P1SEL1 |= BIT7;                     // Connects  P1.7 to the clock
    P2SEL1 &= ~BIT0;                    // Sets P2.0 as an I/O pin
    P2SEL0 &= ~BIT0;                    // Sets P2.0 as an I/O pin
    P1DIR |= BIT7;                      // Sets P1.0 as an output
    P2DIR |= BIT0;                      // Sets P2.0as an output
    P1OUT &= ~BIT7;                     // Turn off LED
    P2OUT &= ~BIT0;                     // Turn off LED

    // Button setup
    P1SEL0 &= ~BIT1;                    // Sets P1.1 as an I/O pin
    P1SEL1 &= ~BIT1;                    // Sets P1.1 as an I/O pin
    P1DIR &= ~BIT1;                     // button is an input
    P1OUT |= BIT1;                      // pull-up resistor
    P1REN |= BIT1;                      // resistor enabled

    // Interrupt setup
    P1IFG &= ~BIT1;                     //No Interrupt Pending
    P1IE |= BIT1;                       //interrupt is enabled
    P1IES |= BIT1;                      //interrupt is set with high-to-low transition


    // Clock Setup
    TB0CTL = TBSSEL_2 + ID_2 + MC_1 + TBCLR + TBIE; // SMCLK, /4, up mode, clear TBR, enable interrupt

    // Timer1 setup
    TB0CCR0 = 255;

    // Timer2 setup- Start at 50% brightness
    TB0CCR1 = 125;
    TB0CCTL1 = OUTMOD_7;                //Reset/Set mode

    _BIS_SR(LPM0_bits + GIE);
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    if (P1IES & BIT1) {                 //button press
        TB0CCR1 += 25;
        if (TB0CCR1 > 255) {
            TB0CCR1 = 0;                //0% duty cycle
        }
        }

    P2OUT ^= BIT0;                      // toggle P2.0
    P1IES ^= BIT1;                      // toggle P1.1 transition mode
    P1IFG &= ~BIT1;                     // Clear interrupt
}
