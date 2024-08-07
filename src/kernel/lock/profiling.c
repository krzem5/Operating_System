#ifndef KERNEL_RELEASE
#include <kernel/clock/clock.h>
#include <kernel/cpu/local.h>
#include <kernel/lock/bitlock.h>
#include <kernel/lock/profiling.h>
#include <kernel/log/log.h>
#include <kernel/mp/thread.h>
#include <kernel/symbol/symbol.h>
#include <kernel/types.h>
#include <kernel/util/util.h>



#define GLOBAL_LOCK_TYPE_BIT 0
#define GLOBAL_LOCK_STAT_BIT 1



static lock_profiling_descriptor_t _lock_profiling_lock_types[LOCK_PROFILING_MAX_LOCK_TYPES];
static KERNEL_ATOMIC u32 _lock_profiling_lock_type_count=0;
static u32 _lock_profiling_global_locks=0;
static u64* KERNEL_INIT_WRITE _lock_profiling_dependency_matrix=NULL;
static __lock_profiling_lock_stack_t KERNEL_EARLY_WRITE _lock_profiling_global_lock_stack={
	.size=0
};
static CPU_LOCAL_DATA(__lock_profiling_lock_stack_t,_lock_profiling_cpu_local_data);
static lock_profiling_stats_t* KERNEL_INIT_WRITE _lock_profiling_lock_stats=NULL;
static u32 _lock_profiling_lock_stat_count=0;
static bool _lock_profiling_address_of_static_lock=0;



static KERNEL_INLINE __lock_profiling_lock_stack_t* KERNEL_NOCOVERAGE _get_lock_stack(void){
	if (!_lock_profiling_cpu_local_data){
		return &_lock_profiling_global_lock_stack;
	}
	if (CPU_HEADER_DATA->current_thread){
		return &(CPU_HEADER_DATA->current_thread->__lock_profiling_lock_stack);
	}
	// return NULL; // no profiling in the scheduler
	return CPU_LOCAL(_lock_profiling_cpu_local_data);
}



static KERNEL_INLINE u32 KERNEL_NOCOVERAGE _get_edge_index(u32 from,u32 to){
	return (from<<LOCK_PROFILING_MAX_LOCK_TYPES_SHIFT)|to;
}



static void KERNEL_NOCOVERAGE _print_error(u32 address,u32 a,u32 b,const char* message){
	const symbol_t* current_symbol=symbol_lookup(address|0xffffffff00000000ull);
	const symbol_t* first_lock_symbol=symbol_lookup((_lock_profiling_lock_types+a)->address|0xffffffff00000000ull);
	const symbol_t* second_lock_symbol=symbol_lookup((_lock_profiling_lock_types+b)->address|0xffffffff00000000ull);
	log_direct("\x1b[1m\x1b[38;2;41;137;255m%s:%s+%u: %s:%s+%u and %s:%s+%u: %s\x1b[0m\n",
		current_symbol->module,current_symbol->name->data,(address|0xffffffff00000000ull)-current_symbol->rb_node.key,
		first_lock_symbol->module,first_lock_symbol->name->data,((_lock_profiling_lock_types+a)->address|0xffffffff00000000ull)-first_lock_symbol->rb_node.key,
		second_lock_symbol->module,second_lock_symbol->name->data,((_lock_profiling_lock_types+b)->address|0xffffffff00000000ull)-second_lock_symbol->rb_node.key,
		message
	);
}



static void KERNEL_NOCOVERAGE _alloc_lock_type(u32 flags,__lock_profiling_data_t* out){
	bitlock_acquire(&_lock_profiling_global_locks,GLOBAL_LOCK_TYPE_BIT);
	out->id=_lock_profiling_lock_type_count;
	(_lock_profiling_lock_types+_lock_profiling_lock_type_count)->id=_lock_profiling_lock_type_count;
	(_lock_profiling_lock_types+_lock_profiling_lock_type_count)->address=out->alloc_address;
	(_lock_profiling_lock_types+_lock_profiling_lock_type_count)->flags=flags;
	_lock_profiling_lock_type_count++;
	bitlock_release(&_lock_profiling_global_locks,GLOBAL_LOCK_TYPE_BIT);
}



KERNEL_PUBLIC void KERNEL_NOCOVERAGE KERNEL_NOINLINE __lock_profiling_init(u32 flags,__lock_profiling_data_t* out){
	out->alloc_address=(u32)(u64)__builtin_return_address(1);
	u32 type_count=_lock_profiling_lock_type_count;
	for (u32 i=0;i<type_count;i++){
		if ((_lock_profiling_lock_types+i)->address==out->alloc_address){
			out->id=i;
			return;
		}
	}
	_alloc_lock_type(flags,out);
}



