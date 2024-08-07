import os
import subprocess
import sys



SIGNATURE_KEY_PREFIX="build/keys/"
SIGNATURE_KEY_SUFFIX=".key"
SIGNATURE_RELEASE_KEY_SUFFIX=".release.key"
SIGNATURE_SECTION_SIZE=4096
SIGNATURE_KEY_NAME_LENGTH=64



__all__=["load_key","get_public_key","sign"]



_signature_keys={}



def _decode_hex(data):
	out=bytearray(len(data)>>1)
	for i,e in enumerate(data.lower()):
		out[i>>1]|=(e-(87 if e>=97 else 48))<<(4-4*(i&1))
	return out



def load_key(key,file_path):
	if (not os.path.exists(file_path)):
		if (subprocess.run(["openssl","genpkey","-algorithm","rsa","-pkeyopt",f"rsa_keygen_bits:{4096 if '.release' in file_path else 512}","-out",file_path]).returncode):
			sys.exit(1)
	process=subprocess.run(["openssl","asn1parse","-in",file_path,"-inform","pem"],stdout=subprocess.PIPE)
	if (process.returncode):
		sys.exit(1)
	buffer=_decode_hex(process.stdout.split(b"[HEX DUMP]:")[1].strip())
	process=subprocess.run(["openssl","asn1parse","-inform","der"],stdout=subprocess.PIPE,input=buffer)
	if (process.returncode):
		sys.exit(1)
	buffer=process.stdout.split(b"\n")
	n=int.from_bytes(_decode_hex(buffer[2].split(b":")[-1]),"big")
	e=int.from_bytes(_decode_hex(buffer[3].split(b":")[-1]),"big")
	_signature_keys[key]=(file_path,n.bit_length()>>3,e,n)



def get_public_key(key):
	return (_signature_keys[key][2],_signature_keys[key][3])



def sign(digest,key):
	process=subprocess.run(["openssl","pkeyutl","-decrypt","-pkeyopt","rsa_padding_mode:none","-rev","-inkey",_signature_keys[key][0]],stdout=subprocess.PIPE,input=digest.ljust(_signature_keys[key][1],b"\x00"))
	return bytes(f"builtin-{key}","utf-8").ljust(SIGNATURE_KEY_NAME_LENGTH,b"\x00")+process.stdout[::-1].ljust(SIGNATURE_SECTION_SIZE-SIGNATURE_KEY_NAME_LENGTH,b"\x00")
