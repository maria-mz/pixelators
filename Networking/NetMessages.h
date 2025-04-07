#ifndef NET_MESSAGES_H_
#define NET_MESSAGES_H_

#include <vector>
// Forward declare NetConnection, let it access NetMessage body to use as buffer
class NetConnection;

enum class NetMessageType : u_int32_t
{
    Unknown = 0,
    ConnectOk,
    ConnectDenied,
    Disconnect,
    Data
};


std::ostream& operator<<(std::ostream& os, NetMessageType type) {
    switch (type) {
        case NetMessageType::ConnectOk:         return os << "ConnectOk";
        case NetMessageType::ConnectDenied:     return os << "ConnectDenied";
        case NetMessageType::Disconnect:        return os << "Disconnect";
        case NetMessageType::Data:              return os << "Data";

        default:                                return os << "Unknown";
    }
}

struct NetMessageHeader
{
    NetMessageType type;
    u_int32_t size;      // Number of bytes in the message body
    u_int16_t seq;
};

struct ConnectOkBody
{
    u_int32_t assignedClientID;
};

struct NetMessage
{
    public:
        NetMessage() : m_header{} {}

        NetMessage(NetMessageType messageType, u_int16_t seq = 0) : m_header{messageType, 0, seq} {}

        template <typename T>
        NetMessage(NetMessageType messageType, const T &data, u_int16_t seq = 0)
            : m_header{messageType, 0, seq}
        {
            static_assert(std::is_trivially_copyable<T>::value,
                "Message body must be trivially copyable");

            m_body.resize(sizeof(T));
            std::memcpy(m_body.data(), &data, sizeof(T));
            m_header.size = sizeof(T);
        }

        NetMessageHeader &header()
        {
            return m_header;
        }

        template <typename T>
        T body()
        {
            static_assert(std::is_trivially_copyable<T>::value,
                          "Message body must be trivially copyable");

            if (m_body.size() != sizeof(T))
            {
                throw std::runtime_error("Message body size doesn't match type size");
            }

            T data;
            std::memcpy(&data, m_body.data(), sizeof(T));
            return data;
        }

    private:
        NetMessageHeader m_header;
        std::vector<uint8_t> m_body;

        friend class NetConnection;
};

#endif