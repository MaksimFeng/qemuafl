#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cjson/cJSON.h"
#include "DefUseChainAccess.h" // Make sure this matches the header file name

static JsonData_list *jsonDataListHead = NULL;

// Forward declaration of functions used internally
static void addDefUsePairToList(JsonData_list *jsonDataNode, target_ulong def, target_ulong use);
static void freeDefUsePairList(DefUsePair_list* head);

target_ulong hexStrToTargetUlong(const char *hexStr) {
    return (target_ulong)strtoul(hexStr, NULL, 16);
}

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
        if (pc) newNode->data.pc = hexStrToTargetUlong(pc->valuestring);
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
                addDefUsePairToList(newNode, hexStrToTargetUlong(def->valuestring), hexStrToTargetUlong(use->valuestring));
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

static void addDefUsePairToList(JsonData_list *jsonDataNode, target_ulong def, target_ulong use) {
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
