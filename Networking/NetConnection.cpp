#include "NetConnection.h"


std::shared_ptr<NetConnection> NetConnection::create(asio::io_context &io_context)
{
    return std::shared_ptr<NetConnection>(new NetConnection(io_context));
}


void NetConnection::readHeader()
{
    m_currentMsgIn = NetMessage();

    asio::async_read(
        m_socket,
        asio::buffer((&m_currentMsgIn.header), sizeof(NetMessageHeader)),
        [this](const std::error_code &ec, size_t length)
        {
            if (!ec)
            {
                if (m_currentMsgIn.header.size > 0)
                {
                    m_currentMsgIn.bodyRaw.resize(m_currentMsgIn.header.size);
                    readBody();
                }
                else
                {
                    // Read a whole message! (header only)
                    m_inMessages.push(m_currentMsgIn);

                    readHeader();
                }
            }
            else
            {
                printf("[NET] Message read fail\n");
                m_socket.close();
                m_inMessages.push(NetMessage{NetMessageType::Disconnect, m_nextInSeq});
            }
        }
    );
}


void NetConnection::readBody()
{
    asio::async_read(
        m_socket,
        asio::buffer((m_currentMsgIn.bodyRaw.data()), m_currentMsgIn.bodyRaw.size()),
        [this](const std::error_code &ec, size_t length)
        {
            if (!ec)
            {
                // Read a whole message! (header and body)
                m_inMessages.push(m_currentMsgIn);

                readHeader();
            }
            else
            {
                printf("[NET] Message read fail\n");
                m_socket.close();
                m_inMessages.push(NetMessage{NetMessageType::Disconnect, m_nextInSeq});
            }
        }
    );
}


void NetConnection::writeHeader()
{
    if (!m_outMessages.top(m_currentMsgOut))
    {
        return;
    }

    asio::async_write(
        m_socket,
        asio::buffer(&m_currentMsgOut.header, sizeof(NetMessageHeader)),
        [this](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                if (m_currentMsgOut.header.size > 0)
                {
                    writeBody();
                }
                else
                {
                    // printf("[NET_CONNECTION: %d] WROTE MESSAGE, seq: %d\n", m_id, m_currentMsgOut.header().seq);
                    m_outMessages.pop();
                    writeHeader();
                }
            }
            else
            {
                printf("[NET] Message write fail\n");
                m_socket.close();
                m_inMessages.push(NetMessage{NetMessageType::Disconnect, m_nextInSeq});
            }
        }
    );
}


void NetConnection::writeBody()
{
    asio::async_write(
        m_socket,
        asio::buffer((m_currentMsgOut.bodyRaw.data()), m_currentMsgOut.bodyRaw.size()),
        [this](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                m_outMessages.pop();
                // printf("[NET_CONNECTION: %d] WROTE MESSAGE, seq: %d\n", m_id, m_currentMsgOut.header().seq);
                writeHeader();
            }
            else
            {
                printf("[NET] Message write fail\n");
                m_socket.close();
                m_inMessages.push(NetMessage{NetMessageType::Disconnect, m_nextInSeq});
            }
        }
    );
}


void NetConnection::startReadLoop()
{
    readHeader();
}


void NetConnection::send(NetMessage &msg)
{
    msg.header.seq = m_outSeq++;

    if (m_outMessages.empty())
    {
        m_outMessages.push(msg);
        writeHeader();
    }
    else
    {
        m_outMessages.push(msg);
    }
}


bool NetConnection::recv(NetMessage &msg)
{
    bool ok = true;

    msg = NetMessage();

    // printf("[NET_CONNECTION: %d] Waiting for next seq: %d\n", m_id, m_nextInSeq);

    NetMessage tempMsg;

    if (m_inMessages.top(tempMsg) && tempMsg.header.seq == m_nextInSeq)
    {
        msg = m_inMessages.pop();
        m_nextInSeq++;
        ok = true;
    }

    return ok;
}


bool NetConnection::blockingRecv(NetMessage &msg)
{
    bool ok = true;

    msg = NetMessage();

    // printf("[NET_CONNECTION: %d] Waiting for next seq: %d\n", m_id, m_nextInSeq);

    ok = m_inMessages.pop_matching(
        msg, [this](const NetMessage &m){ return m.header.seq == m_nextInSeq; }
    );

    if (ok)
    {
        m_nextInSeq++;
    }

    return ok;
}
