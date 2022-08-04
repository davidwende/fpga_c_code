#include "xil_io.h"
//#include "xgpiops.h"
#include "constants.h"
#include <stdio.h>

void init_dac_a(unsigned int offset);
void init_dac_b(unsigned int offset, unsigned int clkdelay);
void adc_delay (unsigned int, unsigned int, unsigned int, unsigned int);
void adc_data_delay (unsigned int);
/* void rst_bitslip_cnt(void); */
unsigned int check_correct1(unsigned int,unsigned int);
unsigned int check_correct2( unsigned int ,unsigned int , unsigned int  );
unsigned int test_good_combo(unsigned int ,unsigned int);
unsigned int check_adc_ramp(unsigned int , unsigned int , unsigned int );



unsigned int awg_vector[] =
{912, 911, 911, 911, 911, 910, 910, 909, 908, 907, 907, 906, 904,
903, 902, 900, 899, 897, 896, 894, 892, 890, 888, 886, 883, 881,
878, 876, 873, 871, 868, 865, 862, 859, 856, 852, 849, 846, 842,
838, 835, 831, 827, 823, 819, 815, 811, 807, 803, 798, 794, 789,
785, 780, 775, 771, 766, 761, 756, 751, 746, 741, 736, 730, 725,
720, 714, 709, 703, 698, 692, 686, 681, 675, 669, 663, 658, 652,
646, 640, 634, 628, 622, 616, 610, 604, 597, 591, 585, 579, 573,
566, 560, 554, 548, 541, 535, 529, 523, 516, 510, 504, 497, 491,
485, 478, 472, 466, 460, 453, 447, 441, 435, 429, 422, 416, 410,
404, 398, 392, 386, 380, 374, 368, 362, 357, 351, 345, 339, 334,
328, 323, 317, 312, 306, 301, 295, 290, 285, 280, 275, 270, 265,
260, 255, 250, 245, 241, 236, 231, 227, 223, 218, 214, 210, 206,
202, 198, 194, 190, 186, 183, 179, 176, 172, 169, 166, 163, 160,
157, 154, 151, 148, 146, 143, 141, 139, 136, 134, 132, 130, 128,
127, 125, 123, 122, 121, 119, 118, 117, 116, 115, 114, 114, 113,
113, 112, 112, 112, 112, 112, 112, 112, 112, 113, 113, 114, 114,
115, 116, 117, 118, 119, 121, 122, 123, 125, 127, 128, 130, 132,
134, 136, 139, 141, 143, 146, 148, 151, 154, 157, 160, 163, 166,
169, 172, 176, 179, 183, 186, 190, 194, 198, 202, 206, 210, 214,
218, 223, 227, 231, 236, 241, 245, 250, 255, 260, 265, 270, 275,
280, 285, 290, 295, 301, 306, 311, 317, 323, 328, 334, 339, 345,
351, 357, 362, 368, 374, 380, 386, 392, 398, 404, 410, 416, 422,
429, 435, 441, 447, 453, 460, 466, 472, 478, 485, 491, 497, 504,
510, 516, 523, 529, 535, 541, 548, 554, 560, 566, 573, 579, 585,
591, 597, 604, 610, 616, 622, 628, 634, 640, 646, 652, 658, 663,
669, 675, 681, 686, 692, 698, 703, 709, 714, 720, 725, 730, 736,
741, 746, 751, 756, 761, 766, 771, 775, 780, 785, 789, 794, 798,
803, 807, 811, 815, 819, 823, 827, 831, 835, 838, 842, 846, 849,
852, 856, 859, 862, 865, 868, 871, 873, 876, 878, 881, 883, 886,
888, 890, 892, 894, 896, 897, 899, 900, 902, 903, 904, 906, 907,
907, 908, 909, 910, 910, 911, 911, 911, 911, 912};

#include "xgpiops.h"
extern XGpioPs myGpio;

// Inits


