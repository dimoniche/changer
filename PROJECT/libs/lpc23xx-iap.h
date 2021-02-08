#ifndef __LPC23xx_IAP_H__
#define __LPC23xx_IAP_H__

#include <stdint.h>

extern uint8_t flashrom_write(uint8_t *dst, const uint8_t *src, size_t size);
extern uint8_t flashrom_erase(uint8_t *addr);

#endif // __LPC23xx_IAP_H__
