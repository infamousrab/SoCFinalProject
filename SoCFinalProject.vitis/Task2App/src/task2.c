/******************************************************************************
* PWM IP test app: Output C4 note (261.63 Hz) at 50% duty cycle indefinitely
******************************************************************************/

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_io.h"
#include "sleep.h"

// Base address for your PWM IP
#define PWM_IP_BASE_ADDR 0x44A10000

// Register offsets
#define PWM_DUTY_REG_OFFSET   0x00
#define PWM_PERIOD_REG_OFFSET 0x04

// AXI Clock in Hz
#define AXI_CLK_HZ 100000000

void set_pwm_config(uint8_t duty_cycle, uint32_t frequency_hz) {
    xil_printf("Configuring PWM...\r\n");
    
    if (frequency_hz == 0) frequency_hz = 1;
    uint32_t period = AXI_CLK_HZ / frequency_hz;

    Xil_Out32(PWM_IP_BASE_ADDR + PWM_DUTY_REG_OFFSET, duty_cycle);
    Xil_Out32(PWM_IP_BASE_ADDR + PWM_PERIOD_REG_OFFSET, period);

    xil_printf("PWM Set: %lu Hz, Duty %u/255\r\n", frequency_hz, duty_cycle);
}

int main() {  
    init_platform();

    xil_printf("PWM Audio Test: C4 Note (261.63 Hz)\r\n");

    uint32_t frequency = 262;   // Close to 261.63 Hz
    uint8_t duty = 128;         // 50% duty cycle

    set_pwm_config(duty, frequency);

    while (1) {
        // Keep the PWM tone playing
        sleep(1);
    }

    cleanup_platform();
    return 0;
}