void init_adc ()
{
/* 10 bit mode

reset - write 1 to 00
10 bits, write 1000 - 0001 - 0000 - 0000 to 0x46

	go = control[31]
	number bytes = control[1:0]
	data = data
*/

	// reg 0x0
	Xil_Out32(R_ADC_SPIDATA, 0x00000001); // 0x46/b1 = reset
	Xil_Out32(R_ADC_SPICONTROL, 0x00000002); // go and count
	Xil_Out32(R_ADC_SPICONTROL, 0x80000002); // go and count
	Xil_Out32(R_ADC_SPICONTROL, 0x00000002); // go and count

	usleep(5000);

	// interleaving
//	Xil_Out32(R_ADC_SPIDATA, 0x00070001); // 0x07/b1 = interleaving
//	Xil_Out32(R_ADC_SPICONTROL, 0x00000002); // go and count
//	Xil_Out32(R_ADC_SPICONTROL, 0x80000002); // go and count
//	Xil_Out32(R_ADC_SPICONTROL, 0x00000002); // go and count

	// reg 0x42 phase ddr
//	Xil_Out32(R_ADC_SPIDATA, 0x00428000); // 0x42/b15 = 1
//	Xil_Out32(R_ADC_SPICONTROL, 0x00000002); // go and count
//	Xil_Out32(R_ADC_SPICONTROL, 0x80000002); // go and count
//	Xil_Out32(R_ADC_SPICONTROL, 0x00000002); // go and count


	// reg 0x46
	// 1000 0001 0000 0000
	Xil_Out32(R_ADC_SPIDATA, 0x00468104); // 10 bit encoding, two's compl
//	Xil_Out32(R_ADC_SPIDATA, 0x00468100); // 10 bit encoding, binary offset

	Xil_Out32(R_ADC_SPICONTROL, 0x00000002); // go and count
	Xil_Out32(R_ADC_SPICONTROL, 0x80000002); // go and count
	Xil_Out32(R_ADC_SPICONTROL, 0x00000002); // go and count
	usleep(5000);

	xil_printf("-----ADC init done-----\r\n");
}


void init_pm_addr ()
{
    // initialize PM max addresses
	Xil_Out32(R_PM0_CONTROL , 0xC3FFC3FF);
	Xil_Out32(R_PM1_CONTROL , 0xC3FFC3FF);
	Xil_Out32(R_PM2_CONTROL , 0xC1FFC1FF);
	xil_printf("-----PM Max addresses initialized-----\r\n");
}

void init_pm_dac ()
{
	// config0
	// U25
	init_dac_a(R_PM0_SPICONTROL);
	// U6
	init_dac_a(R_PM1_SPICONTROL);
	// U45
	init_dac_a(R_PM2_SPICONTROL);


	// config3
	init_dac_b(R_PM0_SPICONTROL, 3); // U25
	init_dac_b(R_PM1_SPICONTROL, 5); // U6
	init_dac_b(R_PM2_SPICONTROL, 3); // U45

	xil_printf("-----PM DAC setup & timing initialized-----\r\n");
}

void init_galvo()
{
	// write 12 to H_MAX and 5 to V_MAX
	Xil_Out32(R_GALVO_CONTROL, 0x00FF03FF);
}

void laser_edfa_off(){
	Xil_Out32(R_VGA, 0x0);
	XGpioPs_WritePin(&myGpio, 0, 1);
}
void rst_bitslip_cnt()
{
	Xil_Out32(R_DEBUG , 1 << b_DBG_BS_RST);
	Xil_Out32(R_DEBUG , 0x0);
}


void rst_adc(unsigned int value)
{
	Xil_Out32(R_DEBUG , value);
	Xil_Out32(R_DEBUG , 0);
}


unsigned int get_fpga_version()
{
	return Xil_In32(R_CONTROL);
}

void tune_pm_u45()
{
	unsigned int i;

	for (i=0; i<8; i++)
	{
		xil_printf("--setting DAC clk delay to %d \r\n", i);

		init_dac_b(0x20, i);
		sleep(2);
	}
}

void init_dac_a(unsigned int base)
{
	//
	// syncrx_ena = 0
	// alighrx_ena = 0
	// sif4_ena = 1
	// twos = 0
	Xil_Out32(base + 4, 0x000042E4);
	Xil_Out32(base, 0x00000002);
	Xil_Out32(base, 0x80000002);
	Xil_Out32(base, 0x00000002);

	usleep(5000);
}

