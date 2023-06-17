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

/**
 * @brief Get the Num Register of corresponding object
 *
 * @param cmdAddr address of register
 * @return int -1 if invalid, 2 if terminal, 4 if other device
 */
int getNumRegister(int *cmdAddr);

/**
 * @param devnum number of device to begin io with
 * @param process the process that began the IO
 */
void beginIO(int devnum, pcb_t *process);

/**
 * @brief ends IO for a device
 *
 * @param devnum the device that ended IO
 */
void endIO(int devnum);

int resolveSemAddr(memaddr semaddr);

#endif /* _INTERRUPTS_H_ */