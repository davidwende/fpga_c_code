
#ifndef __DAVID_H_
#define __DAVID_H_


#define C1 0x335
#define C2 0x273
#define COUNT 2048

// Register addresses
// 0
#define REGS_BASE 0x43D00000
#define R_CONTROL REGS_BASE

// 1, 2, 3
#define R_PM0_CONTROL (REGS_BASE + 0x4)
#define R_PM1_CONTROL (REGS_BASE + 0x8)
#define R_PM2_CONTROL (REGS_BASE + 0xc)

// 4, 5
#define R_GALVO_CONTROL (REGS_BASE + 0x10)
#define R_GALVO_MANUAL  (REGS_BASE + 0x14)

// 6, 7
#define R_ADC_SPICONTROL  (REGS_BASE + 0x18)
#define R_ADC_SPIDATA     (REGS_BASE + 0x1C)

// 8, 9, 10, 11, 12, 13
#define R_PM0_SPICONTROL (REGS_BASE + 0x20)
#define R_PM0_SPIDATA    (REGS_BASE + 0x24)
#define R_PM1_SPICONTROL (REGS_BASE + 0x28)
#define R_PM1_SPIDATA    (REGS_BASE + 0x2C)
#define R_PM2_SPICONTROL (REGS_BASE + 0x30)
#define R_PM2_SPIDATA    (REGS_BASE + 0x34)

// 14, 15
#define R_VGA    (REGS_BASE + 0x38)
#define R_TIMESTAMP    (REGS_BASE + 0x3C)

// 16
#define R_DEBUG   (REGS_BASE + 0x40)

// Control register fields
#define b_RUN 31
#define b_HALT 30
#define b_RST_PEAKS_RDY 29
#define b_CONFIG_FFT 28
#define b_CYCLE_TIME_V 15
#define b_CTRL_CHANNEL 5
#define b_CTRL_LOAD 9

// Status register fields
#define b_STS_PEAK_RDY 31
#define b_STS_PEAK_RDY_HLD 30
#define b_STS_START_FRAME 29

// Debug register fields
#define b_DBG_CLR_ADC_RST 21
#define b_DBG_SET_ADC_RST 20

#define b_DBG_AVERAGE_CLR 19
#define b_DBG_AVERAGE_SET 18
#define b_DBG_WINDOW_NORMAL 17
#define b_DBG_WINDOW_CONSTANT 16
#define b_DBG_MUX 12
#define b_DBG_FFT_EN 10
#define b_DBG_FFT_DIS 9
#define b_DBG_CAPTURE 8
#define b_DBG_FFT_CAPTURE 7
#define b_DBG_FIFO_RST 6
#define b_DBG_FFT_RST 5
#define b_DBG_ADC_RST 3
#define b_DBG_PM_RST 2
#define b_DBG_BS_RST 1
#define b_DBG_FFT_EVT_RST 0

// VGA bit fields
#define b_VGA_DBG0 19
#define b_VGA_DBG1 18


#define NPEAKS 8
#define CHANNELS 1
#define PACKET_SIZE 1000

/* ========= Memory areas =========== */

#define M_GALVO_H 0x43C00000
#define M_GALVO_V 0x43C10000

#define M_PEAKS_IDXS 0x43C90000

#define M_AWG0 0x43C20000
#define M_AWG1 0x43C30000
#define M_AWG2 0x43C40000
#define M_AWG3 0x43C50000
#define M_AWG4 0x43C60000
#define M_AWG5 0x43C70000

#define M_CAPTURE 0x43CB0000
#define M_FFT_CAPTURE 0x43CC0000

#define M_WINDOW 0x43C80000


#endif /* __DAVID_H_ */
