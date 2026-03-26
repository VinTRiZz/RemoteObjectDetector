#include "controllerbase.hpp"

ControllerBase::ControllerBase() {

}


void ControllerBase::sendTextMessage(drogon::HttpStatusCode status, const std::string &textMsg, ResponseCallback_t &&cbk) const
{
    auto pResponse = drogon::HttpResponse::newHttpResponse(status, drogon::CT_TEXT_PLAIN);
    pResponse->setBody(textMsg);
    cbk(pResponse);
}

void ControllerBase::sendJsonMessage(drogon::HttpStatusCode status, const std::string &jsonData, ResponseCallback_t &&cbk) const
{
    auto pResponse = drogon::HttpResponse::newHttpResponse(status, drogon::CT_APPLICATION_JSON);
    pResponse->setBody(jsonData);
    cbk(pResponse);
}