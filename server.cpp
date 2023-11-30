#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main()
{
    NetPacket packet;
    packet.cmd_type = "cmd";
    packet.cam_id = "cam1";
    packet.command = "set";
    packet.params = {};

    json j = packet;
    printf("json_str: %s\n", j.dump().c_str());

    NetPacket packet2 = j;
    printf("cmd_type: %s\n", packet2.cmd_type.c_str());
    printf("cam_id: %s\n", packet2.cam_id.c_str());
    printf("command: %s\n", packet2.command.c_str());
    printf("params: ");
    for (auto &param : packet2.params) {
        printf("%s ", param.c_str());
    }
    printf("\n");

    return 0;
}