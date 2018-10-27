/*Nicholas Klein
 *Created 10/27/18   Last Edit: 10/27/18
 *Software PWM on an MSP430F5529
 */

#include <msp430.h> 

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    // LED Setup
    P1SEL &= ~BIT0;                     // Sets P1.0 as an I/O pin
    P4SEL &= ~BIT7;                     // Sets P4.7 as an I/O pin
    P1DIR |= BIT0;                      //Sets P1.0 as an output
    P4DIR |= BIT7;                      //Sets P4.7as an output
    P1OUT &= ~BIT0;                     //Turn off LED
    P4OUT &= ~BIT7;                     //Turn off LED

    // Button setup
    P2SEL &= ~BIT1;                     //Sets P2.1 as an I/O pin
    P2DIR &= ~BIT1;                     // button is an input
    P2OUT |= BIT1;                      // pull-up resistor
    P2REN |= BIT1;                      // resistor enabled

    // Interrupt setup
    P2IFG &= ~BIT1;                     //No Interrupt Pending
    P2IE |= BIT1;                       //interrupt is enabled
    P2IES |= BIT1;                      //interrupt is set with high-to-low transition


    // Clock Setup
    TA0CTL = TASSEL_2 + ID_2 + MC_1 + TACLR + TAIE; // SMCLK, /4, up mode, clear TAR, enable interrupt

    // Timer1 setup
    TA0CCR0 = 255;

    // Timer2 setup- Start at 50% brightness
    TA0CCR1 = 125;
    TA0CCTL1 = CCIE;

    _BIS_SR(LPM0_bits + GIE);
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {
    if (P2IES & BIT1) {                 //button press
        TA0CCR1 += 25;
        if (TA0CCR1 > 255) {
            TA0CCR1 = 0;                //Reset Clock 1
            P1OUT &= ~BIT0;             //Turn off LED
            TA0CTL = TACLR;             // clear TAR
        }
        else {
            TA0CTL = TASSEL_2 + ID_2 + MC_1 + TAIE; // SMCLK, /4, up mode, clear TAR, enable interrupt
        }
    }

    P4OUT ^= BIT7;                      // toggle P4.7
    P2IES ^= BIT1;                      // toggle P2.1 transition mode
    P2IFG &= ~BIT1;                     // Clear interrupt
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1(void) {
    switch (TA0IV) {
    case 2:
        // CCR1 interrupt
        P1OUT &= ~BIT0;                 //Turn off LED
        break;
    case 14:
        // Timer overflow
        P1OUT |= BIT0;                  //Turn on LED
        break;
    }
}
