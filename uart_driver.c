#include "uart_driver.h"




/*********************************************************************/
/*                                                                   */
/*        Static Helper Function                                        */
/*                                                                   */
/*********************************************************************/

static void hal_uart_enable(USART_TypeDef *usatx)
{
	usatx->CR1 |= USART_REG_CR1_UE;
}

static void hal_uart_diable(USART_TypeDef *usatx)
{
	usatx->CR1 &= ~USART_REG_CR1_UE;
}

static void hal_uart_enable_disable_Tx(USART_TypeDef *usartx, uint32_t tx)
{
	if(tx & USART_REG_CR1_TE)
	{
		usartx->CR1 |= USART_REG_CR1_TE;
	}
	else 
	{
		usartx->CR1 &= ~USART_REG_CR1_TE;
	
	}
}

static void hal_uart_enable_disable_Rx(USART_TypeDef *usartx, uint32_t rx)
{
	if(rx & USART_REG_CR1_RE)
	{
		usartx->CR1 |= USART_REG_CR1_RE;
	}
	else 
	{
		usartx->CR1 &= ~USART_REG_CR1_RE;
	
	}
}

static void hal_uart_configure_wordlength(USART_TypeDef *usartx, uint32_t wl)
{
	if(wl & USART_REG_CR1_WL)
	{
		usartx->CR1 |= USART_REG_CR1_WL;  //9 bit Data
	}
	else 
	{
		usartx->CR1 &= ~USART_REG_CR1_WL;  // 8 bit data
	
	}
}

static void hal_uart_configure_stop_bit(USART_TypeDef *usartx, uint32_t stop)
{
	usartx->CR2 &= ~( 0x3 << USART_REG_CR2_STOP_BIT );
	
	if(stop == USART_REG_CR2_STOP_BIT_1)
	{
		usartx->CR2 |= ( 0x00 << USART_REG_CR2_STOP_BIT );
	}
	else if(stop == USART_REG_CR2_STOP_BIT_0_5) 
	{
		usartx->CR2 |= ( 0x01 << USART_REG_CR2_STOP_BIT );
	
	}
	else if(stop == USART_REG_CR2_STOP_BIT_2) 
	{
		usartx->CR2 |= ( 0x02 << USART_REG_CR2_STOP_BIT );
	
	}
	else if(stop == USART_REG_CR2_STOP_BIT_1_5) 
	{
		usartx->CR2 |= ( 0x03 << USART_REG_CR2_STOP_BIT );
	
	}
}

static void hal_uart_configure_baudrate(USART_TypeDef *usartx, uint32_t baud)
{
	
	uint32_t val;
	if(baud == USART_BAUD_9600)
	{
	  val = 0x683;   //Calculated using formula of BR
	}
	else if(baud == USART_BAUD_115200)
	{
	  val = 0x8A;   //Calculated using formula of BR
	}
	else
	{
		val = 0x8A;
	
	}
	usartx->BRR = val;

}

static void hal_uart_configure_over_sampling(USART_TypeDef *usartx, uint32_t over8)
{
	if(over8)
	{
		usartx->CR1 |= USART_REG_CR1_OVER8;
	}

}

static void hal_uart_configure_txe_interrupt(USART_TypeDef *usartx, uint32_t txe_en)
{
	if(txe_en)
	{
		usartx->CR1 |= USART_REG_CR1_TXEIE;
	}
	else
	{
		usartx->CR1 &= ~USART_REG_CR1_TXEIE;
	}	

}


static void hal_uart_configure_rxe_interrupt(USART_TypeDef *usartx, uint32_t rxe_en)
{
	if(rxe_en)
	{
		usartx->CR1 |= USART_REG_CR1_RXNEIE;
	}
	else
	{
		usartx->CR1 &= ~USART_REG_CR1_RXNEIE;
	}	

}

