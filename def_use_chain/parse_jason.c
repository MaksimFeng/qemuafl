#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cjson/cJSON.h"
#include "uthash.h"
#include "inttypes.h"
#include "DefUseChainAccess.h" // Make sure this matches the header file name

static JsonData_list *jsonDataListHead = NULL;
//using use to trace the def_use_chain, use hash map to store the def_use_chain according to the pc

// Forward declaration of functions used internally
static void addDefUsePairToList(JsonData_list *jsonDataNode, uintptr_t def, uintptr_t use);
static void freeDefUsePairList(DefUsePair_list* head);

target_ulong hexStrToTargetUlong(const char *hexStr) {
    return (target_ulong)strtoul(hexStr, NULL, 16);
}
uintptr_t hexStrTouintptr_t(const char *hexStr) {
    return (uintptr_t)strtoul(hexStr, NULL, 16);
}
// void addDefUsePairToPcMap(PcDefUseMapEntry **map, target_ulong pc, target_ulong def, target_ulong use) {
//     PcDefUseMapEntry *s;
//     HASH_FIND_INT(*map, &pc, s); // Try to find the PC entry
//     if (!s) {
//         s = (PcDefUseMapEntry *)malloc(sizeof(PcDefUseMapEntry));
//         s->pc = pc;
//         s->pairs = NULL; // Initially no pairs
//         HASH_ADD_INT(*map, pc, s);
//     }
//     // Add new pair to the list for this PC
//     DefUsePair *newPair = (DefUsePair *)malloc(sizeof(DefUsePair));
//     newPair->def = def;
//     newPair->use = use;
//     newPair->next = s->pairs; // Insert at head
//     s->pairs = newPair;
// }


void parse_json(const char *json_string) {
    cJSON *json = cJSON_Parse(json_string);
    if (json == NULL) {
        fprintf(stderr, "Error in parsing JSON\n");
        return;
    }

    cJSON *entry = NULL;
    cJSON_ArrayForEach(entry, json) {
        JsonData_list* newNode = (JsonData_list*)malloc(sizeof(JsonData_list));
        if (!newNode) return;

        memset(&newNode->data, 0, sizeof(JsonData)); // Initialize JsonData to zeros
        newNode->next = NULL;

        cJSON *tb = cJSON_GetObjectItemCaseSensitive(entry, "tb");
        cJSON *pc = cJSON_GetObjectItemCaseSensitive(entry, "pc");
        cJSON *tb_code = cJSON_GetObjectItemCaseSensitive(entry, "tb_code");
        cJSON *size = cJSON_GetObjectItemCaseSensitive(entry, "size");
        cJSON *num_def = cJSON_GetObjectItemCaseSensitive(entry, "num_def");
        cJSON *num_use = cJSON_GetObjectItemCaseSensitive(entry, "num_use");

        if (tb) newNode->data.tb = hexStrToTargetUlong(tb->valuestring);
        if (pc) newNode->data.pc = hexStrTouintptr_t(pc->valuestring);
        if (tb_code) newNode->data.tb_code = hexStrToTargetUlong(tb_code->valuestring);
        if (size) newNode->data.size = (size_t)strtoul(size->valuestring, NULL, 16);
        newNode->data.num_def = num_def ? num_def->valueint : 0;
        newNode->data.num_use = num_use ? num_use->valueint : 0;

        // Process the def_use_chain
        cJSON *def_use_chain = cJSON_GetObjectItemCaseSensitive(entry, "def_use_chain");
        cJSON *pair;
        cJSON_ArrayForEach(pair, def_use_chain) {
            cJSON *def = cJSON_GetArrayItem(pair, 0);
            cJSON *use = cJSON_GetArrayItem(pair, 1);
            if (def && use) {
                addDefUsePairToList(newNode, hexStrTouintptr_t(def->valuestring), hexStrTouintptr_t(use->valuestring));
            }
        }

        // Append newNode to the list
        if (jsonDataListHead == NULL) {
            jsonDataListHead = newNode;
        } else {
            JsonData_list *current = jsonDataListHead;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newNode;
        }
    }

    cJSON_Delete(json);
}

void read_json_from_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Unable to open the file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char *)malloc(length + 1);
    if (!data) {
        fclose(file);
        return;
    }

    fread(data, 1, length, file);
    data[length] = '\0';

    parse_json(data);

    free(data);
    fclose(file);
}

