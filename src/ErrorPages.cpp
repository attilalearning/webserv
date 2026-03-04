#include <sstream>

#include "ErrorPages.hpp"

std::string ErrorPages::_template =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "  <meta charset=\"UTF-8\">\n"
    "  <title>{{STATUS_CODE}} {{STATUS_MESSAGE}}</title>\n"
    "  <style>\n"
    "    body { font-family: Arial, sans-serif; background: #f4f4f4; }\n"
    "    .container { margin: 100px auto; width: 600px; text-align: center; }\n"
    "    h1 { font-size: 72px; margin-bottom: 10px; }\n"
    "    p { font-size: 24px; color: #555; }\n"
    "  </style>\n"
    "</head>\n"
    "<body>\n"
    "  <div class=\"container\">\n"
    "    <h1>{{STATUS_CODE}}</h1>\n"
    "    <p>{{STATUS_MESSAGE}}</p>\n"
    "  </div>\n"
    "</body>\n"
    "</html>\n";

ErrorPages::ErrorPages()
{
}

ErrorPages::~ErrorPages()
{
}

std::string ErrorPages::generate(const HTTP_StatusPair &status)
{
    std::stringstream ss;
    ss << status.code;

    std::string result = _template;
    result = replaceAll(result, "{{STATUS_CODE}}", ss.str());
    result = replaceAll(result, "{{STATUS_MESSAGE}}", status.text);

    return result;
}

std::string ErrorPages::replaceAll(const std::string &src,
                                   const std::string &from,
                                   const std::string &to)
{
    if (from.empty())
        return src;

    std::string result = src;
    std::string::size_type pos = 0;

    while ((pos = result.find(from, pos)) != std::string::npos)
    {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}
