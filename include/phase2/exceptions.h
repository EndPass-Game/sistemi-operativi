#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

#include <pandos_types.h>

void exceptionHandler();

/**
 * @brief TODO:
 * Implements pass up or die logic
 * 
 * @param passupType 
 */
void passUpOrDie(int passupType);

/**
 * @brief Returns the corresponding device number given
 * the address of the device. Assumes the alignment is correct
 * e.g. the input address is at the beginning of a word.
 * @param memory_address memory address of a device.
 * @return int the device number associated with that address
 */
int resolveDeviceAddress(memaddr memory_address);

#endif /* _EXCEPTIONS_H */
