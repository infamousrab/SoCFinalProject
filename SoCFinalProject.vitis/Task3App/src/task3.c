#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xil_io.h"
#include "sleep.h"

#include "paddle_hit_tone.c"
#include "point_scored_tone.c"
#include "mary_melody.c"

#define GPIO_ONE XPAR_XGPIO_0_BASEADDR
#define PONG_BASE XPAR_MYIP_PONG2_0_BASEADDR
#define PWM_BASE  0x44A10000

#define PWM_DUTY_OFFSET   0x00
#define PWM_PERIOD_OFFSET 0x04
#define AXI_CLK_HZ 100000000
#define AUDIO_SAMPLE_DELAY_US 125

#define GAME_START 0b10000
#define GAME_STOP 0b00000
#define PADDLE_UP_P2 (1 << 2)
#define PADDLE_DOWN_P2 (1 << 3)

#define X_MAX 39
#define Y_MAX 29
#define BALL_RESET_X 20

XGpio Gpio1;

int gameRunning = 0;

uint8_t score_p1 = 0;
uint8_t score_p2 = 0;

void set_pwm_config(uint32_t freq) {
    uint32_t period = AXI_CLK_HZ / (freq ? freq : 1);
    Xil_Out32(PWM_BASE + PWM_PERIOD_OFFSET, period);
}

void play_audio(const uint8_t* data, uint32_t length, uint32_t freq) {
    set_pwm_config(freq);
    for (uint32_t i = 0; i < length; ++i) {
        Xil_Out32(PWM_BASE + PWM_DUTY_OFFSET, data[i]);
        usleep(AUDIO_SAMPLE_DELAY_US);
    }
    Xil_Out32(PWM_BASE + PWM_DUTY_OFFSET, 0);
}

void play_mary(){
    play_audio(note_E4, sizeof(note_E4), 330);
    play_audio(note_D4, sizeof(note_D4), 294);
    play_audio(note_C4, sizeof(note_C4), 262);
    play_audio(note_D4, sizeof(note_D4), 294);
    play_audio(note_E4, sizeof(note_E4), 330);
    play_audio(note_E4, sizeof(note_E4), 330);
    play_audio(note_E4, sizeof(note_E4), 330);
    play_audio(note_SILENCE, sizeof(note_SILENCE), 1);
    play_audio(note_D4, sizeof(note_D4), 294);
    play_audio(note_D4, sizeof(note_D4), 294);
    play_audio(note_D4, sizeof(note_D4), 294);
    play_audio(note_SILENCE, sizeof(note_SILENCE), 1);
    play_audio(note_E4, sizeof(note_E4), 330);
    play_audio(note_G4, sizeof(note_G4), 392);
    play_audio(note_G4, sizeof(note_G4), 392);                    
    play_audio(note_SILENCE, sizeof(note_SILENCE), 1);
    play_audio(note_E4, sizeof(note_E4), 330);
    play_audio(note_D4, sizeof(note_D4), 294);
    play_audio(note_C4, sizeof(note_C4), 262);
    play_audio(note_D4, sizeof(note_D4), 294);
    play_audio(note_E4, sizeof(note_E4), 330);
    play_audio(note_E4, sizeof(note_E4), 330);
    play_audio(note_E4, sizeof(note_E4), 330);
    play_audio(note_C4, sizeof(note_C4), 262);
    play_audio(note_D4, sizeof(note_D4), 294);
    play_audio(note_D4, sizeof(note_D4), 294);
    play_audio(note_E4, sizeof(note_E4), 330);
    play_audio(note_D4, sizeof(note_D4), 294);
    play_audio(note_C4, sizeof(note_C4), 262);
    play_audio(note_SILENCE, sizeof(note_SILENCE), 1);
}

int system_init(void) {
    if (XGpio_Initialize(&Gpio1, 0) != XST_SUCCESS) return XST_FAILURE;
    if (XGpio_SelfTest(&Gpio1) != XST_SUCCESS) return XST_FAILURE;
    return XST_SUCCESS;
}

