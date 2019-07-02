#ifndef SEMI_U_H__
#define SEMI_U_H__

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

void SGX_UBRIDGE(SGX_NOCONVENTION, semi_ocall_print_string, (const char* str));

sgx_status_t outer_init(sgx_enclave_id_t eid, sgx_enclave_id_t inner_eid);
sgx_status_t secall_test_producer(sgx_enclave_id_t eid, sgx_enclave_id_t inner_eid);
sgx_status_t secall_test_consumer(sgx_enclave_id_t eid, sgx_enclave_id_t inner_eid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
