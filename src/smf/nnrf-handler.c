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

#include "nnrf-handler.h"

void smf_nnrf_handle_nf_register(
        ogs_sbi_nf_instance_t *nf_instance, ogs_sbi_message_t *message)
{
    OpenAPI_nf_profile_t *NFProfile = NULL;
    ogs_sbi_client_t *client = NULL;

    ogs_assert(message);
    ogs_assert(nf_instance);
    client = nf_instance->client;
    ogs_assert(client);

    NFProfile = message->NFProfile;
    if (!NFProfile) {
        ogs_error("No NFProfile");
        return;
    }

    /* Update from NRF */
    nf_instance->time.heartbeat = NFProfile->heart_beat_timer;
}

void smf_nnrf_handle_nf_status_subscribe(
        ogs_sbi_subscription_t *subscription, ogs_sbi_message_t *message)
{
    OpenAPI_subscription_data_t *SubscriptionData = NULL;
    ogs_sbi_client_t *client = NULL;

    ogs_assert(message);
    ogs_assert(subscription);
    client = subscription->client;
    ogs_assert(client);

    SubscriptionData = message->SubscriptionData;
    if (!SubscriptionData) {
        ogs_error("No SubscriptionData");
        return;
    }

    if (!SubscriptionData->subscription_id) {
        ogs_error("No SubscriptionId");
        return;
    }

    ogs_sbi_subscription_set_id(
        subscription, SubscriptionData->subscription_id);

    if (SubscriptionData->validity_time) {
        struct timeval tv;
        struct tm local, next;
        ogs_time_t diff, duration;
        if (ogs_strptime(SubscriptionData->validity_time,
                OGS_TIME_ISO8601_FORMAT, &next)) {
            ogs_gettimeofday(&tv);
            ogs_localtime(tv.tv_sec, &local);
            diff = ogs_mktime(&next) - ogs_mktime(&local);
#define VALIDITY_MARGIN 5 /* 5 seconds */
#define VALIDITY_MINIMUM 5 /* 5 seconds */
            duration = diff - VALIDITY_MARGIN;

            if (duration < VALIDITY_MINIMUM) {
                ogs_warn("Validation period [%ld seconds, %s] is too small",
                        diff, SubscriptionData->validity_time);
                duration = VALIDITY_MINIMUM;
                ogs_warn("Forced to %d seconds", VALIDITY_MINIMUM);
            }

            subscription->t_validity = ogs_timer_add(smf_self()->timer_mgr,
                smf_timer_sbi_no_validity, subscription);
            ogs_assert(subscription->t_validity);
            ogs_timer_start(
                    subscription->t_validity, ogs_time_from_sec(duration));
        }
    }
}

