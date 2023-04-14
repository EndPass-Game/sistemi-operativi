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

unsigned int getPassedTime();

// updates last value of the time of day
void updateProcessTime();

#endif /* _EXCEPTIONS_H */