void reset_fft(){
	Xil_Out32(R_DEBUG, 1 << b_DBG_FFT_RST);
	Xil_Out32(R_DEBUG, 0);
}


void config_fft(){
	Xil_Out32(R_CONTROL, 1 << b_CONFIG_FFT);
	Xil_Out32(R_CONTROL, 0);
}

void init_dac_b(unsigned int base, unsigned int clkdelay)
{
	unsigned int value;

	// config3 write 000 101 000 101 0 00 0
	// clock delay datadlya 15:13,clkdlya 12:10
	// clock delay datadlyb 9:7,  clkdlyb 6:4

	value = (clkdelay << 4) | (clkdelay << 10) | 0x00030000;
	Xil_Out32(base + 4, value);
	Xil_Out32(base, 0x00000002);
	Xil_Out32(base, 0x80000002);
	Xil_Out32(base, 0x00000002);
	usleep(5000);
}

void set_debug_pins(unsigned int d1, unsigned d0)
{
	// b18 is d0 (B35_L25)
	// b19 is d1 (B35_L0)

	unsigned int value;
	value = d0 << 18 | d1 << 19;

	Xil_Out32(0x43C60000 + 14*4, value);
}
void with_window()
{
	Xil_Out32(R_DEBUG, 1 << b_DBG_WINDOW_NORMAL);
	Xil_Out32(R_DEBUG, 0);
}

void without_window()
{
	Xil_Out32(R_DEBUG, 1 << b_DBG_WINDOW_CONSTANT);
	Xil_Out32(R_DEBUG, 0);
}

void no_moving_average()
{
	Xil_Out32(R_DEBUG, 1 << b_DBG_AVERAGE_CLR);
}
void moving_average()
{
	Xil_Out32(R_DEBUG, 1 << b_DBG_AVERAGE_SET);
}


void sample_go()
{
	xil_printf("-----Doing Sample Go-----\r\n");

	Xil_Out32(R_CONTROL, 1 << b_HALT);
	Xil_Out32(R_CONTROL, 1 << b_RUN);
	Xil_Out32(R_CONTROL, 0);
}

void reset_fifo()
{
	Xil_Out32(R_DEBUG, 1 << b_DBG_FIFO_RST);
	Xil_Out32(R_DEBUG, 0);
}

void sample_halt()
{
	xil_printf("-----Doing Sample Halt-----\r\n");

	Xil_Out32(R_CONTROL, 1 << b_HALT);
	Xil_Out32(R_CONTROL, 0);
}

void capture_debug(unsigned int channel)
{
	//xil_printf("-----Doing Capture-----\r\n");

	Xil_Out32(R_DEBUG, 0);
	Xil_Out32(R_DEBUG, (1 << b_DBG_CAPTURE) | (channel << b_DBG_MUX));
//	xil_printf("\n\r to debug mux %X\r\n", (1 << b_CAPTURE) | (channel << b_DBG_MUX));
	Xil_Out32(R_DEBUG, 0);
}

unsigned int get_bs_count(){
	unsigned int status;
	status = Xil_In32(R_DEBUG);
	return (0xFF00 & status) >> 8;
}

unsigned int get_fft_events(){
// ignores bits 1 and 6
	unsigned int status;
	status = Xil_In32(R_CONTROL);
	return 0x3D & status;
}

