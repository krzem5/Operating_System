#ifndef _OPENGL__INTERNAL_STATE_H_
#define _OPENGL__INTERNAL_STATE_H_ 1
#include <GL/gl.h>
#include <glsl/backend.h>
#include <glsl/compiler.h>
#include <glsl/linker.h>
#include <sys/types.h>



#define OPENGL_HANDLE_TYPE_NONE 0
#define OPENGL_HANDLE_TYPE_SHADER 1
#define OPENGL_HANDLE_TYPE_PROGRAM 2
#define OPENGL_HANDLE_TYPE_VERTEX_ARRAY 3
#define OPENGL_HANDLE_TYPE_BUFFER 4
#define OPENGL_HANDLE_TYPE_TEXTURE 5

#define OPENGL_MAX_VERTEX_ATTRIBUTES 16
#define OPENGL_MAX_CONST_VERTEX_ELEMENT_SIZE (4*sizeof(double))

#define OPENGL_MAX_ACTIVE_TEXTURES 48

#define OPENGL_BUFFER_STORAGE_TYPE_NONE 0
#define OPENGL_BUFFER_STORAGE_TYPE_STREAM 1
#define OPENGL_BUFFER_STORAGE_TYPE_STATIC 2
#define OPENGL_BUFFER_STORAGE_TYPE_DYNAMIC 3



typedef u32 opengl_handle_type_t;



typedef u64 opengl_driver_instance_t;



typedef u64 opengl_state_id_t;



typedef void* opengl_state_t;



typedef struct _OPENGL_SHADER_SOURCE{
	GLchar* data;
	GLuint64 length;
} opengl_shader_source_t;



typedef struct _OPENGL_HANDLE_HEADER{
	opengl_handle_type_t type;
	GLuint index;
} opengl_handle_header_t;



typedef struct _OPENGL_PROGRAM_STATE{
	opengl_handle_header_t header;
	glsl_linker_program_t linker_program;
	GLboolean was_linkage_attempted;
	glsl_linker_linked_program_t linked_program;
	glsl_error_t error;
	u64 driver_handle;
	void* uniform_data;
	u32 uniform_data_size;
	u32 sampler_texture_units[OPENGL_MAX_ACTIVE_TEXTURES];
	u64 sampler_texture_unit_bitmap;
	u64 sampler_texture_unit_force_update_bitmap;
} opengl_program_state_t;



typedef struct _OPENGL_SHADER_STATE{
	opengl_handle_header_t header;
	GLenum type;
	GLuint source_count;
	opengl_shader_source_t* sources;
	GLboolean was_compilation_attempted;
	glsl_compilation_output_t compilation_output;
	glsl_error_t error;
	opengl_program_state_t* program;
} opengl_shader_state_t;



typedef struct _OPENGL_VERTEX_ARRAY_STATE_ENTRY{
	GLenum size;
	GLenum type;
	GLboolean normalized;
	GLuint divisor;
	GLsizei stride;
	GLuint64 offset;
} opengl_vertex_array_state_entry_t;



typedef struct _OPENGL_VERTEX_ARRAY_STATE{
	opengl_handle_header_t header;
	u16 enabled_entry_mask; // same number of bits as OPENGL_MAX_VERTEX_ATTRIBUTES
	opengl_vertex_array_state_entry_t entries[OPENGL_MAX_VERTEX_ATTRIBUTES*2];
	u64 driver_handle;
	GLuint stride;
	GLboolean needs_update;
	u64 const_vertex_buffer_driver_handle;
} opengl_vertex_array_state_t;



typedef struct _OPENGL_BUFFER_STATE{
	opengl_handle_header_t header;
	u64 driver_handle;
	GLuint type;
	bool is_mapped;
	GLuint64 size;
} opengl_buffer_state_t;



typedef struct _OPENGL_TEXTURE_STATE{
	opengl_handle_header_t header;
	u64 driver_handle;
	u64 data_version;
	GLenum target;
	GLenum parameter_wrap_s;
	GLenum parameter_wrap_t;
	GLenum parameter_wrap_r;
	GLenum parameter_min_filter;
	GLenum parameter_mag_filter;
	GLenum parameter_compare_func;
	GLenum parameter_compare_mode;
	GLfloat parameter_lod_bias;
	GLfloat parameter_min_lod;
	GLfloat parameter_max_lod;
	GLfloat parameter_border_color[4];
	GLenum parameter_swizzle_r;
	GLenum parameter_swizzle_g;
	GLenum parameter_swizzle_b;
	GLenum parameter_swizzle_a;
} opengl_texture_state_t;



typedef struct _OPENGL_DRIVER_INSTANCE_DATA{
	u16 opengl_version;
	char driver_name[32];
	char renderer_name[64];
	char library[128];
} opengl_driver_instance_data_t;



typedef struct _OPENGL_INTERNAL_STATE{
	opengl_state_id_t state_id;
	opengl_driver_instance_t driver_instance;
	u16 driver_opengl_version;
	const glsl_backend_descriptor_t* glsl_backend_descriptor;
	char gl_renderer[64];
	char gl_shading_language_version[16];
	char gl_vendor[32];
	char gl_version[16];
	GLenum gl_error;
	GLuint gl_active_texture;
	GLuint gl_active_textures[OPENGL_MAX_ACTIVE_TEXTURES];
	u64 gl_active_texture_data_versions[OPENGL_MAX_ACTIVE_TEXTURES];
	u64 gl_active_texture_driver_handles[OPENGL_MAX_ACTIVE_TEXTURES];
	u64 gl_active_texture_bitmap;
	GLuint gl_bound_array_buffer;
	GLuint gl_bound_index_buffer;
	GLuint64 gl_bound_index_offset;
	GLuint gl_bound_index_width;
	GLuint gl_bound_vertex_array;
	GLfloat gl_clear_color_value[4];
	GLdouble gl_clear_depth_value;
	GLint gl_clear_stencil_value;
	bool gl_constant_buffer_needs_update;
	GLuint gl_primitive_restart_index;
	GLuint gl_used_array_buffer;
	GLuint gl_used_index_buffer;
	GLuint64 gl_used_index_offset;
	GLuint gl_used_index_width;
	GLuint gl_used_program;
	GLuint gl_used_texture_2d;
	GLuint gl_used_vertex_array;
	GLint gl_viewport[4];
	opengl_handle_header_t** handles;
	GLuint handle_count;
} opengl_internal_state_t;



extern opengl_state_id_t opengl_current_state_id;



#endif
