#include <thread>
#include <chrono>
#include <sstream>
#include <map>

#include "../Networking/NetClient.h"
#include "../Networking/NetServer.h"
#include "../Networking/NetMessages.h"
#include "../Utils/TSPriorityQueue.h"


#define ASSERT_EQ(lhs, rhs)                                                         \
    do {                                                                            \
        auto _lhs = (lhs);                                                          \
        auto _rhs = (rhs);                                                          \
        if (_lhs != _rhs) {                                                         \
            std::stringstream ss;                                                   \
            ss << "ASSERT_EQ failed: " << #lhs << " != " << #rhs                    \
            << " (" << _lhs << " != " << _rhs << ")";                               \
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


std::ostream& operator<<(std::ostream& os, NetMessageType type) {
    switch (type) {
        case NetMessageType::ConnectOk:         return os << "ConnectOk";
        case NetMessageType::ConnectDenied:     return os << "ConnectDenied";
        case NetMessageType::Disconnect:        return os << "Disconnect";
        case NetMessageType::Data:              return os << "Data";

        default:                                return os << "Unknown";
    }
}

static std::map<std::string, bool> g_testResults;


bool testPriorityQueue()
{
    bool success = true;

    try
    {
        TSPriorityQueue<NetMessage> pQueue(
            [](const NetMessage &a, const NetMessage &b)
            {
                return a.header.seq > b.header.seq;
            }
        );

        NetMessage msg_1;
        msg_1.header.seq = 1;

        NetMessage msg_2;
        msg_2.header.seq = 2;

        NetMessage msg_3;
        msg_3.header.seq = 3;

        // Pushed in order of timestamp
        pQueue.push(msg_1);
        pQueue.push(msg_2);
        pQueue.push(msg_3);

        ASSERT_EQ(pQueue.pop().header.seq, 1);
        ASSERT_EQ(pQueue.pop().header.seq, 2);
        ASSERT_EQ(pQueue.pop().header.seq, 3);

        // Pushed out of order (should get same results)
        pQueue.push(msg_3);
        pQueue.push(msg_1);
        pQueue.push(msg_2);

        ASSERT_EQ(pQueue.pop().header.seq, 1);
        ASSERT_EQ(pQueue.pop().header.seq, 2);
        ASSERT_EQ(pQueue.pop().header.seq, 3);
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
            ASSERT_EQ(client->blockingRecv(msg), true);
            ASSERT_EQ(msg.header.type, NetMessageType::ConnectOk);
        }

        for (auto &client : clients)
        {
            ASSERT_EQ(client->isConnected(), true);
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

        ASSERT_EQ(client->blockingRecv(inMsg), true);
        ASSERT_EQ(inMsg.header.type, NetMessageType::ConnectOk);

        int clientID = inMsg.body<ConnectOkBody>().assignedClientID;

        ASSERT_EQ(server->isClientConnected(clientID), true);

        client->disconnect();

        ASSERT_EQ(server->blockingRecv(clientID, inMsg), true);
        ASSERT_EQ(inMsg.header.type, NetMessageType::Disconnect);
        ASSERT_EQ(server->isClientConnected(clientID), false);
    }
    catch (std::exception &e)
    {
        printf("%s\n", e.what());
        success = false;
    }

    return success;
}


bool testServerDisconnects()
{
    bool success = true;

    std::unique_ptr<NetClient> client = std::unique_ptr<NetClient>(new NetClient());
    std::unique_ptr<NetServer> server = std::unique_ptr<NetServer>(new NetServer(8080));

    try
    {
        server->start();
        client->connectToServer("127.0.0.1", "8080");

        NetMessage inMsg;

        ASSERT_EQ(client->blockingRecv(inMsg), true);
        ASSERT_EQ(inMsg.header.type, NetMessageType::ConnectOk);
        ASSERT_EQ(client->isConnected(), true);

        server->shutdown();

        ASSERT_EQ(client->blockingRecv(inMsg), true);
        ASSERT_EQ(inMsg.header.type, NetMessageType::Disconnect);
        ASSERT_EQ(client->isConnected(), false);
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

        ASSERT_EQ(client->blockingRecv(inMsg), true);
        ASSERT_EQ(inMsg.header.type, NetMessageType::ConnectOk);

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
            ASSERT_EQ(server->blockingRecv(clientID, inMsg), true);
            ASSERT_EQ(inMsg.header.type, NetMessageType::Data);
            ASSERT_EQ(inMsg.body<int>(), 100 + i);
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

        ASSERT_EQ(client->blockingRecv(inMsg), true);
        ASSERT_EQ(inMsg.header.type, NetMessageType::ConnectOk);

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
            ASSERT_EQ(client->blockingRecv(inMsg), true);
            ASSERT_EQ(inMsg.header.type, NetMessageType::Data);
            ASSERT_EQ(inMsg.body<int>(), 100 + i);
        }
    }
    catch (std::exception &e)
    {
        printf("%s\n", e.what());
        success = false;
    }

    return success;
}


bool testConnectToUnknownHost()
{
    bool success = true;

    std::unique_ptr<NetClient> client = std::unique_ptr<NetClient>(new NetClient());

    try
    {
        // Server shouldn't be running at 127.0.0.1:9090
        ASSERT_EQ(client->connectToServer("127.0.0.1", "9090"), false);
        ASSERT_EQ(client->isConnected(), false);
    }
    catch (std::exception &e)
    {
        printf("%s\n", e.what());
        success = false;
    }

    return success;
}


bool testClientMethodsWhileNotConnected()
{
    bool success = true;

    std::unique_ptr<NetClient> client = std::unique_ptr<NetClient>(new NetClient());

    try
    {
        NetMessage inMsg;

        ASSERT_EQ(client->send(inMsg), false);
        ASSERT_EQ(client->recv(inMsg), false);
        ASSERT_EQ(client->blockingRecv(inMsg), false);
    }
    catch (std::exception &e)
    {
        printf("%s\n", e.what());
        success = false;
    }

    return success;
}


bool testServerMethodsOnUnconnectedClient()
{
    bool success = true;

    std::unique_ptr<NetServer> server = std::unique_ptr<NetServer>(new NetServer(8080));

    try
    {
        int randomClientID = 10000;

        NetMessage inMsg;

        ASSERT_EQ(server->send(randomClientID, inMsg), false);
        ASSERT_EQ(server->recv(randomClientID, inMsg), false);
        ASSERT_EQ(server->blockingRecv(randomClientID, inMsg), false);
    }
    catch (std::exception &e)
    {
        printf("%s\n", e.what());
        success = false;
    }

    return success;
}


int main() {
    RUN_TEST(testPriorityQueue, 10);
    RUN_TEST(testConnect, 10);
    RUN_TEST(testClientDisconnects, 10);
    RUN_TEST(testServerDisconnects, 10);
    RUN_TEST(testSendMessagesToServer, 10);
    RUN_TEST(testSendMessagesToClient, 10);
    RUN_TEST(testConnectToUnknownHost, 10);
    RUN_TEST(testClientMethodsWhileNotConnected, 10);
    RUN_TEST(testServerMethodsOnUnconnectedClient, 10);

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
