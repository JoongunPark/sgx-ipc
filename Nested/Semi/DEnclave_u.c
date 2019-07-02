#include "DEnclave_u.h"
#include <errno.h>


typedef struct ms_decall_test_t {
	int* ms_i;
	int* ms_j;
	int* ms_k;
} ms_decall_test_t;

typedef struct ms_docall_print_string_t {
	char* ms_str;
} ms_docall_print_string_t;

static sgx_status_t SGX_CDECL DEnclave_docall_print_string(void* pms)
{
	ms_docall_print_string_t* ms = SGX_CAST(ms_docall_print_string_t*, pms);
	docall_print_string((const char*)ms->ms_str);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_DEnclave = {
	1,
	{
		(void*)DEnclave_docall_print_string,
	}
};
sgx_status_t inner_1_init(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 0, &ocall_table_DEnclave, NULL);
	return status;
}

sgx_status_t decall_test(sgx_enclave_id_t eid, int* i, int* j, int* k)
{
	sgx_status_t status;
	ms_decall_test_t ms;
	ms.ms_i = i;
	ms.ms_j = j;
	ms.ms_k = k;
	status = sgx_ecall(eid, 1, &ocall_table_DEnclave, &ms);
	return status;
}