void set_selectio(unsigned int channel, unsigned int tap)
{
	unsigned int temp;
	xil_printf("\n\rtap:%d ", tap);

	temp =  tap | (channel << b_CTRL_CHANNEL);
	Xil_Out32(R_CONTROL, temp | (1 << b_CTRL_LOAD));
	Xil_Out32(R_CONTROL, temp);
	usleep(10000);
}
void tune_selectio(unsigned int (*func)(unsigned int ,unsigned int  ,unsigned int ) )
{
	unsigned int  i, bs;
unsigned int result1, result2,result3,result4;
unsigned int temp, CH;
//	set adc to some reasonable value

	adc_delay(0,1,3,1);

	// frame channel
	temp =  18 | (8 << b_CTRL_CHANNEL);
	Xil_Out32(R_CONTROL, temp | (1 << b_CTRL_LOAD));
	Xil_Out32(R_CONTROL, temp);
	usleep(10000);


	//step over 32 decs
	for (i = 0; i < 32; i++)
	{
		CH=3;
		temp =  i | (CH << b_CTRL_CHANNEL);
		Xil_Out32(R_CONTROL, temp | (1 << b_CTRL_LOAD));
		Xil_Out32(R_CONTROL, temp);

		usleep(1000);
	    rst_bitslip_cnt();
		usleep(100000);
		// now read bit slip count
		bs = get_bs_count();
		xil_printf("\n\ri= %d,Control = %X, bs= %d, Channel %d => ", i,temp, bs, CH);
		result1 = (*func)(C1, C2, CH);
		result2 = (*func)(C1, C2, CH);
		result3 = (*func)(C1, C2, CH);
		result4 = (*func)(C1, C2, CH);
//		CH=1;
//		xil_printf("\n\ri= %d,Control = %X, bs= %d, Channel %d => ", i,temp, bs, CH);
//		result1 = (*func)(C1, C2, CH);
//		result2 = (*func)(C1, C2, CH);
//		result3 = (*func)(C1, C2, CH);
//		result4 = (*func)(C1, C2, CH);

		if (result1 & result2 & result3 & result4)
			xil_printf("\n\rGood");
	}

}

void tune_single_channel(unsigned int (*func)(unsigned int ,unsigned int  ,unsigned int ), unsigned int CH )
{
	unsigned int r,f,dr,df, bs, i, l;
	unsigned int result,LOOPS;
	r=0;
	f=0;
	LOOPS=4;

//	for (i = 15; i < 25; i++)
//	{
//		xil_printf("\n\r");
//
//		set_selectio(CH, i);
		for (r = 0; r < 8; r++)
			for (f = 0; f < 8; f++)
				for (dr = 0; dr < 4; dr++)
					for (df = 0; df < 4; df++)
					{
//						xil_printf("\n\r");
						adc_delay(r,f, dr, df);
						usleep(20000);
						rst_bitslip_cnt();
						usleep(100000);
						// now read bit slip count
						bs = get_bs_count();
						if (bs == 0)
							{
							// add some delay and recheck
							sleep(1);
							bs = get_bs_count();
							}
						else
							xil_printf("_");
			if (bs == 0){
				result=0;
				xil_printf("\n\rS %d %d %d %d : ", r,f, dr, df);
				for (l=0; l<LOOPS; l++)
					result += (*func)(C1, C2, CH);

				if (result==LOOPS)
					xil_printf("\n\rBS=0: %d %d %d %d \r\n",r,f, dr, df);
			} // if bs
				} // for
//	} // for
} // function

