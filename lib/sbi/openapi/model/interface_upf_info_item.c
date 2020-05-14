
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "interface_upf_info_item.h"

ogs_sbi_interface_upf_info_item_t *ogs_sbi_interface_upf_info_item_create(
    ogs_sbi_up_interface_type_t *interface_type,
    ogs_sbi_list_t *ipv4_endpoint_addresses,
    ogs_sbi_list_t *ipv6_endpoint_addresses,
    char *endpoint_fqdn,
    char *network_instance
    )
{
    ogs_sbi_interface_upf_info_item_t *interface_upf_info_item_local_var = ogs_sbi_malloc(sizeof(ogs_sbi_interface_upf_info_item_t));
    if (!interface_upf_info_item_local_var) {
        return NULL;
    }
    interface_upf_info_item_local_var->interface_type = interface_type;
    interface_upf_info_item_local_var->ipv4_endpoint_addresses = ipv4_endpoint_addresses;
    interface_upf_info_item_local_var->ipv6_endpoint_addresses = ipv6_endpoint_addresses;
    interface_upf_info_item_local_var->endpoint_fqdn = endpoint_fqdn;
    interface_upf_info_item_local_var->network_instance = network_instance;

    return interface_upf_info_item_local_var;
}

void ogs_sbi_interface_upf_info_item_free(ogs_sbi_interface_upf_info_item_t *interface_upf_info_item)
{
    if (NULL == interface_upf_info_item) {
        return;
    }
    ogs_sbi_lnode_t *node;
    ogs_sbi_up_interface_type_free(interface_upf_info_item->interface_type);
    ogs_sbi_list_for_each(interface_upf_info_item->ipv4_endpoint_addresses, node) {
        ogs_free(node->data);
    }
    ogs_sbi_list_free(interface_upf_info_item->ipv4_endpoint_addresses);
    ogs_sbi_list_for_each(interface_upf_info_item->ipv6_endpoint_addresses, node) {
        ogs_free(node->data);
    }
    ogs_sbi_list_free(interface_upf_info_item->ipv6_endpoint_addresses);
    ogs_free(interface_upf_info_item->endpoint_fqdn);
    ogs_free(interface_upf_info_item->network_instance);
    ogs_free(interface_upf_info_item);
}

cJSON *ogs_sbi_interface_upf_info_item_convertToJSON(ogs_sbi_interface_upf_info_item_t *interface_upf_info_item)
{
    cJSON *item = cJSON_CreateObject();
    if (!interface_upf_info_item->interface_type) {
        ogs_error("ogs_sbi_interface_upf_info_item_convertToJSON() failed [interface_type]");
        goto end;
    }
    cJSON *interface_type_local_JSON = ogs_sbi_up_interface_type_convertToJSON(interface_upf_info_item->interface_type);
    if (interface_type_local_JSON == NULL) {
        ogs_error("ogs_sbi_interface_upf_info_item_convertToJSON() failed [interface_type]");
        goto end;
    }
    cJSON_AddItemToObject(item, "interfaceType", interface_type_local_JSON);
    if (item->child == NULL) {
        ogs_error("ogs_sbi_interface_upf_info_item_convertToJSON() failed [interface_type]");
        goto end;
    }

    if (interface_upf_info_item->ipv4_endpoint_addresses) {
        cJSON *ipv4_endpoint_addresses = cJSON_AddArrayToObject(item, "ipv4EndpointAddresses");
        if (ipv4_endpoint_addresses == NULL) {
            ogs_error("ogs_sbi_interface_upf_info_item_convertToJSON() failed [ipv4_endpoint_addresses]");
            goto end;
        }

        ogs_sbi_lnode_t *ipv4_endpoint_addresses_node;
        ogs_sbi_list_for_each(interface_upf_info_item->ipv4_endpoint_addresses, ipv4_endpoint_addresses_node)  {
            if (cJSON_AddStringToObject(ipv4_endpoint_addresses, "", (char*)ipv4_endpoint_addresses_node->data) == NULL) {
                ogs_error("ogs_sbi_interface_upf_info_item_convertToJSON() failed [ipv4_endpoint_addresses]");
                goto end;
            }
        }
    }

    if (interface_upf_info_item->ipv6_endpoint_addresses) {
        cJSON *ipv6_endpoint_addresses = cJSON_AddArrayToObject(item, "ipv6EndpointAddresses");
        if (ipv6_endpoint_addresses == NULL) {
            ogs_error("ogs_sbi_interface_upf_info_item_convertToJSON() failed [ipv6_endpoint_addresses]");
            goto end;
        }

        ogs_sbi_lnode_t *ipv6_endpoint_addresses_node;
        ogs_sbi_list_for_each(interface_upf_info_item->ipv6_endpoint_addresses, ipv6_endpoint_addresses_node)  {
            if (cJSON_AddStringToObject(ipv6_endpoint_addresses, "", (char*)ipv6_endpoint_addresses_node->data) == NULL) {
                ogs_error("ogs_sbi_interface_upf_info_item_convertToJSON() failed [ipv6_endpoint_addresses]");
                goto end;
            }
        }
    }

    if (interface_upf_info_item->endpoint_fqdn) {
        if (cJSON_AddStringToObject(item, "endpointFqdn", interface_upf_info_item->endpoint_fqdn) == NULL) {
            ogs_error("ogs_sbi_interface_upf_info_item_convertToJSON() failed [endpoint_fqdn]");
            goto end;
        }
    }

    if (interface_upf_info_item->network_instance) {
        if (cJSON_AddStringToObject(item, "networkInstance", interface_upf_info_item->network_instance) == NULL) {
            ogs_error("ogs_sbi_interface_upf_info_item_convertToJSON() failed [network_instance]");
            goto end;
        }
    }

end:
    return item;
}

