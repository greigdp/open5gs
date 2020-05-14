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

#include "sbi-path.h"
#include "context.h"
#include "event.h"

static int server_cb(ogs_sbi_session_t *session, ogs_sbi_request_t *request)
{
    nrf_event_t *e = NULL;
    int rv;

    ogs_assert(session);
    ogs_assert(request);

    e = nrf_event_new(NRF_EVT_SBI_SERVER);
    ogs_assert(e);

    e->sbi.session = session;
    e->sbi.request = request;

    rv = ogs_queue_push(nrf_self()->queue, e);
    if (rv != OGS_OK) {
        ogs_error("ogs_queue_push() failed:%d", (int)rv);
        nrf_event_free(e);
        return OGS_ERROR;
    }

    return OGS_OK;
}

int nrf_sbi_open(void)
{
    ogs_sbi_server_start_all(server_cb);

    return OGS_OK;
}

void nrf_sbi_close(void)
{
    ogs_sbi_server_stop_all();
}
