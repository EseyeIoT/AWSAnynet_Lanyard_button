/*
*  Application to send IoT Button Message to AWS IoT Service from Anynet Click
*
*/

#include <stdio.h>
#include "common/mbuf.h"
#include "common/platform.h"
#include "mgos_app.h"
#include "mgos_gpio.h"
#include "mgos_timers.h"
#include "mgos_uart.h"
#include "mgos.h"

#define UART_NO 1

int state =0;

// State machine called on a timer
static void timer_cb(void *arg) {

// First increment the state
       state++;

  switch (state){
    case 1 :
   printf("Visit: www.eseye.com/aws_dev_kit/ to find out more\r\n");
     mgos_uart_printf(UART_NO,"at+awsver\r\n");
     break;

    case 2 :
//   printf("request Software version\r\n");
     mgos_uart_printf(UART_NO,"at+awsver\r\n");
     break;

   case 3 :
//     printf("SIM number\r\n");
     mgos_uart_printf(UART_NO,"at+QCCID\r\n");
     break;

   case 4 :
//     printf("IMEI\r\n");
    mgos_uart_printf(UART_NO,"at+GSN\r\n");
     break;

   default :
//     show that the code is running with a dot on the console
      printf(".\r\n");
      break;

    case 10 :
// print a ! to show that the console is actually scrolling
       printf("!\r\n");
       state= 6;
  }
  (void) arg;
}

 

//callback used on a  press of USER BTN 1
static void button_cb(int pin, void *arg) {
  mgos_uart_printf(UART_NO,"at+awsbutton=SINGLE\r\n");
  (void) arg;
}

int esp32_uart_rx_fifo_len(int uart_no);

//UART Dispatcher callback Echoes anyting from the Click Module to the Console
static void uart_dispatcher(int uart_no, void *arg) {
  assert(uart_no == UART_NO);
  size_t rx_av = mgos_uart_read_avail(uart_no);
  if (rx_av > 0) {
    struct mbuf rxb;
    mbuf_init(&rxb, 0);
    mgos_uart_read_mbuf(uart_no, &rxb, rx_av);
    if (rxb.len > 0) {
      printf("%.*s", (int) rxb.len, rxb.buf);
    }
    mbuf_free(&rxb);
  }
  (void) arg;
}

enum mgos_app_init_result mgos_app_init(void) {
  struct mgos_uart_config ucfg;
  mgos_uart_config_set_defaults(UART_NO, &ucfg);

  /*
   * the Anynet Click Module uses 9600 8n1 so overwrite the defaults
   */

  ucfg.baud_rate = 9600;
  ucfg.num_data_bits = 8;
  ucfg.parity = MGOS_UART_PARITY_NONE;
  ucfg.stop_bits = MGOS_UART_STOP_BITS_1;

  if (!mgos_uart_configure(UART_NO, &ucfg)) {
    return MGOS_APP_INIT_ERROR;
  }

   /* register callback to send IoT Button Message on Press of  */
  mgos_gpio_set_button_handler(32,MGOS_GPIO_PULL_UP, MGOS_GPIO_INT_EDGE_NEG, 500, button_cb, NULL);

  state = 0; // initialised state
  //register a callback every 2 seconds. Used to interogate Click module and to show heartbeat

  mgos_set_timer(2000 /* ms */, true /* repeat */, timer_cb, NULL /* arg */);

  //register a callback for data bytes received from the click module.
  mgos_uart_set_dispatcher(UART_NO, uart_dispatcher, NULL /* arg */);

// and enable the receiver on this UART
  mgos_uart_set_rx_enabled(UART_NO, true);

//  printf("send an at to show the module is connected\r\n");
  mgos_uart_printf(UART_NO,"at\r\n");

  return MGOS_APP_INIT_SUCCESS;
} 

