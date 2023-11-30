#pragma once
#include <zmq.h>
#include <czmq.h>
#include <stdbool.h>

static void
s_renew_sockets (zsock_t **server, zsock_t **client)
{
    zsock_destroy (client);
    zsock_destroy (server);
    *server = zsock_new (ZMQ_PULL);
    assert (*server);
    *client = zsock_new (ZMQ_PUSH);
    assert (*client);
}

static bool
s_can_connect (zsock_t **server, zsock_t **client)
{
    int port_nbr = zsock_bind (*server, "tcp://127.0.0.1:*");
    assert (port_nbr > 0);
    int rc = zsock_connect (*client, "tcp://127.0.0.1:%d", port_nbr);
    assert (rc == 0);
    //  Give the connection time to fail if that's the plan
    if (zsock_mechanism (*client) == ZMQ_CURVE)
        zclock_sleep (1500);
    else
        zclock_sleep (200);

    //  By default PUSH sockets block if there's no peer
    zsock_set_sndtimeo (*client, 200);
    zstr_send (*client, "Hello, World");

    zpoller_t *poller = zpoller_new (*server, NULL);
    assert (poller);
    bool success = (zpoller_wait (poller, 400) == *server);
    zpoller_destroy (&poller);

    // if (renew == true)
    //     s_renew_sockets (server, client);

    return success;
}