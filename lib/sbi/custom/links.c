
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "links.h"

cJSON *ogs_sbi_links_convertToJSON(ogs_sbi_links_t *links)
{
    cJSON *item = NULL;
    OpenAPI_lnode_t *node;

    ogs_assert(links);

    item = cJSON_CreateObject();

    OpenAPI_list_for_each(links->items, node) {
        if (node->data) {
            ogs_fatal("node->data = %s", (char*)node->data);
        }
    }

    return item;
}
