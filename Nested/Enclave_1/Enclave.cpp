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
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
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


#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */
#include <string.h>      
#include <sgx_thread.h>
#include <sgx_spinlock.h>

#include "Enclave.h"
#include "Enclave_1_t.h"  /* print_string */
#include "DEnclave_1_t.h"  /* print_string */

char* parent_message = "hello";  // parent process will write this message
char* child_message = "goodbye"; // child process will then write this one
void* QUEUE_POS; 
struct queue_root* root; 

//Exactly same structure but different type name. 
struct queue_root {
	struct queue_head *in_queue;
	struct queue_head *out_queue;
//	pthread_spinlock_t lock;
	//pthread_mutex_t lock;
	sgx_spinlock_t lock;
};


#ifndef _cas
# define _cas(ptr, oldval, newval) \
         __sync_bool_compare_and_swap(ptr, oldval, newval)
#endif

void inner_1_init(){
	return;
}

void INIT_QUEUE_HEAD(struct queue_head *head)
{
	head->next = (struct queue_head*)QUEUE_POS;
}

void queue_put(struct queue_head *new_node,
	       struct queue_root *root)
{
	sgx_spin_lock(&root->lock);
	while (1) {
		struct queue_head *in_queue = root->in_queue;
		new_node->next = in_queue;
		if (_cas(&root->in_queue, in_queue, new_node)) {
			break;
		}
	}
	//sleep(1);
	sgx_spin_unlock(&root->lock);
}

void initialize_queue(void *buf){
	root = (struct queue_root*)buf;	
	QUEUE_POS = (struct queue_root *)(buf + sizeof(struct queue_head));	
}

void ecall_test_producer(void *buf)
{
	int i;
	initialize_queue(buf);
//	printf("producer %x %x\n", root, root->in_queue);
//	printf("Put start\n");

	for (i=0; i < 2000000; i++) {
		struct queue_head *item = (struct queue_head*)((void *)QUEUE_POS + i*sizeof(struct queue_head));
		INIT_QUEUE_HEAD(item);
		//snprintf(item->buf, 128, "i = %d", i);
		queue_put(item, root);
		//printf("Put %s\n", item->buf);
	}
	printf("Put done\n");
//	sleep(1);
}

void sleep(int time)
{
	ocall_sleep(time);
}
/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
}
