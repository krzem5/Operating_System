#include <glsl/ast.h>
#include <glsl/builtin_types.h>
#include <glsl/error.h>
#include <glsl/lexer.h>
#include <glsl/parser.h>
#include <glsl/shader.h>
#include <sys/string/string.h>
#include <sys/types.h>
#include <test/glsl_common.h>
#include <test/test.h>



static glsl_error_t _execute_parser(const char* src,glsl_shader_type_t shader_type,glsl_ast_t* out){
	glsl_lexer_token_list_t token_list;
	TEST_ASSERT(!glsl_lexer_extract_tokens(src,&token_list));
	glsl_error_t error=glsl_parser_parse_tokens(&token_list,shader_type,out);
	glsl_lexer_delete_token_list(&token_list);
	return error;
}



static bool _compare_storage(const glsl_ast_var_storage_t* storage,u8 type,u8 flags,u16 layout_location,const glsl_ast_block_t* block){
	bool out=1;
	out&=(storage->type==type);
	out&=(storage->flags==flags);
	out&=(storage->block==block);
	out&=(!(storage->flags&GLSL_AST_VAR_STORAGE_FLAG_HAS_LAYOUT_LOCATION)||storage->layout_location==layout_location);
	return out;
}



static const glsl_ast_block_t* _check_block(const glsl_ast_t* ast,const char* name,u8 storage_type,u8 storage_flags,u16 storage_layout_location){
	const glsl_ast_block_t* block=NULL;
	for (u32 i=0;i<ast->block_count;i++){
		if (!sys_string_compare(ast->blocks[i]->name,name)){
			block=ast->blocks[i];
			break;
		}
	}
	return (block&&_compare_storage(&(block->storage),storage_type,storage_flags,storage_layout_location,NULL)?block:NULL);
}



static bool _check_var_storage(const glsl_ast_t* ast,const char* name,u8 storage_type,u8 storage_flags,u16 storage_layout_location,const glsl_ast_block_t* storage_block){
	const glsl_ast_var_t* var=NULL;
	for (u32 i=0;i<ast->var_count;i++){
		if (!sys_string_compare(ast->vars[i]->name,name)){
			var=ast->vars[i];
			break;
		}
	}
	return (var&&_compare_storage(&(var->storage),storage_type,storage_flags,storage_layout_location,storage_block));
}



static bool _check_var_type_builtin(const glsl_ast_t* ast,const char* name,glsl_builtin_type_t builtin_type){
	const glsl_ast_var_t* var=NULL;
	for (u32 i=0;i<ast->var_count;i++){
		if (!sys_string_compare(ast->vars[i]->name,name)){
			var=ast->vars[i];
			break;
		}
	}
	bool out=0;
	if (var){
		out=(var->type->type==GLSL_AST_TYPE_TYPE_BUILTIN);
		out&=(var->type->builtin_type==builtin_type);
	}
	return out;
}



static bool _check_var_type_array_length(const glsl_ast_t* ast,const char* name,u32 array_length){
	const glsl_ast_var_t* var=NULL;
	for (u32 i=0;i<ast->var_count;i++){
		if (!sys_string_compare(ast->vars[i]->name,name)){
			var=ast->vars[i];
			break;
		}
	}
	return (var&&var->type->array_length==array_length);
}



static bool _check_var_value_const_int(const glsl_ast_t* ast,const char* name,s32 value){
	const glsl_ast_var_t* var=NULL;
	for (u32 i=0;i<ast->var_count;i++){
		if (!sys_string_compare(ast->vars[i]->name,name)){
			var=ast->vars[i];
			break;
		}
	}
	bool out=0;
	if (var&&var->value){
		out=1;
		out&=(var->value->type==GLSL_AST_NODE_TYPE_VAR_CONST);
		out&=(var->value->value_type->type==GLSL_AST_TYPE_TYPE_BUILTIN);
		out&=(var->value->value_type->builtin_type==GLSL_BUILTIN_TYPE_INT);
		out&=(var->value->var_int==value);
	}
	return out;
}



