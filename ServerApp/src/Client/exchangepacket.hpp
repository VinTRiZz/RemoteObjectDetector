#ifndef UL_EXCHANGEPACKET_H
#define UL_EXCHANGEPACKET_H

#ifdef QT_CORE_LIB
#include <QString>
#include <QDebug>
#endif // QT_CORE_LIB

#include <sstream>
#include <openssl/pem.h>
#include <openssl/bio.h>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/map.hpp>

#include <boost/algorithm/hex.hpp>

namespace Exchange
{

namespace Internal
{

inline std::string encodeBase64(const std::string &input)
{
    BIO *bio, *b64;
    BUF_MEM* bufferPtr;

    bio = BIO_new(BIO_s_mem());
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.c_str(), static_cast<int>(input.length()));
    BIO_flush(bio);

    BIO_get_mem_ptr(bio, &bufferPtr);
    std::string output(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);

    return output;
}

inline std::string decodeBase64(const std::string &input)
{
    BIO *bio, *b64;
    char* buffer = new char[input.size()];
    memset(buffer, 0, input.size());

    bio = BIO_new_mem_buf(input.c_str(), static_cast<int>(input.length()));
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    int len = BIO_read(bio, buffer, static_cast<int>(input.size()));

    BIO_free_all(bio);

    std::string output(buffer, len);
    delete[] buffer;

    return output;
}

inline std::string encodeHex(const std::string& input)
{
    std::string convertedStr;
    convertedStr.reserve(input.size());
    boost::algorithm::hex(input.begin(), input.end(), std::back_inserter(convertedStr));
    return convertedStr;
}

inline std::string decodeHex(const std::string& input)
{
    std::string convertedStr;
    convertedStr.reserve(input.size());
    boost::algorithm::unhex(input, std::back_inserter(convertedStr));
    return convertedStr;
}

}

const uint64_t DOWNLOAD_PAYLOAD_SIZE {20000};

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
    PACKET_INFO_CT_UPD_OBJECT,
    PACKET_INFO_CT_REM_OBJECT,
};

struct Packet
{
    Packet() = default;
    Packet(uint8_t metadata, const std::string& payload) : packetMetadata{metadata}, payload{payload} {}
    uint8_t packetMetadata {PACKET_INFO_NULL_PACKET};
    std::string payload;

    // Serialization for boost
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & packetMetadata;
        ar & payload;
    }
};

struct StatusData
{
    std::map<std::string, std::string> statusMap;
    // Serialization for boost
    template<class Archive>
    void serialize(Archive & ar, const unsigned int) {
        ar & statusMap;
    }
};

template <typename _T_packetType>
inline QByteArray encode(const _T_packetType& data_p)
{
    try {
        std::ostringstream oss;
        boost::archive::text_oarchive oarchive(oss);
        oarchive << data_p;  // Serialize the structure
        auto result = Internal::encodeHex( oss.str() );
        return result.c_str();
    }
    catch (std::exception& ex) {
        return {};
    }
}

template <typename _T_packetType>
inline _T_packetType decode(const std::string& data_ba)
{
    try {
        std::string inputDecoded = Internal::decodeHex(data_ba);
        std::istringstream iss(inputDecoded);
        boost::archive::text_iarchive iarchive(iss);
        _T_packetType result;
        iarchive >> result;  // Deserialize the structure
        return result;
    }
    catch (std::exception& ex) {
        return {};
    }
}

template <typename _T_packetType>
inline _T_packetType decode(const QByteArray& data_ba)
{
    return decode<_T_packetType>(data_ba.toStdString());
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
