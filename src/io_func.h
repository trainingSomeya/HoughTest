#ifndef IO_FUNC_H
#define IO_FUNC_H

#include<stdint.h>
#include<stdio.h>
#include"app.h"

int32_t read_command(App * app, uint8_t *buf, int32_t buf_size, uint8_t *arg[], int32_t max_n_arg);

#endif
