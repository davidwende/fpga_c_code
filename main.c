/*
 * Copyright (C) 2009 - 2019 Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

#include <stdio.h>

#include "xparameters.h"

#include "netif/xadapter.h"

#include "platform.h"
#include "platform_config.h"
#if defined (__arm__) || defined(__aarch64__)
#include "xil_printf.h"
#endif

#include "lwip/tcp.h"
#include "xil_cache.h"

#if LWIP_IPV6==1
#include "lwip/ip.h"
#else
#if LWIP_DHCP==1
#include "lwip/dhcp.h"
#endif
#endif

// added DW
#include "lwipopts.h"
#include "xil_printf.h"
#include "sleep.h"
#include "xgpiops.h"
#include "constants.h"
#include "xgpiops.h"

// prototypes for DW
void init_pm_dac ();
void init_pm_addr ();
void init_adc ();
void sample_go();
void sample_halt();

void print_bitslip_cnt(void);
void rst_bitslip_cnt(void);
void start_application_echo(void);
void capture_debug(void);
void init_galvo(void);
void manual_galvo(unsigned int , unsigned int );
void tune_adc_clk_data(unsigned int (*func)(unsigned int ,unsigned int ,unsigned int));
void tune_selectio(unsigned int (*func)(unsigned int ,unsigned int ,unsigned int));
void tune_single_channel(unsigned int (*func)(unsigned int ,unsigned int  ,unsigned int ), unsigned int );

void tune_adc_data(void);
void adc_delay (unsigned int, unsigned int, unsigned int, unsigned int);
void tune_adc(void);
void tune_pm_u45(void);
void adc_ramp(void);
void set_dbg_mux(unsigned int );
void adc_custom1(unsigned int );
void adc_custom2(unsigned int , unsigned int);
unsigned int check_correct2(unsigned int,unsigned int, unsigned int  );
unsigned int check_adc_ramp(unsigned int , unsigned int, unsigned int);
void disable(unsigned int);
unsigned int get_fpga_version();
void set_debug_pins(unsigned int , unsigned );
void get_peaks();
void with_window();
void without_window();
void laser_edfa_off();
void enable_fft();
void disable_fft();
void set_selectio(unsigned int , unsigned int );
void rst_adc(unsigned int);
void test_good_combos(unsigned int , unsigned int );
void reset_fft();
void config_fft();

unsigned int get_fft_events();
void moving_average_set();
void moving_average_clr();
void reset_fifo();
void load_awg_vector(unsigned int);
void load_galvo_vectors();

/* defined by each RAW mode application */
void print_app_header();
int start_application();
int transfer_data();
void tcp_fasttmr(void);
void tcp_slowtmr(void);
void wait_peak_ready();
void wait_start_frame();

/* missing declaration in lwIP */
void lwip_init();

#if LWIP_IPV6==0
#if LWIP_DHCP==1
extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
#endif
#endif

extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;
static struct netif server_netif;
struct netif *echo_netif;

unsigned int results[PACKET_SIZE*CHANNELS*2];

