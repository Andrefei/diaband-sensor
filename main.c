#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#define _OPEN_THREADS
#include <time.h>
#include <errno.h>
#include "linux_nfc_api.h"

pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

nfcTagCallback_t g_TagCB;
nfc_tag_info_t g_tagInfos;

void onTagArrival(nfc_tag_info_t *pTagInfo){
    g_tagInfos = *pTagInfo;
    pthread_cond_signal(&condition);
}

void onTagDeparture(void){
    // We do not want anything to occur during tag departure
}

int main(int argc, char ** argv) {
  int res = 0x00;
  int i;

  g_TagCB.onTagArrival = onTagArrival;
  g_TagCB.onTagDeparture = onTagDeparture;

  nfcManager_doInitialize();
  nfcManager_registerTagCallback(&g_TagCB);
  nfcManager_enableDiscovery(DEFAULT_NFA_TECH_MASK, 0x01, 0, 0);
  //pthread_cond_t cond;
  //pthread_mutex_t mutex;
  time_t T;
  struct timespec t;

  if (pthread_mutex_init(&mutex, NULL) != 0) {
    perror("pthread_mutex_init() error");
    exit(1);
  }

  if (pthread_cond_init(&condition, NULL) != 0) {
    perror("pthread_cond_init() error");
    exit(2);
  }

  if (pthread_mutex_lock(&mutex) != 0) {
    perror("pthread_mutex_lock() error");
    exit(3);
  }

  time(&T);
  t.tv_sec = T + 2;
  printf("starting timedwait at %s", ctime(&T));
  if (pthread_cond_timedwait(&condition, &mutex, &t) != 0)
    if (errno == EAGAIN)
      puts("wait timed out");
    else {
      perror("pthread_cond_timedwait() error");
      exit(4);
    }

  time(&T);
  printf("timedwait over at %s", ctime(&T));


  //pthread_cond_wait(&condition, &mutex);
  // The only case for our project is ISO15693
  if (g_tagInfos.technology == TARGET_TYPE_ISO15693) {
    unsigned char ReadCmd[] = {0x02U, 0x20, /*block*/ 0x04};
    unsigned char Resp[255];
    res = nfcTag_transceive(g_tagInfos.handle, ReadCmd, sizeof(ReadCmd), Resp, 32, 3000);
    if(0x00 == res) {
        printf("RAW Tag transceive failed\n");
    }
    else {
        // Raw access to tag succeeded
        for(i = 0x00; i < (unsigned int)res; i++) printf("%02X ", Resp[i]); printf("\n\n");
    }
  }

  nfcManager_doDeinitialize();
  return 0;
}
