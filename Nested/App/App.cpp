/*
 * Copyright (C) 2011-2017 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <stdio.h>
#include <string.h>
#include <assert.h>

# include <unistd.h>
# include <pwd.h>
# define MAX_PATH FILENAME_MAX

#include "sgx_urts.h"
#include "App.h"
//#include "Enclave_u.h"

#include "Semi_u.h"
#include "Enclave_1_u.h"
#include "Enclave_2_u.h"
#include "Enclave_3_u.h"

#include <sys/mman.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <time.h>

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid_1 = 0;
sgx_enclave_id_t global_eid_2 = 0;
sgx_enclave_id_t global_eid_3 = 0;
sgx_enclave_id_t global_eid_4 = 0;
sgx_enclave_id_t global_o_eid = 0;

int mem_size;
void* shmem;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
        printf("Error: Unexpected error occurred.\n");
}

/* Initialize the enclave:
 *   Step 1: try to retrieve the launch token saved by last transaction
 *   Step 2: call sgx_create_enclave to initialize an enclave instance
 *   Step 3: save the launch token if it is updated
 */
int initialize_enclave(void)
{
    char token_path[MAX_PATH] = {'\0'};
    sgx_launch_token_t token = {0};
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    int updated = 0;
    
//    /* Step 1: try to retrieve the launch token saved by last transaction 
//     *         if there is no token, then create a new one.
//     */
//    /* try to get the token saved in $HOME */
//    const char *home_dir = getpwuid(getuid())->pw_dir;
//    
//    if (home_dir != NULL && 
//        (strlen(home_dir)+strlen("/")+sizeof(TOKEN_FILENAME)+1) <= MAX_PATH) {
//        /* compose the token path */
//        strncpy(token_path, home_dir, strlen(home_dir));
//        strncat(token_path, "/", strlen("/"));
//        strncat(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME)+1);
//    } else {
//        /* if token path is too long or $HOME is NULL */
//        strncpy(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME));
//    }
//
//    FILE *fp = fopen(token_path, "rb");
//    if (fp == NULL && (fp = fopen(token_path, "wb")) == NULL) {
//        printf("Warning: Failed to create/open the launch token file \"%s\".\n", token_path);
//    }
//
//    if (fp != NULL) {
//        /* read the token from saved file */
//        size_t read_num = fread(token, 1, sizeof(sgx_launch_token_t), fp);
//        if (read_num != 0 && read_num != sizeof(sgx_launch_token_t)) {
//            /* if token is invalid, clear the buffer */
//            memset(&token, 0x0, sizeof(sgx_launch_token_t));
//            printf("Warning: Invalid launch token read from \"%s\".\n", token_path);
//        }
//    }
    /* Step 2: call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
//    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);

    sgx_create_enclave(ENCLAVE_FILENAME_1, SGX_DEBUG_FLAG, &token, &updated, &global_eid_1, NULL);
    sgx_create_enclave(ENCLAVE_FILENAME_1, SGX_DEBUG_FLAG, &token, &updated, &global_eid_2, NULL);
   	sgx_create_enclave(ENCLAVE_FILENAME_1, SGX_DEBUG_FLAG, &token, &updated, &global_eid_3, NULL);
   	sgx_create_enclave(ENCLAVE_FILENAME_1, SGX_DEBUG_FLAG, &token, &updated, &global_eid_4, NULL);
   	ret = sgx_create_outer_enclave(ENCLAVE_OUTER_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_o_eid, NULL);
	sgx_map_inner_to_outer(global_eid_1, global_o_eid);
	sgx_map_inner_to_outer(global_eid_2, global_o_eid);
	sgx_map_inner_to_outer(global_eid_3, global_o_eid);
	sgx_map_inner_to_outer(global_eid_4, global_o_eid);

    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
 //       if (fp != NULL) fclose(fp);
        return -1;
    }

//    /* Step 3: save the launch token if it is updated */
//    if (updated == FALSE || fp == NULL) {
//        /* if the token is not updated, or file handler is invalid, do not perform saving */
//        if (fp != NULL) fclose(fp);
//        return 0;
//    }
//
//    /* reopen the file with write capablity */
//    fp = freopen(token_path, "wb", fp);
//    if (fp == NULL) return 0;
//    size_t write_num = fwrite(token, 1, sizeof(sgx_launch_token_t), fp);
//    if (write_num != sizeof(sgx_launch_token_t))
//        printf("Warning: Failed to save launch token to \"%s\".\n", token_path);
//    fclose(fp);
    return 0;
}

