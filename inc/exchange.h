#ifndef EXCHANGE_PROTOCOL_H
#define EXCHANGE_PROTOCOL_H

#include <sstream>

#include <openssl/pem.h>
#include <openssl/bio.h>

#include <msgpack.hpp>

namespace Exchange
{

enum PACKET_TYPES
{
    ACTION_PACKET_TYPE,
    INFO_PACKET_TYPE,
    ERROR_PACKET_TYPE
};

struct Packet
{
    int type {0};
    std::string command;
    std::string data;
    bool isValid {false};

    MSGPACK_DEFINE(command, type, data)
};

inline std::string base64_encode(std::string && input)
{
    BIO * bio, * b64;
    BUF_MEM * pBuffer;

    bio = BIO_new(BIO_s_mem());
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.c_str(), static_cast<int>(input.length()));
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &pBuffer);
    std::string result(pBuffer->data, pBuffer->length);
    BIO_free_all(bio);
    return result;
}

inline std::string base64_decode(const std::string & input)
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
    return std::move(result);
}

class PacketConverter
{
public:
    static std::string convert(const Packet & data_p)
    {
        try
        {
            std::stringstream buffer;
            msgpack::pack(buffer, data_p);
            return base64_encode(buffer.str());
        }
        catch (std::exception & ex)
        { return std::string(); }
    }

    static Packet convert(const std::string & data_ba)
    {
        try
        {
            std::stringstream sbuf;
            sbuf.write(data_ba.data(), data_ba.size());
            std::string strBuf = sbuf.str();
            strBuf = base64_decode(strBuf);

            msgpack::object_handle oh = msgpack::unpack( strBuf.data(), strBuf.size() );
            msgpack::object obj = oh.get();

            Packet result;
            obj.convert(result);

            result.isValid = true; // Shows that conversion complete with success
            return std::move(result);
        }
        catch (std::exception & ex)
        { return Packet(); }
    }
};

}

#endif // EXCHANGE_PROTOCOL_H
