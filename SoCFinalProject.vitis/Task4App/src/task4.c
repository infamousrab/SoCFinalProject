#include "xparameters.h"
#include "xil_printf.h"
#include "xil_io.h"
#include "xil_cache.h"
#include "xaxicdma.h"
#include "platform.h"
#include "sleep.h"
#include "mary_melody.c"  // Contains note_C4, note_D4, note_E4, note_G4 arrays and lengths
#include <stdio.h>
#include "xgpio.h"

// === Peripheral Base Addresses & Constants ===
#define CDMA_DEVICE_ID        0
#define BRAM_BASE_ADDR        XPAR_XBRAM_0_BASEADDR
#define BRAM_TEMP_DUTY_ADDR   (BRAM_BASE_ADDR + 0x1000)
#define PWM_BASE_ADDR         XPAR_PWM_IP_0_BASEADDR
#define PWM_DUTY_OFFSET       0x00
#define PWM_PERIOD_OFFSET     0x04
#define AXI_CLOCK_HZ          100000000  // 100 MHz
#define AUDIO_SAMPLE_DELAY_US 125

#define GPIO_ONE             XPAR_XGPIO_0_BASEADDR
#define PONG_BASE            XPAR_MYIP_PONG2_0_BASEADDR
#define MAX_NOTES            4

// === Game Control Bit Masks ===
#define GAME_START           0b10000
#define GAME_STOP            0b00000
#define PADDLE_UP_P2         (1 << 2)
#define PADDLE_DOWN_P2       (1 << 3)

// === Game Dimensions and Ball Reset ===
#define X_MAX                39
#define Y_MAX                29
#define BALL_RESET_X         20

// === Global Variables ===
XGpio Gpio1;
int gameRunning = 0;
uint8_t score_p1 = 0;
uint8_t score_p2 = 0;

// === Note Struct and Table ===
typedef struct {
    const char* name;
    uint32_t freq;
    const uint8_t* data;
    uint32_t length;
    uint32_t bram_offset;
    uint32_t bram_period_addr;
} NoteEntry;

NoteEntry noteTable[MAX_NOTES] = {
    { "C4", 262, note_C4, note_C4_len, 0, 0 },
    { "D4", 294, note_D4, note_D4_len, 0, 0 },
    { "E4", 330, note_E4, note_E4_len, 0, 0 },
    { "G4", 392, note_G4, note_G4_len, 0, 0 }
};

// === Load Notes into BRAM ===
uint32_t loadNotesToBRAM(uint32_t bram_base) {
    uint32_t offset = 0;

    for (int i = 0; i < MAX_NOTES; ++i) {
        noteTable[i].bram_offset = offset;

        for (uint32_t j = 0; j < noteTable[i].length; ++j)
            Xil_Out8(bram_base + offset + j, noteTable[i].data[j]);

        offset += noteTable[i].length;

        noteTable[i].bram_period_addr = bram_base + offset;
        uint32_t period = AXI_CLOCK_HZ / noteTable[i].freq;
        Xil_Out32(noteTable[i].bram_period_addr, period);
        offset += sizeof(uint32_t);
    }

    return offset;
}

// === Play a Note Using CDMA Transfers ===
void playNote(const char* name, XAxiCdma* cdma, uint32_t bram_base, uint32_t pwm_base) {
    for (int i = 0; i < MAX_NOTES; ++i) {
        if (strcmp(noteTable[i].name, name) == 0) {
            XAxiCdma_SimpleTransfer(cdma,
                (UINTPTR)noteTable[i].bram_period_addr,
                (UINTPTR)(pwm_base + PWM_PERIOD_OFFSET),
                sizeof(uint32_t), NULL, NULL);
            while (XAxiCdma_IsBusy(cdma));

            for (uint32_t j = 0; j < noteTable[i].length; ++j) {
                uint8_t sample = Xil_In8(bram_base + noteTable[i].bram_offset + j);
                Xil_Out32(BRAM_TEMP_DUTY_ADDR, (uint32_t)sample);

                XAxiCdma_SimpleTransfer(cdma,
                    (UINTPTR)BRAM_TEMP_DUTY_ADDR,
                    (UINTPTR)(pwm_base + PWM_DUTY_OFFSET),
                    sizeof(uint32_t), NULL, NULL);
                while (XAxiCdma_IsBusy(cdma));

                usleep(AUDIO_SAMPLE_DELAY_US);
            }

            // Stop tone after playback
            Xil_Out32(BRAM_TEMP_DUTY_ADDR, 0);
            XAxiCdma_SimpleTransfer(cdma,
                (UINTPTR)BRAM_TEMP_DUTY_ADDR,
                (UINTPTR)(pwm_base + PWM_DUTY_OFFSET),
                sizeof(uint32_t), NULL, NULL);
            while (XAxiCdma_IsBusy(cdma));

            return;
        }
    }

    xil_printf("Note %s not found\r\n", name);
}

