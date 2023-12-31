#include <GL/gl.h>
#include <opengl/command_buffer.h>
#include <opengl/protocol.h>
#include <sys/io.h>



static GLenum _gl_error=GL_NO_ERROR;
static GLfloat _gl_clear_color_value[4]={0.0f,0.0f,0.0f,1.0f};
static GLdouble _gl_clear_depth_value=0.0f;
static GLint _gl_clear_stencil_value=0;
static GLint _gl_viewport[4]={0,0,0,0};



static inline void _push_single_command(const opengl_protocol_header_t* header){
	opengl_command_buffer_set_lock(1);
	opengl_command_buffer_ensure_space(header->length);
	opengl_command_buffer_push(header);
	opengl_command_buffer_set_lock(0);
}



SYS_PUBLIC void glActiveTexture(GLenum texture){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glActiveTexture\x1b[0m\n");
}



SYS_PUBLIC void glAttachShader(GLuint program,GLuint shader){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glAttachShader\x1b[0m\n");
}



SYS_PUBLIC void glBeginConditionalRender(GLuint id,GLenum mode){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBeginConditionalRender\x1b[0m\n");
}



SYS_PUBLIC void glBeginQuery(GLenum target,GLuint id){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBeginQuery\x1b[0m\n");
}



SYS_PUBLIC void glBeginTransformFeedback(GLenum primitiveMode){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBeginTransformFeedback\x1b[0m\n");
}



SYS_PUBLIC void glBindAttribLocation(GLuint program,GLuint index,const GLchar* name){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBindAttribLocation\x1b[0m\n");
}



SYS_PUBLIC void glBindBuffer(GLenum target,GLuint buffer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBindBuffer\x1b[0m\n");
}



SYS_PUBLIC void glBindBufferBase(GLenum target,GLuint index,GLuint buffer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBindBufferBase\x1b[0m\n");
}



SYS_PUBLIC void glBindBufferRange(GLenum target,GLuint index,GLuint buffer,GLintptr offset,GLsizeiptr size){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBindBufferRange\x1b[0m\n");
}



SYS_PUBLIC void glBindFragDataLocation(GLuint program,GLuint color,const GLchar* name){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBindFragDataLocation\x1b[0m\n");
}



SYS_PUBLIC void glBindFragDataLocationIndexed(GLuint program,GLuint colorNumber,GLuint index,const GLchar* name){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBindFragDataLocationIndexed\x1b[0m\n");
}



SYS_PUBLIC void glBindFramebuffer(GLenum target,GLuint framebuffer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBindFramebuffer\x1b[0m\n");
}



SYS_PUBLIC void glBindRenderbuffer(GLenum target,GLuint renderbuffer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBindRenderbuffer\x1b[0m\n");
}



SYS_PUBLIC void glBindSampler(GLuint unit,GLuint sampler){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBindSampler\x1b[0m\n");
}



SYS_PUBLIC void glBindTexture(GLenum target,GLuint texture){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBindTexture\x1b[0m\n");
}



SYS_PUBLIC void glBindVertexArray(GLuint array){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBindVertexArray\x1b[0m\n");
}



SYS_PUBLIC void glBlendColor(GLfloat red,GLfloat green,GLfloat blue,GLfloat alpha){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBlendColor\x1b[0m\n");
}



SYS_PUBLIC void glBlendEquation(GLenum mode){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBlendEquation\x1b[0m\n");
}



SYS_PUBLIC void glBlendEquationSeparate(GLenum modeRGB,GLenum modeAlpha){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBlendEquationSeparate\x1b[0m\n");
}



SYS_PUBLIC void glBlendFunc(GLenum sfactor,GLenum dfactor){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBlendFunc\x1b[0m\n");
}



SYS_PUBLIC void glBlendFuncSeparate(GLenum sfactorRGB,GLenum dfactorRGB,GLenum sfactorAlpha,GLenum dfactorAlpha){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBlendFuncSeparate\x1b[0m\n");
}



SYS_PUBLIC void glBlitFramebuffer(GLint srcX0,GLint srcY0,GLint srcX1,GLint srcY1,GLint dstX0,GLint dstY0,GLint dstX1,GLint dstY1,GLbitfield mask,GLenum filter){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBlitFramebuffer\x1b[0m\n");
}



SYS_PUBLIC void glBufferData(GLenum target,GLsizeiptr size,const void* data,GLenum usage){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBufferData\x1b[0m\n");
}



SYS_PUBLIC void glBufferSubData(GLenum target,GLintptr offset,GLsizeiptr size,const void* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glBufferSubData\x1b[0m\n");
}



SYS_PUBLIC GLenum glCheckFramebufferStatus(GLenum target){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCheckFramebufferStatus\x1b[0m\n");
	return 0;
}



SYS_PUBLIC void glClampColor(GLenum target,GLenum clamp){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glClampColor\x1b[0m\n");
}



SYS_PUBLIC void glClear(GLbitfield mask){
	if (!mask){
		return;
	}
	if (mask&(~(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT))){
		_gl_error=GL_INVALID_VALUE;
		return;
	}
	u32 flags=0;
	if (mask&GL_COLOR_BUFFER_BIT){
		flags|=OPENGL_PROTOCOL_CLEAR_FLAG_COLOR;
	}
	if (mask&GL_DEPTH_BUFFER_BIT){
		flags|=OPENGL_PROTOCOL_CLEAR_FLAG_DEPTH;
	}
	if (mask&GL_STENCIL_BUFFER_BIT){
		flags|=OPENGL_PROTOCOL_CLEAR_FLAG_STENCIL;
	}
	opengl_protocol_clear_t command={
		.header.type=OPENGL_PROTOCOL_TYPE_CLEAR,
		.header.length=sizeof(opengl_protocol_clear_t),
		.flags=flags,
		.color[0]=_gl_clear_color_value[0],
		.color[1]=_gl_clear_color_value[1],
		.color[2]=_gl_clear_color_value[2],
		.color[3]=_gl_clear_color_value[3],
		.depth=_gl_clear_depth_value,
		.stencil=_gl_clear_stencil_value,
	};
	_push_single_command(&(command.header));
}



SYS_PUBLIC void glClearBufferfi(GLenum buffer,GLint drawbuffer,GLfloat depth,GLint stencil){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glClearBufferfi\x1b[0m\n");
}



SYS_PUBLIC void glClearBufferfv(GLenum buffer,GLint drawbuffer,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glClearBufferfv\x1b[0m\n");
}



SYS_PUBLIC void glClearBufferiv(GLenum buffer,GLint drawbuffer,const GLint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glClearBufferiv\x1b[0m\n");
}



SYS_PUBLIC void glClearBufferuiv(GLenum buffer,GLint drawbuffer,const GLuint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glClearBufferuiv\x1b[0m\n");
}



SYS_PUBLIC void glClearColor(GLfloat red,GLfloat green,GLfloat blue,GLfloat alpha){
	_gl_clear_color_value[0]=red;
	_gl_clear_color_value[1]=green;
	_gl_clear_color_value[2]=blue;
	_gl_clear_color_value[3]=alpha;
}



SYS_PUBLIC void glClearDepth(GLdouble depth){
	_gl_clear_depth_value=depth;
}



SYS_PUBLIC void glClearStencil(GLint s){
	_gl_clear_stencil_value=s;
}



SYS_PUBLIC GLenum glClientWaitSync(GLsync sync,GLbitfield flags,GLuint64 timeout){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glClientWaitSync\x1b[0m\n");
	return 0;
}



SYS_PUBLIC void glColorMask(GLboolean red,GLboolean green,GLboolean blue,GLboolean alpha){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glColorMask\x1b[0m\n");
}



SYS_PUBLIC void glColorMaski(GLuint index,GLboolean r,GLboolean g,GLboolean b,GLboolean a){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glColorMaski\x1b[0m\n");
}



SYS_PUBLIC void glCompileShader(GLuint shader){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCompileShader\x1b[0m\n");
}



SYS_PUBLIC void glCompressedTexImage1D(GLenum target,GLint level,GLenum internalformat,GLsizei width,GLint border,GLsizei imageSize,const void* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCompressedTexImage1D\x1b[0m\n");
}



SYS_PUBLIC void glCompressedTexImage2D(GLenum target,GLint level,GLenum internalformat,GLsizei width,GLsizei height,GLint border,GLsizei imageSize,const void* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCompressedTexImage2D\x1b[0m\n");
}



SYS_PUBLIC void glCompressedTexImage3D(GLenum target,GLint level,GLenum internalformat,GLsizei width,GLsizei height,GLsizei depth,GLint border,GLsizei imageSize,const void* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCompressedTexImage3D\x1b[0m\n");
}



SYS_PUBLIC void glCompressedTexSubImage1D(GLenum target,GLint level,GLint xoffset,GLsizei width,GLenum format,GLsizei imageSize,const void* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCompressedTexSubImage1D\x1b[0m\n");
}



SYS_PUBLIC void glCompressedTexSubImage2D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLsizei width,GLsizei height,GLenum format,GLsizei imageSize,const void* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCompressedTexSubImage2D\x1b[0m\n");
}



SYS_PUBLIC void glCompressedTexSubImage3D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLint zoffset,GLsizei width,GLsizei height,GLsizei depth,GLenum format,GLsizei imageSize,const void* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCompressedTexSubImage3D\x1b[0m\n");
}



SYS_PUBLIC void glCopyBufferSubData(GLenum readTarget,GLenum writeTarget,GLintptr readOffset,GLintptr writeOffset,GLsizeiptr size){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCopyBufferSubData\x1b[0m\n");
}



SYS_PUBLIC void glCopyTexImage1D(GLenum target,GLint level,GLenum internalformat,GLint x,GLint y,GLsizei width,GLint border){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCopyTexImage1D\x1b[0m\n");
}



SYS_PUBLIC void glCopyTexImage2D(GLenum target,GLint level,GLenum internalformat,GLint x,GLint y,GLsizei width,GLsizei height,GLint border){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCopyTexImage2D\x1b[0m\n");
}



SYS_PUBLIC void glCopyTexSubImage1D(GLenum target,GLint level,GLint xoffset,GLint x,GLint y,GLsizei width){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCopyTexSubImage1D\x1b[0m\n");
}



SYS_PUBLIC void glCopyTexSubImage2D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLint x,GLint y,GLsizei width,GLsizei height){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCopyTexSubImage2D\x1b[0m\n");
}



SYS_PUBLIC void glCopyTexSubImage3D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLint zoffset,GLint x,GLint y,GLsizei width,GLsizei height){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCopyTexSubImage3D\x1b[0m\n");
}



SYS_PUBLIC GLuint glCreateProgram(void){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCreateProgram\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLuint glCreateShader(GLenum type){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCreateShader\x1b[0m\n");
	return 0;
}



SYS_PUBLIC void glCullFace(GLenum mode){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glCullFace\x1b[0m\n");
}



SYS_PUBLIC void glDeleteBuffers(GLsizei n,const GLuint* buffers){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDeleteBuffers\x1b[0m\n");
}



SYS_PUBLIC void glDeleteFramebuffers(GLsizei n,const GLuint* framebuffers){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDeleteFramebuffers\x1b[0m\n");
}



SYS_PUBLIC void glDeleteProgram(GLuint program){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDeleteProgram\x1b[0m\n");
}



SYS_PUBLIC void glDeleteQueries(GLsizei n,const GLuint* ids){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDeleteQueries\x1b[0m\n");
}



SYS_PUBLIC void glDeleteRenderbuffers(GLsizei n,const GLuint* renderbuffers){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDeleteRenderbuffers\x1b[0m\n");
}



SYS_PUBLIC void glDeleteSamplers(GLsizei count,const GLuint* samplers){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDeleteSamplers\x1b[0m\n");
}



SYS_PUBLIC void glDeleteShader(GLuint shader){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDeleteShader\x1b[0m\n");
}



SYS_PUBLIC void glDeleteSync(GLsync sync){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDeleteSync\x1b[0m\n");
}



SYS_PUBLIC void glDeleteTextures(GLsizei n,const GLuint* textures){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDeleteTextures\x1b[0m\n");
}



SYS_PUBLIC void glDeleteVertexArrays(GLsizei n,const GLuint* arrays){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDeleteVertexArrays\x1b[0m\n");
}



SYS_PUBLIC void glDepthFunc(GLenum func){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDepthFunc\x1b[0m\n");
}



SYS_PUBLIC void glDepthMask(GLboolean flag){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDepthMask\x1b[0m\n");
}



SYS_PUBLIC void glDepthRange(GLdouble n,GLdouble f){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDepthRange\x1b[0m\n");
}



SYS_PUBLIC void glDetachShader(GLuint program,GLuint shader){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDetachShader\x1b[0m\n");
}



SYS_PUBLIC void glDisable(GLenum cap){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDisable\x1b[0m\n");
}



SYS_PUBLIC void glDisablei(GLenum target,GLuint index){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDisablei\x1b[0m\n");
}



SYS_PUBLIC void glDisableVertexAttribArray(GLuint index){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDisableVertexAttribArray\x1b[0m\n");
}



SYS_PUBLIC void glDrawArrays(GLenum mode,GLint first,GLsizei count){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDrawArrays\x1b[0m\n");
}



SYS_PUBLIC void glDrawArraysInstanced(GLenum mode,GLint first,GLsizei count,GLsizei instancecount){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDrawArraysInstanced\x1b[0m\n");
}



SYS_PUBLIC void glDrawBuffer(GLenum buf){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDrawBuffer\x1b[0m\n");
}



SYS_PUBLIC void glDrawBuffers(GLsizei n,const GLenum* bufs){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDrawBuffers\x1b[0m\n");
}



SYS_PUBLIC void glDrawElements(GLenum mode,GLsizei count,GLenum type,const void* indices){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDrawElements\x1b[0m\n");
}



SYS_PUBLIC void glDrawElementsBaseVertex(GLenum mode,GLsizei count,GLenum type,const void* indices,GLint basevertex){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDrawElementsBaseVertex\x1b[0m\n");
}



SYS_PUBLIC void glDrawElementsInstanced(GLenum mode,GLsizei count,GLenum type,const void* indices,GLsizei instancecount){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDrawElementsInstanced\x1b[0m\n");
}



SYS_PUBLIC void glDrawElementsInstancedBaseVertex(GLenum mode,GLsizei count,GLenum type,const void* indices,GLsizei instancecount,GLint basevertex){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDrawElementsInstancedBaseVertex\x1b[0m\n");
}



SYS_PUBLIC void glDrawRangeElements(GLenum mode,GLuint start,GLuint end,GLsizei count,GLenum type,const void* indices){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDrawRangeElements\x1b[0m\n");
}



SYS_PUBLIC void glDrawRangeElementsBaseVertex(GLenum mode,GLuint start,GLuint end,GLsizei count,GLenum type,const void* indices,GLint basevertex){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glDrawRangeElementsBaseVertex\x1b[0m\n");
}



SYS_PUBLIC void glEnable(GLenum cap){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glEnable\x1b[0m\n");
}



SYS_PUBLIC void glEnablei(GLenum target,GLuint index){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glEnablei\x1b[0m\n");
}



SYS_PUBLIC void glEnableVertexAttribArray(GLuint index){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glEnableVertexAttribArray\x1b[0m\n");
}



SYS_PUBLIC void glEndConditionalRender(void){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glEndConditionalRender\x1b[0m\n");
}



SYS_PUBLIC void glEndQuery(GLenum target){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glEndQuery\x1b[0m\n");
}



SYS_PUBLIC void glEndTransformFeedback(void){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glEndTransformFeedback\x1b[0m\n");
}



SYS_PUBLIC GLsync glFenceSync(GLenum condition,GLbitfield flags){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glFenceSync\x1b[0m\n");
	return 0;
}



SYS_PUBLIC void glFinish(void){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glFinish\x1b[0m\n");
}



SYS_PUBLIC void glFlush(void){
	opengl_command_buffer_flush();
}



SYS_PUBLIC void glFlushMappedBufferRange(GLenum target,GLintptr offset,GLsizeiptr length){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glFlushMappedBufferRange\x1b[0m\n");
}



SYS_PUBLIC void glFramebufferRenderbuffer(GLenum target,GLenum attachment,GLenum renderbuffertarget,GLuint renderbuffer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glFramebufferRenderbuffer\x1b[0m\n");
}



SYS_PUBLIC void glFramebufferTexture(GLenum target,GLenum attachment,GLuint texture,GLint level){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glFramebufferTexture\x1b[0m\n");
}



SYS_PUBLIC void glFramebufferTexture1D(GLenum target,GLenum attachment,GLenum textarget,GLuint texture,GLint level){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glFramebufferTexture1D\x1b[0m\n");
}



SYS_PUBLIC void glFramebufferTexture2D(GLenum target,GLenum attachment,GLenum textarget,GLuint texture,GLint level){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glFramebufferTexture2D\x1b[0m\n");
}



SYS_PUBLIC void glFramebufferTexture3D(GLenum target,GLenum attachment,GLenum textarget,GLuint texture,GLint level,GLint zoffset){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glFramebufferTexture3D\x1b[0m\n");
}



SYS_PUBLIC void glFramebufferTextureLayer(GLenum target,GLenum attachment,GLuint texture,GLint level,GLint layer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glFramebufferTextureLayer\x1b[0m\n");
}



SYS_PUBLIC void glFrontFace(GLenum mode){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glFrontFace\x1b[0m\n");
}



SYS_PUBLIC void glGenBuffers(GLsizei n,GLuint* buffers){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGenBuffers\x1b[0m\n");
}



SYS_PUBLIC void glGenerateMipmap(GLenum target){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGenerateMipmap\x1b[0m\n");
}



SYS_PUBLIC void glGenFramebuffers(GLsizei n,GLuint* framebuffers){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGenFramebuffers\x1b[0m\n");
}



SYS_PUBLIC void glGenQueries(GLsizei n,GLuint* ids){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGenQueries\x1b[0m\n");
}



SYS_PUBLIC void glGenRenderbuffers(GLsizei n,GLuint* renderbuffers){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGenRenderbuffers\x1b[0m\n");
}



SYS_PUBLIC void glGenSamplers(GLsizei count,GLuint* samplers){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGenSamplers\x1b[0m\n");
}



SYS_PUBLIC void glGenTextures(GLsizei n,GLuint* textures){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGenTextures\x1b[0m\n");
}



SYS_PUBLIC void glGenVertexArrays(GLsizei n,GLuint* arrays){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGenVertexArrays\x1b[0m\n");
}



SYS_PUBLIC void glGetActiveAttrib(GLuint program,GLuint index,GLsizei bufSize,GLsizei* length,GLint* size,GLenum* type,GLchar* name){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetActiveAttrib\x1b[0m\n");
}



SYS_PUBLIC void glGetActiveUniform(GLuint program,GLuint index,GLsizei bufSize,GLsizei* length,GLint* size,GLenum* type,GLchar* name){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetActiveUniform\x1b[0m\n");
}



SYS_PUBLIC void glGetActiveUniformBlockiv(GLuint program,GLuint uniformBlockIndex,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetActiveUniformBlockiv\x1b[0m\n");
}



SYS_PUBLIC void glGetActiveUniformBlockName(GLuint program,GLuint uniformBlockIndex,GLsizei bufSize,GLsizei* length,GLchar* uniformBlockName){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetActiveUniformBlockName\x1b[0m\n");
}



SYS_PUBLIC void glGetActiveUniformName(GLuint program,GLuint uniformIndex,GLsizei bufSize,GLsizei* length,GLchar* uniformName){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetActiveUniformName\x1b[0m\n");
}



SYS_PUBLIC void glGetActiveUniformsiv(GLuint program,GLsizei uniformCount,const GLuint* uniformIndices,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetActiveUniformsiv\x1b[0m\n");
}



SYS_PUBLIC void glGetAttachedShaders(GLuint program,GLsizei maxCount,GLsizei* count,GLuint* shaders){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetAttachedShaders\x1b[0m\n");
}



SYS_PUBLIC GLint glGetAttribLocation(GLuint program,const GLchar* name){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetAttribLocation\x1b[0m\n");
	return 0;
}



SYS_PUBLIC void glGetBooleani_v(GLenum target,GLuint index,GLboolean* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetBooleani_v\x1b[0m\n");
}



SYS_PUBLIC void glGetBooleanv(GLenum pname,GLboolean* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetBooleanv\x1b[0m\n");
}



SYS_PUBLIC void glGetBufferParameteri64v(GLenum target,GLenum pname,GLint64* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetBufferParameteri64v\x1b[0m\n");
}



SYS_PUBLIC void glGetBufferParameteriv(GLenum target,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetBufferParameteriv\x1b[0m\n");
}



SYS_PUBLIC void glGetBufferPointerv(GLenum target,GLenum pname,void* *params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetBufferPointerv\x1b[0m\n");
}



SYS_PUBLIC void glGetBufferSubData(GLenum target,GLintptr offset,GLsizeiptr size,void* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetBufferSubData\x1b[0m\n");
}



SYS_PUBLIC void glGetCompressedTexImage(GLenum target,GLint level,void* img){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetCompressedTexImage\x1b[0m\n");
}



SYS_PUBLIC void glGetDoublev(GLenum pname,GLdouble* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetDoublev\x1b[0m\n");
}



SYS_PUBLIC GLenum glGetError(void){
	GLenum out=_gl_error;
	_gl_error=GL_NO_ERROR;
	return out;
}



SYS_PUBLIC void glGetFloatv(GLenum pname,GLfloat* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetFloatv\x1b[0m\n");
}



SYS_PUBLIC GLint glGetFragDataIndex(GLuint program,const GLchar* name){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetFragDataIndex\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLint glGetFragDataLocation(GLuint program,const GLchar* name){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetFragDataLocation\x1b[0m\n");
	return 0;
}



SYS_PUBLIC void glGetFramebufferAttachmentParameteriv(GLenum target,GLenum attachment,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetFramebufferAttachmentParameteriv\x1b[0m\n");
}



SYS_PUBLIC void glGetInteger64i_v(GLenum target,GLuint index,GLint64* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetInteger64i_v\x1b[0m\n");
}



SYS_PUBLIC void glGetInteger64v(GLenum pname,GLint64* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetInteger64v\x1b[0m\n");
}



SYS_PUBLIC void glGetIntegeri_v(GLenum target,GLuint index,GLint* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetIntegeri_v\x1b[0m\n");
}



SYS_PUBLIC void glGetIntegerv(GLenum pname,GLint* data){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetIntegerv\x1b[0m\n");
}



SYS_PUBLIC void glGetMultisamplefv(GLenum pname,GLuint index,GLfloat* val){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetMultisamplefv\x1b[0m\n");
}



SYS_PUBLIC void glGetPointerv(GLenum pname,void* *params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetPointerv\x1b[0m\n");
}



SYS_PUBLIC void glGetProgramInfoLog(GLuint program,GLsizei bufSize,GLsizei* length,GLchar* infoLog){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetProgramInfoLog\x1b[0m\n");
}



SYS_PUBLIC void glGetProgramiv(GLuint program,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetProgramiv\x1b[0m\n");
}



SYS_PUBLIC void glGetQueryiv(GLenum target,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetQueryiv\x1b[0m\n");
}



SYS_PUBLIC void glGetQueryObjecti64v(GLuint id,GLenum pname,GLint64* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetQueryObjecti64v\x1b[0m\n");
}



SYS_PUBLIC void glGetQueryObjectiv(GLuint id,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetQueryObjectiv\x1b[0m\n");
}



SYS_PUBLIC void glGetQueryObjectui64v(GLuint id,GLenum pname,GLuint64* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetQueryObjectui64v\x1b[0m\n");
}



SYS_PUBLIC void glGetQueryObjectuiv(GLuint id,GLenum pname,GLuint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetQueryObjectuiv\x1b[0m\n");
}



SYS_PUBLIC void glGetRenderbufferParameteriv(GLenum target,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetRenderbufferParameteriv\x1b[0m\n");
}



SYS_PUBLIC void glGetSamplerParameterfv(GLuint sampler,GLenum pname,GLfloat* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetSamplerParameterfv\x1b[0m\n");
}



SYS_PUBLIC void glGetSamplerParameterIiv(GLuint sampler,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetSamplerParameterIiv\x1b[0m\n");
}



SYS_PUBLIC void glGetSamplerParameterIuiv(GLuint sampler,GLenum pname,GLuint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetSamplerParameterIuiv\x1b[0m\n");
}



SYS_PUBLIC void glGetSamplerParameteriv(GLuint sampler,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetSamplerParameteriv\x1b[0m\n");
}



SYS_PUBLIC void glGetShaderInfoLog(GLuint shader,GLsizei bufSize,GLsizei* length,GLchar* infoLog){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetShaderInfoLog\x1b[0m\n");
}



SYS_PUBLIC void glGetShaderiv(GLuint shader,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetShaderiv\x1b[0m\n");
}



SYS_PUBLIC void glGetShaderSource(GLuint shader,GLsizei bufSize,GLsizei* length,GLchar* source){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetShaderSource\x1b[0m\n");
}



SYS_PUBLIC const GLubyte* glGetString(GLenum name){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetString\x1b[0m\n");
	return NULL;
}



SYS_PUBLIC const GLubyte* glGetStringi(GLenum name,GLuint index){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetStringi\x1b[0m\n");
	return NULL;
}



SYS_PUBLIC void glGetSynciv(GLsync sync,GLenum pname,GLsizei count,GLsizei* length,GLint* values){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetSynciv\x1b[0m\n");
}



SYS_PUBLIC void glGetTexImage(GLenum target,GLint level,GLenum format,GLenum type,void* pixels){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetTexImage\x1b[0m\n");
}



SYS_PUBLIC void glGetTexLevelParameterfv(GLenum target,GLint level,GLenum pname,GLfloat* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetTexLevelParameterfv\x1b[0m\n");
}



SYS_PUBLIC void glGetTexLevelParameteriv(GLenum target,GLint level,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetTexLevelParameteriv\x1b[0m\n");
}



SYS_PUBLIC void glGetTexParameterfv(GLenum target,GLenum pname,GLfloat* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetTexParameterfv\x1b[0m\n");
}



SYS_PUBLIC void glGetTexParameterIiv(GLenum target,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetTexParameterIiv\x1b[0m\n");
}



SYS_PUBLIC void glGetTexParameterIuiv(GLenum target,GLenum pname,GLuint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetTexParameterIuiv\x1b[0m\n");
}



SYS_PUBLIC void glGetTexParameteriv(GLenum target,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetTexParameteriv\x1b[0m\n");
}



SYS_PUBLIC void glGetTransformFeedbackVarying(GLuint program,GLuint index,GLsizei bufSize,GLsizei* length,GLsizei* size,GLenum* type,GLchar* name){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetTransformFeedbackVarying\x1b[0m\n");
}



SYS_PUBLIC GLuint glGetUniformBlockIndex(GLuint program,const GLchar* uniformBlockName){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetUniformBlockIndex\x1b[0m\n");
	return 0;
}



SYS_PUBLIC void glGetUniformfv(GLuint program,GLint location,GLfloat* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetUniformfv\x1b[0m\n");
}



SYS_PUBLIC void glGetUniformIndices(GLuint program,GLsizei uniformCount,const GLchar* const*uniformNames,GLuint* uniformIndices){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetUniformIndices\x1b[0m\n");
}



SYS_PUBLIC void glGetUniformiv(GLuint program,GLint location,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetUniformiv\x1b[0m\n");
}



SYS_PUBLIC GLint glGetUniformLocation(GLuint program,const GLchar* name){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetUniformLocation\x1b[0m\n");
	return 0;
}



SYS_PUBLIC void glGetUniformuiv(GLuint program,GLint location,GLuint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetUniformuiv\x1b[0m\n");
}



SYS_PUBLIC void glGetVertexAttribdv(GLuint index,GLenum pname,GLdouble* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetVertexAttribdv\x1b[0m\n");
}



SYS_PUBLIC void glGetVertexAttribfv(GLuint index,GLenum pname,GLfloat* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetVertexAttribfv\x1b[0m\n");
}



SYS_PUBLIC void glGetVertexAttribIiv(GLuint index,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetVertexAttribIiv\x1b[0m\n");
}



SYS_PUBLIC void glGetVertexAttribIuiv(GLuint index,GLenum pname,GLuint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetVertexAttribIuiv\x1b[0m\n");
}



SYS_PUBLIC void glGetVertexAttribiv(GLuint index,GLenum pname,GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetVertexAttribiv\x1b[0m\n");
}



SYS_PUBLIC void glGetVertexAttribPointerv(GLuint index,GLenum pname,void* *pointer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glGetVertexAttribPointerv\x1b[0m\n");
}



SYS_PUBLIC void glHint(GLenum target,GLenum mode){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glHint\x1b[0m\n");
}



SYS_PUBLIC GLboolean glIsBuffer(GLuint buffer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glIsBuffer\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLboolean glIsEnabled(GLenum cap){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glIsEnabled\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLboolean glIsEnabledi(GLenum target,GLuint index){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glIsEnabledi\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLboolean glIsFramebuffer(GLuint framebuffer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glIsFramebuffer\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLboolean glIsProgram(GLuint program){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glIsProgram\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLboolean glIsQuery(GLuint id){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glIsQuery\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLboolean glIsRenderbuffer(GLuint renderbuffer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glIsRenderbuffer\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLboolean glIsSampler(GLuint sampler){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glIsSampler\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLboolean glIsShader(GLuint shader){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glIsShader\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLboolean glIsSync(GLsync sync){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glIsSync\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLboolean glIsTexture(GLuint texture){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glIsTexture\x1b[0m\n");
	return 0;
}



SYS_PUBLIC GLboolean glIsVertexArray(GLuint array){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glIsVertexArray\x1b[0m\n");
	return 0;
}



SYS_PUBLIC void glLineWidth(GLfloat width){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glLineWidth\x1b[0m\n");
}



SYS_PUBLIC void glLinkProgram(GLuint program){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glLinkProgram\x1b[0m\n");
}



SYS_PUBLIC void glLogicOp(GLenum opcode){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glLogicOp\x1b[0m\n");
}



SYS_PUBLIC void* glMapBuffer(GLenum target,GLenum access){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glMapBuffer\x1b[0m\n");
	return NULL;
}



SYS_PUBLIC void* glMapBufferRange(GLenum target,GLintptr offset,GLsizeiptr length,GLbitfield access){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glMapBufferRange\x1b[0m\n");
	return NULL;
}



SYS_PUBLIC void glMultiDrawArrays(GLenum mode,const GLint* first,const GLsizei* count,GLsizei drawcount){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glMultiDrawArrays\x1b[0m\n");
}



SYS_PUBLIC void glMultiDrawElements(GLenum mode,const GLsizei* count,GLenum type,const void* const*indices,GLsizei drawcount){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glMultiDrawElements\x1b[0m\n");
}



SYS_PUBLIC void glMultiDrawElementsBaseVertex(GLenum mode,const GLsizei* count,GLenum type,const void* const*indices,GLsizei drawcount,const GLint* basevertex){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glMultiDrawElementsBaseVertex\x1b[0m\n");
}



SYS_PUBLIC void glPixelStoref(GLenum pname,GLfloat param){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glPixelStoref\x1b[0m\n");
}



SYS_PUBLIC void glPixelStorei(GLenum pname,GLint param){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glPixelStorei\x1b[0m\n");
}



SYS_PUBLIC void glPointParameterf(GLenum pname,GLfloat param){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glPointParameterf\x1b[0m\n");
}



SYS_PUBLIC void glPointParameterfv(GLenum pname,const GLfloat* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glPointParameterfv\x1b[0m\n");
}



SYS_PUBLIC void glPointParameteri(GLenum pname,GLint param){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glPointParameteri\x1b[0m\n");
}



SYS_PUBLIC void glPointParameteriv(GLenum pname,const GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glPointParameteriv\x1b[0m\n");
}



SYS_PUBLIC void glPointSize(GLfloat size){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glPointSize\x1b[0m\n");
}



SYS_PUBLIC void glPolygonMode(GLenum face,GLenum mode){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glPolygonMode\x1b[0m\n");
}



SYS_PUBLIC void glPolygonOffset(GLfloat factor,GLfloat units){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glPolygonOffset\x1b[0m\n");
}



SYS_PUBLIC void glPrimitiveRestartIndex(GLuint index){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glPrimitiveRestartIndex\x1b[0m\n");
}



SYS_PUBLIC void glProvokingVertex(GLenum mode){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glProvokingVertex\x1b[0m\n");
}



SYS_PUBLIC void glQueryCounter(GLuint id,GLenum target){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glQueryCounter\x1b[0m\n");
}



SYS_PUBLIC void glReadBuffer(GLenum src){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glReadBuffer\x1b[0m\n");
}



SYS_PUBLIC void glReadPixels(GLint x,GLint y,GLsizei width,GLsizei height,GLenum format,GLenum type,void* pixels){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glReadPixels\x1b[0m\n");
}



SYS_PUBLIC void glRenderbufferStorage(GLenum target,GLenum internalformat,GLsizei width,GLsizei height){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glRenderbufferStorage\x1b[0m\n");
}



SYS_PUBLIC void glRenderbufferStorageMultisample(GLenum target,GLsizei samples,GLenum internalformat,GLsizei width,GLsizei height){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glRenderbufferStorageMultisample\x1b[0m\n");
}



SYS_PUBLIC void glSampleCoverage(GLfloat value,GLboolean invert){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glSampleCoverage\x1b[0m\n");
}



SYS_PUBLIC void glSampleMaski(GLuint maskNumber,GLbitfield mask){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glSampleMaski\x1b[0m\n");
}



SYS_PUBLIC void glSamplerParameterf(GLuint sampler,GLenum pname,GLfloat param){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glSamplerParameterf\x1b[0m\n");
}



SYS_PUBLIC void glSamplerParameterfv(GLuint sampler,GLenum pname,const GLfloat* param){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glSamplerParameterfv\x1b[0m\n");
}



SYS_PUBLIC void glSamplerParameteri(GLuint sampler,GLenum pname,GLint param){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glSamplerParameteri\x1b[0m\n");
}



SYS_PUBLIC void glSamplerParameterIiv(GLuint sampler,GLenum pname,const GLint* param){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glSamplerParameterIiv\x1b[0m\n");
}



SYS_PUBLIC void glSamplerParameterIuiv(GLuint sampler,GLenum pname,const GLuint* param){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glSamplerParameterIuiv\x1b[0m\n");
}



SYS_PUBLIC void glSamplerParameteriv(GLuint sampler,GLenum pname,const GLint* param){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glSamplerParameteriv\x1b[0m\n");
}



SYS_PUBLIC void glScissor(GLint x,GLint y,GLsizei width,GLsizei height){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glScissor\x1b[0m\n");
}



SYS_PUBLIC void glShaderSource(GLuint shader,GLsizei count,const GLchar* const*string,const GLint* length){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glShaderSource\x1b[0m\n");
}



SYS_PUBLIC void glStencilFunc(GLenum func,GLint ref,GLuint mask){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glStencilFunc\x1b[0m\n");
}



SYS_PUBLIC void glStencilFuncSeparate(GLenum face,GLenum func,GLint ref,GLuint mask){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glStencilFuncSeparate\x1b[0m\n");
}



SYS_PUBLIC void glStencilMask(GLuint mask){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glStencilMask\x1b[0m\n");
}



SYS_PUBLIC void glStencilMaskSeparate(GLenum face,GLuint mask){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glStencilMaskSeparate\x1b[0m\n");
}



SYS_PUBLIC void glStencilOp(GLenum fail,GLenum zfail,GLenum zpass){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glStencilOp\x1b[0m\n");
}



SYS_PUBLIC void glStencilOpSeparate(GLenum face,GLenum sfail,GLenum dpfail,GLenum dppass){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glStencilOpSeparate\x1b[0m\n");
}



SYS_PUBLIC void glTexBuffer(GLenum target,GLenum internalformat,GLuint buffer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexBuffer\x1b[0m\n");
}



SYS_PUBLIC void glTexImage1D(GLenum target,GLint level,GLint internalformat,GLsizei width,GLint border,GLenum format,GLenum type,const void* pixels){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexImage1D\x1b[0m\n");
}



SYS_PUBLIC void glTexImage2D(GLenum target,GLint level,GLint internalformat,GLsizei width,GLsizei height,GLint border,GLenum format,GLenum type,const void* pixels){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexImage2D\x1b[0m\n");
}



SYS_PUBLIC void glTexImage2DMultisample(GLenum target,GLsizei samples,GLenum internalformat,GLsizei width,GLsizei height,GLboolean fixedsamplelocations){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexImage2DMultisample\x1b[0m\n");
}



SYS_PUBLIC void glTexImage3D(GLenum target,GLint level,GLint internalformat,GLsizei width,GLsizei height,GLsizei depth,GLint border,GLenum format,GLenum type,const void* pixels){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexImage3D\x1b[0m\n");
}



SYS_PUBLIC void glTexImage3DMultisample(GLenum target,GLsizei samples,GLenum internalformat,GLsizei width,GLsizei height,GLsizei depth,GLboolean fixedsamplelocations){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexImage3DMultisample\x1b[0m\n");
}



SYS_PUBLIC void glTexParameterf(GLenum target,GLenum pname,GLfloat param){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexParameterf\x1b[0m\n");
}



SYS_PUBLIC void glTexParameterfv(GLenum target,GLenum pname,const GLfloat* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexParameterfv\x1b[0m\n");
}



SYS_PUBLIC void glTexParameteri(GLenum target,GLenum pname,GLint param){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexParameteri\x1b[0m\n");
}



SYS_PUBLIC void glTexParameterIiv(GLenum target,GLenum pname,const GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexParameterIiv\x1b[0m\n");
}



SYS_PUBLIC void glTexParameterIuiv(GLenum target,GLenum pname,const GLuint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexParameterIuiv\x1b[0m\n");
}



SYS_PUBLIC void glTexParameteriv(GLenum target,GLenum pname,const GLint* params){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexParameteriv\x1b[0m\n");
}



SYS_PUBLIC void glTexSubImage1D(GLenum target,GLint level,GLint xoffset,GLsizei width,GLenum format,GLenum type,const void* pixels){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexSubImage1D\x1b[0m\n");
}



SYS_PUBLIC void glTexSubImage2D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLsizei width,GLsizei height,GLenum format,GLenum type,const void* pixels){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexSubImage2D\x1b[0m\n");
}



SYS_PUBLIC void glTexSubImage3D(GLenum target,GLint level,GLint xoffset,GLint yoffset,GLint zoffset,GLsizei width,GLsizei height,GLsizei depth,GLenum format,GLenum type,const void* pixels){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTexSubImage3D\x1b[0m\n");
}



SYS_PUBLIC void glTransformFeedbackVaryings(GLuint program,GLsizei count,const GLchar* const*varyings,GLenum bufferMode){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glTransformFeedbackVaryings\x1b[0m\n");
}



SYS_PUBLIC void glUniform1f(GLint location,GLfloat v0){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform1f\x1b[0m\n");
}



SYS_PUBLIC void glUniform1fv(GLint location,GLsizei count,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform1fv\x1b[0m\n");
}



SYS_PUBLIC void glUniform1i(GLint location,GLint v0){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform1i\x1b[0m\n");
}



SYS_PUBLIC void glUniform1iv(GLint location,GLsizei count,const GLint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform1iv\x1b[0m\n");
}



SYS_PUBLIC void glUniform1ui(GLint location,GLuint v0){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform1ui\x1b[0m\n");
}



SYS_PUBLIC void glUniform1uiv(GLint location,GLsizei count,const GLuint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform1uiv\x1b[0m\n");
}



SYS_PUBLIC void glUniform2f(GLint location,GLfloat v0,GLfloat v1){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform2f\x1b[0m\n");
}



SYS_PUBLIC void glUniform2fv(GLint location,GLsizei count,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform2fv\x1b[0m\n");
}



SYS_PUBLIC void glUniform2i(GLint location,GLint v0,GLint v1){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform2i\x1b[0m\n");
}



SYS_PUBLIC void glUniform2iv(GLint location,GLsizei count,const GLint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform2iv\x1b[0m\n");
}



SYS_PUBLIC void glUniform2ui(GLint location,GLuint v0,GLuint v1){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform2ui\x1b[0m\n");
}



SYS_PUBLIC void glUniform2uiv(GLint location,GLsizei count,const GLuint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform2uiv\x1b[0m\n");
}



SYS_PUBLIC void glUniform3f(GLint location,GLfloat v0,GLfloat v1,GLfloat v2){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform3f\x1b[0m\n");
}



SYS_PUBLIC void glUniform3fv(GLint location,GLsizei count,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform3fv\x1b[0m\n");
}



SYS_PUBLIC void glUniform3i(GLint location,GLint v0,GLint v1,GLint v2){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform3i\x1b[0m\n");
}



SYS_PUBLIC void glUniform3iv(GLint location,GLsizei count,const GLint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform3iv\x1b[0m\n");
}



SYS_PUBLIC void glUniform3ui(GLint location,GLuint v0,GLuint v1,GLuint v2){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform3ui\x1b[0m\n");
}



SYS_PUBLIC void glUniform3uiv(GLint location,GLsizei count,const GLuint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform3uiv\x1b[0m\n");
}



SYS_PUBLIC void glUniform4f(GLint location,GLfloat v0,GLfloat v1,GLfloat v2,GLfloat v3){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform4f\x1b[0m\n");
}



SYS_PUBLIC void glUniform4fv(GLint location,GLsizei count,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform4fv\x1b[0m\n");
}



SYS_PUBLIC void glUniform4i(GLint location,GLint v0,GLint v1,GLint v2,GLint v3){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform4i\x1b[0m\n");
}



SYS_PUBLIC void glUniform4iv(GLint location,GLsizei count,const GLint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform4iv\x1b[0m\n");
}



SYS_PUBLIC void glUniform4ui(GLint location,GLuint v0,GLuint v1,GLuint v2,GLuint v3){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform4ui\x1b[0m\n");
}



SYS_PUBLIC void glUniform4uiv(GLint location,GLsizei count,const GLuint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniform4uiv\x1b[0m\n");
}



SYS_PUBLIC void glUniformBlockBinding(GLuint program,GLuint uniformBlockIndex,GLuint uniformBlockBinding){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniformBlockBinding\x1b[0m\n");
}



SYS_PUBLIC void glUniformMatrix2fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniformMatrix2fv\x1b[0m\n");
}



SYS_PUBLIC void glUniformMatrix2x3fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniformMatrix2x3fv\x1b[0m\n");
}



SYS_PUBLIC void glUniformMatrix2x4fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniformMatrix2x4fv\x1b[0m\n");
}



SYS_PUBLIC void glUniformMatrix3fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniformMatrix3fv\x1b[0m\n");
}



SYS_PUBLIC void glUniformMatrix3x2fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniformMatrix3x2fv\x1b[0m\n");
}



SYS_PUBLIC void glUniformMatrix3x4fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniformMatrix3x4fv\x1b[0m\n");
}



SYS_PUBLIC void glUniformMatrix4fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniformMatrix4fv\x1b[0m\n");
}



SYS_PUBLIC void glUniformMatrix4x2fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniformMatrix4x2fv\x1b[0m\n");
}



SYS_PUBLIC void glUniformMatrix4x3fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUniformMatrix4x3fv\x1b[0m\n");
}



SYS_PUBLIC GLboolean glUnmapBuffer(GLenum target){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUnmapBuffer\x1b[0m\n");
	return 0;
}



SYS_PUBLIC void glUseProgram(GLuint program){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glUseProgram\x1b[0m\n");
}



SYS_PUBLIC void glValidateProgram(GLuint program){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glValidateProgram\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib1d(GLuint index,GLdouble x){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib1d\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib1dv(GLuint index,const GLdouble* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib1dv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib1f(GLuint index,GLfloat x){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib1f\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib1fv(GLuint index,const GLfloat* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib1fv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib1s(GLuint index,GLshort x){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib1s\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib1sv(GLuint index,const GLshort* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib1sv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib2d(GLuint index,GLdouble x,GLdouble y){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib2d\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib2dv(GLuint index,const GLdouble* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib2dv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib2f(GLuint index,GLfloat x,GLfloat y){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib2f\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib2fv(GLuint index,const GLfloat* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib2fv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib2s(GLuint index,GLshort x,GLshort y){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib2s\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib2sv(GLuint index,const GLshort* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib2sv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib3d(GLuint index,GLdouble x,GLdouble y,GLdouble z){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib3d\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib3dv(GLuint index,const GLdouble* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib3dv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib3f(GLuint index,GLfloat x,GLfloat y,GLfloat z){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib3f\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib3fv(GLuint index,const GLfloat* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib3fv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib3s(GLuint index,GLshort x,GLshort y,GLshort z){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib3s\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib3sv(GLuint index,const GLshort* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib3sv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4bv(GLuint index,const GLbyte* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4bv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4d(GLuint index,GLdouble x,GLdouble y,GLdouble z,GLdouble w){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4d\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4dv(GLuint index,const GLdouble* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4dv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4f(GLuint index,GLfloat x,GLfloat y,GLfloat z,GLfloat w){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4f\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4fv(GLuint index,const GLfloat* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4fv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4iv(GLuint index,const GLint* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4iv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4Nbv(GLuint index,const GLbyte* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4Nbv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4Niv(GLuint index,const GLint* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4Niv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4Nsv(GLuint index,const GLshort* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4Nsv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4Nub(GLuint index,GLubyte x,GLubyte y,GLubyte z,GLubyte w){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4Nub\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4Nubv(GLuint index,const GLubyte* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4Nubv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4Nuiv(GLuint index,const GLuint* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4Nuiv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4Nusv(GLuint index,const GLushort* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4Nusv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4s(GLuint index,GLshort x,GLshort y,GLshort z,GLshort w){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4s\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4sv(GLuint index,const GLshort* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4sv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4ubv(GLuint index,const GLubyte* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4ubv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4uiv(GLuint index,const GLuint* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4uiv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttrib4usv(GLuint index,const GLushort* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttrib4usv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribDivisor(GLuint index,GLuint divisor){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribDivisor\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI1i(GLuint index,GLint x){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI1i\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI1iv(GLuint index,const GLint* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI1iv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI1ui(GLuint index,GLuint x){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI1ui\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI1uiv(GLuint index,const GLuint* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI1uiv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI2i(GLuint index,GLint x,GLint y){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI2i\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI2iv(GLuint index,const GLint* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI2iv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI2ui(GLuint index,GLuint x,GLuint y){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI2ui\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI2uiv(GLuint index,const GLuint* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI2uiv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI3i(GLuint index,GLint x,GLint y,GLint z){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI3i\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI3iv(GLuint index,const GLint* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI3iv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI3ui(GLuint index,GLuint x,GLuint y,GLuint z){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI3ui\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI3uiv(GLuint index,const GLuint* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI3uiv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI4bv(GLuint index,const GLbyte* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI4bv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI4i(GLuint index,GLint x,GLint y,GLint z,GLint w){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI4i\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI4iv(GLuint index,const GLint* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI4iv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI4sv(GLuint index,const GLshort* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI4sv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI4ubv(GLuint index,const GLubyte* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI4ubv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI4ui(GLuint index,GLuint x,GLuint y,GLuint z,GLuint w){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI4ui\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI4uiv(GLuint index,const GLuint* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI4uiv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribI4usv(GLuint index,const GLushort* v){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribI4usv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribIPointer(GLuint index,GLint size,GLenum type,GLsizei stride,const void* pointer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribIPointer\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribP1ui(GLuint index,GLenum type,GLboolean normalized,GLuint value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribP1ui\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribP1uiv(GLuint index,GLenum type,GLboolean normalized,const GLuint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribP1uiv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribP2ui(GLuint index,GLenum type,GLboolean normalized,GLuint value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribP2ui\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribP2uiv(GLuint index,GLenum type,GLboolean normalized,const GLuint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribP2uiv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribP3ui(GLuint index,GLenum type,GLboolean normalized,GLuint value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribP3ui\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribP3uiv(GLuint index,GLenum type,GLboolean normalized,const GLuint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribP3uiv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribP4ui(GLuint index,GLenum type,GLboolean normalized,GLuint value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribP4ui\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribP4uiv(GLuint index,GLenum type,GLboolean normalized,const GLuint* value){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribP4uiv\x1b[0m\n");
}



SYS_PUBLIC void glVertexAttribPointer(GLuint index,GLint size,GLenum type,GLboolean normalized,GLsizei stride,const void* pointer){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glVertexAttribPointer\x1b[0m\n");
}



SYS_PUBLIC void glViewport(GLint x,GLint y,GLsizei width,GLsizei height){
	_gl_viewport[0]=x;
	_gl_viewport[1]=y;
	_gl_viewport[2]=width;
	_gl_viewport[3]=height;
	opengl_protocol_set_viewport_t command={
		.header.type=OPENGL_PROTOCOL_TYPE_SET_VIEWPORT,
		.header.length=sizeof(opengl_protocol_set_viewport_t),
		.tx=x+width/2.0f,
		.ty=y+height/2.0f,
		.sx=width/2.0f,
		.sy=-height/2.0f
	};
	_push_single_command(&(command.header));
}



SYS_PUBLIC void glWaitSync(GLsync sync,GLbitfield flags,GLuint64 timeout){
	printf("\x1b[1m\x1b[38;2;231;72;86mUnimplemented: glWaitSync\x1b[0m\n");
}