void test_good_combos(unsigned int funct_type, unsigned int CH )
{
//	unsigned int result;

	set_selectio(CH,18);adc_delay( 0,0,1,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 0,0,3,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 0,2,3,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 0,4,2,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 0,4,2,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 0,4,3,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 1,0,3,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 1,3,0,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 1,3,1,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 1,4,2,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 1,5,0,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 1,5,1,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 1,6,3,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 2,0,2,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 2,1,2,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 2,5,1,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 2,6,1,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 2,7,2,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 2,7,2,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 3,0,3,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 4,0,3,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 4,3,3,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 4,6,2,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,18);adc_delay( 4,6,3,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 0,1,0,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 0,2,1,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 0,4,0,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 0,4,2,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 0,5,3,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 1,0,1,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 1,2,2,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 1,3,0,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 1,4,3,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 1,5,1,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 1,6,2,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 2,0,1,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 2,0,2,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 2,0,3,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 2,5,3,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,19);adc_delay( 3,4,2,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 0,0,1,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 0,1,0,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 0,2,2,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 0,3,0,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 0,3,1,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 0,3,3,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 0,4,2,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 0,7,1,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 0,7,3,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 0,7,3,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 1,1,1,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 1,2,2,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 1,4,0,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 1,6,1,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 2,1,3,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 2,4,0,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 2,4,3,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 2,6,3,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 3,1,1,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 3,4,1,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 3,4,3,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 3,6,2,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 4,1,1,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 4,3,2,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 4,5,3,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 4,5,3,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 5,3,3,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 5,4,3,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 5,5,2,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 5,5,3,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 5,7,2,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 6,1,0,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 6,2,0,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 6,2,3,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 6,3,0,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 6,4,1,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 6,4,2,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 6,5,1,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 7,0,2,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 7,2,1,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 7,3,2,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 7,5,0,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 7,6,2,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 7,6,3,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,20);adc_delay( 7,7,2,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 0,0,3,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 0,5,1,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 0,6,1,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 0,6,3,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 0,7,0,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 0,7,3,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 1,0,0,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 1,0,0,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 1,0,1,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 1,6,1,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 1,6,1,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 1,7,2,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 2,0,0,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 2,4,0,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 3,1,2,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 3,3,0,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 3,5,0,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 3,6,3,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 4,0,0,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 4,0,1,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 4,7,0,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 5,0,2,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 5,2,1,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 5,2,3,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 5,7,2,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 6,1,0,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 6,1,0,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 6,1,1,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 6,1,2,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 6,2,0,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 6,4,0,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 6,6,1,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 7,0,2,0 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 7,0,2,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 7,1,0,2 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 7,1,3,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 7,3,2,3 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
	set_selectio(CH,21);adc_delay( 7,5,1,1 );
	if (1 == test_good_combo(1 ,CH)) xil_printf("\n\rOK");
}

unsigned int test_good_combo(unsigned int func_type ,unsigned int CH)
{
	unsigned int bs, result, LOOPS, l;
	LOOPS=4;
	usleep(20000);
	rst_bitslip_cnt();
	usleep(100000);
	bs = get_bs_count();
	if (bs == 0)
		{
		sleep(1);
		bs = get_bs_count();
		}
	else
		xil_printf("_");

	if (bs == 0){
		result=0;
		for (l=0; l<LOOPS; l++)
			if (1 == func_type)
				result += check_correct2(C1, C2, CH);
			else
				result += check_adc_ramp(C1, C2, CH);

		if (result>=LOOPS-1)
			return 1;
		else
			return 0;
	}
	return 0;
}

void tune_adc_clk_data(unsigned int (*func)(unsigned int ,unsigned int  ,unsigned int ) )
{
unsigned int r,f,dr,df, bs;
unsigned int result1, result2, result3;

xil_printf("Enter tune adc clock & data\n\r");

//8, 8, 4, 4

for (r = 0; r < 8; r++)
	for (f = 0; f < 8; f++)
		for (dr = 0; dr < 4; dr ++)
			for (df = 0; df < 4; df ++)
	{
		adc_delay(r,f, dr, df);
		usleep(20000);
	    rst_bitslip_cnt();
		usleep(100000);
		// now read bit slip count
		bs = get_bs_count();
		if (bs == 0)
			{
			// add some delay and recheck
			sleep(2);
			bs = get_bs_count();
			}
		else
			xil_printf("_");

		// if still 0, then check data
		if (bs == 0){
//			xil_printf("S"); // long bit slip free
			xil_printf("\n\rS %d %d %d %d : ", r,f, dr, df);

			if (1)
				{
				result1 = (*func)(C1, C2, 0);
				if (result1) result2 = (*func)( C1, C2, 1);

				if (result1 && result2) result3 = (*func)(C1, C2, 2);

				if (result1 && result2 && result3)
					xil_printf("\n\r BS=0:Correct = %d, Clock delay rising = %d falling= %d, Data delay: rising = %d, falling = %d \r\n", COUNT,r,f, dr, df);
				}
					} else
						;

				} // for
		}

unsigned int check_correct1(unsigned int v1, unsigned int channel)
{
	unsigned int result, i, address;
	result = 0;
    capture_debug(channel);
    usleep(1000);
    address = M_CAPTURE;
	for (i=0; i<COUNT; i++)
	{
		if (v1 == Xil_In32(address))
			result++;
		address = address+4;
	}
	return result;
}

unsigned int check_correct2(unsigned int v1,unsigned int v2, unsigned int channel)
{
	unsigned int result1, i, address;
	volatile unsigned int readback;
	result1 = 0;
//	result2 = 0;
    capture_debug(channel);
    usleep(50000);
    address = M_CAPTURE;
	for (i=0; i<COUNT; i++)
	{
		readback = Xil_In32(address);

		if ((v1 == readback) || (v2 == readback))
			result1++;

		address = address+4;
	}
	xil_printf(" %d ", result1);

	if (result1 >= COUNT-1){
		xil_printf(".");
		return 1;
	} else {
		xil_printf(" ");
		return 0;
	}
}

unsigned int check_adc_ramp(unsigned int dummy1, unsigned int dummy2, unsigned int channel)
{
    unsigned int i, address, previous;
    volatile unsigned int readback;
    unsigned int success = 0;

    // clear capture memory
 //   for (i=0; i<30; i++)
 //   	Xil_Out32(M_CAPTURE + i*4, 0);

    capture_debug(channel);
    usleep(50000);
    address = M_CAPTURE;
    previous = Xil_In32(address);
//    for (i=0; i<20; i++)
//    {
//        xil_printf("%d\n\r", Xil_In32(address+i*4));
//    }

    address = address+4;
    for (i=0; i<COUNT-1; i++)
    {
        readback = Xil_In32(address);
        if ((1 != (readback - previous)) && (0 != readback))
        ;
        else
        {
        	success += 1;
        }
        previous = readback;
        address = address+4;
    }
//        //xil_printf("%d\n\r", readback);
//        if ((1 != (readback - previous)) && (0 != readback))
//        {
//        	if (i>100)
//        		xil_printf("%d\n\r", i);
//        	return 0;
//        }
//        else
//        {
//            previous = readback;
//            address = address+4;
//        }


            xil_printf("%d\n\r", success);

    if (success > 2046)
    	return 1;
    else
    	return 0;
}

void tune_adc_data()
{
    unsigned i;
    for (i = 0; i < 4; i++)
    {
        xil_printf("Setting data delay to %d \r\n", i);
        adc_data_delay(i);
        usleep(2000);
        rst_bitslip_cnt();
        sleep(3);
        // now read bitslip count
        xil_printf("bitslip count is %d \r\n", get_bs_count());
    }
}

void disable(unsigned int value)
{
    Xil_Out32(0x43C60000 + 4*15, value);
}

void tune_adc()
{
    unsigned r,f,dr,df;
    r = 0; f = 7; dr = 1; df = 0;
    xil_printf("\n\rSetting clock delay to rising = %d falling= %d, dd r %d, dd f %d \r\n", r,f, dr, df);
    adc_delay(r,f, dr, df);
    usleep(2000);
    rst_bitslip_cnt();
    sleep(5);
//    status = Xil_In32(0x43C60000);
//    bs = (0xFF00 & status) >> 8;
    xil_printf("bitslip count is %d \r\n", get_bs_count());

    r = 7; f = 7; dr = 1; df = 0;
    xil_printf("\n\rSetting clock delay to rising = %d falling= %d, dd r %d, dd f %d \r\n", r,f, dr, df);
    adc_delay(r,f, dr, df);
    usleep(2000);
    rst_bitslip_cnt();
    sleep(5);
//    status = Xil_In32(R_DEBUG);
//    bs = (0xFF00 & status) >> 8;
    xil_printf("bitslip count is %d \r\n", get_bs_count());


    r = 7; f = 7; dr = 0; df = 1;
    xil_printf("\n\rSetting clock delay to rising = %d falling= %d, dd r %d, dd f %d \r\n", r,f, dr, df);
    adc_delay(r,f, dr, df);
    usleep(2000);
    rst_bitslip_cnt();
    sleep(5);
//    status = Xil_In32(R_DEBUG);
//    bs = (0xFF00 & status) >> 8;
    xil_printf("bitslip count is %d \r\n", get_bs_count());

    r = 7; f = 7; dr = 2; df = 0;
    xil_printf("\n\rSetting clock delay to rising = %d falling= %d, dd r %d, dd f %d \r\n", r,f, dr, df);
    adc_delay(r,f, dr, df);
    usleep(2000);
    rst_bitslip_cnt();
    sleep(5);
//    status = Xil_In32(0x43C60000);
//    bs = (0xFF00 & status) >> 8;
    xil_printf("bitslip count is %d \r\n", get_bs_count());

    r = 7; f = 7; dr = 1; df = 1;
    xil_printf("\n\rSetting clock delay to rising = %d falling= %d, dd r %d, dd f %d \r\n", r,f, dr, df);
    adc_delay(r,f, dr, df);
    usleep(2000);
    rst_bitslip_cnt();
    sleep(5);
 //   status = Xil_In32(0x43C60000);
 //   bs = (0xFF00 & status) >> 8;
    xil_printf("bitslip count is %d \r\n", get_bs_count());
}

void adc_ramp()
{
    unsigned int value;
    xil_printf("Setting ADC to ramp\r\n");
    value = 0x00250000 | (1 << 6);
    Xil_Out32(R_ADC_SPIDATA, value);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x80000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    usleep(2000);
}

void adc_custom1(unsigned int custom1)
{
    unsigned int value;
    xil_printf("Setting ADC to single custom\n\r");
    value = 0x00250000 | (1 << 4)  | ((custom1 & 0x300) >> 8);
    Xil_Out32(R_ADC_SPIDATA, value);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x80000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    usleep(2000);

    value = 0x00260000 | ((custom1 & 0x0FF) << 8);
    Xil_Out32(R_ADC_SPIDATA, value);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x80000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    usleep(2000);
}

void adc_custom2(unsigned int custom1, unsigned int custom2)
{
    unsigned int value;
    xil_printf("Setting ADC to dual custom\n\r");
    value = 0x00250000 | (1 << 5)  | ((custom1 & 0x300) >> 8 | ((custom2 & 0x300) >> 6));
    Xil_Out32(R_ADC_SPIDATA, value);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x80000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    usleep(2000);

    value = 0x00260000 | ((custom1 & 0xFF) << 8 );
    Xil_Out32(R_ADC_SPIDATA, value);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x80000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    usleep(2000);

    value = 0x00270000 | ((custom2 & 0xFF) << 8 );
    Xil_Out32(R_ADC_SPIDATA, value);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x80000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    usleep(2000);
}


void adc_delay (unsigned int r, unsigned int f, unsigned int dr,unsigned int df)
{
    unsigned int value;
    value = 0x00BE8000 | f | (r << 5) | (dr << 8) | (df << 3);
//	xil_printf("delay:%d %d %d %d", r,f,dr,df);

    // reg 0x0
    Xil_Out32(R_ADC_SPIDATA, value);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x80000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    usleep(2000);
}

void adc_data_delay (unsigned int delay)
{
    unsigned int value;
    value = 0x00BE8000 | (delay << 8) | (delay << 3);
    // reg 0x0
    Xil_Out32(R_ADC_SPIDATA, value);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x80000002);
    Xil_Out32(R_ADC_SPICONTROL, 0x00000002);
    usleep(2000);
}