/* OCall functions */
void ocall_print_string(const char *str)
{
    printf("%s", str);
}

void semi_ocall_print_string(const char *str)
{
    printf("%s", str);
}

void ocall_sleep(int time)
{
	//usleep(time);
	msync(shmem, mem_size, MS_SYNC);
	//usleep(10);
}

void* create_shared_memory(size_t size) {
  // Our memory buffer will be readable and writable:
  int protection = PROT_READ | PROT_WRITE;

  // The buffer will be shared (meaning other processes can access it), but
  // anonymous (meaning third-party processes cannot obtain an address for it),
  // so only this process and its children will be able to use it:
  int visibility = MAP_ANONYMOUS | MAP_SHARED;

  // The remaining parameters to `mmap()` are not important for this use case,
  // but the manpage for `mmap` explains their purpose.
  return mmap(NULL, size, protection, visibility, 0, 0);
}

struct queue_root {
	struct queue_head *in_queue;
	struct queue_head *out_queue;
	pthread_mutex_t lock;
};

struct queue_root *ALLOC_QUEUE_ROOT(void *shmem)
{
	int ret = 0; 
	struct queue_root* root = (struct queue_root *)shmem; 	
	//set mutexattr for sharing
	pthread_mutexattr_t mattr;
	pthread_mutexattr_init(&mattr);
	//pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ERRORCHECK_NP);
	pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);

	pthread_mutex_init(&root->lock, &mattr);

	root->in_queue = NULL;
	root->out_queue = NULL;
	return root;
}

void *test_producer(void* data)
{
	secall_test_producer(global_o_eid, *(sgx_enclave_id_t *)data);
}
//void *test_producer_2(void* data)
//{
//	secall_test_producer(global_o_eid, global_eid_2);
//}
//
//void *test_consumer(void* data)
//{
//	secall_test_consumer(global_o_eid, global_eid_3);
//}
//
//void *test_consumer_2(void* data)
//{
//	secall_test_consumer(global_o_eid, global_eid_4);
//}

/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);

    if(initialize_enclave() < 0){
        printf("Enter a character before exit ...\n");
        getchar();
        return -1; 
    }

    pthread_t p_thread[3];
    int thr_id;
    int status;

	inner_1_init(global_eid_1);
	inner_1_init(global_eid_2);
	inner_1_init(global_eid_3);
	inner_1_init(global_eid_4);

	outer_init(global_o_eid, global_eid_1);
	outer_init(global_o_eid, global_eid_2);
	outer_init(global_o_eid, global_eid_3);
	outer_init(global_o_eid, global_eid_4);

	clock_t start1, start2, end1, end2;
	float res1, res2;
	int i;

	start1 = clock();
	secall_test_producer(global_o_eid, global_eid_1);
	secall_test_producer(global_o_eid, global_eid_2);
	secall_test_producer(global_o_eid, global_eid_3);
	secall_test_producer(global_o_eid, global_eid_4);
	end1 = clock();
	res1 = (float)(end1 - start1)/CLOCKS_PER_SEC;

//	start2 = clock();
//	pthread_create(&p_thread[0], NULL, test_producer, &global_eid_1);
//	pthread_create(&p_thread[1], NULL, test_producer, &global_eid_2);
//	pthread_create(&p_thread[2], NULL, test_producer, &global_eid_3);
//	pthread_create(&p_thread[3], NULL, test_producer, &global_eid_4);
//    pthread_join(p_thread[0], (void **)&status);
//    pthread_join(p_thread[1], (void **)&status);
//    pthread_join(p_thread[2], (void **)&status);
//    pthread_join(p_thread[3], (void **)&status);
//	end2	= clock();
//	res2 = (float)(end2 - start2)/CLOCKS_PER_SEC;

	printf(" 일반함수 소요된 시간 : %.6f \n", res1);
	printf(" line함수 소요된 시간 : %.6f \n", res2);

    sgx_destroy_enclave(global_eid_1);
    sgx_destroy_enclave(global_eid_2);
    sgx_destroy_enclave(global_eid_3);
    sgx_destroy_enclave(global_eid_4);
//    sgx_destroy_enclave(global_o_eid);
    
    return 0;
}

