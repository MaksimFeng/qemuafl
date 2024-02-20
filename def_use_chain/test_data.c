#include "DefUseChainAccess.h"
#include <stdio.h>

extern void printDefUsePairs(const DefUsePair_list* list); 

void useDataInAnotherFile() {
    JsonData_list* dataList = get_json_data_list();
    if (dataList) {
        // Example: Just print the first entry
        printf("------------------------\n");
        printf("TB: %x, PC: %x, TB_CODE: %x\n", dataList->data.tb, dataList->data.pc, dataList->data.tb_code);

        printf("------------------------\n");

        printDefUsePairs(dataList->data.def_use_list_head);
    }
    dataList = dataList->next;
    if (dataList) {
        // Example: Just print the first entry
        printf("------------------------\n");
        printf("TB: %x, PC: %x, TB_CODE: %x\n", dataList->data.tb, dataList->data.pc, dataList->data.tb_code);

        printf("------------------------\n");

        printDefUsePairs(dataList->data.def_use_list_head);
    }
}