// === Play "Mary Had a Little Lamb" Melody ===
void play_mary(XAxiCdma* cdma) {
    playNote("E4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("D4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("C4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("D4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("E4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("E4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("E4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    usleep(200000);

    playNote("D4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("D4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("D4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    usleep(200000);

    playNote("E4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("G4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("G4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    usleep(200000);

    playNote("E4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("D4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("C4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("D4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("E4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("E4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("E4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);

    playNote("C4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("D4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("D4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("E4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("D4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
    playNote("C4", cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
}

// === Initialize GPIO ===
int system_init(void) {
    if (XGpio_Initialize(&Gpio1, 0) != XST_SUCCESS) return XST_FAILURE;
    if (XGpio_SelfTest(&Gpio1) != XST_SUCCESS) return XST_FAILURE;
    return XST_SUCCESS;
}

// === Main Application ===
int main() {
    init_platform();
    xil_printf("Game booting\r\n");

    if (system_init() != XST_SUCCESS) return -1;

    XAxiCdma cdma;
    XAxiCdma_Config* cfg = XAxiCdma_LookupConfig(CDMA_DEVICE_ID);
    if (!cfg || XAxiCdma_CfgInitialize(&cdma, cfg, cfg->BaseAddress) != XST_SUCCESS) {
        xil_printf("CDMA initialization failed.\r\n");
        return -1;
    }

    loadNotesToBRAM(BRAM_BASE_ADDR);  // Load audio waveforms and periods to BRAM

    uint8_t prev_ball_x = 0;
    uint8_t prev_ball_dir_x = 0;
    int ball_was_reset = 0;

    while (1) {
        // Wait for game to start
        while (gameRunning == 0) {
            if (XGpio_DiscreteRead(&Gpio1, 1) == 0b11) {
                gameRunning = 1;
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

        // Basic AI for Player 2 paddle movement
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

        // Audio Feedback for Hits
        if (ball_x != BALL_RESET_X) {
            ball_was_reset = 0;

            if (ball_dir_x == 1 && prev_ball_dir_x == 0) {
                xil_printf("Player 1 paddle hit\r\n");
                playNote("C4", &cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
            } else if (ball_dir_x == 0 && prev_ball_dir_x == 1 && ball_x > BALL_RESET_X) {
                xil_printf("Player 2 paddle hit\r\n");
                playNote("C4", &cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
            }
        } else if (!ball_was_reset && (prev_ball_x == 1 || prev_ball_x == X_MAX - 1)) {
            ball_was_reset = 1;

            if (prev_ball_dir_x == 0) {
                xil_printf("Player 2 scored a point!\r\n");
                score_p2++;
                if (score_p2 == 9) {
                    score_p2 = 0;
                    xil_printf("Player 2 wins the game!\r\n");
                    gameRunning = 0;
                    Xil_Out32(PONG_BASE, GAME_STOP);
                    play_mary(&cdma);
                } else {
                    playNote("G4", &cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
                }
            } else if (prev_ball_dir_x == 1) {
                xil_printf("Player 1 scored a point\r\n");
                score_p1++;
                if (score_p1 == 9) {
                    score_p1 = 0;
                    xil_printf("Player 1 wins the game!\r\n");
                    gameRunning = 0;
                    Xil_Out32(PONG_BASE, GAME_STOP);
                    play_mary(&cdma);
                } else {
                    playNote("G4", &cdma, BRAM_BASE_ADDR, PWM_BASE_ADDR);
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
