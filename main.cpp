#include <nan.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
	// run a process and create a streambuf that reads its stdout and stderr
	//printf("Start of Cpp\n");
	time_t start_time, end_time;
	double dif;
	double timeout = 3.0;
	time(&start_time);	// Set start_time to be current time in seconds
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
		while (!feof(stream)) {
			//printf("Hello from loop \n");
			if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
			pclose(stream);
			time(&end_time);	// Set end_time to current time in seconds
			dif = difftime(end_time, start_time);	// calculate time elapsed
			if (dif >= timeout) break;
		}
	}
	printf("close stream \n");
	printf("DATA!: %s\n", data.c_str());

	 //Copy data to str as a C-String
	 uint8_t length = data.size();
	 char* str =  new char [length + 1];
	 std::strcpy (str, data.c_str());
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
