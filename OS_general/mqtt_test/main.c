/*******************************************************************************
 * Copyright (c) 2012, 2020 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   https://www.eclipse.org/legal/epl-2.0/
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MQTTClient.h"

#define ADDRESS "150.95.104.87"
#define CLIENTID "dungnt98"
#define USER "admin"
#define PASSWORD "admin"

#define TOPIC "sub"
#define PAYLOAD "Hello World!"
#define QOS 1
#define TIMEOUT 10000L

static MQTTClient g_sClient;
static MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

void on_connection_lost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);

    int rc = MQTTClient_connect(g_sClient, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
    }
}

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    char *payload = message->payload;
    printf("Received operation %s\n", payload);
    publish(g_sClient, "s/us", "hello");
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void publish(MQTTClient client, char *topic, char *payload)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 1;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message '%s' with delivery token %d delivered\n", payload, token);
}

int mqtt_subscribe(char *topic, int qos)
{
    int rc = rc = MQTTClient_subscribe(g_sClient, topic, qos);
    if (rc != MQTTCLIENT_SUCCESS)
        printf("Failed to subscribe, return code %d\n", rc);

    return rc;
}

int mqtt_connect(char *url, char *client_id, char *user, char *password)
{
    int rc = 0;

    conn_opts.username = user;
    conn_opts.password = password;

    rc = MQTTClient_create(&g_sClient, url, client_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf("Client create fail, return code %d\n", rc);
        goto create_fail;
    }

    rc = MQTTClient_setCallbacks(g_sClient, NULL, NULL, on_message, NULL);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf("Mqtt set callback fail, return code %d\n", rc);
        goto set_callback_fail;
    }

    rc = MQTTClient_connect(g_sClient, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        goto connect_fail;
    }

    return rc;

connect_fail:
set_callback_fail:
    MQTTClient_destroy(&g_sClient);
create_fail:
    return rc;
}

int main(int argc, char *argv[])
{
    int ret = 0;

    ret = mqtt_connect(ADDRESS, CLIENTID, USER, PASSWORD);
    if (ret != MQTTCLIENT_SUCCESS)
        return ret;

    ret = mqtt_subscribe("hello_topic", 0);
    if (ret != MQTTCLIENT_SUCCESS)
        goto subscribe_fail;

    publish(g_sClient, "hello_topic", "<this is a payload>");

    for (;;)
    {
    }

subscribe_fail:
    MQTTClient_disconnect(g_sClient, 1000);
    MQTTClient_destroy(&g_sClient);
}