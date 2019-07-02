#include "DEnclave_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */

#include <errno.h>
#include <string.h> /* for memcpy etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)



typedef struct ms_decall_test_t {
	int* ms_i;
	int* ms_j;
	int* ms_k;
} ms_decall_test_t;

typedef struct ms_docall_print_string_t {
	char* ms_str;
} ms_docall_print_string_t;

static sgx_status_t SGX_CDECL sgx_inner_3_init(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	inner_3_init();
	return status;
}

static sgx_status_t SGX_CDECL sgx_decall_test(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_decall_test_t));
	ms_decall_test_t* ms = SGX_CAST(ms_decall_test_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	int* _tmp_i = ms->ms_i;
	size_t _len_i = sizeof(*_tmp_i);
	int* _in_i = NULL;
	int* _tmp_j = ms->ms_j;
	size_t _len_j = sizeof(*_tmp_j);
	int* _in_j = NULL;
	int* _tmp_k = ms->ms_k;
	size_t _len_k = sizeof(*_tmp_k);
	int* _in_k = NULL;

	CHECK_UNIQUE_POINTER(_tmp_i, _len_i);
	CHECK_UNIQUE_POINTER(_tmp_j, _len_j);
	CHECK_UNIQUE_POINTER(_tmp_k, _len_k);

	if (_tmp_i != NULL) {
		_in_i = (int*)malloc(_len_i);
		if (_in_i == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_i, _tmp_i, _len_i);
	}
	if (_tmp_j != NULL) {
		_in_j = (int*)malloc(_len_j);
		if (_in_j == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_j, _tmp_j, _len_j);
	}
	if (_tmp_k != NULL) {
		_in_k = (int*)malloc(_len_k);
		if (_in_k == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_k, _tmp_k, _len_k);
	}
	decall_test(_in_i, _in_j, _in_k);
err:
	if (_in_i) {
		memcpy(_tmp_i, _in_i, _len_i);
		free(_in_i);
	}
	if (_in_j) {
		memcpy(_tmp_j, _in_j, _len_j);
		free(_in_j);
	}
	if (_in_k) {
		memcpy(_tmp_k, _in_k, _len_k);
		free(_in_k);
	}

	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv;} ecall_table[2];
} g_ecall_table_demi = {
	2,
	{
		{(void*)(uintptr_t)sgx_inner_3_init, 0},
		{(void*)(uintptr_t)sgx_decall_test, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[1][2];
} g_dyn_entry_table_demi = {
	1,
	{
		{0, 0, },
	}
};


sgx_status_t SGX_CDECL docall_print_string(const char* str)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_str = str ? strlen(str) + 1 : 0;

	ms_docall_print_string_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_docall_print_string_t);
	void *__tmp = NULL;

	ocalloc_size += (str != NULL && sgx_is_within_enclave(str, _len_str)) ? _len_str : 0;

	__tmp = sgx_ocalloc_outer(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree_outer();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_docall_print_string_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_docall_print_string_t));

	if (str != NULL && sgx_is_within_enclave(str, _len_str)) {
		ms->ms_str = (char*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_str);
		memcpy((void*)ms->ms_str, str, _len_str);
	} else if (str == NULL) {
		ms->ms_str = NULL;
	} else {
		sgx_ocfree_outer();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(0+10000, ms);


	sgx_ocfree_outer();	return status;
}