static void hal_uart_configure_error_interrupt(USART_TypeDef *usartx, uint32_t er_en)
{
	if(er_en)
	{
		usartx->CR3 |= USART_REG_CR3_ERR_INT_EN;
	}
	else
	{
		usartx->CR3 &= ~USART_REG_CR3_ERR_INT_EN;
	}	

}

static void hal_uart_configure_parity_error_interrupt(USART_TypeDef *usartx, uint32_t pe_en)
{
	if(pe_en)
	{
		usartx->CR1 |= USART_REG_CR1_PEIE;
	
	}
	else
	{
		usartx->CR1 &= ~USART_REG_CR1_PEIE;
	}
		

}

static void hal_uart_clear_error_flag(uart_handle_t *huart)  
{
  uint32_t tempreg = 0x00;
	tempreg = huart->Instance->SR;  //Sequence to clear Error Flag
	tempreg = huart->Instance->DR;
}


static void hal_uart_error_cb(uart_handle_t *huart)
{
	//Blink LED FOR INFINITE TO INDICATE ERROR
}

static void hal_uart_handle_TXE_interrupt(uart_handle_t *huart)
{
	uint32_t temp; 
	uint8_t val;
	
	temp = huart->tx_state;
	
	if(temp == HAL_UART_STATE_BUSY_TX)
	{
		val = *huart->pBufferTX++;
		huart->Instance->DR = val;
		
		if(--huart->RxXferCount == 0)
		{
			huart->Instance->CR1 &= USART_REG_CR1_TXEIE; //disable txe Interrupt
			
			huart->Instance->CR1 |= USART_REG_CR1_TCIE; // Enable TC interrupt
		
		}
		
	}
}


static void hal_uart_handle_TC_interrupt(uart_handle_t *huart)
{
	//Disable TC interrupt
	huart->Instance->CR1 &= ~USART_REG_CR1_TCIE;
	huart->tx_state = HAL_UART_STATE_READY;
	
	//call the application callback
	if(huart->tx_comp_cb)
	{
		huart->tx_comp_cb(&huart->TxXferSize);
	}
	
}

static void hal_uart_handle_RXNE_interrupt(uart_handle_t *huart)
{
	uint32_t temp = 0;
	
	if(temp == HAL_UART_STATE_BUSY_RX)
	{
		//Cheack if parity bit is used
		if(huart->Init.Parity == USART_REG_CR1_PCE)
		{
			*huart->pBufferRX++ = (uint8_t)huart->Instance->DR & (uint8_t)0x007F; //PArity Bit should be masked
		}
		else
		{
			*huart->pBufferRX++ = (uint8_t)huart->Instance->DR & (uint8_t)0x00FF;
		}
		
		if(--huart->RxXferCount == 0)
		{
			huart->Instance->CR1 &= ~USART_REG_CR1_RXNEIE; 
			
			huart->Instance->CR1 &= ~USART_REG_CR1_PEIE;
			
			huart->Instance->CR3 &= ~USART_REG_CR3_ERR_INT_EN;
			
			huart->rx_state = HAL_UART_STATE_READY;
			
			//Call the appication callback
			if(huart->rx_comp_cb)
			{
				huart->rx_comp_cb(&huart->RxXferSize);
			
			}
		
		}
	}
}

/*************************************************************/


void hal_uart_init(uart_handle_t *handle)
{
	
	hal_uart_enable(handle->Instance);
	
	hal_uart_enable_disable_Tx(handle->Instance, handle->Init.Mode);
	
	hal_uart_enable_disable_Rx(handle->Instance, handle->Init.Mode);
	
	hal_uart_configure_wordlength(handle->Instance, handle->Init.Wordlenght);
	
	hal_uart_configure_baudrate(handle->Instance, handle->Init.Baudrate);

	hal_uart_configure_stop_bit(handle->Instance, handle->Init.StopBits);
	
	hal_uart_configure_over_sampling(handle->Instance, handle->Init.OverSampling);

	

}

