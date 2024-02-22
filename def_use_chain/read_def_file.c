#include "DefUseChainAccess.h"
#include <stdio.h>
#include <inttypes.h>

void printDefUsePair(const DefUsePair* pair) {
    printf("Def: %x, Use: %x\n", pair->def, pair->use);
}

void printDefUsePairs(const DefUsePair_list* list) {
    while (list != NULL) {
        printDefUsePair(list->def_use_chain);
        list = list->next;
    }
}


int main() {
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
    target_long lookup_pc = 0x401000;
    printf("Printing def-use pairs for PC: 0x%" PRIx32 "\n", lookup_pc);
    printDefUsePairsForPC(lookup_pc);
    free_json_data_list();
    freeDefUseMap();
    return 0;
}