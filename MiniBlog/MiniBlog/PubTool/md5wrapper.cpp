/*
	This is part of my wrapper-class to create
	a MD5 Hash from a string and a file.

	This code is completly free, you 
	can copy it, modify it, or do 
	what ever you want with it.

	Feb. 2005
	Benjamin Grüdelbach
*/

//basic includes
#include "stdafx.h"
#include <string>
#include <fstream>
#include <iostream>

//my includes
#include "md5wrapper.h"
#include "md5.h"

//namespaces
using namespace std;

//---------privates--------------------------

/*
internal hash function, calling
the basic methods from md5.h
*/	
char* md5wrapper::hashit(char* text)
{
	MD5_CTX ctx;
	
	//init md5
	md5->MD5Init(&ctx);
	//update with our string
	md5->MD5Update(&ctx,
		 (unsigned char*)text,
		 strlen(text));
	
	//create the hash
	unsigned char buff[16] = "";	
	md5->MD5Final((unsigned char*)buff,&ctx);

	//converte the hash to a string and return it
	return convToString(buff);	
}

/*
converts the numeric hash to
a valid std::string.
(based on Jim Howard's code;
http://www.codeproject.com/cpp/cmd5.asp)
*/
char* md5wrapper::convToString(unsigned char *bytes)
{
	char asciihash[33]="";

	int p = 0;
	for(int i=0; i<16; i++)
	{
		::sprintf(&asciihash[p],"%02x",bytes[i]);
		p += 2;
	}
	asciihash[32]='\0';
	char* tmpstr = new char[strlen(asciihash)+1];
	strcpy(tmpstr, asciihash);
	return tmpstr;
}

//---------publics--------------------------

//constructor
md5wrapper::md5wrapper()
{
	md5 = new MD5();
}


//destructor
md5wrapper::~md5wrapper()
{
	try
	{
	delete md5;
	}
	catch(...)
	{
		md5 = NULL;
	}
}

/*
creates a MD5 hash from
"text" and returns it as
string
*/	
char* md5wrapper::getHashFromString(char* text)
{
	return this->hashit(text); 
}


/*
creates a MD5 hash from
a file specified in "filename" and 
returns it as string
(based on Ronald L. Rivest's code
from RFC1321 "The MD5 Message-Digest Algorithm")
*/	
string md5wrapper::getHashFromFile(string filename)	
{
	FILE *file;
  	MD5_CTX context;
  
	int len;
  	unsigned char buffer[1024], digest[16];

	//open file
  	if ((file = fopen (filename.c_str(), "rb")) == NULL)
	{
		return "-1";
	}

	//init md5
 	md5->MD5Init (&context);
 	
	//read the filecontent
	while (len = fread (buffer, 1, 1024, file))
   	{
		md5->MD5Update (&context, buffer, len);
	}
	
	/*
	generate hash, close the file and return the
	hash as std::string
	*/
	md5->MD5Final (digest, &context);
 	fclose (file);
	return convToString(digest);
 }	
