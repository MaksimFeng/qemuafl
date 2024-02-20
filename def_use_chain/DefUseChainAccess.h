#include <stdint.h>
#ifndef DATA_ACCESS_H
#define DATA_ACCESS_H
typedef int32_t target_long;
typedef uint32_t target_ulong;
typedef __SIZE_TYPE__ size_t;

// Update to include a structure definition
typedef struct {
    target_ulong def;
    target_ulong use;
}DefUsePair;

typedef struct DefUsePair_list DefUsePair_list;

struct DefUsePair_list{
    DefUsePair *def_use_chain;
    DefUsePair_list *next;     
} ;

typedef struct {
    target_ulong tb;
    target_ulong pc;
    target_ulong tb_code;
    size_t size;
    int num_def;
    int num_use;
    DefUsePair_list* def_use_list_head; // Head of the linked list of def-use pairs
    size_t def_use_count; 
} JsonData;

typedef struct JsonData_list {
    JsonData data;
    struct JsonData_list *next;
} JsonData_list;
// extern JsonData_list *jsonDataListHead;
void read_json_from_file(const char *filename);
JsonData_list* get_json_data_list(void);
void free_json_data_list(void);
void useDataInAnotherFile(void);

#endif