void set_dbg_mux(unsigned int value)
{
	unsigned int v;
	v = 1 << value;
	Xil_Out32(0x43C60000, 0x3000 & (v << 12));
	xil_printf("Setting debug mux to 0x%X \r\n", v);
}

void moving_average_set()
{
//	set 7
//	clr 8
	Xil_Out32(0x43C60000, 1  << 7);
	Xil_Out32(0x43C60000, 0);
}
void moving_average_clr()
{
//	set 7
//	clr 8
	Xil_Out32(0x43C60000, 1  << 8);
	Xil_Out32(0x43C60000, 0);
}


void manual_galvo(unsigned int horiz, unsigned int vert)
{
	unsigned int v;
	xil_printf("Setting galvo manual mode \r\n");

	v = (horiz & 0x7FF) | ((vert & 0x7FF) << 16);
	//Xil_Out32(0x43C60000 + 4*5, 0x00000000 | v);
	Xil_Out32(R_GALVO_MANUAL, 0x80000000 | v);
	//Xil_Out32(0x43C60000 + 4*5, 0x00000000 | v);
}

void get_peaks()
{
	unsigned int i, apeak, aindex;
	unsigned int indices[16], peaks[16];
	unsigned add1, add2;

	apeak = M_PEAKS_IDXS;
	// reset peaks ready
	Xil_Out32(0x43C60000, 1 << 8);
	Xil_Out32(0x43C60000, 0);

	while (0 == (Xil_In32(0x43C60000) & 0x40000000))
		;
    for (i = 0; i < NPEAKS; i++)
    {
    	peaks[i] = Xil_In32(apeak);
    	indices[i] = Xil_In32(aindex);
    	apeak = apeak + 4;
    	aindex = aindex + 4;
    }
	xil_printf("Ch 0: Index     Peak\r\n");
    for (i = 0; i < NPEAKS; i++)
    	xil_printf("      %4d %6d  %5X\r\n",indices[i], peaks[i], peaks[i]);

    add1 = peaks[0] + peaks[1] + peaks[2];
    add2 = peaks[3] + peaks[4] + peaks[5];
	xil_printf("add1 = %d  add2 = %d\r\n",add1,add2);

	printf("Ratio = %5.2e \r\n",(float)add1/(float)add2);
/*
    // now sort by index
    for (k = 0; k < NPEAKS; ++k){
       for (j = k + 1; j < NPEAKS; ++j){
          if (indices[k] < indices[j]){
             a = indices[k];
             aa = peaks[k];
             indices[k] = indices[j];
             peaks[k] = peaks[j];
             indices[j] = a;
             peaks[j] = aa;
          }
       }
    }
	xil_printf("  ==>   Now in order\r\n");

    for (i = 0; i < NPEAKS; i++)
    	xil_printf("      %4d %6d  %5X\r\n",indices[i], peaks[i], peaks[i]);


    add1 = peaks[0] + peaks[1] + peaks[2]+ peaks[3]+ peaks[4];
    add2 = peaks[5] + peaks[6] + peaks[7]+ peaks[8]+ peaks[8];
	xil_printf("add1 = %d  add2 = %d\r\n",add1,add2);

	printf("Ratio = %5.2e \r\n",(float)add1/(float)add2);
*/

//	xil_printf("Ch 1: Index     Peak\r\n");
//    for (i = 8; i < 16; i++)
//    	xil_printf("      %4d %6d  %5X\r\n",indices[i], peaks[i], peaks[i]);
}

