#include "platform/serial.h"
#include "cmsis_os.h"
#include "stm32f1xx.h"
#include <stdlib.h>
#include <string.h>

#define USART_IRQ_PRIORITY 6

typedef struct {
    uint8_t sndbuff[48];
    uint8_t bitmask;
    USART_TypeDef* USART;
    osSemaphoreId sem;
    osMessageQId qtransmit;
    unsigned char qtransmit_fl;
    osMessageQId qreceive;
    int qtransmit_size;
    int qreceive_size;
    int qtransmit_err;
    int qreceive_err;
    int qack_err;

    int rec_bytes;
    int usb_bytes;
    int send_bytes;
    int rs_ore;
    int rs_fe;
    int rs_ne;

    int weakup_tx;
    SerialCB* cb;

} UsartItem;

UsartItem usats[UART_AMOUNT];

#define MAX_QUEUE_SIZE_UART 64

osStatus osMessagePut_wrap(UsartItem* uart, osMessageQId queue_id, uint32_t info, uint32_t millisec);


// kolejki
osMessageQId xQueueUART1recHandle;
osMessageQDef(xQueueUART1rec, MAX_QUEUE_SIZE_UART, uint8_t);

osMessageQId xQueueUART2recHandle;
osMessageQDef(xQueueUART2rec, MAX_QUEUE_SIZE_UART, uint8_t);

osMessageQId xQueueUART1transHandle;
osMessageQDef(xQueueUART1trans, MAX_QUEUE_SIZE_UART, uint8_t);

osMessageQId xQueueUART2transHandle;
osMessageQDef(xQueueUART2trans, MAX_QUEUE_SIZE_UART, uint8_t);

osSemaphoreId UART1EmptyQueue;
osSemaphoreDef(UART1EmptyQueue_def);

osSemaphoreId UART2EmptyQueue;
osSemaphoreDef(UART2EmptyQueue_def);

// watki
void StartTaskUSART1(void const * argument);
osThreadId TaskUSART1Handle;
osThreadDef(TaskUSART1, StartTaskUSART1, osPriorityNormal, 0, 128);

void StartTaskUSART2(void const * argument);
osThreadId TaskUSART2Handle;
osThreadDef(TaskUSART2, StartTaskUSART2, osPriorityNormal, 0, 128);

void StartTaskUSART1Rec(void const * argument);
osThreadId TaskUSART1RecHandle;
osThreadDef(TaskUSART1Rec, StartTaskUSART1Rec, osPriorityNormal, 0, 128);

void StartTaskUSART2Rec(void const * argument);
osThreadId TaskUSART2RecHandle;
osThreadDef(TaskUSART2Rec, StartTaskUSART2Rec, osPriorityNormal, 0, 128);

void StartTaskUSART(UsartItem* uart) {
    osEvent evt;
    for (;;) {
        evt = osMessageGet(uart->qtransmit, osWaitForever);
        uart->send_bytes++;
        uart->weakup_tx = 1;
        uart->USART->DR = evt.value.v;
        uart->USART->CR1 |= USART_CR1_TCIE;
        osSemaphoreWait(uart->sem, osWaitForever);
    }
}

void StartTaskUSART1(void const * argument) {
    StartTaskUSART(&usats[USART1_]);
}
void StartTaskUSART2(void const * argument) {
    StartTaskUSART(&usats[USART2_]);
}





void StartTaskRec(UsartItem* uart) {
    osEvent evt;
    char TempChar;

    if (uart->cb->term!=NULL) {

    	BufferOutput* buffout = buffer_init(256);
    	terminal_get_header(uart->cb->term, buffout);
    	write_serial(uart->cb, buffer_get(buffout),strlen(buffer_get(buffout)));
    	for (;;) {
        	evt = osMessageGet(uart->qreceive, osWaitForever);  //Wysylka z uart 1
        	if (evt.status == osEventMessage) {
        		//osMessagePut_wrap(uart, uart->qtransmit, evt.value.v, 0);
        		TempChar = evt.value.v;
        		terminal_input(uart->cb->term, &TempChar, 1, buffout);
        		int size = buffer_size(buffout);
        		if (size!=0) {
        			write_serial(uart->cb, buffer_get(buffout),size);
        		}
        	}
    	}
    }

    if (uart->cb->io!=NULL) {
    	for (;;) {
            evt = osMessageGet(uart->qreceive, osWaitForever);  //Wysylka z uart 1
            if (evt.status == osEventMessage) {
            	uart->cb->io->read(uart->cb->io,&TempChar,sizeof(TempChar));
            }
    	}
    }
}

void StartTaskUSART1Rec(void const * argument) {
	StartTaskRec(&usats[USART1_]);
}
void StartTaskUSART2Rec(void const * argument) {
	StartTaskRec(&usats[USART2_]);
}