void test_glsl_parser(void){
	TEST_MODULE("glsl_parser");
	TEST_FUNC("glsl_parser_parse_tokens");
	TEST_GROUP("empty input");
	glsl_ast_t ast;
	TEST_ASSERT(!_execute_parser("",GLSL_SHADER_TYPE_ANY,&ast));
	glsl_ast_delete(&ast);
	TEST_GROUP("precision");
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("precision",GLSL_SHADER_TYPE_ANY,&ast),"Expected precision qualifier, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("precision 10.5",GLSL_SHADER_TYPE_ANY,&ast),"Expected precision qualifier, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("precision lowp",GLSL_SHADER_TYPE_ANY,&ast),"Expected type, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("precision mediump",GLSL_SHADER_TYPE_ANY,&ast),"Expected type, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("precision highp",GLSL_SHADER_TYPE_ANY,&ast),"Expected type, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("precision lowp int",GLSL_SHADER_TYPE_ANY,&ast),"Expected semicolon, got ???"));
	TEST_ASSERT(!_execute_parser("precision mediump float;",GLSL_SHADER_TYPE_ANY,&ast));
	glsl_ast_delete(&ast);
	TEST_GROUP("storage");
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("layout",GLSL_SHADER_TYPE_ANY,&ast),"Expected layout qualifiers, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("layout(",GLSL_SHADER_TYPE_ANY,&ast),"Expected layout qualifier, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("layout(<<=",GLSL_SHADER_TYPE_ANY,&ast),"Expected layout qualifier, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("layout(key",GLSL_SHADER_TYPE_ANY,&ast),"Expected end of layout qualifiers, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("layout(key=value",GLSL_SHADER_TYPE_ANY,&ast),"Expected integer constant, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("layout(key=2",GLSL_SHADER_TYPE_ANY,&ast),"Expected end of layout qualifiers, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("layout(location=1,location=2)",GLSL_SHADER_TYPE_ANY,&ast),"Duplicated layout qualifier 'location'"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("layout(location,",GLSL_SHADER_TYPE_ANY,&ast),"Expected layout qualifier, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("layout(location+",GLSL_SHADER_TYPE_ANY,&ast),"Expected end of layout qualifiers, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("layout(key)",GLSL_SHADER_TYPE_ANY,&ast),"Unknown layout qualifier 'key'"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("attribute",GLSL_SHADER_TYPE_ANY,&ast),"Deprecated keyword: attribute"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("varying",GLSL_SHADER_TYPE_ANY,&ast),"Deprecated keyword: varying"));
	TEST_ASSERT(!_execute_parser("int var_a=5;\ncentroid const float var_b=1.0;\nlayout (location=2) in mat4 var_c;\nlayout(location=1) centroid out bool var_d;\nuniform mat3x2 var_e;",GLSL_SHADER_TYPE_ANY,&ast));
	TEST_ASSERT(_check_var_storage(&ast,"var_a",GLSL_AST_VAR_STORAGE_TYPE_DEFAULT,0,0,NULL));
	TEST_ASSERT(_check_var_storage(&ast,"var_b",GLSL_AST_VAR_STORAGE_TYPE_CONST,GLSL_AST_VAR_STORAGE_FLAG_CENTROID,0,NULL));
	TEST_ASSERT(_check_var_storage(&ast,"var_c",GLSL_AST_VAR_STORAGE_TYPE_IN,GLSL_AST_VAR_STORAGE_FLAG_HAS_LAYOUT_LOCATION,2,NULL));
	TEST_ASSERT(_check_var_storage(&ast,"var_d",GLSL_AST_VAR_STORAGE_TYPE_OUT,GLSL_AST_VAR_STORAGE_FLAG_CENTROID|GLSL_AST_VAR_STORAGE_FLAG_HAS_LAYOUT_LOCATION,1,NULL));
	TEST_ASSERT(_check_var_storage(&ast,"var_e",GLSL_AST_VAR_STORAGE_TYPE_UNIFORM,0,0,NULL));
	glsl_ast_delete(&ast);
	TEST_GROUP("type");
	TEST_ASSERT(!_execute_parser("int var_a=1;\nfloat var_b=1.0;\nmat4 var_c;",GLSL_SHADER_TYPE_ANY,&ast));
	TEST_ASSERT(_check_var_type_builtin(&ast,"var_a",GLSL_BUILTIN_TYPE_INT));
	TEST_ASSERT(_check_var_type_builtin(&ast,"var_b",GLSL_BUILTIN_TYPE_FLOAT));
	TEST_ASSERT(_check_var_type_builtin(&ast,"var_c",GLSL_BUILTIN_TYPE_MAT44));
	glsl_ast_delete(&ast);
	// named types
	// structures
	TEST_GROUP("block");
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in block_name",GLSL_SHADER_TYPE_ANY,&ast),"Expected type, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in block_name-",GLSL_SHADER_TYPE_ANY,&ast),"Expected type, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in block_name{",GLSL_SHADER_TYPE_ANY,&ast),"Expected block member type, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in block_name{varying",GLSL_SHADER_TYPE_ANY,&ast),"Deprecated keyword: varying"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in block_name{centroid",GLSL_SHADER_TYPE_ANY,&ast),"Expected block member type, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in block_name{int",GLSL_SHADER_TYPE_ANY,&ast),"Expected block member name, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("float xyz;in block_name{int xyz",GLSL_SHADER_TYPE_ANY,&ast),"Identifier 'xyz' is already defined"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in block_name{int xyz",GLSL_SHADER_TYPE_ANY,&ast),"Expected semicolon, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in block_name{int xyz[",GLSL_SHADER_TYPE_ANY,&ast),"Expected right bracket, got ???"));
	// nonempty array length
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in block_name{int xyz[]",GLSL_SHADER_TYPE_ANY,&ast),"Expected semicolon, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in block_name{int xyz+",GLSL_SHADER_TYPE_ANY,&ast),"Expected semicolon, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in block_name{int xyz;}",GLSL_SHADER_TYPE_ANY,&ast),"Expected semicolon, got ???"));
	TEST_ASSERT(!_execute_parser("in block_name_a{};\ncentroid out block_name_b{out mat4 xxx;float yyy[];};",GLSL_SHADER_TYPE_ANY,&ast));
	TEST_ASSERT(_check_block(&ast,"block_name_a",GLSL_AST_VAR_STORAGE_TYPE_IN,0,0));
	const glsl_ast_block_t* block=_check_block(&ast,"block_name_b",GLSL_AST_VAR_STORAGE_TYPE_OUT,GLSL_AST_VAR_STORAGE_FLAG_CENTROID,0);
	TEST_ASSERT(block);
	TEST_ASSERT(_check_var_storage(&ast,"xxx",GLSL_AST_VAR_STORAGE_TYPE_OUT,GLSL_AST_VAR_STORAGE_FLAG_BLOCK,0,block));
	TEST_ASSERT(_check_var_type_builtin(&ast,"xxx",GLSL_BUILTIN_TYPE_MAT44));
	TEST_ASSERT(_check_var_type_array_length(&ast,"xxx",GLSL_AST_TYPE_NO_ARRAY));
	TEST_ASSERT(_check_var_storage(&ast,"yyy",GLSL_AST_VAR_STORAGE_TYPE_OUT,GLSL_AST_VAR_STORAGE_FLAG_BLOCK,0,block));
	TEST_ASSERT(_check_var_type_builtin(&ast,"yyy",GLSL_BUILTIN_TYPE_FLOAT));
	TEST_ASSERT(_check_var_type_array_length(&ast,"yyy",0));
	glsl_ast_delete(&ast);
	// block instance name
	TEST_GROUP("variable");
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in",GLSL_SHADER_TYPE_ANY,&ast),"Expected type, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int",GLSL_SHADER_TYPE_ANY,&ast),"Expected variable or function name, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int var",GLSL_SHADER_TYPE_ANY,&ast),"Expected semicolon, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int var[",GLSL_SHADER_TYPE_ANY,&ast),"Expected right bracket, got ???"));
	// nonempty array length
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int var[]",GLSL_SHADER_TYPE_ANY,&ast),"Expected semicolon, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int var;float var;",GLSL_SHADER_TYPE_ANY,&ast),"Identifier 'var' is already defined"));
	TEST_ASSERT(!_execute_parser("layout(location=3) out float xxx;\nuniform int yyy[];",GLSL_SHADER_TYPE_ANY,&ast));
	TEST_ASSERT(_check_var_storage(&ast,"xxx",GLSL_AST_VAR_STORAGE_TYPE_OUT,GLSL_AST_VAR_STORAGE_FLAG_HAS_LAYOUT_LOCATION,3,NULL));
	TEST_ASSERT(_check_var_type_builtin(&ast,"xxx",GLSL_BUILTIN_TYPE_FLOAT));
	TEST_ASSERT(_check_var_type_array_length(&ast,"xxx",GLSL_AST_TYPE_NO_ARRAY));
	TEST_ASSERT(_check_var_storage(&ast,"yyy",GLSL_AST_VAR_STORAGE_TYPE_UNIFORM,0,0,NULL));
	TEST_ASSERT(_check_var_type_builtin(&ast,"yyy",GLSL_BUILTIN_TYPE_INT));
	TEST_ASSERT(_check_var_type_array_length(&ast,"yyy",0));
	glsl_ast_delete(&ast);
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int var;float var=",GLSL_SHADER_TYPE_ANY,&ast),"Identifier 'var' is already defined"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int var=",GLSL_SHADER_TYPE_ANY,&ast),"Expected expression, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int var=if",GLSL_SHADER_TYPE_ANY,&ast),"Expected expression, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("in float xxx;\nint var=1.0+xxx;",GLSL_SHADER_TYPE_ANY,&ast),"Initializer is not constant"));
	TEST_ASSERT(!_execute_parser("const int var=5;\n",GLSL_SHADER_TYPE_ANY,&ast));
	TEST_ASSERT(_check_var_storage(&ast,"var",GLSL_AST_VAR_STORAGE_TYPE_CONST,0,0,NULL));
	TEST_ASSERT(_check_var_type_builtin(&ast,"var",GLSL_BUILTIN_TYPE_INT));
	TEST_ASSERT(_check_var_type_array_length(&ast,"var",GLSL_AST_TYPE_NO_ARRAY));
	TEST_ASSERT(_check_var_value_const_int(&ast,"var",5));
	glsl_ast_delete(&ast);
	TEST_GROUP("function");
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int func+",GLSL_SHADER_TYPE_ANY,&ast),"Expected argument list, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("out int func(",GLSL_SHADER_TYPE_ANY,&ast),"Storage '???' cannot be applied to a function"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("centroid int func(",GLSL_SHADER_TYPE_ANY,&ast),"Storage '???' cannot be applied to a function"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int func(",GLSL_SHADER_TYPE_ANY,&ast),"Expected argument list, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int func(void",GLSL_SHADER_TYPE_ANY,&ast),"Expected end of argument list, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int func(void+",GLSL_SHADER_TYPE_ANY,&ast),"Expected end of argument list, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int func()",GLSL_SHADER_TYPE_ANY,&ast),"Expected function definition or declaration, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int func(void)",GLSL_SHADER_TYPE_ANY,&ast),"Expected function definition or declaration, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int func(void){}\nint func(void){}",GLSL_SHADER_TYPE_ANY,&ast),"Identifier 'func' is already defined"));
	TEST_ASSERT(!_execute_parser("int func(void);\nint func(void){}",GLSL_SHADER_TYPE_ANY,&ast));
	glsl_ast_delete(&ast);
	TEST_ASSERT(!_execute_parser("float func(void){}\nint func(void){}",GLSL_SHADER_TYPE_ANY,&ast));
	glsl_ast_delete(&ast);
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int func(void)+",GLSL_SHADER_TYPE_ANY,&ast),"Expected function definition, got ???"));
	TEST_GROUP("statements");
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("void func(void){",GLSL_SHADER_TYPE_ANY,&ast),"Expected statement, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("void func(void){int",GLSL_SHADER_TYPE_ANY,&ast),"Expected variable name, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int func(void){int func",GLSL_SHADER_TYPE_ANY,&ast),"Identifier 'func' is already defined"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int func(void){int x=2,",GLSL_SHADER_TYPE_ANY,&ast),"Expected variable name, got ???"));
	TEST_ASSERT(test_glsl_check_and_cleanup_error(_execute_parser("int func(void){int x=2,y=3",GLSL_SHADER_TYPE_ANY,&ast),"Expected operator, got ???"));
	TEST_GROUP("expressions");
	// expressions
}
