#ifndef ENCLAVE_U_H__
#define ENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_satus_t etc. */

#include "user_types.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_print_string, (const char* str));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_sleep, (int time));

sgx_status_t ecall_test_producer(sgx_enclave_id_t eid, void* buf);
sgx_status_t ecall_test_consumer(sgx_enclave_id_t eid, void* buf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