void init_uart() {

	// kolejki
    UART1EmptyQueue = osSemaphoreCreate(osSemaphore(UART1EmptyQueue_def), 1);
    UART2EmptyQueue = osSemaphoreCreate(osSemaphore(UART2EmptyQueue_def), 1);
    xQueueUART1recHandle = osMessageCreate(osMessageQ(xQueueUART1rec), NULL);
    xQueueUART2recHandle = osMessageCreate(osMessageQ(xQueueUART2rec), NULL);
    xQueueUART1transHandle = osMessageCreate(osMessageQ(xQueueUART1trans), NULL);
    xQueueUART2transHandle = osMessageCreate(osMessageQ(xQueueUART2trans), NULL);


    usats[USART1_].USART = USART1;
    usats[USART1_].qreceive = xQueueUART1recHandle;
    usats[USART1_].qtransmit = xQueueUART1transHandle;
    usats[USART1_].sem = UART1EmptyQueue;
    usats[USART1_].rec_bytes = 0;
    usats[USART1_].send_bytes = 0;
    usats[USART1_].usb_bytes = 0;
    usats[USART1_].qtransmit_size = 0;
    usats[USART1_].qreceive_size = 0;
    usats[USART1_].rs_ore = 0;
    usats[USART1_].rs_fe = 0;
    usats[USART1_].rs_ne = 0;
    usats[USART1_].qtransmit_err = 0;
    usats[USART1_].qreceive_err = 0;
    usats[USART1_].qack_err = 0;
    usats[USART1_].bitmask = 0xFF;
    usats[USART1_].weakup_tx = 0;

    usats[USART2_].USART = USART2;
    usats[USART2_].qreceive = xQueueUART2recHandle;
    usats[USART2_].qtransmit = xQueueUART2transHandle;
    usats[USART2_].sem = UART2EmptyQueue;
    usats[USART2_].rec_bytes = 0;
    usats[USART2_].send_bytes = 0;
    usats[USART2_].usb_bytes = 0;
    usats[USART2_].qtransmit_size = 0;
    usats[USART2_].qreceive_size = 0;
    usats[USART2_].rs_ore = 0;
    usats[USART2_].rs_fe = 0;
    usats[USART2_].rs_ne = 0;
    usats[USART2_].qtransmit_err = 0;
    usats[USART2_].qreceive_err = 0;
    usats[USART2_].qack_err = 0;
    usats[USART2_].bitmask = 0xFF;
    usats[USART2_].weakup_tx = 0;

    USART1->CR1 |= USART_CR1_RXNEIE;
    NVIC_SetPriority(USART1_IRQn, USART_IRQ_PRIORITY);
    NVIC_EnableIRQ(USART1_IRQn);

    USART2->CR1 |= USART_CR1_RXNEIE;
    NVIC_SetPriority(USART2_IRQn, USART_IRQ_PRIORITY);
    NVIC_EnableIRQ(USART2_IRQn);


    TaskUSART1Handle = osThreadCreate(osThread(TaskUSART1), NULL);
    TaskUSART2Handle = osThreadCreate(osThread(TaskUSART2), NULL);
//    TaskUSART1RecHandle = osThreadCreate(osThread(TaskUSART1Rec), NULL);
//    TaskUSART2RecHandle = osThreadCreate(osThread(TaskUSART2Rec), NULL);

}

osStatus osMessagePut_wrap(UsartItem* uart, osMessageQId queue_id, uint32_t info, uint32_t millisec) {
    osStatus result;
    int *size = 0;
    int *error = 0;
    if (uart->qreceive == queue_id) {
        size = &uart->qreceive_size;
        error = &uart->qreceive_err;
    }
    if (uart->qtransmit == queue_id) {
        size = &uart->qtransmit_size;
        error = &uart->qtransmit_err;
    }
    *size = osMessageWaiting(queue_id);
    result = osMessagePut(queue_id, info, millisec);
    if (result != osOK) {
        *error = *error + 1;
    }
    return result;
}



static void USART_IRQHandler(UsartItem* uart) {

    uint8_t temp = 0;
    volatile osEvent evt;
    if (uart->USART->SR & USART_SR_RXNE) {
        temp = (uint8_t) (uart->USART->DR & uart->bitmask);
        uart->rec_bytes++;
        osMessagePut_wrap(uart, uart->qreceive, temp, 0);
    }

    if (uart->USART->SR & USART_SR_TC) {
        evt = osMessageGet(uart->qtransmit, 0);
        uart->qtransmit_size = osMessageWaiting(uart->qtransmit);
        if (evt.status == osEventMessage) {
        	uart->weakup_tx = 1;
            uart->USART->DR = evt.value.v;
            uart->send_bytes++;
        } else {
            // koniec nadawania bloku
        	if (uart->weakup_tx) {
        		uart->weakup_tx = 0;
        		osSemaphoreRelease(uart->sem);
        		uart->USART->CR1 &= ~USART_CR1_TCIE;
        	}
        }

    }

}

void USART1_IRQHandler(void) {
    USART_IRQHandler(&usats[USART1_]);
}

void USART2_IRQHandler(void) {
    USART_IRQHandler(&usats[USART2_]);
}



SerialCB* open_serial(int usart, int baud_rate) {
	SerialCB* result;
	result = (SerialCB*)malloc(sizeof(SerialCB));
	usats[usart].cb = result;
	result->usr = &usats[usart];
	return result;
}

void close_serial(SerialCB* cb) {

}

unsigned char write_serial(SerialCB* cb, const char* buff, int size) {
	UsartItem* uart = (UsartItem*)cb->usr;
	for(int i=0;i<size;i++) {
		osMessagePut_wrap(uart, uart->qtransmit, buff[i], 0);
	}

	return 1;
}

static void start_usart_read_thread(SerialCB* cb) {
	// odpalenie w¹tków po otwarciu
	if (cb->usr == &usats[USART1_]) {
		TaskUSART1RecHandle = osThreadCreate(osThread(TaskUSART1Rec), NULL);
	}
	if (cb->usr == &usats[USART2_]) {
		TaskUSART2RecHandle = osThreadCreate(osThread(TaskUSART2Rec), NULL);
	}

}

void start_read_serial_term(SerialCB* cb, TerminalContext* term) {
	cb->term = term;
	terminal_set_echo(term, 0);
	cb->io = NULL;
	start_usart_read_thread(cb);
}

void start_read_serial_io(SerialCB* cb, ProcessIo* io){
	cb->io = io;
	cb->term = NULL;
	start_usart_read_thread(cb);

}
