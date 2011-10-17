/*
	This is my wrapper-class to create
	a MD5 Hash from a string and a file.

	This code is completly free, you 
	can copy it, modify it, or do 
	what ever you want with it.

	Feb. 2005
	Benjamin Grüdelbach
*/

//include protection
#ifndef MD5WRAPPER_H
#define MD5WRAPPER_H

//basic includes
#include <string>

//forwards
class MD5;

//namespaces
using namespace std;

class md5wrapper
{
	private:
		MD5 *md5;
	
		/*
		internal hash function, calling
		the basic methods from md5.h
		*/	
		char* hashit(char* text);

		/*
		converts the numeric giets to
		a valid std::string
		*/
		char* convToString(unsigned char *bytes);
	public:
		//constructor
		md5wrapper();

		//destructor
		~md5wrapper();
		
		/*
		creates a MD5 hash from
		"text" and returns it as
		string
		*/	
		char* getHashFromString(char* text);

		/*
		creates a MD5 hash from
		a file specified in "filename" and 
		returns it as string
		*/	
		string getHashFromFile(string filename);
};


//include protection
#endif
