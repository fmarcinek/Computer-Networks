// Filip Marcinek 282905

#ifndef HEADER_INFO_H
#define HEADER_INFO_H

#include "common_includes.h"

void get_extension(char* path, char* ext);
bool is_dir(const char* path);
bool target_file_is_in_domain(HeaderInfo *hi, const char* dir_path, char *path);
void get_realpath(HeaderInfo* hi, char* dir_path, char* resolved_path);
HeaderInfo* extractHeaderInfo(char* buffer);

#endif