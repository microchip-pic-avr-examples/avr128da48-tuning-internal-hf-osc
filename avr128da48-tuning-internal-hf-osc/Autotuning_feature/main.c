/**
 * \file main.c
 *
 * \brief Main source file.
 *
 (c) 2020 Microchip Technology Inc. and its subsidiaries.
    Subject to your compliance with these terms, you may use this software and
    any derivatives exclusively with Microchip products. It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

#define F_CPU                                   1000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DELAY_TIME                              1000
#define PRESSED                                 1
#define NOT_PRESSED                             0
#define PULL_UP_ENABLE                          0x08
#define BUTTON_PIN                              PIN7_bm

void CLK_init(void);
void PORT_init(void);

uint8_t volatile button_event = NOT_PRESSED;

int main(void)
{
    cli();
    CLK_init();
    PORT_init();
    sei();

    while (1) 
    {
        if(button_event == PRESSED) 
        {
            cli();
            /* Set OSCHF clock to 1 MHz and enable auto-tune */
            _PROTECTED_WRITE (CLKCTRL.OSCHFCTRLA, (CLKCTRL_FREQSEL_1M_gc | CLKCTRL_AUTOTUNE_bm));
            _delay_ms(DELAY_TIME);
            /* Disable auto-tune and keep the 1 MHz clock */
            _PROTECTED_WRITE (CLKCTRL.OSCHFCTRLA, CLKCTRL_FREQSEL_1M_gc);
            button_event = NOT_PRESSED;
            sei();
        }
    }
}

void CLK_init(void)
{
    /* Set OSCHF clock to 1 MHz */
    _PROTECTED_WRITE (CLKCTRL.OSCHFCTRLA, CLKCTRL_FREQSEL_1M_gc);
    /* Enable external crystal oscillator */
    _PROTECTED_WRITE (CLKCTRL.XOSC32KCTRLA, CLKCTRL_ENABLE_bm);
    /* Set OSCHF as main clock and enable the clock signal output on pin PA7 */
    _PROTECTED_WRITE (CLKCTRL.MCLKCTRLA, (CLKCTRL_CLKSEL_OSCHF_gc) | (CLKCTRL_CLKOUT_bm)); 
}

void PORT_init(void)
{
    /* Set PC7 direction as input */
    PORTC.DIRCLR = BUTTON_PIN;
    /* Clear the interrupt */
    PORTC.INTFLAGS = BUTTON_PIN;
    /* Configure PC7 interrupt as rising and enable the internal pull-up */
    PORTC.PIN7CTRL = PORT_ISC_RISING_gc | PULL_UP_ENABLE;
}

ISR(PORTC_PORT_vect)
{
    /* Set the button state as PRESSED */
    button_event = PRESSED;
    /* Clear the interrupt */
    PORTC.INTFLAGS = BUTTON_PIN;
}
