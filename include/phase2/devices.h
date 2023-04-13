#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include <pandos_types.h>

/**
 * @brief Returns the corresponding device number given
 * the address of the device. Assumes the alignment is correct
 * e.g. the input address is at the beginning of a word.
 * @param memory_address memory address of a device.
 * @return int the device number associated with that address
 */
int resolveDeviceAddress(memaddr memory_address);

/**
 * @brief general handler for device interrupts
 */
void handleDeviceInt(int device_reg);

// TODO: docs
int getNumRegister(int *cmdAddr);

// TODO: docs
void beginIO(int devnum, pcb_t *process);

// TODO: docs
void endIO(int devnum);

#endif /* _INTERRUPTS_H_ */