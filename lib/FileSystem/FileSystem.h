#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "ControleIrUtils.h"
#include <LittleFS.h>

void fsInit();

bool saveIRDataToFile(const char* filename, storedIRDataStruct &entry);

#endif