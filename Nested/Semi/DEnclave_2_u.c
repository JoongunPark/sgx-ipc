#include "DEnclave_2_u.h"
#include <errno.h>


typedef struct ms_ecall_test_consumer_t {
	void* ms_buf;
} ms_ecall_test_consumer_t;

typedef struct ms_docall_print_string_t {
	char* ms_str;
} ms_docall_print_string_t;

static sgx_status_t SGX_CDECL DEnclave_2_docall_print_string(void* pms)
{
	ms_docall_print_string_t* ms = SGX_CAST(ms_docall_print_string_t*, pms);
	docall_print_string((const char*)ms->ms_str);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_DEnclave_2 = {
	1,
	{
		(void*)DEnclave_2_docall_print_string,
	}
};
sgx_status_t inner_2_init(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 0, &ocall_table_DEnclave_2, NULL);
	return status;
}

sgx_status_t ecall_test_consumer(sgx_enclave_id_t eid, void* buf)
{
	sgx_status_t status;
	ms_ecall_test_consumer_t ms;
	ms.ms_buf = buf;
	status = sgx_ecall(eid, 1, &ocall_table_DEnclave_2, &ms);
	return status;
}