void load_awg_vector(unsigned int a){
	unsigned int i, address;

	address = a;
    for (i = 0; i < 1024; i++)
    {
    	Xil_Out32(address, 512);
//    	Xil_Out32(address, awg_vector[i]);

    	address += 4;
    }
    xil_printf("Wrote vector to %X\r\n",a);
}

void load_galvo_vectors(){
	unsigned int i, address;

	address = M_GALVO_H;
    for (i = 0; i < 2048; i++)
    {
    	Xil_Out32(address, i*2);
    	address += 4;
    }
	address = M_GALVO_V;
    for (i = 0; i < 2048; i++)
    {
    	Xil_Out32(address, i*2);
    	address += 4;
    }
}

void enable_fft(){
	Xil_Out32(R_DEBUG, 1 << b_DBG_FFT_EN);
	Xil_Out32(R_DEBUG, 0);
}

void disable_fft(){
	Xil_Out32(R_DEBUG, 1 << b_DBG_FFT_DIS);
	Xil_Out32(R_DEBUG, 0);
}

void wait_peak_ready(){
	// this blocks until peaks are ready
	volatile unsigned int rdy;
	rdy = Xil_In32(R_CONTROL) & b_STS_PEAK_RDY_HLD;
	while((0== rdy))
		rdy = Xil_In32(R_CONTROL) & b_STS_PEAK_RDY_HLD;
	// now reset the hold bit
	Xil_Out32(R_CONTROL, 1 << b_RST_PEAKS_RDY);
}

void wait_start_frame()
{
	while(0 == (Xil_In32(R_CONTROL) & (1<< b_STS_START_FRAME)))
		;
	//	if(0 != (Xil_In32(R_CONTROL) & (1<< b_STS_START_FRAME)))
}
