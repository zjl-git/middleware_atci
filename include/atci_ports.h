#ifndef __ATCI_PORTS_H__
#define __ATCI_PORTS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

#include "atci.h"

void *atci_ports_malloc(uint32_t num);

void atci_ports_free(void *memory);

uint32_t atci_ports_queue_create(uint32_t queue_length, uint32_t item_size);

void atci_ports_queue_send(uint32_t queue, void *data, uint8_t from_isr);

void atci_ports_send_data(atci_port port, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif 
