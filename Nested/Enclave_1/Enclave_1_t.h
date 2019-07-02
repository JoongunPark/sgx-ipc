#ifndef ENCLAVE_1_T_H__
#define ENCLAVE_1_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */

#include "user_types.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif


void inner_1_init();
void ecall_test_producer(void* buf);

sgx_status_t SGX_CDECL ocall_print_string(const char* str);
sgx_status_t SGX_CDECL ocall_sleep(int time);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
