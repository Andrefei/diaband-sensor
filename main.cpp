#include <nan.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>

using namespace std;
using namespace Nan;
using namespace v8;

//
//	The function that we are going to call from NodeJS
//
NAN_METHOD(Sum)
{
	// run a process and create a streambuffer that reads its stdout and stderr
	string cmd = "./tag-raw_example";
	 string data;
	 FILE * stream;
	 const int max_buffer = 256;
	 char buffer[max_buffer];
	 cmd.append(" 2>&1");
	 printf("Open stream \n");
	 stream = popen(cmd.c_str(), "r");
	 if (stream) {
	 printf("if strem ==  true \n");
		 while (!feof(stream))
			 if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
			 pclose(stream);
	 }
	 printf("close stream \n");
	 printf("DATA!: %s\n", data.c_str());

	 uint8_t length = data.size();
	 char* str =  new char [length + 1];
	 std::strcpy (str, data.c_str());
	 //Str now contains a Cstring copy of data
	info
	.GetReturnValue()
	.Set(NewBuffer(str, length).ToLocalChecked());
}

//
//	The constructor
//
NAN_MODULE_INIT(Init)
{
	//
	//	Expose the method or methods to NodeJS
	//
	Nan::Set(
		target,
		New<String>("readData").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(Sum)).ToLocalChecked()
	);
}

//
//	Load the constructor
//
NODE_MODULE(basic_nan, Init)