#if LWIP_IPV6==1
void print_ip6(char *msg, ip_addr_t *ip)
{
	print(msg);
	xil_printf(" %x:%x:%x:%x:%x:%x:%x:%x\n\r",
			IP6_ADDR_BLOCK1(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK2(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK3(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK4(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK5(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK6(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK7(&ip->u_addr.ip6),
			IP6_ADDR_BLOCK8(&ip->u_addr.ip6));

}
#else
void
print_ip(char *msg, ip_addr_t *ip)
{
	print(msg);
	xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip),
			ip4_addr3(ip), ip4_addr4(ip));
}

void
print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
{

	print_ip("Board IP: ", ip);
	print_ip("Netmask : ", mask);
	print_ip("Gateway : ", gw);
}
#endif

#if defined (__arm__) && !defined (ARMR5)
#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
int ProgramSi5324(void);
int ProgramSfpPhy(void);
#endif
#endif

#ifdef XPS_BOARD_ZCU102
#ifdef XPAR_XIICPS_0_DEVICE_ID
int IicPhyReset(void);
#endif
#endif

XGpioPs myGpio;
XGpioPs_Config *ConfigPtr;

int main()
{

//	XGpioPs myGpio;
//	XGpioPs_Config *ConfigPtr;
	int Status;
	unsigned int count = 0;
	unsigned int events, i;
	unsigned int startframe, pixel;
	unsigned int r, peak, idx, side, side_amp, channel;
	volatile unsigned int rdy, rdy2;

#if LWIP_IPV6==0
	ip_addr_t ipaddr, netmask, gw;

#endif
	/* the mac address of the board. this should be unique per board */
	unsigned char mac_ethernet_address[] =
	{ 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	echo_netif = &server_netif;
#if defined (__arm__) && !defined (ARMR5)
#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
	ProgramSi5324();
	ProgramSfpPhy();
#endif
#endif

/* Define this board specific macro in order perform PHY reset on ZCU102 */
#ifdef XPS_BOARD_ZCU102
	if(IicPhyReset()) {
		xil_printf("Error performing PHY reset \n\r");
		return -1;
	}
#endif

	/* Initialize gpiops */
	ConfigPtr = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
	Status = XGpioPs_CfgInitialize(&myGpio, ConfigPtr, ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	XGpioPs_SetDirectionPin(&myGpio, 0, 1); // 0 input, 1 output
	XGpioPs_SetDirectionPin(&myGpio, 11, 1); // 0 input, 1 output
	laser_edfa_off();
	XGpioPs_SetOutputEnablePin(&myGpio, 0, 1);
	XGpioPs_SetOutputEnablePin(&myGpio, 11, 1);

	init_platform();

    // DW inits

    xil_printf("FPGA version is %X \r\n", get_fpga_version());

    while(0)
    {
//    	XGpioPs_WritePin(&myGpio, 0, 0);
//    	XGpioPs_WritePin(&myGpio, 11, 0);

        xil_printf("debug 1,0\r\n");
    	set_debug_pins(1,0);
    	sleep(1);

        xil_printf("debug 0,1\r\n");
       	set_debug_pins(0,1);
        sleep(1);

//        xil_printf("debug 0,0\r\n");
//    	XGpioPs_WritePin(&myGpio, 0, 1);
//    	XGpioPs_WritePin(&myGpio, 11, 1);

//    	usleep(10000);

    }

    reset_fifo();
    reset_fft();
    config_fft();

    // ========== ADC ============

    rst_adc(1 << b_DBG_SET_ADC_RST);
    init_adc();
    sleep(1);
    sleep(1);

    /* set ADC test mode */
    /* adc_custom1(0x246); */
//    adc_custom2(C1, C2);
//	    adc_ramp();

    while(0) {
	    sample_go();
	    sleep(1);
	    sample_halt();
	    sleep(1);
    }
	   // initialize PM max addresses and vectors
    	load_awg_vector(M_AWG0);
    	load_awg_vector(M_AWG1);
    	load_awg_vector(M_AWG2);
    	load_awg_vector(M_AWG3);
    	load_awg_vector(M_AWG4);
    	load_awg_vector(M_AWG5);

    // =============  AWG   ================
//	    sleep(1);
	    init_pm_addr();
//	    sleep(1);
	    init_pm_dac();
//	    sleep(1);


//	    sleep(1);
	    // need to uncomment to get start of frame
	    load_galvo_vectors();
	    xil_printf("About to init galvo\r\n");
	    init_galvo();
	    xil_printf("galvo done\r\n");
	    sleep(1);
//	    manual_galvo(0x600, 0x800);

//	    xil_printf("All enabled\r\n");
//	    disable(0);


        /* start the pixel process */
	    xil_printf("GO in 1 seconds\r\n");
	    sleep(1);
	    with_window();
//	    without_window();
	    sample_go();




//	    enable_fft();
//    	disable_fft();

        /* tune the ADC clock and data delay settings */
//	    first set taps to something
//	    set_selectio(8, 18); // frame
//	    set_selectio(1, 21); // channel 1
//		adc_delay(0,7,3,1);

//	    set_selectio(0, 20);
//	    set_selectio(1, 21);
//	    set_selectio(2, 19);
//	    set_selectio(3, 20);

//	    test_good_combos(1, 1);
//	    tune_single_channel(check_correct2, 1);

//	    tune_adc_clk_data(check_correct2);
//	    tune_selectio(check_correct2);

//	    tune_adc_clk_data(check_adc_ramp);

//	    set delay manually
		adc_delay(0,0,3,3);
//		adc_delay(0,1,3,1);
//		adc_delay(0,1,3,1);
//		adc_delay(0,1,3,2);


		// wait for galvo start frame then pixel PACKET_SIZE points
		while (1)
			{
			xil_printf("\r\nWaiting for SoF");
			wait_start_frame();
			xil_printf("\r\nGot for SoF");

			// now at start of frame, get PACKET_SIZE points for each channel
			for (pixel = 0; pixel < PACKET_SIZE*2; pixel++)
			{
//				if (0 == (pixel % 100000))
//					xil_printf("\r\n%d", pixel);

				wait_peak_ready();

				for (channel = 0; channel < 1; channel++)
					{
						results[pixel*6 + channel*2] = Xil_In32(M_PEAKS_IDXS + (channel *2));
						results[pixel*6 + channel*2 + 1] = Xil_In32(M_PEAKS_IDXS + (channel *2) + 4);
					}
			}
			if (1)
			{
			// now print out results
			xil_printf("\r\nFinished sampling from peaks:");
			xil_printf("\r\np-amp p-idx s-amp side");

			for (pixel = 0; pixel < PACKET_SIZE*2; pixel++)
				{
					for (channel = 0; channel < 1; channel++)
						{
							side_amp = results[pixel*6 + channel*2] & 0xFFFF;
							side = results[pixel*6 + channel*2] >> 16;

							idx = results[pixel*6 + channel*2 + 1] & 0x7FFF;
							peak = results[pixel*6 + channel*2 + 1] >> 16;
							xil_printf("\n\r %d %d %d %d", peak, idx, side_amp, side );
						}
					}
			}
			sleep(1);
		}

// now get some stats
//		for (i=0; i<10; i++)
//		{
//		 xil_printf("\n\rChannel 0 : %d, 2: %d, 3: %d", check_adc_ramp(0, 0, 0), check_adc_ramp(0, 0, 2), check_adc_ramp(0, 0, 3));
//		}


//		Xil_Out32(0x43C60000 + (4*14), 0x00000000);

		moving_average_clr();
//		moving_average_set();

		while(0)
		{
//		xil_printf("0\r\n");

//			Xil_Out32(0x43C60000 + (4*14), 0x00000000);
		    xil_printf("%d\r\n", count++);
		get_peaks();
		events = get_fft_events();
		if (events)
		    xil_printf("===  FFT EVENTS ===%X\r\n", events);

    	sleep(3);
//		xil_printf("3\r\n");

//		Xil_Out32(0x43C60000 + (4*14), 0x00000003);
//   	sleep(2);
		}
        // DW finished

#if LWIP_IPV6==0
#if LWIP_DHCP==1
    ipaddr.addr = 0;
	gw.addr = 0;
	netmask.addr = 0;
#else
	/* initialize IP addresses to be used */
	IP4_ADDR(&ipaddr,  192, 168,   1, 10);   // DW
	IP4_ADDR(&netmask, 255, 255, 255,  0);
	IP4_ADDR(&gw,      192, 168,   1,  1);
#endif
#endif
	print_app_header();

	lwip_init();

#if (LWIP_IPV6 == 0)
	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(echo_netif, &ipaddr, &netmask,
						&gw, mac_ethernet_address,
						PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Error adding N/W interface\n\r");
		return -1;
	}
#else
	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(echo_netif, NULL, NULL, NULL, mac_ethernet_address,
						PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Error adding N/W interface\n\r");
		return -1;
	}
	echo_netif->ip6_autoconfig_enabled = 1;

	netif_create_ip6_linklocal_address(echo_netif, 1);
	netif_ip6_addr_set_state(echo_netif, 0, IP6_ADDR_VALID);

	print_ip6("\n\rBoard IPv6 address ", &echo_netif->ip6_addr[0].u_addr.ip6);

#endif
	netif_set_default(echo_netif);

	/* now enable interrupts */
	platform_enable_interrupts();

	/* specify that the network if is up */
	netif_set_up(echo_netif);

#if (LWIP_IPV6 == 0)
#if (LWIP_DHCP==1)
	/* Create a new DHCP client for this interface.
	 * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
	 * the predefined regular intervals after starting the client.
	 */
	dhcp_start(echo_netif);
	dhcp_timoutcntr = 2; // DW 24

	while(((echo_netif->ip_addr.addr) == 0) && (dhcp_timoutcntr > 0))
		xemacif_input(echo_netif);

	if (dhcp_timoutcntr <= 0) {
		if ((echo_netif->ip_addr.addr) == 0) {
			xil_printf("DHCP Timeout\r\n");
			xil_printf("Configuring default IP of 192.168.1.10\r\n");
			IP4_ADDR(&(echo_netif->ip_addr),  192, 168,   1, 10); //DW
			IP4_ADDR(&(echo_netif->netmask), 255, 255, 255,  0);
			IP4_ADDR(&(echo_netif->gw),      192, 168,   1,  1);
		}
	}

	ipaddr.addr = echo_netif->ip_addr.addr;
	gw.addr = echo_netif->gw.addr;
	netmask.addr = echo_netif->netmask.addr;
#endif

	print_ip_settings(&ipaddr, &netmask, &gw);

#endif
	/* start the application (web server, rxtest, txtest, etc..) */
	start_application();

	/* receive and process packets */
	while (1) {
		if (TcpFastTmrFlag) {
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		if (TcpSlowTmrFlag) {
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}
		xemacif_input(echo_netif);
		transfer_data();
	}

	/* never reached */
	cleanup_platform();

	return 0;
}
