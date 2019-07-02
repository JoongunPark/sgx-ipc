#include "Semi_u.h"
#include <errno.h>

typedef struct ms_outer_init_t {
	sgx_enclave_id_t ms_inner_eid;
} ms_outer_init_t;

typedef struct ms_secall_test_producer_t {
	sgx_enclave_id_t ms_inner_eid;
} ms_secall_test_producer_t;

typedef struct ms_secall_test_consumer_t {
	sgx_enclave_id_t ms_inner_eid;
} ms_secall_test_consumer_t;

typedef struct ms_semi_ocall_print_string_t {
	char* ms_str;
} ms_semi_ocall_print_string_t;

static sgx_status_t SGX_CDECL Semi_semi_ocall_print_string(void* pms)
{
	ms_semi_ocall_print_string_t* ms = SGX_CAST(ms_semi_ocall_print_string_t*, pms);
	semi_ocall_print_string((const char*)ms->ms_str);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_Semi = {
	1,
	{
		(void*)Semi_semi_ocall_print_string,
	}
};
sgx_status_t outer_init(sgx_enclave_id_t eid, sgx_enclave_id_t inner_eid)
{
	sgx_status_t status;
	ms_outer_init_t ms;
	ms.ms_inner_eid = inner_eid;
	status = sgx_ecall(eid, 0, &ocall_table_Semi, &ms);
	return status;
}

sgx_status_t secall_test_producer(sgx_enclave_id_t eid, sgx_enclave_id_t inner_eid)
{
	sgx_status_t status;
	ms_secall_test_producer_t ms;
	ms.ms_inner_eid = inner_eid;
	status = sgx_ecall(eid, 1, &ocall_table_Semi, &ms);
	return status;
}

sgx_status_t secall_test_consumer(sgx_enclave_id_t eid, sgx_enclave_id_t inner_eid)
{
	sgx_status_t status;
	ms_secall_test_consumer_t ms;
	ms.ms_inner_eid = inner_eid;
	status = sgx_ecall(eid, 2, &ocall_table_Semi, &ms);
	return status;
}

