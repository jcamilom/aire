#include "select-demo.h"
#if DEMO == DEMO_TEST
#include "mbed.h"
#include "functions.h"
#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include <stdio.h>
#include <errno.h>

SDBlockDevice bd(PTE3, PTE1, PTE2, PTE4);
FATFileSystem fs("fs");

int main() {
  int error = 0;
  printf("Mounting the filesystem on \"/fs\". ");
  error = fs.mount(&bd);
  return_error(error);
  printf("Opening a new file, numbers.txt.\r\n");
  FILE* fd = fopen("/fs/pairs.txt", "w");
  errno_error(fd);
  printf("Writing decimals\r\n");
  for (int i = 0; i < 20; i++){
    fprintf(fd, "%d\r\n", i*2);
  }

  printf("Closing file.\r\n");
  fclose(fd);
  printf(" done.\r\n");

  printf("Re-opening file read-only.\r\n");
  fd = fopen("/fs/samples.csv", "r");
  errno_error(fd);

  printf("Dumping file to screen.\r\n");
  char buff[16] = {0};
  while (!feof(fd)){
    int size = fread(&buff[0], 1, 15, fd);
    fwrite(&buff[0], 1, size, stdout);
  }
  printf("EOF.\r\n");

  printf("Closing file.\r\n");
  fclose(fd);
  printf(" done.\r\n");

  printf("Opening root directory.\r\n");
  DIR* dir = opendir("/fs/");
  errno_error(fd);

  struct dirent* de;
  printf("Printing all filenames:\r\n");
  while((de = readdir(dir)) != NULL){
    printf("  %s\r\n", &(de->d_name)[0]);
  }

  printf("Closing root directory. \r\n");
  error = closedir(dir);
  return_error(error);
  printf("Filesystem Demo complete.\r\n");
  while (true) {}
}
#endif