void hal_uart_Tx(uart_handle_t *handle, uint8_t *buffer, uint16_t len)
{
	handle->pBufferTX = buffer;
	handle->RxXferSize = len;
	handle->RxXferCount = len;
	
	handle->tx_state = HAL_UART_STATE_BUSY_TX;
	
	hal_uart_enable(handle->Instance);
	
	
	hal_uart_configure_txe_interrupt(handle->Instance, 1);


}

void hal_uart_Rx(uart_handle_t *handle, uint8_t *buffer, uint16_t len)
{
	uint32_t val; 
	
	handle->pBufferTX = buffer;
	handle->RxXferSize = len;
	handle->RxXferCount = len;
	
	handle->rx_state = HAL_UART_STATE_BUSY_RX;
	
	hal_uart_configure_parity_error_interrupt(handle->Instance, 1);
	
	
	
	hal_uart_enable(handle->Instance);
	
	
	hal_uart_configure_txe_interrupt(handle->Instance, 1);
	
	hal_uart_configure_error_interrupt(handle->Instance, 1);
	
	val = handle->Instance->DR;
	
	hal_uart_configure_rxe_interrupt(handle->Instance, 1);


}

void hal_uart_interrupt(uart_handle_t *handle)
{
	uint32_t temp1, temp2;
	
	temp1 = handle->Instance->SR & USART_REG_SR_PE;
	temp2 = handle->Instance->CR1 & USART_REG_CR1_PEIE;
	//UART parity error interrupt occur
	if(temp1 && temp2 )
	{
		hal_uart_clear_error_flag(handle);
		
		handle->ErrorCode |= HAL_UART_ERROR_PE;
	
	}
	
	temp1 = handle->Instance->SR & USART_REG_SR_FE;
	temp2 = handle->Instance->CR3 & USART_REG_CR3_ERR_INT_EN;
	//UART Frame error interrupt occur
	if(temp1 && temp2 )
	{
		hal_uart_clear_error_flag(handle);
		
		handle->ErrorCode |= HAL_UART_ERROR_FE;
	
	}
	
	temp1 = handle->Instance->SR & USART_REG_SR_NF;
	temp2 = handle->Instance->CR3 & USART_REG_CR3_ERR_INT_EN;
	//UART Noise error interrupt occur
	if(temp1 && temp2 )
	{
		hal_uart_clear_error_flag(handle);
		
		handle->ErrorCode |= HAL_UART_ERROR_NE;
	
	}
	
	temp1 = handle->Instance->SR & USART_REG_SR_ORE;
	temp2 = handle->Instance->CR3 & USART_REG_CR3_ERR_INT_EN;
	//UART Overrun interrupt occur
	if(temp1 && temp2 )
	{
		hal_uart_clear_error_flag(handle);
		
		handle->ErrorCode |= HAL_UART_ERROR_ORE;
	
	}
	
	temp1 = handle->Instance->SR & USART_REG_SR_RXNE;
	temp2 = handle->Instance->CR1 & USART_REG_CR1_RXNEIE;
	//UART RXe interrupt occur
	if(temp1 && temp2 )
	{
    hal_uart_handle_RXNE_interrupt(handle);
	
	}

	temp1 = handle->Instance->SR & USART_REG_SR_TXE;
	temp2 = handle->Instance->CR1 & USART_REG_CR1_TXEIE;
	//UART RXe interrupt occur
	if(temp1 && temp2 )
	{
    hal_uart_handle_TXE_interrupt(handle);
	
	}
	
	temp1 = handle->Instance->SR & USART_REG_SR_TC;
	temp2 = handle->Instance->CR1 & USART_REG_CR1_TCIE;
	//UART RXe interrupt occur
	if(temp1 && temp2 )
	{
    hal_uart_handle_TC_interrupt(handle);
	
	}
	
	if(handle->ErrorCode != HAL_UART_ERROR_NONE)
	{
		handle->tx_state = HAL_UART_STATE_READY;
		handle->rx_state = HAL_UART_STATE_READY;
		
		hal_uart_error_cb(handle);
	
	}
}

