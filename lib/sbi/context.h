/*
 * Copyright (C) 2019 by Sukchan Lee <acetcom@gmail.com>
 *
 * This file is part of Open5GS.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#if !defined(OGS_SBI_INSIDE) && !defined(OGS_SBI_COMPILATION)
#error "This header cannot be included directly."
#endif

#ifndef OGS_SBI_CONTEXT_H
#define OGS_SBI_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ogs_sbi_client_s ogs_sbi_client_t;
typedef struct ogs_sbi_context_s {
    ogs_pollset_t       *pollset;       /* Poll Set for I/O Multiplexing */
    ogs_timer_mgr_t     *timer_mgr;     /* Timer Manager */

    uint32_t            http_port;      /* SBI HTTP local port */
    uint32_t            https_port;     /* SBI HTTPS local port */

#define OGS_SBI_HEARTBEAT_RETRYCOUNT 4
    int                 heart_beat_timer;

    ogs_list_t          server_list;
    ogs_list_t          client_list;

    ogs_uuid_t          uuid;
    char                nf_instance_id[OGS_UUID_FORMATTED_LENGTH + 1];

    const char          *content_encoding;
} ogs_sbi_context_t;

void ogs_sbi_context_init(ogs_pollset_t *pollset, ogs_timer_mgr_t *timer_mgr);
void ogs_sbi_context_final(void);
ogs_sbi_context_t *ogs_sbi_self(void);
int ogs_sbi_context_parse_config(const char *local, const char *remote);

#ifdef __cplusplus
}
#endif

#endif /* OGS_SBI_CONTEXT_H */
