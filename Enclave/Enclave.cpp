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


#include <stdarg.h>
#include <stdio.h>      /* vsnprintf */
#include <string.h>      
#include <sgx_thread.h>
#include <sgx_spinlock.h>

#include "Enclave.h"
#include "Enclave_t.h"  /* print_string */

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


void INIT_QUEUE_HEAD(struct queue_head *head)
{
	head->next = (struct queue_head*)QUEUE_POS;
}

void queue_put(struct queue_head *new_node,
	       struct queue_root *root)
{
	while (1) {
		struct queue_head *in_queue = root->in_queue;
		new_node->next = in_queue;
		if (_cas(&root->in_queue, in_queue, new_node)) {
			break;
		}
	}
}

struct queue_head *queue_get(struct queue_root *root)
{
//	pthread_spin_lock(&root->lock);
	//pthread_mutex_lock(&root->lock);
	sgx_spin_lock(&root->lock);

	if (!root->out_queue) {
		while (1) {
			struct queue_head *head = root->in_queue;
			if (!head) {
				break;
			}
			if (_cas(&root->in_queue, head, NULL)) {
				// Reverse the order
				while (head) {
					struct queue_head *next = head->next;
					head->next = root->out_queue;
					root->out_queue = head;
					head = next;
				}
				break;
			}
		}
	}

	struct queue_head *head = root->out_queue;
	if (head) {
		root->out_queue = head->next;
	}
//	pthread_spin_unlock(&root->lock);
	//pthread_mutex_unlock(&root->lock);
	sgx_spin_unlock(&root->lock);
	return head;
}

void initialize_queue(void *buf){
	root = (struct queue_root*)buf;	
	QUEUE_POS = (struct queue_root *)(buf + sizeof(struct queue_head));	
}

void ecall_test_consumer(void *buf)
{
	initialize_queue(buf);

	int cnt = 0, total = 128;
	struct queue_head* head = NULL;
	while(total){
		cnt++;
		if(cnt > 3)
			sleep(1);
		head = queue_get(root);	
		if(head != NULL)
		{
			printf("%s\n", head->buf);
			cnt=0;
			total--;
			head = NULL;
		}
	}

//	printf("Child read: %s\n", (char *)buf);
//	memcpy(buf, child_message, sizeof(child_message));
//	printf("Child wrote: %s\n", (char *)buf);
}

void ecall_test_producer(void *buf)
{
	int i;
	initialize_queue(buf);
	printf("Put start\n");
	for (i=0; i < 128; i++) {
		struct queue_head *item = (struct queue_head*)((void *)QUEUE_POS + i*sizeof(struct queue_head));
		INIT_QUEUE_HEAD(item);
		snprintf(item->buf, 128, "i = %d", i);
		queue_put(item, root);
		//printf("Put %s\n", item->buf);
	}
	printf("Put done\n");
//	sleep(1);
//	printf("After 1s, parent read: %s\n", (char *)buf);
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
