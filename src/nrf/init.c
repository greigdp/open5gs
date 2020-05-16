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

#include "context.h"

static ogs_thread_t *thread;
static void nrf_main(void *data);
static int initialized = 0;

int nrf_initialize()
{
    int rv;

    nrf_context_init();
    nrf_event_init(); /* Create event with poll, timer */
    ogs_sbi_context_init(nrf_self()->pollset, nrf_self()->timer_mgr); 

    rv = ogs_sbi_context_parse_config("nrf", NULL);
    if (rv != OGS_OK) return rv;

    rv = nrf_context_parse_config();
    if (rv != OGS_OK) return rv;

    rv = ogs_log_config_domain(
            ogs_config()->logger.domain, ogs_config()->logger.level);
    if (rv != OGS_OK) return rv;

    thread = ogs_thread_create(nrf_main, NULL);
    if (!thread) return OGS_ERROR;

    initialized = 1;

    return OGS_OK;
}

void nrf_terminate(void)
{
    if (!initialized) return;

    nrf_event_term(); /* Termniate event */

    ogs_thread_destroy(thread);

    nrf_context_final();
    ogs_sbi_context_final();

    nrf_event_final(); /* Destroy event */
}

static void nrf_main(void *data)
{
    ogs_fsm_t nrf_sm;
    nrf_event_t e;
    int rv;

    ogs_fsm_create(&nrf_sm, nrf_state_initial, nrf_state_final);
    ogs_fsm_init(&nrf_sm, &e);

    for ( ;; ) {
        ogs_pollset_poll(nrf_self()->pollset,
                ogs_timer_mgr_next(nrf_self()->timer_mgr));

        /* Process the MESSAGE FIRST.
         *
         * For example, if UE Context Release Complete is received,
         * the MME_TIMER_UE_CONTEXT_RELEASE is first stopped */
        for ( ;; ) {
            nrf_event_t *e = NULL;

            rv = ogs_queue_trypop(nrf_self()->queue, (void**)&e);
            ogs_assert(rv != OGS_ERROR);

            if (rv == OGS_DONE)
                goto done;

            if (rv == OGS_RETRY)
                break;

            ogs_assert(e);
            ogs_fsm_dispatch(&nrf_sm, e);
            nrf_event_free(e);
        }

        ogs_timer_mgr_expire(nrf_self()->timer_mgr);

        /* AND THEN, process the TIMER. */
        for ( ;; ) {
            nrf_event_t *e = NULL;

            rv = ogs_queue_trypop(nrf_self()->queue, (void**)&e);
            ogs_assert(rv != OGS_ERROR);

            if (rv == OGS_DONE)
                goto done;

            if (rv == OGS_RETRY)
                break;

            ogs_assert(e);
            ogs_fsm_dispatch(&nrf_sm, e);
            nrf_event_free(e);
        }
    }
done:

    ogs_fsm_fini(&nrf_sm, 0);
    ogs_fsm_delete(&nrf_sm);
}
