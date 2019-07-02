#ifndef SEMI_T_H__
#define SEMI_T_H__

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


void outer_init(sgx_enclave_id_t inner_eid);
void secall_test_producer(sgx_enclave_id_t inner_eid);
void secall_test_consumer(sgx_enclave_id_t inner_eid);

sgx_status_t SGX_CDECL semi_ocall_print_string(const char* str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
