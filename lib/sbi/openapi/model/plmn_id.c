
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "plmn_id.h"

ogs_sbi_plmn_id_t *ogs_sbi_plmn_id_create(
    char *mcc,
    char *mnc
    )
{
    ogs_sbi_plmn_id_t *plmn_id_local_var = ogs_sbi_malloc(sizeof(ogs_sbi_plmn_id_t));
    if (!plmn_id_local_var) {
        return NULL;
    }
    plmn_id_local_var->mcc = mcc;
    plmn_id_local_var->mnc = mnc;

    return plmn_id_local_var;
}

void ogs_sbi_plmn_id_free(ogs_sbi_plmn_id_t *plmn_id)
{
    if (NULL == plmn_id) {
        return;
    }
    ogs_sbi_lnode_t *node;
    ogs_free(plmn_id->mcc);
    ogs_free(plmn_id->mnc);
    ogs_free(plmn_id);
}

cJSON *ogs_sbi_plmn_id_convertToJSON(ogs_sbi_plmn_id_t *plmn_id)
{
    cJSON *item = cJSON_CreateObject();
    if (!plmn_id->mcc) {
        ogs_error("ogs_sbi_plmn_id_convertToJSON() failed [mcc]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "mcc", plmn_id->mcc) == NULL) {
        ogs_error("ogs_sbi_plmn_id_convertToJSON() failed [mcc]");
        goto end;
    }

    if (!plmn_id->mnc) {
        ogs_error("ogs_sbi_plmn_id_convertToJSON() failed [mnc]");
        goto end;
    }
    if (cJSON_AddStringToObject(item, "mnc", plmn_id->mnc) == NULL) {
        ogs_error("ogs_sbi_plmn_id_convertToJSON() failed [mnc]");
        goto end;
    }

end:
    return item;
}

ogs_sbi_plmn_id_t *ogs_sbi_plmn_id_parseFromJSON(cJSON *plmn_idJSON)
{
    ogs_sbi_plmn_id_t *plmn_id_local_var = NULL;
    cJSON *mcc = cJSON_GetObjectItemCaseSensitive(plmn_idJSON, "mcc");
    if (!mcc) {
        ogs_error("ogs_sbi_plmn_id_parseFromJSON() failed [mcc]");
        goto end;
    }


    if (!cJSON_IsString(mcc)) {
        ogs_error("ogs_sbi_plmn_id_parseFromJSON() failed [mcc]");
        goto end;
    }

    cJSON *mnc = cJSON_GetObjectItemCaseSensitive(plmn_idJSON, "mnc");
    if (!mnc) {
        ogs_error("ogs_sbi_plmn_id_parseFromJSON() failed [mnc]");
        goto end;
    }


    if (!cJSON_IsString(mnc)) {
        ogs_error("ogs_sbi_plmn_id_parseFromJSON() failed [mnc]");
        goto end;
    }

    plmn_id_local_var = ogs_sbi_plmn_id_create (
        ogs_strdup(mcc->valuestring),
        ogs_strdup(mnc->valuestring)
        );

    return plmn_id_local_var;
end:
    return NULL;
}