ogs_sbi_interface_upf_info_item_t *ogs_sbi_interface_upf_info_item_parseFromJSON(cJSON *interface_upf_info_itemJSON)
{
    ogs_sbi_interface_upf_info_item_t *interface_upf_info_item_local_var = NULL;
    cJSON *interface_type = cJSON_GetObjectItemCaseSensitive(interface_upf_info_itemJSON, "interfaceType");
    if (!interface_type) {
        ogs_error("ogs_sbi_interface_upf_info_item_parseFromJSON() failed [interface_type]");
        goto end;
    }

    ogs_sbi_up_interface_type_t *interface_type_local_nonprim = NULL;

    interface_type_local_nonprim = ogs_sbi_up_interface_type_parseFromJSON(interface_type);

    cJSON *ipv4_endpoint_addresses = cJSON_GetObjectItemCaseSensitive(interface_upf_info_itemJSON, "ipv4EndpointAddresses");

    ogs_sbi_list_t *ipv4_endpoint_addressesList;
    if (ipv4_endpoint_addresses) {
        cJSON *ipv4_endpoint_addresses_local;
        if (!cJSON_IsArray(ipv4_endpoint_addresses)) {
            ogs_error("ogs_sbi_interface_upf_info_item_parseFromJSON() failed [ipv4_endpoint_addresses]");
            goto end;
        }
        ipv4_endpoint_addressesList = ogs_sbi_list_create();

        cJSON_ArrayForEach(ipv4_endpoint_addresses_local, ipv4_endpoint_addresses) {
            if (!cJSON_IsString(ipv4_endpoint_addresses_local)) {
                ogs_error("ogs_sbi_interface_upf_info_item_parseFromJSON() failed [ipv4_endpoint_addresses]");
                goto end;
            }
            ogs_sbi_list_add(ipv4_endpoint_addressesList, ogs_strdup(ipv4_endpoint_addresses_local->valuestring));
        }
    }

    cJSON *ipv6_endpoint_addresses = cJSON_GetObjectItemCaseSensitive(interface_upf_info_itemJSON, "ipv6EndpointAddresses");

    ogs_sbi_list_t *ipv6_endpoint_addressesList;
    if (ipv6_endpoint_addresses) {
        cJSON *ipv6_endpoint_addresses_local;
        if (!cJSON_IsArray(ipv6_endpoint_addresses)) {
            ogs_error("ogs_sbi_interface_upf_info_item_parseFromJSON() failed [ipv6_endpoint_addresses]");
            goto end;
        }
        ipv6_endpoint_addressesList = ogs_sbi_list_create();

        cJSON_ArrayForEach(ipv6_endpoint_addresses_local, ipv6_endpoint_addresses) {
            if (!cJSON_IsString(ipv6_endpoint_addresses_local)) {
                ogs_error("ogs_sbi_interface_upf_info_item_parseFromJSON() failed [ipv6_endpoint_addresses]");
                goto end;
            }
            ogs_sbi_list_add(ipv6_endpoint_addressesList, ogs_strdup(ipv6_endpoint_addresses_local->valuestring));
        }
    }

    cJSON *endpoint_fqdn = cJSON_GetObjectItemCaseSensitive(interface_upf_info_itemJSON, "endpointFqdn");

    if (endpoint_fqdn) {
        if (!cJSON_IsString(endpoint_fqdn)) {
            ogs_error("ogs_sbi_interface_upf_info_item_parseFromJSON() failed [endpoint_fqdn]");
            goto end;
        }
    }

    cJSON *network_instance = cJSON_GetObjectItemCaseSensitive(interface_upf_info_itemJSON, "networkInstance");

    if (network_instance) {
        if (!cJSON_IsString(network_instance)) {
            ogs_error("ogs_sbi_interface_upf_info_item_parseFromJSON() failed [network_instance]");
            goto end;
        }
    }

    interface_upf_info_item_local_var = ogs_sbi_interface_upf_info_item_create (
        interface_type_local_nonprim,
        ipv4_endpoint_addresses ? ipv4_endpoint_addressesList : NULL,
        ipv6_endpoint_addresses ? ipv6_endpoint_addressesList : NULL,
        endpoint_fqdn ? ogs_strdup(endpoint_fqdn->valuestring) : NULL,
        network_instance ? ogs_strdup(network_instance->valuestring) : NULL
        );

    return interface_upf_info_item_local_var;
end:
    return NULL;
}

