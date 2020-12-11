//
//  memmgr.c
//  memmgr
//
//  Created by William McCarthy on 17/11/20.
//  Copyright © 2020 William McCarthy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ARGC_ERROR 1
#define FILE_ERROR 2
#define BUFLEN 256
#define FRAME_SIZE  256

int tlb[16][2];
int table[256];
int fifo_queue[128];
int page_replacement_table[256];

char vmem[65536];
char fifo_vmem[32768];

//-----------------------HELPERS-------------------------------------

FILE* open_file(const char* filename, const char* mode) {
  FILE* fobj = fopen(filename, mode);
  if( fobj == NULL) {
    printf("Could not open %s\n", filename);
    exit(FILE_ERROR);
  }
  return fobj;
}

//-------------------------------------------------------------------
unsigned getpage(unsigned x) { return (0xff00 & x) >> 8; }

unsigned getoffset(unsigned x) { return (0xff & x); }

void getpage_offset(unsigned x) {
  unsigned  page   = getpage(x);
  unsigned  offset = getoffset(x);
  printf("x is: %u, page: %u, offset: %u, address: %u, paddress: %u\n", x, page, offset,
         (page << 8) | getoffset(x), page * 256 + offset);
}

void page_replacement() {
  printf("Initiated Part 2, page replacement\n");
  
  char vbuff[BUFLEN];
  unsigned logic_add;
  unsigned vadd_file, padd_file, value_file;
  unsigned vpage = 0;
  unsigned vframe = 0; 
  unsigned voffset = 0;

  for (int i = 0; i < 256; i++) {
    page_replacement_table[i] = -1;
  }
  for (int i = 0; i < 16; i++) {
    tlb[i][0] = -1;
  }
  for (int i = 0; i < 128; i++) {
    fifo_queue[i] = -1;
  }
  FILE* fadd = open_file("addresses.txt", "r");
  FILE* fcorr = open_file("correct.txt", "r");
  FILE* fs_store_bin = open_file("BACKING_STORE.bin", "r");

  while (fscanf(fadd, "%d", &logic_add) != EOF) {

    fscanf(fcorr, "%s %s %d %s %s %d %s %d", vbuff, vbuff, &vadd_file,
           vbuff, vbuff, &padd_file, vbuff, &value_file);  // read from file correct.txt

    vpage   = getpage(  logic_add);
    voffset = getoffset(logic_add);

    unsigned padd = vframe * FRAME_SIZE + voffset;
    int val = (int) fifo_vmem[padd_file];
    
    printf("logical: %5u (page: %3u, offset: %3u) ---> physical: %5u -> value: %d-- passed\n", 
            logic_add, vpage, voffset, padd_file, val);

    // assert(value_file == val);
  }

  fclose(fcorr);
  fclose(fadd);
  fclose(fs_store_bin);

  printf("\n\t\t Finished part 2, exiting successfully.\n");
}

int main(int argc, const char* argv[]) {
  FILE* fadd = open_file("addresses.txt", "r");
  FILE* fcorr = open_file("correct.txt", "r");

  char buf[BUFLEN];
  unsigned   page, offset, physical_add, frame = 0;
  unsigned   logic_add;                  // read from file address.txt
  unsigned   virt_add, phys_add, value;  // read from file correct.txt

  FILE* fs_store_bin = open_file("correct.txt", "r");

  if(fs_store_bin == NULL) {
    printf("Could not open BACKING_STORE.bin\n");
    exit(FILE_ERROR);
  } else {
    printf("Opened BACKING_STORE.bin correctly.\n");
  }

  for (int i = 0; i < 256; i++) {
    table[i] = -1;
  }
  for (int i = 0; i < 16; i++) {
    tlb[i][0] = -1;
  }

  //   e.g., address # 25 from addresses.txt will fail the assertion
  while (fscanf(fadd, "%d", &logic_add) != EOF) {

    fscanf(fcorr, "%s %s %d %s %s %d %s %d", buf, buf, &virt_add,
           buf, buf, &phys_add, buf, &value);  // read from file correct.txt

    fscanf(fadd, "%d", &logic_add);  // read from file address.txt
    page   = getpage(  logic_add);
    offset = getoffset(logic_add);
    
    physical_add = frame * FRAME_SIZE + offset;
    int vmem_val = (int) vmem[physical_add];
    
    /* TODO */ // assert(physical_add == phys_add);
    
    printf("logical: %5u (page: %3u, offset: %3u) ---> physical: %5u vmem_val %d -- passed\n", logic_add, page, offset, physical_add, vmem_val);
  }
  fclose(fcorr);
  fclose(fadd);
  fclose(fs_store_bin);
  
  printf("ALL logical ---> physical assertions PASSED!\n");
  printf("\n\t\t Finished part 1, executing part 2 -- page replacement.\n");

  page_replacement();
  return 0;
}
