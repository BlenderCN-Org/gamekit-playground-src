
#include "gkCrypt.h"

#include "gkDebugScreen.h"

#ifndef OGREKIT_USE_OPENSSL
	gkString gkCrypt::md5(const gkString& md5){
		gkLogger::write("tried to use md5 but openssl is not included!",true);
		return "";
	}

	gkString gkCrypt::blowfishEncrypt(const gkString& keydata, const gkString& in){
		gkLogger::write("tried to encrypt blowfish but openssl is not included!",true);
		return "";
	}

	gkString gkCrypt::blowfishDecrypt(const gkString& keydata, const gkString& in){
		gkLogger::write("tried to decrypt blowfishbut openssl is not included!",true);
		return "";
	}

#else

# include "openssl/md5.h"
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <stdio.h>
# include <openssl/blowfish.h>

	gkString gkCrypt::md5(const gkString& md5) {
		unsigned char digest[16];
		const char* string = md5.c_str();
		MD5_CTX context;
		MD5_Init(&context);
		MD5_Update(&context, string, strlen(string));
		MD5_Final(digest, &context);

		char md5string[33];
		for(int i = 0; i < 16; ++i)
		    sprintf(&md5string[i*2], "%02x", (unsigned int)digest[i]);

		return gkString(md5string);
	}



	/************************************************** ****************
	* ARGS:
	* keydata == ascii text, the encryption passphrase
	* keydatalen == how long keydata is
	* in == the data to be encrypted
	* out == the encrypted data.
	* length(in) == length(out), apparently
	* inlen == length of the in array
	************************************************** ****************/
//	void bfencrypt(unsigned char *keydata, int keydatalen, char *in, char
	gkString gkCrypt::blowfishEncrypt(const gkString& keydata, const gkString& in) {
		gkDebugScreen::printTo("WARNING! blowfish encryption not working!");
		BF_KEY key;
		unsigned char ivec[32];
		int num=0;
		// set up for decryption
		const unsigned char* us =
		reinterpret_cast<const unsigned char*>(keydata.c_str());
		BF_set_key(&key, keydata.length(), us);
		memset(ivec, '\0', 32);

		unsigned char* out = new unsigned char[64000];

		us = reinterpret_cast<const unsigned char*>(in.c_str());
		BF_cfb64_encrypt(us, out, in.length(), &key, ivec, &num, BF_ENCRYPT);

		char res[65000];
		for(int i = 0; i < in.length(); ++i)
			sprintf(&res[i], "%02x", (unsigned char)out[i]);

		gkString result(res);

		delete out;
		return result;
	}

	gkString gkCrypt::blowfishDecrypt(const gkString& keydata, const gkString& in) {
		gkDebugScreen::printTo("WARNING! blowfish encryption not working!");

		BF_KEY key;
		unsigned char ivec[32];
		int num=0;
		// set up for decryption
		const unsigned char* us =
		reinterpret_cast<const unsigned char*>(keydata.c_str());
		BF_set_key(&key, keydata.length(), us);
		memset(ivec, '\0', 32);

		unsigned char* out = new unsigned char[64000];

		us = reinterpret_cast<const unsigned char*>(in.c_str());
		BF_cfb64_encrypt(us, out, in.length(), &key, ivec, &num, BF_DECRYPT);

		char res[65000];
		for(int i = 0; i < in.length(); ++i)
			sprintf(&res[i], "%02x", (unsigned char)out[i]);

		gkString result(res);

		delete out;
		return result;
		}



#endif


