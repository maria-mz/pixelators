#ifndef NET_CONNECTION_H_
#define NET_CONNECTION_H_

#include <functional>
#include <memory>
#include <chrono>

#include <asio.hpp>

#include "NetMessages.h"
#include "../Utils/TSPriorityQueue.h"


class NetConnection : public std::enable_shared_from_this<NetConnection>
{
    public:
        static std::shared_ptr<NetConnection> create(asio::io_context &io_context);

        void startReadLoop();
        void send(NetMessage &msg);

        bool recv(NetMessage &msg);
        bool blockingRecv(NetMessage &msg);

        asio::ip::tcp::socket m_socket;
        u_int32_t m_id;

    private:
        NetConnection(asio::io_context &io_context)
        : m_socket(io_context)
        , m_inMessages([](const NetMessage &a, const NetMessage &b) { return a.header.seq > b.header.seq; })
        , m_outMessages([](const NetMessage &a, const NetMessage &b) { return a.header.seq > b.header.seq; })
        {}

        void readHeader();
        void readBody();
        void writeHeader();
        void writeBody();

        u_int16_t m_outSeq = 1;
        u_int16_t m_nextInSeq = 1;

        NetMessage m_currentMsgIn;
        NetMessage m_currentMsgOut;

        TSPriorityQueue<NetMessage> m_inMessages;
        TSPriorityQueue<NetMessage> m_outMessages;
};

#endif