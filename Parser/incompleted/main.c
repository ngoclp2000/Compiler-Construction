/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "reader.h"
#include "parser.h"

/******************************************************************/

int main(int argc, char *argv[]) {
  rand();
  if (argc <= 1) {
    printf("parser: no input file.\n");
    return -1;
  }

  if (compile(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}
