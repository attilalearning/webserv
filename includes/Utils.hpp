#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

class Utils
{
    public:
        /*String Manipulations*/
        static std::string trim(const std::string& str);
        static std::string toLowerCase(const std::string& str);
        static std::string toUpperCase(const std::string& str);
        static std::vector<std::string> split(const std::string& str, char delimiter);
        static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
        static bool startsWith(const std::string& str, const std::string& prefix);
        static bool endsWith(const std::string& str, const std::string& suffix);

        /*File operations*/
        static bool fileExists(const std::string& path);
        static bool isDirectory(const std::string& path);
        static bool isFile(const std::string& path);
        static std::string readFile(const std::string& path);
        static bool writeFile(const std::string& path, const std::string& content);
        static bool deleteFile(const std::string& path);
        static std::vector<std::string> listDirectory(const std::string& path);

        /*Path operations*/

        /*MIME types*/

        /*URL operations*/

        /*Number conversions*/
        static int toInt(const std::string& str);
        static size_t toSizeT(const std::string& str);

        /*Time*/

    private:
        Utils();
        ~Utils();
        Utils(const Utils&);
        Utils& operator=(const Utils&);
};


#endif