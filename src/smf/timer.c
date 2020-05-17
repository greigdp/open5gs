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

#include "timer.h"
#include "event.h"
#include "context.h"

static smf_timer_cfg_t g_smf_timer_cfg[MAX_NUM_OF_SMF_TIMER] = {
    [SMF_TIMER_PFCP_ASSOCIATION] =
        { .duration = ogs_time_from_sec(12) },
    [SMF_TIMER_PFCP_HEARTBEAT] =
        { .duration = ogs_time_from_sec(12) },
    [SMF_TIMER_SBI_REGISTRATION] =
        { .duration = ogs_time_from_sec(3) },
};

smf_timer_cfg_t *smf_timer_cfg(smf_timer_e id)
{
    ogs_assert(id < MAX_NUM_OF_SMF_TIMER);
    return &g_smf_timer_cfg[id];
}

const char *smf_timer_get_name(smf_timer_e id)
{
    switch (id) {
    case SMF_TIMER_PFCP_ASSOCIATION:
        return "SMF_TIMER_PFCP_ASSOCIATION";
    case SMF_TIMER_PFCP_HEARTBEAT:
        return "SMF_TIMER_PFCP_HEARTBEAT";
    case SMF_TIMER_SBI_REGISTRATION:
        return "SMF_TIMER_SBI_REGISTRATION";
    case SMF_TIMER_SBI_HEARTBEAT:
        return "SMF_TIMER_SBI_HEARTBEAT";
    case SMF_TIMER_SBI_NO_HEARTBEAT:
        return "SMF_TIMER_SBI_NO_HEARTBEAT";
    case SMF_TIMER_SBI_VALIDITY:
        return "SMF_TIMER_SBI_VALIDITY";
    case SMF_TIMER_SBI_NO_VALIDITY:
        return "SMF_TIMER_SBI_NO_VALIDITY";
    default: 
       break;
    }

    return "UNKNOWN_TIMER";
}

static void timer_send_event(int timer_id, void *data)
{
    int rv;
    smf_event_t *e = NULL;
    ogs_assert(data);

    switch (timer_id) {
    case SMF_TIMER_PFCP_ASSOCIATION:
    case SMF_TIMER_PFCP_HEARTBEAT:
        e = smf_event_new(SMF_EVT_N4_TIMER);
        e->timer_id = timer_id;
        e->pfcp_node = data;
        break;
    case SMF_TIMER_SBI_REGISTRATION:
    case SMF_TIMER_SBI_HEARTBEAT:
    case SMF_TIMER_SBI_NO_HEARTBEAT:
    case SMF_TIMER_SBI_VALIDITY:
    case SMF_TIMER_SBI_NO_VALIDITY:
        e = smf_event_new(SMF_EVT_SBI_TIMER);
        e->timer_id = timer_id;
        e->sbi.data = data;
        break;
    default:
        ogs_fatal("Unknown timer id[%d]", timer_id);
        ogs_assert_if_reached();
        break;
    }

    rv = ogs_queue_push(smf_self()->queue, e);
    if (rv != OGS_OK) {
        ogs_warn("ogs_queue_push() failed:%d", (int)rv);
        smf_event_free(e);
    }
}

void smf_timer_pfcp_association(void *data)
{
    timer_send_event(SMF_TIMER_PFCP_ASSOCIATION, data);
}

void smf_timer_pfcp_heartbeat(void *data)
{
    timer_send_event(SMF_TIMER_PFCP_HEARTBEAT, data);
}

void smf_timer_sbi_registration(void *data)
{
    timer_send_event(SMF_TIMER_SBI_REGISTRATION, data);
}

void smf_timer_sbi_heartbeat(void *data)
{
    timer_send_event(SMF_TIMER_SBI_HEARTBEAT, data);
}

void smf_timer_sbi_no_heartbeat(void *data)
{
    timer_send_event(SMF_TIMER_SBI_NO_HEARTBEAT, data);
}

void smf_timer_sbi_validity(void *data)
{
    timer_send_event(SMF_TIMER_SBI_VALIDITY, data);
}

void smf_timer_sbi_no_validity(void *data)
{
    timer_send_event(SMF_TIMER_SBI_NO_VALIDITY, data);
}
