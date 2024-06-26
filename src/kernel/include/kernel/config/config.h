#ifndef _KERNEL_CONFIG_CONFIG_H_
#define _KERNEL_CONFIG_CONFIG_H_ 1
#include <kernel/memory/smm.h>
#include <kernel/types.h>
#include <kernel/vfs/node.h>



#define CONFIG_TAG_TYPE_NONE 0
#define CONFIG_TAG_TYPE_ARRAY 1
#define CONFIG_TAG_TYPE_STRING 2
#define CONFIG_TAG_TYPE_INT 3
#define CONFIG_TAG_TYPE_INT_NEGATIVE 4

#define CONFIG_SAVE_FLAG_COMPRESS 1
#define CONFIG_SAVE_FLAG_TEXT 2

#define CONFIG_PASSWORD_MASTER_KEY ((void*)0xffffffffffffffffull)



typedef struct _CONFIG_TAG{
	struct _CONFIG_TAG* parent;
	string_t* name;
	u32 type;
	u32 _index;
	union{
		struct _CONFIG_TAG_ARRAY* array;
		string_t* string;
		s64 int_;
	};
} config_tag_t;



typedef struct _CONFIG_TAG_ARRAY{
	u32 length;
	config_tag_t* data[];
} config_tag_array_t;



config_tag_t* config_tag_create(u32 type,const char* name);



void config_tag_delete(config_tag_t* tag);



void config_tag_attach(config_tag_t* tag,config_tag_t* child);



void config_tag_detach(config_tag_t* child);



u64 config_tag_find(config_tag_t* tag,const char* name,u64 pointer,config_tag_t** out);



config_tag_t* config_tag_iter_start(config_tag_t* tag);



config_tag_t* config_tag_iter_next(config_tag_t* tag,config_tag_t* child);



config_tag_t* config_load(const void* data,u64 length,const char* password);



config_tag_t* config_load_from_file(vfs_node_t* file,const char* password);



bool config_save(const config_tag_t* tag,void** data,u64* length,const char* password,u32 flags);



bool config_save_to_file(const config_tag_t* tag,vfs_node_t* file,const char* password,u32 flags);



#endif
