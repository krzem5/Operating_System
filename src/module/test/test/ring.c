#include <kernel/log/log.h>
#include <kernel/random/random.h>
#include <kernel/ring/ring.h>
#include <kernel/types.h>
#include <test/test.h>
#define KERNEL_LOG_NAME "test"



#define TEST_RING_SIZE 64
#define TEST_BUFFER_SIZE 8



KERNEL_AWAITS void test_ring(void){
	TEST_MODULE("ring");
	TEST_FUNC("ring_create");
	TEST_GROUP("empty ring");
	TEST_ASSERT(!ring_init("test.ring",0));
	TEST_GROUP("capacity not a power of 2");
	TEST_ASSERT(!ring_init("test.ring",3));
	TEST_GROUP("correct args");
	ring_t* ring=ring_init("test.ring",TEST_RING_SIZE);
	TEST_ASSERT(ring);
	ring_deinit(ring);
	TEST_FUNC("ring_push");
	ring=ring_init("test.ring",TEST_RING_SIZE);
	void* test_buffer[TEST_BUFFER_SIZE];
	TEST_GROUP("correct args");
	random_generate(test_buffer,TEST_BUFFER_SIZE*sizeof(void*));
	for (u32 i=0;i<TEST_BUFFER_SIZE;i++){
		TEST_ASSERT(ring_push(ring,test_buffer[i],0));
	}
	for (u32 i=0;i<TEST_BUFFER_SIZE;i++){
		TEST_ASSERT(ring_pop(ring,0)==test_buffer[i]);
	}
	TEST_GROUP("full ring");
	for (u32 i=0;i<TEST_RING_SIZE;i++){
		TEST_ASSERT(ring_push(ring,(void*)12345,0));
	}
	TEST_ASSERT(!ring_push(ring,(void*)12345,0));
	for (u32 i=0;i<TEST_RING_SIZE;i++){
		TEST_ASSERT(ring_pop(ring,0)==((void*)12345));
	}
	TEST_FUNC("ring_pop");
	TEST_GROUP("empty ring");
	TEST_ASSERT(!ring_pop(ring,0));
	TEST_GROUP("correct args");
	random_generate(test_buffer,TEST_BUFFER_SIZE*sizeof(void*));
	for (u32 i=0;i<TEST_BUFFER_SIZE;i++){
		TEST_ASSERT(ring_push(ring,test_buffer[i],0));
	}
	for (u32 i=0;i<TEST_BUFFER_SIZE;i++){
		TEST_ASSERT(ring_pop(ring,0)==test_buffer[i]);
	}
	TEST_FUNC("ring_peek");
	TEST_GROUP("empty ring");
	TEST_ASSERT(!ring_peek(ring,0));
	TEST_GROUP("correct args");
	random_generate(test_buffer,TEST_BUFFER_SIZE*sizeof(void*));
	for (u32 i=0;i<TEST_BUFFER_SIZE;i++){
		TEST_ASSERT(ring_push(ring,test_buffer[i],0));
	}
	for (u32 i=0;i<TEST_BUFFER_SIZE;i++){
		TEST_ASSERT(ring_peek(ring,0)==test_buffer[i]);
		TEST_ASSERT(ring_pop(ring,0)==test_buffer[i]);
	}
	ring_deinit(ring);
}
