#include <kernel/log/log.h>
#include <kernel/memory/omm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/smm.h>
#include <kernel/symbol/symbol.h>
#include <kernel/tree/rb_tree.h>
#include <kernel/types.h>
#include <kernel/util/string.h>
#include <kernel/scheduler/scheduler.h>
#include <kernel/util/util.h>
#define KERNEL_LOG_NAME "symbol"



static omm_allocator_t* KERNEL_INIT_WRITE _symbol_allocator=NULL;
static rb_tree_t _symbol_tree;

volatile const u64* KERNEL_EARLY_READ __kernel_symbol_data=NULL;



KERNEL_EARLY_EARLY_INIT(){
	LOG("Initializing symbol tree...");
	_symbol_allocator=omm_init("kernel.symbol",sizeof(symbol_t),8,2);
	rwlock_init(&(_symbol_allocator->lock));
	rb_tree_init(&_symbol_tree);
	for (u32 i=0;__kernel_symbol_data[i];i+=2){
		symbol_add("kernel",(const char*)(__kernel_symbol_data[i+1]),__kernel_symbol_data[i]|0x8000000000000000ull,__kernel_symbol_data[i]>>63);
	}
}



void symbol_add(const char* module,const char* name,u64 address,bool is_public){
	symbol_t* symbol=(symbol_t*)rb_tree_lookup_node(&_symbol_tree,address);
	if (symbol){
		ERROR("Conflicting symbols: '%s' and '%s'",name,symbol->name->data);
		panic("Conflicting symbols");
	}
	symbol=omm_alloc(_symbol_allocator);
	symbol->rb_node.key=address;
	symbol->module=module;
	symbol->name=smm_alloc(name,0);
	symbol->is_public=is_public;
	rb_tree_insert_node(&_symbol_tree,&(symbol->rb_node));
}



void symbol_remove(const char* module){
	for (u64 address=0;1;){
		symbol_t* symbol=(symbol_t*)rb_tree_lookup_increasing_node(&_symbol_tree,address);
		if (!symbol){
			return;
		}
		address=symbol->rb_node.key+1;
		if (str_equal(symbol->module,module)){
			rb_tree_remove_node(&_symbol_tree,&(symbol->rb_node));
			smm_dealloc(symbol->name);
			omm_dealloc(_symbol_allocator,symbol);
		}
	}
}



KERNEL_PUBLIC const symbol_t* symbol_lookup(u64 address){
	return (const symbol_t*)rb_tree_lookup_decreasing_node(&_symbol_tree,address);
}



KERNEL_PUBLIC const symbol_t* symbol_lookup_by_name(const char* name){
	SMM_TEMPORARY_STRING str=smm_alloc(name,0);
	scheduler_pause();
	for (rb_tree_node_t* rb_node=rb_tree_iter_start(&_symbol_tree);rb_node;rb_node=rb_tree_iter_next(&_symbol_tree,rb_node)){
		const symbol_t* symbol=(const symbol_t*)rb_node;
		if (smm_equal(symbol->name,str)){
			scheduler_resume(1);
			return symbol;
		}
	}
	scheduler_resume(1);
	return NULL;
}
