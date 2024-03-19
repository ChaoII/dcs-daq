import asyncio

import logging
from asyncua import ua, Server

_logger = logging.getLogger(__name__)


def get_tag_table(filepath: str) -> list:
    ret = []
    with open(filepath, "r", encoding="utf8") as f:
        for line in f.readlines():
            data = line.strip().split(",")
            print(data)
            if len(data) != 3:
                _logger.error("label file format error")
                continue
            ret.append(data)
    return ret


def type_str_to_ua_val_type(type_str: str) -> ua.VariantType:
    if type_str.lower() == "float":
        return ua.VariantType.Float
    elif type_str.lower() == "double":
        return ua.VariantType.Double
    elif type_str.lower() == "bool":
        return ua.VariantType.Boolean
    elif type_str.lower() == "int32":
        return ua.VariantType.Int32
    elif type_str.lower() == "int64":
        return ua.VariantType.Int64
    elif type_str.lower() == "byte":
        return ua.VariantType.Byte
    elif type_str.lower() == "datetime":
        return ua.VariantType.DateTime
    elif type_str.lower() == "bytestring":
        return ua.VariantType.ByteString
    elif type_str.lower() == "string":
        return ua.VariantType.String
    elif type_str.lower() == "guid":
        return ua.VariantType.Guid
    return ua.VariantType.Int32


async def main():
    server = Server()
    await server.init()
    server.set_endpoint("opc.tcp://0.0.0.0:4840/freeopcua/server/")
    server.set_server_name("FreeOpcUa Example Server")
    # set all possible endpoint policies for clients to connect through
    # server.set_security_policy(
    #     [
    #         ua.SecurityPolicyType.NoSecurity,
    #         ua.SecurityPolicyType.Basic256Sha256_SignAndEncrypt,
    #         ua.SecurityPolicyType.Basic256Sha256_Sign,
    #     ]
    # )
    # setup our own namespace
    uri = "DCSDAQ"
    idx = await server.register_namespace(uri)
    # create directly some objects and variables
    obj = await server.nodes.objects.add_object(idx, "DCSDAQ")
    for node_info in get_tag_table("label.csv"):
        var = await obj.add_variable(ua.NodeId.from_string(node_info[0]), node_info[1], val=0.0,
                                     varianttype=type_str_to_ua_val_type(node_info[2]))
        await var.set_writable()  # Set MyVariable to be writable by clients
    # starting!
    async with server:
        print("Available loggers are: ", logging.Logger.manager.loggerDict.keys())

        while True:
            await asyncio.sleep(0.1)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    asyncio.run(main())
