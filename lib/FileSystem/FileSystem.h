#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <LittleFS.h>

void fsInit();

bool saveIRDataToFile(const char* filename, const String &data);

String readIRFile(const char* filename);
bool deleteIRFile(const char* filename);

#endif