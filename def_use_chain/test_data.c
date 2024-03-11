#include "DefUseChainAccess.h"
#include <stdio.h>
#include <inttypes.h>

extern void printDefUsePairs(const DefUsePair_list* list); 
extern JsonData_list *jsonDataListHead; 

uintptr_t get_address() {
    return 0x401000;
}

void useDataInAnotherFile() {
    // JsonData_list* dataList = get_json_data_list();




    read_json_from_file("data_def_use_chain.json");

    // useDataInAnotherFile();
    JsonData_list* dataList = get_json_data_list();

    while (dataList) {
        JsonData* data = &dataList->data;
        DefUsePair_list* defUseList = data->def_use_list_head;
        // printf("TB: %x, PC: %x, TB_CODE: %x\n", data->tb, data->pc, data->tb_code);
        // printf("Size: %zu, Num_Def: %d, Num_Use: %d\n", data->size, data->num_def, data->num_use);
        
        // printDefUsePairs(data->def_use_list_head);
        while(defUseList){
            DefUsePair* pair = defUseList->def_use_chain;
            addDefUsePairToMap(data->pc, pair);
            defUseList = defUseList->next;
        }
        
        dataList = dataList->next;
    }

    // target_ulong lookup_pc = 0x401000;
    // // printf("Printing def-use pairs for PC: 0x%" PRIx32 "\n", lookup_pc);
    // // printDefUsePairsForPC(lookup_pc);
    // DefUsePair* pair = getDefUsePairForPC(lookup_pc);

    // while (pair != NULL) {
    //     printf("the finding Def: 0x%x, Use: 0x%x\n", pair->def, pair->use);
    //     pair = pair->next; // Move to the next pair in the list
    // }





    // if (dataList) {
    //     // Example: Just print the first entry
    //     printf("------------------------\n");
    //     printf("TB: %x, PC: %x, TB_CODE: %x\n", dataList->data.tb, dataList->data.pc, dataList->data.tb_code);

    //     printf("------------------------\n");

    //     printDefUsePairs(dataList->data.def_use_list_head);
    // }
    // dataList = dataList->next;
    // if (dataList) {
    //     // Example: Just print the first entry
    //     printf("------------------------\n");
    //     printf("TB: %x, PC: %x, TB_CODE: %x\n", dataList->data.tb, dataList->data.pc, dataList->data.tb_code);

    //     printf("------------------------\n");

    //     printDefUsePairs(dataList->data.def_use_list_head);
    // }
    // for (JsonData_list *current = jsonDataListHead; current != NULL; current = current->next) {
    //     PcDefUseMapEntry *mapEntry, *tmp;
    //     HASH_ITER(hh, current->data.pcMap, mapEntry, tmp) {
    //         printf("PC: %x\n", mapEntry->pc);
    //         for (DefUsePair *pair = mapEntry->pairs; pair != NULL; pair = pair->next) {
    //             printf(" Def: %x, Use: %x\n", pair->def, pair->use);
    //         }
    //     }
    // }
}