JsonData_list* get_json_data_list(void) {
    return jsonDataListHead;
}

void free_json_data_list(void) {
    while (jsonDataListHead != NULL) {
        JsonData_list* temp = jsonDataListHead;
        jsonDataListHead = jsonDataListHead->next;
        freeDefUsePairList(temp->data.def_use_list_head); // Free the DefUsePair list
        free(temp); // Then free the JsonData_list node itself
    }
}

static void addDefUsePairToList(JsonData_list *jsonDataNode, uintptr_t def, uintptr_t use) {
    DefUsePair_list* newNode = (DefUsePair_list*)malloc(sizeof(DefUsePair_list));
    if (!newNode) return;
    
    newNode->def_use_chain = (DefUsePair*)malloc(sizeof(DefUsePair));
    if (!newNode->def_use_chain) {
        free(newNode);
        return;
    }

    newNode->def_use_chain->def = def;
    newNode->def_use_chain->use = use;
    newNode->next = jsonDataNode->data.def_use_list_head;
    jsonDataNode->data.def_use_list_head = newNode;
    jsonDataNode->data.def_use_count++;
}




static void freeDefUsePairList(DefUsePair_list* head) {
    while (head != NULL) {
        DefUsePair_list* temp = head;
        head = head->next;
        free(temp->def_use_chain);
        free(temp);
    }
}



// manipulate the hash table
// void initDefUseMap() {
//     pcDefUseMap = NULL; // Ensure it's empty
//     HASH_INIT(pcDefUseMap, UTHASH_PC, 0, 0, 0); // Initialize uthash
// }


// Global hash map variable
PcDefUseMapEntry *pcDefUseMap = NULL;

// Adds a def-use pair to the hash map for a given PC
void addDefUsePairToMap(uintptr_t pc, DefUsePair *pair) {
    PcDefUseMapEntry *s;

    // Check if the entry for this PC already exists
    HASH_FIND(hh, pcDefUseMap, &pc, sizeof(pc), s);
    if (s == NULL) {
        s = (PcDefUseMapEntry *)malloc(sizeof(PcDefUseMapEntry));
        s->pc = pc;
        printf("s->pc: %"PRIxPTR"\n", s->pc);
        s->pairs = NULL;
        s->num_pairs = 0;
        HASH_ADD(hh, pcDefUseMap, pc, sizeof(pc), s);
    }

    // Add the new pair to the front of the list
    pair->next = s->pairs;
    s->pairs = pair;
    s->num_pairs++;
}

// Prints all def-use pairs for a given PC
void printDefUsePairsForPC(uintptr_t pc) {
    PcDefUseMapEntry *s;

    HASH_FIND(hh, pcDefUseMap, &pc, sizeof(pc), s);
    if (s != NULL) {
        printf("PC: 0x%"PRIxPTR" has %d def-use pairs:\n", s->pc, s->num_pairs);
        DefUsePair *pair = s->pairs;
        while (pair != NULL) {
            printf("  Def: 0x%"PRIxPTR", Use: 0x%"PRIxPTR"\n", pair->def, pair->use);
            // return pair;
            pair = pair->next;
        }
    } else {
        printf("No def-use pairs found for PC: 0x%"PRIxPTR"\n", pc);
    }
    // return pair;
}
DefUsePair* getDefUsePairForPC(uintptr_t pc){
    //return defs and uses according to pc
    PcDefUseMapEntry *s;

    HASH_FIND(hh, pcDefUseMap, &pc, sizeof(pc), s);
    if (s != NULL) {
        DefUsePair *pair = s->pairs;
        return pair;
    } else {
        printf("No def-use pairs found for PC: 0x%"PRIxPTR"\n", pc);
        return NULL;
    }    
}

// Frees the hash map
void freeDefUseMap() {
    PcDefUseMapEntry *current, *tmp;

    HASH_ITER(hh, pcDefUseMap, current, tmp) {
        // Free the list of def-use pairs
        DefUsePair *pair = current->pairs;
        while (pair != NULL) {
            DefUsePair *next = pair->next;
            free(pair);
            pair = next;
        }
        // Delete the hash map entry
        HASH_DEL(pcDefUseMap, current);
        free(current);
    }
}
