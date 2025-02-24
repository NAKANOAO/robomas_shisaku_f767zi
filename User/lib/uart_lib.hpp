#ifndef UART_COMM_HPP_
#define UART_COMM_HPP_

#include "main.h"
#include "stdio.h"
#include "string.h"
#include <stdarg.h>

namespace UserLib{

class StLinkDebug{
private:
    UART_HandleTypeDef *huart;
    uint32_t timeout;

public:
    StLinkDebug(UART_HandleTypeDef *_huart, uint32_t _timeout) : huart(_huart), timeout(_timeout){
        // nothing to do
    }

    void transmit_debug(const char *format, ...){
        char buffer[100];

        va_list args;

        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        HAL_UART_Transmit(huart, (uint8_t *)buffer, strlen(buffer), timeout);
    }

};

}




#endif /* UART_COMM_HPP_ */