#include "DefUseChainAccess.h"
#include <stdio.h>
#include <inttypes.h>

void printDefUsePair(const DefUsePair* pair) {
    printf("Def: %"PRIxPTR", Use: %"PRIxPTR"\n", pair->def, pair->use);
}

void printDefUsePairs(const DefUsePair_list* list) {
    while (list != NULL) {
        printDefUsePair(list->def_use_chain);
        list = list->next;
    }
}


int main() {
    useDataInAnotherFile();
    uintptr_t lookup_pc = 401000;
    // uintptr_t lookup_pc2 = strtoul(lookup_pc, NULL, 16);
    printf("Printing def-use pairs for PC: 0x%"PRIxPTR"\n", lookup_pc);
    // printDefUsePairsForPC(lookup_pc);
    DefUsePair* pair = getDefUsePairForPC(lookup_pc);
    // printf("Def: 0x%x, Use: 0x%x\n", pair->def, pair->use);
    // if (pair == NULL) {
    //     printf("No def-use pairs found for PC: 0x%x\n", lookup_pc);
    // }

    while (pair != NULL) {
        printf("the finding Def: 0x%"PRIxPTR", Use: 0x%"PRIxPTR"\n", pair->def, pair->use);
        pair = pair->next; // Move to the next pair in the list
    }
    free_json_data_list();
    freeDefUseMap();
    return 0;
}