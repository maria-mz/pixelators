#include <thread>
#include <chrono>
#include <sstream>
#include <map>

#include "../Networking/NetClient.h"
#include "../Networking/NetServer.h"
#include "../Networking/NetMessages.h"


#define SOFT_ASSERT_EQ(lhs, rhs)                                                    \
    do {                                                                            \
        auto _lhs = (lhs);                                                          \
        auto _rhs = (rhs);                                                          \
        if (_lhs != _rhs) {                                                         \
            std::stringstream ss;                                                   \
            ss << "Assert error " #lhs " == " #rhs " Expected: " << _rhs << "\n"    \
               << "  Actual: " << _lhs << "\n";                                     \
            throw std::runtime_error(ss.str());                                     \
        }                                                                           \
    } while (0)


#define RUN_TEST(fn, timeout)                                                                \
    do {                                                                                     \
        printf("### %s ###\n", #fn);                                                         \
        auto future = std::async(std::launch::async, fn);                                    \
        if (future.wait_for(std::chrono::seconds(timeout)) == std::future_status::ready) {   \
            bool result = future.get();                                                      \
            g_testResults[#fn] = result;                                                     \
        } else {                                                                             \
            printf("Timeout error: %s took too long\n", #fn);                                \
            std::exit(1);                                                                    \
        }                                                                                    \
    } while (0)


static std::map<std::string, bool> g_testResults;


bool testChannel()
{
    bool success = true;

    try
    {
        Channel<NetMessage> ch;
        ch.setOrderFunc([](NetMessage &msg) { return msg.header().seq; });

        NetMessage msg_1;
        msg_1.header().seq = 1;

        NetMessage msg_2;
        msg_2.header().seq = 2;

        NetMessage msg_3;
        msg_3.header().seq = 3;

        // Pushed in order of timestamp
        ch.write(msg_1);
        ch.write(msg_2);
        ch.write(msg_3);

        SOFT_ASSERT_EQ(ch.read_one().header().seq, 1);
        SOFT_ASSERT_EQ(ch.read_one().header().seq, 2);
        SOFT_ASSERT_EQ(ch.read_one().header().seq, 3);

        // Pushed out of order (should get same results)
        ch.write(msg_3);
        ch.write(msg_1);
        ch.write(msg_2);

        SOFT_ASSERT_EQ(ch.read_one().header().seq, 1);
        SOFT_ASSERT_EQ(ch.read_one().header().seq, 2);
        SOFT_ASSERT_EQ(ch.read_one().header().seq, 3);
    }
    catch (std::exception &e)
    {
        printf("%s\n", e.what());
        success = false;
    }

    return success;
}


bool testConnect()
{
    bool success = true;

    std::vector<std::shared_ptr<NetClient>> clients;

    for (int i = 0; i < 5; i++)
    {
        clients.push_back(std::shared_ptr<NetClient>(new NetClient()));
    }

    std::unique_ptr<NetServer> server = std::unique_ptr<NetServer>(new NetServer(8080));

    try
    {
        server->start();

        for (auto &client : clients)
        {
            client->connectToServer("127.0.0.1", "8080");
        }

        NetMessage msg;

        for (auto &client : clients)
        {
            SOFT_ASSERT_EQ(client->blockingRecv(msg), true);
            SOFT_ASSERT_EQ(msg.header().type, NetMessageType::ConnectOk);
        }

        for (auto &client : clients)
        {
            SOFT_ASSERT_EQ(client->isConnected(), true);
        }
    }
    catch (std::exception &e)
    {
        printf("%s\n", e.what());
        success = false;
    }

    return success;
}


bool testClientDisconnects()
{
    bool success = true;

    std::unique_ptr<NetClient> client = std::unique_ptr<NetClient>(new NetClient());
    std::unique_ptr<NetServer> server = std::unique_ptr<NetServer>(new NetServer(8080));

    try
    {
        server->start();
        client->connectToServer("127.0.0.1", "8080");

        NetMessage inMsg;

        SOFT_ASSERT_EQ(client->blockingRecv(inMsg), true);
        SOFT_ASSERT_EQ(inMsg.header().type, NetMessageType::ConnectOk);

        int clientID = inMsg.body<ConnectOkBody>().assignedClientID;

        SOFT_ASSERT_EQ(server->isClientConnected(clientID), true);

        client->disconnect();

        inMsg = NetMessage();

        SOFT_ASSERT_EQ(server->blockingRecv(clientID, inMsg), true);
        SOFT_ASSERT_EQ(inMsg.header().type, NetMessageType::Disconnect);
        SOFT_ASSERT_EQ(server->isClientConnected(clientID), false);
    }
    catch (std::exception &e)
    {
        printf("%s\n", e.what());
        success = false;
    }

    return success;
}


bool testSendMessagesToServer()
{
    bool success = true;

    std::unique_ptr<NetClient> client = std::unique_ptr<NetClient>(new NetClient());
    std::unique_ptr<NetServer> server = std::unique_ptr<NetServer>(new NetServer(8080));

    try
    {
        server->start();
        client->connectToServer("127.0.0.1", "8080");

        NetMessage inMsg;

        SOFT_ASSERT_EQ(client->blockingRecv(inMsg), true);
        SOFT_ASSERT_EQ(inMsg.header().type, NetMessageType::ConnectOk);

        int clientID = inMsg.body<ConnectOkBody>().assignedClientID;

        std::vector<NetMessage> outMsgs;
        for (int i = 0; i < 1000; i++)
        {
            outMsgs.push_back(NetMessage{NetMessageType::Data, 100 + i});
        }

        for (auto &msg : outMsgs)
        {
            client->send(msg);
        }

        for (int i = 0; i < 1000; i++)
        {
            SOFT_ASSERT_EQ(server->blockingRecv(clientID, inMsg), true);
            SOFT_ASSERT_EQ(inMsg.header().type, NetMessageType::Data);
            SOFT_ASSERT_EQ(inMsg.body<int>(), 100 + i);
        }
    }
    catch (std::exception &e)
    {
        printf("%s\n", e.what());
        success = false;
    }

    return success;
}


bool testSendMessagesToClient()
{
    bool success = true;

    std::unique_ptr<NetClient> client = std::unique_ptr<NetClient>(new NetClient());
    std::unique_ptr<NetServer> server = std::unique_ptr<NetServer>(new NetServer(8080));

    try
    {
        server->start();
        client->connectToServer("127.0.0.1", "8080");

        NetMessage inMsg;

        SOFT_ASSERT_EQ(client->blockingRecv(inMsg), true);
        SOFT_ASSERT_EQ(inMsg.header().type, NetMessageType::ConnectOk);

        int clientID = inMsg.body<ConnectOkBody>().assignedClientID;

        std::vector<NetMessage> outMsgs;

        for (int i = 0; i < 1000; i++)
        {
            outMsgs.push_back(NetMessage{NetMessageType::Data, 100 + i});
        }

        for (auto &msg : outMsgs)
        {
            server->send(clientID, msg);
        }

        for (int i = 0; i < 1000; i++)
        {
            SOFT_ASSERT_EQ(client->blockingRecv(inMsg), true);
            SOFT_ASSERT_EQ(inMsg.header().type, NetMessageType::Data);
            SOFT_ASSERT_EQ(inMsg.body<int>(), 100 + i);
        }
    }
    catch (std::exception &e)
    {
        printf("%s\n", e.what());
        success = false;
    }

    return success;
}


int main() {
    RUN_TEST(testChannel, 5);
    RUN_TEST(testConnect, 10);
    RUN_TEST(testClientDisconnects, 10);
    RUN_TEST(testSendMessagesToServer, 10);
    RUN_TEST(testSendMessagesToClient, 10);

    printf("\n### Results ###\n");

    for (const auto& [name, passed] : g_testResults)
    {
        printf("%s: %s%s%s\n",
               name.c_str(),
               passed ? "\033[92m" : "\033[31m", // green color : red color
               passed ? "PASSED" : "FAILED",
               "\033[0m"); // reset color
    }


    return 0;
}
