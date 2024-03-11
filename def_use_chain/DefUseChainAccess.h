#include <stdint.h>
#include "uthash.h"
#ifndef DEFUSECHAINACCESS_H
#define DEFUSECHAINACCESS_H


// #include "disas/disas.h"
// #include "exec/exec-all.h"
// #include "tcg/tcg.h"
// typedef int32_t target_long;
typedef uint32_t target_ulong;
typedef __SIZE_TYPE__ size_t;

// Update to include a structure definition
typedef struct DefUsePair{
    uintptr_t def;
    uintptr_t use;
    struct DefUsePair *next;    
}DefUsePair;


typedef struct PcDefUseMapEntry{
    uintptr_t pc; // Key
    DefUsePair *pairs; 
    int num_pairs;
    UT_hash_handle hh; 
} PcDefUseMapEntry;

extern PcDefUseMapEntry *pcDefUseMap;

// void initDefUseMap();
void addDefUsePairToMap(uintptr_t pc, DefUsePair *pair);
void printDefUsePairsForPC(uintptr_t pc);
void freeDefUseMap(void);
DefUsePair* getDefUsePairForPC(uintptr_t pc);


typedef struct DefUsePair_list DefUsePair_list;

struct DefUsePair_list{
    DefUsePair *def_use_chain;
    DefUsePair_list *next;     
} ;

typedef struct {
    target_ulong tb;
    uintptr_t pc;
    target_ulong tb_code;
    size_t size;
    int num_def;
    int num_use;
    DefUsePair_list* def_use_list_head; // Head of the linked list of def-use pairs
    size_t def_use_count; 
    PcDefUseMapEntry *pcMap;
} JsonData;

typedef struct JsonData_list {
    JsonData data;
    struct JsonData_list *next;
} JsonData_list;

void read_json_from_file(const char *filename);
JsonData_list* get_json_data_list(void);
void free_json_data_list(void);
void useDataInAnotherFile(void);
#endif
