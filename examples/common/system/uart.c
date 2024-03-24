/*
 *  Copyright 2023 Sergey Khabarov, sergeykhbr@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <inttypes.h>
#include <stdarg.h>
#include <string.h>
#include "vprintfmt.h"
#include "uart.h"

void uart_putc(USART_registers_type *dev, char s, char ctx) {
    while ((read16(&dev->SR) & (1 << 7)) == 0) {}  // [7] TXE, transmit data register empty
    write16(&dev->DR, (uint8_t)s);
}

int uart_putchar(int ch, void *putdat) {
    USART_registers_type *dev = (USART_registers_type *)putdat;
    uart_putc(dev, (char)ch, 0);
    return 0;
}

void uart_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vprintfmt_lib((f_putch)uart_putchar, (void *)USART1_BASE, fmt, ap);
    va_end(ap);
}
