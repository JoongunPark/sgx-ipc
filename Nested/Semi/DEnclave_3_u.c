#include "DEnclave_3_u.h"
#include <errno.h>


static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_DEnclave_3 = {
	0,
	{ NULL },
};
sgx_status_t inner_3_init(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 0, &ocall_table_DEnclave_3, NULL);
	return status;
}

