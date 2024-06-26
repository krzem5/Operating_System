#include <common/compressor/compressor.h>
#include <common/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc,const char** argv){
	if (argc<4){
		printf("Usage:\n\n%s <file> <compression_level> <output_file>\n",(argc?argv[0]:"compressor"));
		return 1;
	}
	u32 compression_level;
	if (!strcmp(argv[2],"none")){
		compression_level=COMPRESSOR_COMPRESSION_LEVEL_NONE;
	}
	else if (!strcmp(argv[2],"fast")){
		compression_level=COMPRESSOR_COMPRESSION_LEVEL_FAST;
	}
	else if (!strcmp(argv[2],"full")){
		compression_level=COMPRESSOR_COMPRESSION_LEVEL_FULL;
	}
	else{
		printf("Invalid compression level '%s'\n",argv[2]);
		return 1;
	}
	FILE* in=fopen(argv[1],"rb");
	if (!in){
		printf("Unable to open input file '%s'\n",argv[1]);
		return 1;
	}
	FILE* out=fopen(argv[3],"wb");
	if (!out){
		fclose(in);
		printf("Unable to open output file '%s'\n",argv[3]);
		return 1;
	}
	fseek(in,0,SEEK_END);
	u32 in_length=ftell(in);
	fseek(in,0,SEEK_SET);
	void* in_data=malloc(in_length);
	if (fread(in_data,1,in_length,in)!=in_length||fwrite(&in_length,1,sizeof(u32),out)!=sizeof(u32)){
		goto _error;
	}
	compressor_output_t output={
		out
	};
	compressor_compress(in_data,in_length,compression_level,&output);
	free(in_data);
	fclose(in);
	fclose(out);
	return 0;
_error:
	free(in_data);
	fclose(in);
	fclose(out);
	printf("Unable to compress file\n");
	return 1;
}
