#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

typedef void (*orig_flip_f_type)();

void al_flip_display () {
  static unsigned char debugged;
  if (!debugged) {
    debugged = 1;
    pid_t pid = getpid();
    printf("Pid: 0d%d\n", pid);
    char buf[128];
    sprintf(buf, "/proc/%d/maps", pid);
    printf("Maps file location: %s\n", buf);
    FILE* fp = fopen(buf, "r");
    if (fp==NULL) {
      printf("Error opening file.\n");
    }
    else {
      char line[256];
      fgets(line, 256, fp);
      printf("First line of maps file: %s\n", line);
      fclose(fp);
      char* baseAddressText = strtok(line,"-");
      long long int baseAddress = (long long int)strtol(baseAddressText, NULL, 16);
      printf("The base address of memory appears to be: 0x%lX\n", baseAddress);
      long long int result = baseAddress + 0x5148;
      printf("Therefore, the address of the game data pointer should be 0x%lX\n", result);
      long long int* GameDataGlobalPointer = (long long int*)result;
      printf("The data at this location is 0x%lX\n", *GameDataGlobalPointer);
      long long int* GameDataAddress = (long long int*) *GameDataGlobalPointer;
      long long int* BoardPointer = GameDataAddress + 1;
      printf("The data in the offset for the board pointer is 0x%lX\n", *BoardPointer);
      unsigned char* Board = (unsigned char*) *BoardPointer;
      printf("The board is %dx%d\n", Board[4], Board[5]);
      long long int* BoardData = (long long int*)(Board + 8);
      printf("The address of the board data is 0x%lX\n", *BoardData);
      long long int* Data = (long long int*)(*BoardData);
      printf("Full dump of the board:\n");
      for(int i = 0; i<Board[4]; i++) {
        for(int j = 0; j<Board[5]; j++) {
          int index = i * Board[5] + j;
          if (Data[index] == 0x1) {
            printf("[ ]");
          } else {
            if (Data[index] == 0x2) {
              printf("[*]");
            } else {
              printf("[?]");
            }
          }
        }
        printf("\n");
      }
    }
  }
  orig_flip_f_type orig_flip;
  orig_flip = (orig_flip_f_type)dlsym(RTLD_NEXT,"al_flip_display");
  orig_flip();
  return;
}