void KERNEL_NOCOVERAGE KERNEL_NOINLINE __lock_profiling_init_lock_stack(__lock_profiling_lock_stack_t* out){
	out->size=0;
}



KERNEL_PUBLIC void KERNEL_NOCOVERAGE KERNEL_NOINLINE __lock_profiling_acquire_start(__lock_profiling_data_t* lock,__lock_profiling_acquisition_context_t* ctx){
	if (!_lock_profiling_dependency_matrix||!_lock_profiling_lock_stats){
		return;
	}
	__lock_profiling_lock_stack_t* stack=_get_lock_stack();
	if (!stack||stack->disabled){
		return;
	}
	if (lock->id>=_lock_profiling_lock_type_count||lock->alloc_address!=(_lock_profiling_lock_types+lock->id)->address){
		lock->alloc_address=(u32)(u64)(&_lock_profiling_address_of_static_lock);
		_alloc_lock_type(0,lock);
	}
	stack->disabled=1;
	u32 address=(u64)__builtin_return_address(1);
	u32 stat_count=_lock_profiling_lock_stat_count;
	for (u32 i=0;i<stat_count;i++){
		if ((_lock_profiling_lock_stats+i)->id==lock->id&&(_lock_profiling_lock_stats+i)->address==address){
			ctx->stat_id=i;
			goto _skip_stat_alloc;
		}
	}
	bitlock_acquire(&_lock_profiling_global_locks,GLOBAL_LOCK_STAT_BIT);
	ctx->stat_id=_lock_profiling_lock_stat_count;
	(_lock_profiling_lock_stats+ctx->stat_id)->id=lock->id;
	(_lock_profiling_lock_stats+ctx->stat_id)->address=address;
	_lock_profiling_lock_stat_count++;
	bitlock_release(&_lock_profiling_global_locks,GLOBAL_LOCK_STAT_BIT);
_skip_stat_alloc:
	if (stack->size>=LOCK_PROFILING_MAX_NESTED_LOCKS){
		log_direct("\x1b[1m\x1b[38;2;41;137;255m: Lock stack too small\x1b[0m\n");
		panic("lock error");
	}
	for (u64 i=0;i<stack->size;i++){
		if (stack->data[i]->id==lock->id&&stack->data[i]!=lock){
			continue;
		}
		if (((_lock_profiling_lock_types+lock->id)->flags&LOCK_PROFILING_FLAG_PREEMPTIBLE)&&!((_lock_profiling_lock_types+stack->data[i]->id)->flags&LOCK_PROFILING_FLAG_PREEMPTIBLE)){
			_print_error(address,lock->id,stack->data[i]->id,"Priority inversion");
		}
		u32 edge=_get_edge_index(stack->data[i]->id,lock->id);
		_lock_profiling_dependency_matrix[edge>>6]|=1ull<<(edge&63);
		u32 inverse_edge=_get_edge_index(lock->id,stack->data[i]->id);
		if (_lock_profiling_dependency_matrix[inverse_edge>>6]&(1ull<<(inverse_edge&63))){
			_print_error(address,lock->id,stack->data[i]->id,"Deadlock");
		}
	}
	stack->data[stack->size]=lock;
	stack->size++;
	ctx->start_ticks=clock_get_ticks();
	stack->disabled=0;
}



KERNEL_PUBLIC void KERNEL_NOCOVERAGE KERNEL_NOINLINE __lock_profiling_acquire_end(__lock_profiling_data_t* lock,__lock_profiling_acquisition_context_t* ctx){
	u64 end_ticks=clock_get_ticks();
	lock->holder=(!_lock_profiling_cpu_local_data?0xffffffffffffffffull:(CPU_HEADER_DATA->current_thread?(u64)CPU_HEADER_DATA->current_thread:CPU_HEADER_DATA->index));
	if (!_lock_profiling_dependency_matrix||!_lock_profiling_lock_stats){
		return;
	}
	__lock_profiling_lock_stack_t* stack=_get_lock_stack();
	if (!stack||stack->disabled){
		return;
	}
	u64 ticks=end_ticks-ctx->start_ticks;
	lock_profiling_stats_t* stats=_lock_profiling_lock_stats+ctx->stat_id;
	__atomic_add_fetch(&(stats->ticks),ticks,__ATOMIC_SEQ_CST);
	__atomic_add_fetch(&(stats->count),1,__ATOMIC_SEQ_CST);
	if (ticks>stats->max_ticks){
		__atomic_store_n(&(stats->max_ticks),ticks,__ATOMIC_SEQ_CST);
	}
}



