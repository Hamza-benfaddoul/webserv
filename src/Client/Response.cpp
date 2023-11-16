#include "Response.hpp"
#include "Request.hpp"

Response::Response(const Request &req) : request(req)
{
}

Response::~Response()
{
}