int main() {
    init_platform();
    xil_printf("Pong Audio Logic Started\r\n");

    if (system_init() != XST_SUCCESS) return -1;

    uint8_t prev_ball_x = 0;
    uint8_t prev_ball_dir_x = 0;
    int ball_was_reset = 0;

    while (1) {
        while(gameRunning==0){
            if(XGpio_DiscreteRead(&Gpio1, 1) == 0b11){
                gameRunning=1;
                Xil_Out32(PONG_BASE, GAME_START);
            }
        }



        uint32_t output = GAME_START;
        uint32_t paddle_pos_p1 = XGpio_DiscreteRead(&Gpio1, 1);
        output |= paddle_pos_p1;

        uint32_t reg1 = Xil_In32(PONG_BASE + 4);
        uint32_t reg2 = Xil_In32(PONG_BASE + 8);
        uint32_t reg3 = Xil_In32(PONG_BASE + 12);

        uint16_t paddle_pos_p2 = reg1;
        uint16_t ball_x = reg2 >> 1;
        uint16_t ball_dir_x = reg2 & 1;
        uint16_t ball_y = reg3 >> 1;
        uint16_t ball_dir_y = reg3 & 1;

        //xil_printf("Ball X: %d, Dir X: %d\r\n", ball_x, ball_dir_x);

        // AI Control
        static int calc_done = 0;
        static uint8_t ideal_pos_p2 = 0;
        if (ball_dir_x && !calc_done) {
            calc_done = 1;
            int dist_x = X_MAX - ball_x;
            int magnitude = (!ball_dir_y) ? ((dist_x - ball_y) % Y_MAX)
                                          : ((ball_y + dist_x) % Y_MAX);
            ideal_pos_p2 = Y_MAX - magnitude;
        } else if (!ball_dir_x) {
            calc_done = 0;
        }

        if ((paddle_pos_p2 + 1) < ideal_pos_p2) output |= PADDLE_UP_P2;
        else if ((paddle_pos_p2 + 1) > ideal_pos_p2) output |= PADDLE_DOWN_P2;

        Xil_Out32(PONG_BASE, output);

        if (ball_x != BALL_RESET_X) {
            ball_was_reset = 0;

            if (ball_dir_x == 1 && prev_ball_dir_x == 0) {
                xil_printf("Player 1 paddle hit\r\n");
                play_audio(paddle_hit_tone, sizeof(paddle_hit_tone), 261);
            } else if (ball_dir_x == 0 && prev_ball_dir_x == 1 && ball_x > BALL_RESET_X) {
                xil_printf("Player 2 paddle hit\r\n");
                play_audio(paddle_hit_tone, sizeof(paddle_hit_tone), 261);
            }
        } else if (!ball_was_reset && (prev_ball_x == 1 || prev_ball_x == X_MAX - 1)) {
            ball_was_reset = 1;

            if (prev_ball_dir_x == 0) {
                xil_printf("Player 2 scored a point!\r\n");
                score_p2++;
                if(score_p2 == 9){
                    score_p2=0;
                    xil_printf("Player 2 wins the game!\r\n");
                    gameRunning=0;
                    Xil_Out32(PONG_BASE, GAME_STOP);
                    play_mary();                    
                }else{
                    play_audio(point_scored_tone, sizeof(point_scored_tone), 392);
                }
                
            } else if (prev_ball_dir_x == 1) {
                xil_printf("Player 1 scored a point\r\n");
                score_p1++;
                if(score_p1 == 9){
                    score_p1=0;
                    xil_printf("Player 1 wins the game!\r\n");
                    gameRunning=0;
                    Xil_Out32(PONG_BASE, GAME_STOP);
                    play_mary();

                }else{
                    play_audio(point_scored_tone, sizeof(point_scored_tone), 392);
                }
                
            }
        }

        prev_ball_x = ball_x;
        prev_ball_dir_x = ball_dir_x;
        usleep(1000);
    }

    cleanup_platform();
    return 0;
}
