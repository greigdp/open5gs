#ifndef OGS_SBI_LIST_H
#define OGS_SBI_LIST_H

#include "../external/cJSON.h"
#include "ogs-core.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ogs_sbi_malloc(__sIZE) __sIZE == 0 ? NULL : ogs_malloc(__sIZE)

typedef struct ogs_sbi_list_s ogs_sbi_list_t;

typedef struct ogs_sbi_lnode_s ogs_sbi_lnode_t;

struct ogs_sbi_lnode_s {
    ogs_sbi_lnode_t *next;
    ogs_sbi_lnode_t *prev;
    void *data;
};

typedef struct ogs_sbi_list_s {
    ogs_sbi_lnode_t *first;
    ogs_sbi_lnode_t *last;

    long count;
} ogs_sbi_list_t;

#define ogs_sbi_list_for_each(list, element) for(element = (list != NULL) ? (list)->first : NULL; element != NULL; element = element->next)

ogs_sbi_list_t *ogs_sbi_list_create(void);
void ogs_sbi_list_free(ogs_sbi_list_t *listToFree);

void ogs_sbi_list_add(ogs_sbi_list_t *list, void *dataToAddInList);
ogs_sbi_lnode_t *ogs_sbi_list_find(ogs_sbi_list_t *list, long indexOfElement);
void ogs_sbi_list_remove(
    ogs_sbi_list_t *list, ogs_sbi_lnode_t *elementToRemove);

void ogs_sbi_list_iterate_forward(ogs_sbi_list_t *list,
                                  void (*operationToPerform)(ogs_sbi_lnode_t*, void*),
                                  void *additionalDataNeededForCallbackFunction);
void ogs_sbi_list_iterate_backward(ogs_sbi_list_t *list,
                                   void (*operationToPerform)(ogs_sbi_lnode_t*, void*),
                                   void *additionalDataNeededForCallbackFunction);

void ogs_sbi_lnode_print(ogs_sbi_lnode_t *listEntry, void *additionalData);
void ogs_sbi_lnode_free(ogs_sbi_lnode_t *listEntry, void *additionalData);

#ifdef __cplusplus
}
#endif

#endif // OGS_SBI_LIST_H
