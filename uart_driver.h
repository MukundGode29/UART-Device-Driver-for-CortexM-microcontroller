#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "stm32f446xx.h"
#include <stdint.h>


/**************************************************************/
/*                                                            */
/*        UART Register Bit Defination MACROS                 */
/*                                                            */
/**************************************************************/


/*       Register Bit defination of USART SR register        */

#define USART_REG_SR_TXE                      ((uint32_t) 1 << 7 )
#define USART_REG_SR_TC                       ((uint32_t) 1 << 6 )
#define USART_REG_SR_RXNE                     ((uint32_t) 1 << 5 )
#define USART_REG_SR_IDLE                     ((uint32_t) 1 << 4 )
#define USART_REG_SR_ORE                      ((uint32_t) 1 << 3 )
#define USART_REG_SR_NF                       ((uint32_t) 1 << 2 )
#define USART_REG_SR_FE                       ((uint32_t) 1 << 1 )
#define USART_REG_SR_PE                       ((uint32_t) 1 << 0 )


/*       Register Bit defination of USART BRR register        */

#define USART_REG_BRR_MANTISSA                ((uint32_t) 1 << 4 )
#define USART_REG_BRR_FRACTION                ((uint32_t) 1 << 0 )
#define USART_BAUD_9600                        9600
#define USART_BAUD_115200                      115200

/*       Register Bit defination of USART CR1 register        */



#define USART_REG_CR1_OVER8                     ((uint32_t) 1 << 15)
#define USRAT_ENABLE_OVER8                      1
#define USART_ENABLE_OVER16                     0

#define USART_REG_CR1_UE                        ((uint32_t) 1 << 13 )
#define USART_REG_CR1_WL                        ((uint32_t) 1 << 12 )
#define USART_ENABLE_1S8B                       0
#define USART_ENABLE_1S9B                       1

#define USART_REG_CR1_PCE                       ((uint32_t) 1 << 10 )
#define USART_REG_CR1_PS                        ((uint32_t) 1 << 9  )

#define USART_REG_CR1_PEIE                      ((uint32_t) 1 << 8  )
#define USART_REG_CR1_TXEIE                     ((uint32_t) 1 << 7  )
#define USART_REG_CR1_TCIE                      ((uint32_t) 1 << 6  )
#define USART_REG_CR1_RXNEIE                    ((uint32_t) 1 << 5  )
#define USART_REG_CR1_TE                        ((uint32_t) 1 << 3  )
#define USART_REG_CR1_RE                        ((uint32_t) 1 << 2  )


/*          Register Bit Defination For CR3 Register           */



#define  USART_REG_CR2_STOP_BIT                  12
#define  USART_REG_CR2_STOP_BIT_1               ((uint32_t) 0 << 12  )
#define  USART_REG_CR2_STOP_BIT_0_5             ((uint32_t) 1 << 12  )
#define  USART_REG_CR2_STOP_BIT_2               ((uint32_t) 2 << 12  )
#define  USART_REG_CR2_STOP_BIT_1_5             ((uint32_t) 3 << 12  )

/*          Register Bit Defination For CR3 Register           */

#define USART_REG_CR3_ERR_INT_EN                ((uint32_t) 1 << 0 )

typedef enum
{
	
	HAL_UART_STATE_RESET      = 0x00,
	HAL_UART_STATE_READY      = 0x01,
	HAL_UART_STATE_BUSY       = 0x02,
	HAL_UART_STATE_BUSY_TX    = 0x03,
  HAL_UART_STATE_BUSY_RX    = 0x04,
	HAL_UART_STATE_BUSY_RX_TX = 0x05,
	
}hal_uart_state_t;

#define HAL_UART_ERROR_NONE                    ((uint32_t) 0x00000000 )
#define HAL_UART_ERROR_PE                      ((uint32_t) 0x00000001 )
#define HAL_UART_ERROR_NE                      ((uint32_t) 0x00000002 )
#define HAL_UART_ERROR_FE                      ((uint32_t) 0x00000004 )
#define HAL_UART_ERROR_ORE                     ((uint32_t) 0x00000008 )
#define HAL_UART_ERROR_DMA                     ((uint32_t) 0x00000010 )


#define UART_1 USART1
#define UART_2 USART2
#define UART_3 USART3
#define UART_4 USART4
#define UART_5 USART5
#define UART_6 USART6




#define HAL_RCC_USART1_CLK_ENABLE                        (RCC->APB2ENR |= 1 << 4  )
#define HAL_RCC_USART2_CLK_ENABLE                        (RCC->APB1ENR |= 1 << 17 )
#define HAL_RCC_USART3_CLK_ENABLE                        (RCC->APB1ENR |= 1 << 18 )
#define HAL_RCC_USART4_CLK_ENABLE                        (RCC->APB1ENR |= 1 << 19 )
#define HAL_RCC_USART5_CLK_ENABLE                        (RCC->APB1ENR |= 1 << 20 )
#define HAL_RCC_USART6_CLK_ENABLE                        (RCC->APB2ENR |= 1 << 5  )

#define UART_PARITY_NONE 
	
	

/*********************************************************************/
/*                                                                   */
/*        Data Structure OF UART Intialisation                       */
/*                                                                   */
/*********************************************************************/

/*
 * @brief UART Initialsation Structure
 */
 
typedef struct
{
	uint32_t Baudrate;
	
	uint32_t Wordlenght;
	
	uint32_t Parity;
	
	uint32_t Mode;
	
	uint32_t OverSampling;
	
	uint32_t StopBits;


}uart_init_t;

// Application callback typedef
typedef void ( TX_COMP_CB_t ) (void *ptr);
typedef void ( RX_COMP_CB_t ) (void *ptr);

/*
 * @brief UART HAndle Structure
 */

typedef struct
{
	USART_TypeDef    *Instance;
	
	uart_init_t      Init;
	
	uint8_t          *pBufferTX;
	
	uint8_t          *pBufferRX;
	
	uint16_t         TxXferSize;
	
	uint16_t         RxXferSize;
	
  uint16_t         TxXferCount;
	
	uint16_t         RxXferCount;
	
	uint32_t         ErrorCode;
	
	hal_uart_state_t  rx_state;
	
	hal_uart_state_t  tx_state;
	
	TX_COMP_CB_t      *tx_comp_cb;     //Application Call back when TX Completed
	
	RX_COMP_CB_t      *rx_comp_cb;    //Application Call back when RX Completed
		

}uart_handle_t;


/*********************************************************************/
/*                                                                   */
/*        Driver Exposed APIs                                        */
/*                                                                   */
/*********************************************************************/


void hal_uart_init(uart_handle_t *handle);

void hal_uart_Tx(uart_handle_t *handle, uint8_t *buffer, uint16_t len);

void hal_uart_Rx(uart_handle_t *handle, uint8_t *buffer, uint16_t len);

void hal_uart_interrupt(uart_handle_t *handle);


#endif