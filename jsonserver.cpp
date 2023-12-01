#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <czmq.h>

#include <string>
#include <vector>

#include "json.hpp"

using json = nlohmann::json;

class NetPacket
{
public:
    std::string cmd_type = "None";
    std::string cam_id = "None";
    std::string command = "None";
    std::vector<std::string> params = {};

    NetPacket() {
        cmd_type = "None";
        cam_id = "None";
        command = "None";
        params = {};
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NetPacket, cmd_type, cam_id, command, params)
};

class address {
      public:
        std::string street;
        int housenumber;
        int postcode;

      public:
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(address, street, housenumber, postcode)
    };

namespace std
{
    std::string to_string(const NetPacket &packet) noexcept
    {
        std::string result = "{";
        result += "cmd_type: " + packet.cmd_type + ", ";
        result += "cam_id: " + packet.cam_id + ", ";
        result += "command: " + packet.command + ", ";
        result += "params: [";
        for (int i = 0; i < packet.params.size(); i++)
        {
            result += packet.params[i];
            if (i != packet.params.size() - 1)
                result += ", ";
        }
        result += "]}";
        return result;
    }

    std::string to_string(const address &a) noexcept
    {
        return std::to_string(a.housenumber) + " " + a.street + ", " + std::to_string(a.postcode);
    }
}

int main()
{
    zsock_t *pipe = zsock_new_rep("tcp://*:5555");
    assert(pipe);
    zpoller_t *poller = zpoller_new(pipe, NULL);
    assert(poller);
    uint32_t count = 0;
    while (!zsys_interrupted)
    {
        zsock_t *which = (zsock_t *)zpoller_wait(poller, 1000);
        printf("Loop %" PRIu32 ": ", ++count);
        if (which == NULL)
        {
            printf("No message\n");
            fflush(stdout);
            continue;
        }
        char *message = zstr_recv(which);
        if (message == NULL)
        {
            printf("Invalid message\n");
            fflush(stdout);
            continue;
        }
        printf("Message: %s\n", message);
        fflush(stdout);
        printf("\n\n");
        NetPacket packet = json::parse(message);
        zstr_free(&message);
        printf("%s\n", std::to_string(packet).c_str());
        printf("\n\n");
        fflush(stdout);
        packet.params.clear();
        packet.params.push_back("Hello");
        packet.params.push_back("World");
        json j = packet;
        zstr_send(which, j.dump().c_str());
    }
    zpoller_destroy(&poller);
    zsock_destroy(&pipe);
    return 0;
}