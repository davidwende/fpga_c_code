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
#include <string.h>

#include "lwip/err.h"
#include "lwip/tcp.h"
#if defined (__arm__) || defined (__aarch64__)
#include "xil_printf.h"
#endif

// added by by DW
#include "xil_io.h"
#include "netif/xadapter.h"
#include "constants.h"
#include "xgpiops.h"

extern XGpioPs myGpio;

extern struct netif *echo_netif;
// finished DW

int transfer_data() {
	return 0;
}

void print_app_header()
{
#if (LWIP_IPV6==0)
	xil_printf("\n\r\n\r-----lwIP TCP echo server ------\n\r");
#else
	xil_printf("\n\r\n\r-----lwIPv6 TCP echo server ------\n\r");
#endif
	xil_printf("TCP packets sent to port 6001 will be echoed back\n\r");
}

err_t recv_callback(void *arg, struct tcp_pcb *tpcb,
        struct pbuf *p, err_t err)
{
    static char command, id;
    static uint32_t inContinue;
    uint32_t value, payload_count, offset;
    static uint32_t address, remaining_bytes, field_length, previous_count, current_length;
    uint32_t i;
    static uint8_t buffer[2048*4];
    static uint32_t vga;

    // void *mybuf;

    /* do not read the packet if we are not in ESTABLISHED state */
    if (!p) {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    /* indicate that the packet has been received */
    tcp_recved(tpcb, p->len);


    /* if remainder = 0, then have 1st packet so:
     * get length from p and from field
     *
     * if ==, then packet is not split, so:
     * 		set address to new, command to new, remainder to 0
     * 		process
     * if remainder !=, then this is a continued packet, so:
     * 		keep old command
     * 		calculate new remainder
     * 		calculate new length of current packet
     *
     */
    xil_printf("\n\r\n\r====>  In callback - remainder is: %d, inContinue=%d\n\r", remaining_bytes, inContinue);

    if (0 == inContinue)
    {
        // first packet
        previous_count = 0;

        field_length = *(const u8_t *)(p->payload + 6);
        field_length = (field_length << 8) + *(const u8_t *)(p->payload + 6 + 1);
        field_length = (field_length << 8) + *(const u8_t *)(p->payload + 6 + 2);
        field_length = (field_length << 8) + *(const u8_t *)(p->payload + 6 + 3);
        xil_printf("field_length from packet=: %d, should have packet of: %d bytes\n\r", field_length, field_length*4 + 10);
        xil_printf("Actual packet size is %d\n\r", p->len);

        command = *(const u8_t *)(p->payload);
        id      = *(const u8_t *)(p->payload + sizeof(char));
        //id = 34;
        address = 0;
        address =                  *(const u8_t *)(p->payload + 2);
        address = (address << 8) + *(const u8_t *)(p->payload + 2 + 1);
        address = (address << 8) + *(const u8_t *)(p->payload + 2 + 2);
        address = (address << 8) + *(const u8_t *)(p->payload + 2 + 3);
        //	xil_printf("address %X\n\r", address);
        //	xil_printf("IDX %d\n\r", id);
        //	xil_printf("Command is %d\n\r", command);

        payload_count = p->len - 10;

        switch (command)
        {
            case 1:
            	// write
                remaining_bytes = (field_length * 4) - payload_count;
                break;
            case 2:
            	// read
                remaining_bytes = 10 - p->len;
        }
        xil_printf("remaining_bytes after calc = %d\n\r", remaining_bytes);

    } else {
        	print("we are in a continuation packet\n\r");
        	xil_printf("\n\rPacket length is %d\n\r", p->len);
        	remaining_bytes -= p->len;
        	xil_printf("Calculated remaining_bytes after this packet %d \n\r", remaining_bytes);
        	payload_count = p->len;
    }

    if ((char) 1 == command)
    {
    	if (0 != inContinue)
    		offset = 0;
    	else
    		offset = 10;

        xil_printf("WRITE so store in temp buffer, previous count=%d, offset=%d\n\r", previous_count, offset);
        for (i=0; i < payload_count; i++)
        {
        	buffer[previous_count + i] = *(const u8_t *)(p->payload + offset + i);
//            xil_printf(".");
        }
        previous_count += payload_count;
    }

    xil_printf("\n\rDone store - Remaining bytes are %d\n\r", remaining_bytes);

    switch (command)
    {
        case 1:
            // write
            if (!remaining_bytes)
            {
                xil_printf("Write requested to address %X\n\r", address);
                xil_printf("To Write %d values\n\r", field_length);

                for (i = 0; i < field_length ; i++ )
                {
                    value = buffer[i*4];
                    value = (value << 8) + buffer[i*4 +1];
                    value = (value << 8) + buffer[i*4 +2];
                    value = (value << 8) + buffer[i*4 +3];
                    //value = (value << 8) + *(const u8_t *)(p->payload + 10 + (count*4 + 2));
                    //value = (value << 8) + *(const u8_t *)(p->payload + 10 + (count*4 + 3));
//                    xil_printf("writing value %X to %X\n\r", value, address);
                    //xil_printf(".");

                    Xil_Out32(address, value);
                    address += 4;
                }

                xil_printf("\n\rNow send ACK back to PC\n\r");
                *(u8_t *)p->payload = 0xAA;
                p->len = 1;

                if (tcp_sndbuf(tpcb) > 1) {
                    xil_printf("Sending response\n\r");
                    err = tcp_write(tpcb, p->payload, p->len, 1);
                    tcp_output(tpcb);
                } else
                    xil_printf("no space in tcp_sndbuf\n\r");

                /* free the received pbuf */
                pbuf_free(p);

                inContinue = 0;
            }
            else
            {
            	inContinue = 1;
            }
            break;

        case 2:
            // read
            xil_printf("Read requested, %d words from address %X\n\r", field_length, address);

            for (i = 0; i < field_length ; i++ )
            {
                value = Xil_In32(address);
                //xil_printf("I read value %X from %X\n\r", value, address);
                *(u8_t *)(p->payload + i*4) =     (value >> 24) & 0xFF;
                *(u8_t *)(p->payload + i*4 + 1) = (value >> 16) & 0xFF;
                *(u8_t *)(p->payload + i*4 + 2) = (value >>  8) & 0xFF;
                *(u8_t *)(p->payload + i*4 + 3) = (value      ) & 0xFF;
                address += 4;
            }
            /* echo back the payload */
            /* in this case, we assume that the payload is < TCP_SND_BUF */

            p->len = 4*field_length;

            if (tcp_sndbuf(tpcb) >= 4*field_length) {
                xil_printf("Starting to send to response %d bytes\n\r", p->len);
                err = tcp_write(tpcb, p->payload, p->len, 1);
                tcp_output(tpcb);
        		xemacif_input(echo_netif);
            } else
                xil_printf("no space in tcp_sndbuf\n\r");

            /* free the received pbuf */
            pbuf_free(p);
            xil_printf("Done sending response\n\r");

            //
            break;

        case 3:
        	// command
            xil_printf("In command 3 with id %X\n\r",  id);

            switch (id)
            {
            // laser at J2 is B35_L25 (pin H5, debug[0])
            // laser at J3 is MIO0
            // edfa is B35_L0 (pin H6, VGA = debug[1])

                case 0:
                    // edfa on
                	vga |= (1 << b_VGA_DBG0);
                    xil_printf("Writing VGA %X\n\r",  vga);
                	Xil_Out32(R_VGA, vga);
                	break;
                case 1:
                    // edfa off
                	vga &= ~(1 << b_VGA_DBG0);
                	Xil_Out32(R_VGA, vga);
                    xil_printf("Writing VGA %X\n\r",  vga);
                	break;

                case 2:
                    // laser J3 off
                	XGpioPs_WritePin(&myGpio, 0, 1);
                    xil_printf("Writing GPIO 1\n\r");
                	break;

                case 3:
                    // laser J3 on
                	XGpioPs_WritePin(&myGpio, 0, 0);
                    xil_printf("Writing GPIO 0\n\r");
                	break;

                case 4:
                    // j2 on
                	vga |= (1 << b_VGA_DBG1);
                	Xil_Out32(R_VGA, vga);
                    xil_printf("Writing VGA %X\n\r",  vga);
                	break;

                case 5:
                    // j2 off
                	vga &= ~(1 << b_VGA_DBG1);
                	Xil_Out32(R_VGA, vga);
                    xil_printf("Writing VGA %X\n\r",  vga);
                	break;

                default:
                    // laser J2 off
                	;
            }

            xil_printf("\n\rNow send ACK back to PC\n\r");
             *(u8_t *)p->payload = 0xAA;
             p->len = 1;

             if (tcp_sndbuf(tpcb) > 1) {
                 xil_printf("Sending response\n\r");
                 err = tcp_write(tpcb, p->payload, p->len, 1);
                 tcp_output(tpcb);
             } else
                 xil_printf("no space in tcp_sndbuf for ACK\n\r");

             /* free the received pbuf */
             pbuf_free(p);

        	break;

        default:
            xil_printf("default requested\n\r");
    }
    return ERR_OK;
}

/* err_t recv_callback(void *arg, struct tcp_pcb *tpcb, */
/*                                struct pbuf *p, err_t err) */
/* { */
/* 	/1* do not read the packet if we are not in ESTABLISHED state *1/ */
/* 	if (!p) { */
/* 		tcp_close(tpcb); */
/* 		tcp_recv(tpcb, NULL); */
/* 		return ERR_OK; */
/* 	} */

/* 	/1* indicate that the packet has been received *1/ */
/* 	tcp_recved(tpcb, p->len); */

/* 	/1* echo back the payload *1/ */
/* 	/1* in this case, we assume that the payload is < TCP_SND_BUF *1/ */
/* 	if (tcp_sndbuf(tpcb) > p->len) { */
/* 		err = tcp_write(tpcb, p->payload, p->len, 1); */
/* 	} else */
/* 		xil_printf("no space in tcp_sndbuf\n\r"); */

/* 	/1* free the received pbuf *1/ */
/* 	pbuf_free(p); */

/* 	return ERR_OK; */
/* } */

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	static int connection = 1;

	/* set the receive callback for this connection */
	tcp_recv(newpcb, recv_callback);

	/* just use an integer number indicating the connection id as the
	   callback argument */
	tcp_arg(newpcb, (void*)(UINTPTR)connection);

	/* increment for subsequent accepted connections */
	connection++;

	return ERR_OK;
}


int start_application()
{
	struct tcp_pcb *pcb;
	err_t err;
	unsigned port = 7;

	/* create new TCP PCB structure */
	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}

	/* bind to specified @port */
	err = tcp_bind(pcb, IP_ANY_TYPE, port);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", port, err);
		return -2;
	}

	/* we do not need any arguments to callback functions */
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	if (!pcb) {
		xil_printf("Out of memory while tcp_listen\n\r");
		return -3;
	}

	/* specify callback to use for incoming connections */
	tcp_accept(pcb, accept_callback);

	xil_printf("TCP echo server started @ port %d\n\r", port);

	return 0;
}
