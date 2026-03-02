#include "detectorendpoint.hpp"

DetectorEndpoint::DetectorEndpoint()
{

}

DetectorEndpoint::~DetectorEndpoint()
{

}

ImageProcessing::Streamer &DetectorEndpoint::getStreamer()
{
    return m_imgStreamer;
}
