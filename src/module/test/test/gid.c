#include <kernel/error/error.h>
#include <kernel/handle/handle.h>
#include <kernel/id/flags.h>
#include <kernel/id/group.h>
#include <kernel/log/log.h>
#include <kernel/memory/mmap.h>
#include <kernel/memory/pmm.h>
#include <kernel/mp/event.h>
#include <kernel/mp/process.h>
#include <kernel/mp/thread.h>
#include <kernel/scheduler/scheduler.h>
#include <kernel/types.h>
#include <kernel/util/util.h>
#include <test/test.h>
#define KERNEL_LOG_NAME "test"



extern error_t syscall_gid_get();
extern error_t syscall_gid_set();
extern error_t syscall_gid_get_name();



static void _thread(void){
	mmap_region_t* temp_mmap_region=mmap_alloc(&(THREAD_DATA->process->mmap),0,PAGE_SIZE,NULL,MMAP_REGION_FLAG_VMM_NOEXECUTE|MMAP_REGION_FLAG_VMM_READWRITE|MMAP_REGION_FLAG_VMM_USER,NULL,0);
	char* buffer=(void*)(temp_mmap_region->rb_node.key);
	TEST_FUNC("syscall_gid_get");
	TEST_GROUP("correct args");
	TEST_ASSERT(!syscall_gid_get());
	THREAD_DATA->process->gid=1000;
	TEST_ASSERT(syscall_gid_get()==1000);
	THREAD_DATA->process->gid=0;
	TEST_FUNC("syscall_gid_set");
	TEST_GROUP("not root");
	THREAD_DATA->process->uid=1000;
	THREAD_DATA->process->gid=1000;
	TEST_ASSERT(syscall_gid_set(1001)==ERROR_DENIED);
	THREAD_DATA->process->uid=0;
	THREAD_DATA->process->gid=0;
	TEST_GROUP("correct args");
	TEST_ASSERT(!syscall_gid_get());
	TEST_ASSERT(syscall_gid_set(1000)==ERROR_OK);
	TEST_ASSERT(syscall_gid_get()==1000);
	THREAD_DATA->process->gid=0;
	TEST_FUNC("syscall_gid_get_name");
	TEST_GROUP("invalid buffer length");
	TEST_ASSERT(syscall_gid_get_name(0,buffer,0)==ERROR_INVALID_ARGUMENT(2));
	TEST_GROUP("invalid buffer");
	TEST_ASSERT(syscall_gid_get_name(0,NULL,PAGE_SIZE)==ERROR_INVALID_ARGUMENT(1));
	TEST_GROUP("invalid gid");
	TEST_ASSERT(syscall_gid_get_name(0xaabbccdd,buffer,PAGE_SIZE)==ERROR_NOT_FOUND);
	TEST_GROUP("correct args");
	TEST_ASSERT(syscall_gid_get_name(0,buffer,PAGE_SIZE)==ERROR_OK);
	TEST_ASSERT(streq(buffer,"root"));
	mmap_dealloc_region(&(THREAD_DATA->process->mmap),temp_mmap_region);
}



void test_gid(void){
	TEST_MODULE("gid");
	TEST_FUNC("gid_create");
	TEST_GROUP("already present");
	TEST_ASSERT(gid_create(0,"root")==ERROR_ALREADY_PRESENT);
	TEST_GROUP("correct args");
	TEST_ASSERT(gid_create(1000,"user")==ERROR_OK);
	TEST_ASSERT(gid_delete(1000)==ERROR_OK);
	TEST_FUNC("gid_delete");
	TEST_GROUP("invalid gid");
	TEST_ASSERT(gid_delete(0xaabbccdd)==ERROR_NOT_FOUND);
	TEST_GROUP("correct args");
	TEST_ASSERT(gid_create(1000,"user")==ERROR_OK);
	TEST_ASSERT(gid_delete(1000)==ERROR_OK);
	TEST_ASSERT(gid_delete(1000)==ERROR_NOT_FOUND);
	TEST_FUNC("gid_get_name");
	TEST_GROUP("invalid buffer length");
	TEST_ASSERT(gid_get_name(0,NULL,0)==ERROR_NO_SPACE);
	char buffer[256];
	TEST_GROUP("invalid gid");
	TEST_ASSERT(gid_get_name(0xaabbccdd,buffer,256)==ERROR_NOT_FOUND);
	TEST_GROUP("correct args");
	TEST_ASSERT(gid_get_name(0,buffer,256)==ERROR_OK);
	TEST_ASSERT(streq(buffer,"root"));
	TEST_FUNC("gid_get_flags");
	TEST_GROUP("invalid gid");
	TEST_ASSERT(gid_get_flags(0xaabbccdd)==ERROR_NOT_FOUND);
	TEST_GROUP("correct args");
	TEST_ASSERT(!gid_get_flags(0));
	TEST_ASSERT(gid_set_flags(0,0,ID_FLAG_BYPASS_VFS_PERMISSIONS)==ERROR_OK);
	TEST_ASSERT(gid_get_flags(0)==ID_FLAG_BYPASS_VFS_PERMISSIONS);
	TEST_ASSERT(gid_set_flags(0,ID_FLAG_BYPASS_VFS_PERMISSIONS,0)==ERROR_OK);
	TEST_FUNC("gid_set_flags");
	TEST_GROUP("invalid gid");
	TEST_ASSERT(gid_set_flags(0xaabbccdd,0,0)==ERROR_NOT_FOUND);
	TEST_GROUP("correct args");
	TEST_ASSERT(!gid_get_flags(0));
	TEST_ASSERT(gid_set_flags(0,0,ID_FLAG_BYPASS_VFS_PERMISSIONS)==ERROR_OK);
	TEST_ASSERT(gid_get_flags(0)==ID_FLAG_BYPASS_VFS_PERMISSIONS);
	TEST_ASSERT(gid_set_flags(0,ID_FLAG_BYPASS_VFS_PERMISSIONS,ID_FLAG_BYPASS_VFS_PERMISSIONS|ID_FLAG_ROOT_ACL)==ERROR_OK);
	TEST_ASSERT(gid_get_flags(0)==(ID_FLAG_BYPASS_VFS_PERMISSIONS|ID_FLAG_ROOT_ACL));
	TEST_ASSERT(gid_set_flags(0,ID_FLAG_BYPASS_VFS_PERMISSIONS|ID_FLAG_ROOT_ACL,0)==ERROR_OK);
	TEST_ASSERT(!gid_get_flags(0));
	process_t* test_process=process_create("test-process","test-process");
	scheduler_enqueue_thread(thread_create_kernel_thread(test_process,"test-gid-thread",_thread,0x200000,0));
	event_await(test_process->event,0);
}