bool smf_nnrf_handle_nf_status_notify(ogs_sbi_server_t *server,
        ogs_sbi_session_t *session, ogs_sbi_message_t *message)
{
    ogs_sbi_response_t *response = NULL;
    OpenAPI_notification_data_t *NotificationData = NULL;
    OpenAPI_nf_profile_t *NFProfile = NULL;
    ogs_sbi_nf_instance_t *nf_instance = NULL;
    bool handled;

    ogs_assert(session);
    ogs_assert(message);

    NotificationData = message->NotificationData;
    if (!NotificationData) {
        ogs_error("No NotificationData");
        ogs_sbi_server_send_error(session, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No NotificationData", NULL);
        return false;
    }

    NFProfile = NotificationData->nf_profile;
    if (!NFProfile) {
        ogs_error("No NFProfile");
        ogs_sbi_server_send_error(session, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "No NFProfile", NULL);
        return false;
    }

    if (NotificationData->event ==
            OpenAPI_notification_event_type_NF_REGISTERED) {
        ogs_sbi_client_t *client = NULL;

        nf_instance = ogs_sbi_nf_instance_find(NFProfile->nf_instance_id);
        if (!nf_instance) {
            nf_instance = ogs_sbi_nf_instance_add(NFProfile->nf_instance_id);
            ogs_assert(nf_instance);

            ogs_info("(NRF-notify) NF registered [%s]", nf_instance->id);
        } else
            ogs_warn("(NRF-notifY) NFInstance [%s] has already been added",
                    NFProfile->nf_instance_id);

            handled = ogs_sbi_nnrf_handle_nf_profile(
                        nf_instance, NFProfile, session, message);
            if (!handled) return false;

        if (ogs_sbi_nf_instance_is_self(nf_instance->id) == false) {
            client = ogs_sbi_nf_instance_find_client(nf_instance);
            if (!client) {
                ogs_error("Cannot find client");
                ogs_sbi_server_send_error(session,
                        OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                        message, "Cannot find client", NULL);
                return false;
            }

            OGS_SETUP_SBI_CLIENT(nf_instance, client);

            ogs_info("(NRF-notify) NF Profile updated [%s]", nf_instance->id);
        }


    } else if (NotificationData->event ==
            OpenAPI_notification_event_type_NF_DEREGISTERED) {
        nf_instance = ogs_sbi_nf_instance_find(NFProfile->nf_instance_id);
        if (nf_instance) {
            ogs_info("(NRF-notify) NF de-registered [%s]", nf_instance->id);
            ogs_sbi_nf_instance_remove(nf_instance);

            /* FIXME : Remove unnecessary Client */
        } else
            ogs_warn("(NRF-notify) NFInstance [%s] has already been removed",
                    NFProfile->nf_instance_id);
    } else {
        char *eventstr = OpenAPI_notification_event_type_ToString(
                            NotificationData->event);
        ogs_error("Not supported event [%d:%s]",
                NotificationData->event, eventstr ? eventstr : "Unknown");
        ogs_sbi_server_send_error(session, OGS_SBI_HTTP_STATUS_BAD_REQUEST,
                message, "Not supported event",
                eventstr ? eventstr : "Unknown");
        return false;
    }

    response = ogs_sbi_build_response(message);
    ogs_assert(response);
    ogs_sbi_server_send_response(session, response,
            OGS_SBI_HTTP_STATUS_NO_CONTENT);

    return true;
}

void smf_nnrf_handle_nf_discover(ogs_sbi_message_t *message)
{
    OpenAPI_search_result_t *SearchResult = NULL;
    OpenAPI_lnode_t *node = NULL;
    bool handled;

    ogs_assert(message);

    SearchResult = message->SearchResult;
    if (!SearchResult) {
        ogs_error("No SearchResult");
        return;
    }

    ogs_fatal("validity = %d", SearchResult->validity_period);

    OpenAPI_list_for_each(SearchResult->nf_instances, node) {
        OpenAPI_nf_profile_t *NFProfile = NULL;
        ogs_sbi_nf_instance_t *nf_instance = NULL;
        ogs_sbi_client_t *client = NULL;

        if (!node->data) continue;

        NFProfile = node->data;

        nf_instance = ogs_sbi_nf_instance_find(NFProfile->nf_instance_id);
        if (!nf_instance) {
            nf_instance = ogs_sbi_nf_instance_add(NFProfile->nf_instance_id);
            ogs_assert(nf_instance);

            ogs_info("(NF-Discover) NF registered [%s]", nf_instance->id);
        } else
            ogs_warn("(NF-Discover) NFInstance [%s] has already been added",
                    NFProfile->nf_instance_id);

        if (ogs_sbi_nf_instance_is_self(nf_instance->id) == true) {
            handled = ogs_sbi_nnrf_handle_nf_profile(
                        nf_instance, NFProfile, NULL, NULL);
            if (!handled) return;

            client = ogs_sbi_nf_instance_find_client(nf_instance);
            if (!client) {
                ogs_error("Cannot find client");
                return;
            }

            OGS_SETUP_SBI_CLIENT(nf_instance, client);

            ogs_info("(NF-Discover) NF Profile updated [%s]", nf_instance->id);
        }
    }
}
