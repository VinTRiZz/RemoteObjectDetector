#ifndef UL_EXCHANGEPACKET_H
#define UL_EXCHANGEPACKET_H

#ifdef QT_CORE_LIB
#include <QString>
#include <QDebug>
#endif // QT_CORE_LIB

#include <sstream>
#include <openssl/pem.h>
#include <openssl/bio.h>

#include <msgpack.hpp>

namespace Exchange
{

enum PacketMetaInfo
{
    // For exchanging one turn only (without response)
    PACKET_INFO_NULL_PACKET = 0,

    // Identification
    PACKET_INFO_CT_SET_TOKEN = 1,
    PACKET_INFO_CT_GET_TOKEN = 2,

    // System
    PACKET_INFO_CT_SETUP,
    PACKET_INFO_CT_RECONNECT,
    PACKET_INFO_CT_REBOOT,
    PACKET_INFO_CT_STATUS,

    // Work
    PACKET_INFO_CT_START,
    PACKET_INFO_CT_STOP,

    PACKET_INFO_CT_PHOTO,
    PACKET_INFO_CT_PHOTO_BEGIN,
    PACKET_INFO_CT_PHOTO_IN_PROCESS,
    PACKET_INFO_CT_PHOTO_END,

    // Objects
    PACKET_INFO_CT_LIST,
    PACKET_INFO_CT_DETECTED,
    PACKET_INFO_CT_ADD_OBJECT,
    PACKET_INFO_CT_UPD_OBJECT, // Rename
    PACKET_INFO_CT_REM_OBJECT,
};

struct Packet
{
    Packet() = default;
    Packet(uint8_t metadata, const std::string& payload) : packetMetadata{metadata}, payload{payload} {}
    uint8_t packetMetadata {PACKET_INFO_NULL_PACKET};
    std::string payload;

    MSGPACK_DEFINE(packetMetadata, payload)
};

struct StatusData
{
    std::map<std::string, std::string> statusMap;
    MSGPACK_DEFINE(statusMap)
};

inline std::string base64_encode(std::string&& input)
{
    BIO * bio, * b64;
    BUF_MEM * pBuffer;

    bio = BIO_new(BIO_s_mem());
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.c_str(), static_cast<int>(input.length()));
    BIO_flush(bio);
    BIO_get_mem_ptr(bio,&pBuffer);
    std::string result(pBuffer->data, pBuffer->length);
    BIO_free_all(bio);
    return result;
}

inline std::string base64_decode(const std::string& input)
{
    BIO * bio, * b64;
    char * pBuffer = new char[input.size()];
    memset(pBuffer, 0, input.size());
    bio = BIO_new_mem_buf(input.c_str(), static_cast<int>(input.length()));
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    int l = BIO_read(bio, pBuffer, static_cast<int>(input.size()));
    BIO_free_all(bio);
    std::string result(pBuffer, l);
    delete[] pBuffer;
    return result;
}

template <typename _T_packetType>
inline QByteArray encode(const _T_packetType& data_p)
{
    try {
        std::stringstream buffer;
        msgpack::pack(buffer, data_p);
        QByteArray result = base64_encode(buffer.str()).c_str();
        return result;
    }
    catch (std::exception& ex) {
        return {};
    }
}

template <typename _T_packetType, typename _T_arrayType>
inline _T_packetType decode(const _T_arrayType& data_ba);

template <typename _T_packetType>
inline _T_packetType decode(const QByteArray& data_ba)
{
    try {
        std::stringstream sbuf;
        sbuf.write(data_ba.data(), data_ba.size());
        std::string strBuf = sbuf.str();
        strBuf = base64_decode(strBuf);

        msgpack::object_handle oh = msgpack::unpack( strBuf.data(), strBuf.size() );
        msgpack::object obj = oh.get();

        _T_packetType result;
        obj.convert(result);

        return result;
    }
    catch (std::exception& ex) {
        return {};
    }
}

template <typename _T_packetType>
inline _T_packetType decode(const std::string& data_ba)
{
    try {
        std::stringstream sbuf;
        sbuf.write(data_ba.data(), data_ba.size());
        std::string strBuf = sbuf.str();
        strBuf = base64_decode(strBuf);

        msgpack::object_handle oh = msgpack::unpack( strBuf.data(), strBuf.size() );
        msgpack::object obj = oh.get();

        _T_packetType result;
        obj.convert(result);

        return result;
    }
    catch (std::exception& ex) {
        return {};
    }
}

class ConnectionException : public std::exception
{
public:
    enum class ErrorType
    {
        NoError,
        ConnectionError,
    };

    ConnectionException(ErrorType errt) { m_errorType = errt; };

    ErrorType errorType() { return m_errorType; }

private:
    ErrorType m_errorType;
};

}


#endif // UL_EXCHANGEPACKET_H