KERNEL_PUBLIC void KERNEL_NOCOVERAGE KERNEL_NOINLINE __lock_profiling_release(__lock_profiling_data_t* lock){
	lock->holder=0;
	if (!_lock_profiling_dependency_matrix||!_lock_profiling_lock_stats){
		return;
	}
	__lock_profiling_lock_stack_t* stack=_get_lock_stack();
	if (!stack||stack->disabled){
		return;
	}
	stack->disabled=1;
	u64 i=0;
	for (;i<stack->size&&stack->data[i]!=lock;i++);
	if (i==stack->size){
		log_direct("\x1b[1m\x1b[38;2;41;137;255mLock '%p' not acquired in this context\x1b[0m\n",lock);
		panic("lock error");
	}
	stack->size--;
	stack->data[i]=stack->data[stack->size];
	stack->disabled=0;
}



KERNEL_PUBLIC void KERNEL_NOCOVERAGE KERNEL_NOINLINE __lock_profiling_assert_empty(__lock_profiling_lock_stack_t* stack){
	if (!stack){
		stack=CPU_LOCAL(_lock_profiling_cpu_local_data);
	}
	if (!stack||stack->disabled||!stack->size){
		return;
	}
	log_direct("\x1b[1m\x1b[38;2;41;137;255mLock stack not empty\x1b[0m\n");
	for (u32 i=0;i<stack->size;i++){
		const symbol_t* symbol=symbol_lookup(stack->data[i]->alloc_address|0xffffffff00000000ull);
		log_direct("\x1b[1m\x1b[38;2;41;137;255m  [%u]: %s:%s+%u\x1b[0m\n",i,symbol->module,symbol->name->data,(stack->data[i]->alloc_address|0xffffffff00000000ull)-symbol->rb_node.key);
	}
	panic("lock error");
}



void KERNEL_NOCOVERAGE KERNEL_EARLY_EXEC lock_profiling_enable_dependency_graph(void){
	pmm_counter_descriptor_t* counter=pmm_alloc_counter("kernel.lock_profiling");
	_lock_profiling_dependency_matrix=(void*)(pmm_alloc(pmm_align_up_address(1<<(LOCK_PROFILING_MAX_LOCK_TYPES_SHIFT<<1))>>PAGE_SIZE_SHIFT,counter,0)+VMM_HIGHER_HALF_ADDRESS_OFFSET);
	_lock_profiling_lock_stats=(void*)(pmm_alloc(pmm_align_up_address(LOCK_PROFILING_MAX_LOCK_USES*sizeof(lock_profiling_stats_t))>>PAGE_SIZE_SHIFT,counter,0)+VMM_HIGHER_HALF_ADDRESS_OFFSET);
}



KERNEL_PUBLIC bool lock_profiling_get_descriptor(u32 index,lock_profiling_descriptor_t* out){
	if (index>=_lock_profiling_lock_type_count){
		return 0;
	}
	*out=*(_lock_profiling_lock_types+index);
	return 1;
}



KERNEL_PUBLIC bool lock_profiling_get_stats(u32 index,lock_profiling_stats_t* out){
	if (index>=_lock_profiling_lock_stat_count){
		return 0;
	}
	*out=*(_lock_profiling_lock_stats+index);
	return 1;
}



#else
#include <kernel/lock/profiling.h>
#include <kernel/types.h>



KERNEL_PUBLIC void KERNEL_NOCOVERAGE KERNEL_NOINLINE __lock_profiling_init(u32 flags,__lock_profiling_data_t* out){
	return;
}



void KERNEL_NOCOVERAGE KERNEL_NOINLINE __lock_profiling_init_lock_stack(__lock_profiling_lock_stack_t* out){
	return;
}



KERNEL_PUBLIC void KERNEL_NOCOVERAGE KERNEL_NOINLINE __lock_profiling_acquire_start(__lock_profiling_data_t* lock,__lock_profiling_acquisition_context_t* ctx){
	return;
}



KERNEL_PUBLIC void KERNEL_NOCOVERAGE KERNEL_NOINLINE __lock_profiling_acquire_end(__lock_profiling_data_t* lock,__lock_profiling_acquisition_context_t* ctx){
	return;
}



KERNEL_PUBLIC void KERNEL_NOCOVERAGE KERNEL_NOINLINE __lock_profiling_release(__lock_profiling_data_t* lock){
	return;
}



KERNEL_PUBLIC void KERNEL_NOCOVERAGE KERNEL_NOINLINE __lock_profiling_assert_empty(__lock_profiling_lock_stack_t* stack){
	return;
}



void KERNEL_NOCOVERAGE KERNEL_EARLY_EXEC lock_profiling_enable_dependency_graph(void){
	return;
}



KERNEL_PUBLIC bool lock_profiling_get_descriptor(u32 index,lock_profiling_descriptor_t* out){
	return 0;
}



KERNEL_PUBLIC bool lock_profiling_get_stats(u32 index,lock_profiling_stats_t* out){
	return 0;
}



#endif
