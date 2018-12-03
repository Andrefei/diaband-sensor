#include <nan.h>
#include <iostream>
//Example inclusions
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include "linux_nfc_api.h"
//---
using namespace std;
using namespace Nan;
using namespace v8;

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

nfcTagCallback_t g_TagCB;
nfc_tag_info_t g_tagInfos;

void onTagArrival(nfc_tag_info_t *pTagInfo){
    printf("Tag detected\n");
    g_tagInfos = *pTagInfo;
    pthread_cond_signal(&condition);
}

void onTagDeparture(void){
    printf("Tag removed\n-------------\n");
    printf("\n-------------\nWaiting for tag...\n");
}

int main(int argc, char ** argv) {

}

//
//	The function that we are going to call from NodeJS
//
NAN_METHOD(Sum)
{
	pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

	nfcTagCallback_t g_TagCB;
	nfc_tag_info_t g_tagInfos;

	void onTagArrival(nfc_tag_info_t *pTagInfo){
	    printf("Tag detected\n");
	    g_tagInfos = *pTagInfo;
	    pthread_cond_signal(&condition);
	}

	void onTagDeparture(void){
	    printf("Tag removed\n-------------\n");
	    printf("\n-------------\nWaiting for tag...\n");
	}

	int res = 0x00;
	int i;

	g_TagCB.onTagArrival = onTagArrival;
	g_TagCB.onTagDeparture = onTagDeparture;
	nfcManager_doInitialize();
	nfcManager_registerTagCallback(&g_TagCB);
	nfcManager_enableDiscovery(DEFAULT_NFA_TECH_MASK, 0x01, 0, 0);

	printf("\n-------------\nWaiting for tag...\n");
	do{
			pthread_cond_wait(&condition, &mutex);

			/* Raw access to tag */
			printf("Tag read and the tech is: ");
			printf(" %s\n", g_tagInfos.technology);
			/*
			switch (g_tagInfos.technology)
			{
					case TARGET_TYPE_MIFARE_UL:
					{
							unsigned char ReadCmd[] = {0x30U, 0x00};
							unsigned char Resp[16];
							printf("MifareUL Read command: ");
							for(i = 0x00; i < (unsigned int) sizeof(ReadCmd) ; i++) printf("%02X ", ReadCmd[i]); printf("\n");
							res = nfcTag_transceive(g_tagInfos.handle, ReadCmd, sizeof(ReadCmd), Resp, sizeof(Resp), 500);
							if(0x00 == res) {
									printf("RAW Tag transceive failed\n");
							}
							else {
									printf("MifareUL Read command sent - Response: ");
									for(i = 0x00; i < (unsigned int)res; i++) printf("%02X ", Resp[i]); printf("\n\n");
							}
					} break;

					case TARGET_TYPE_MIFARE_CLASSIC:
					{
							unsigned char AuthCmd[] = {0x60U, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
							unsigned char ReadCmd[] = {0x30U, 0x01};
							unsigned char Resp[20];
							printf("MifareClassic Authenticate command: ");
							for(i = 0x00; i < (unsigned int) sizeof(AuthCmd) ; i++) printf("%02X ", AuthCmd[i]); printf("\n");
							res = nfcTag_transceive(g_tagInfos.handle, AuthCmd, 12, Resp, sizeof(Resp), 500);
							if(0x00 == res)
							{
									printf("\n\t\tRAW Tag transceive failed\n");
							}
							else {
									printf("MifareClassic Authenticate command sent - Response: ");
									for(i = 0x00; i < (unsigned int)res; i++) printf("%02X ", Resp[i]); printf("\n\n");
							}
							printf("MifareUL Read command: ");
							for(i = 0x00; i < (unsigned int) sizeof(ReadCmd) ; i++) printf("%02X ", ReadCmd[i]); printf("\n");
							res = nfcTag_transceive(g_tagInfos.handle, ReadCmd, sizeof(ReadCmd), Resp, sizeof(Resp), 500);
							if(0x00 == res) {
									printf("RAW Tag transceive failed\n");
							}
							else {
									printf("MifareUL Read command sent - Response: ");
									for(i = 0x00; i < (unsigned int)res; i++) printf("%02X ", Resp[i]); printf("\n\n");
							}
					} break;

					case TARGET_TYPE_ISO15693:
					{
							unsigned char ReadCmd[] = {0x02U, 0x20, 0x04};
							unsigned char Resp[255];
							printf("ISO15693 Read command: ");
							for(i = 0x00; i < (unsigned int) sizeof(ReadCmd) ; i++) printf("%02X ", ReadCmd[i]); printf("\n");
							res = nfcTag_transceive(g_tagInfos.handle, ReadCmd, sizeof(ReadCmd), Resp, 32, 3000);
							if(0x00 == res) {
									printf("RAW Tag transceive failed\n");
							}
							else {
									printf("ISO15693 Read command sent - Response: ");
									for(i = 0x00; i < (unsigned int)res; i++) printf("%02X ", Resp[i]); printf("\n\n");
							}

					} break;

					default:
					{
							printf("Unsupported tag type\n");
					} break;
			}
			*/
	}while(1);
	nfcManager_doDeinitialize();
	//
	//	1.	Save the buffers that passed from NodeJS in to local variables
	//
	unsigned int nrOne = info[0]->Uint32Value();
	unsigned int nrDwo = info[1]->Uint32Value();

	//
	//	2.	Sum the two numbers together
	//
	uint32_t sum = nrOne + nrDwo;

	//
	//	->	Send the buffer back to NodeJS with the result of our calculation.
	//
	info
	.GetReturnValue()
	.Set(
		Nan::New<Uint32>(sum));
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
		New<String>("sum").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(Sum)).ToLocalChecked()
	);
}

//
//	Load the constructor
//
NODE_MODULE(basic_nan, Init)
