#include <iostream>
#include <fstream>
#include <restclient-cpp/connection.h>
#include <tinyxml2.h>

RestClient::Response listFilesQuery(RestClient::Connection& connection) {
    return connection.get("Products?$orderby=IngestionDate&$top=1");
}

int main(int argc, char** argv) {
    if (argc < 4) {
        return 1;
    }
    RestClient::Connection connection("https://dhr1.cesnet.cz/odata/v1/");
    connection.SetBasicAuth(argv[1], argv[2]);
    if (argv[3] == std::string("list")) {
        auto res = listFilesQuery(connection);
        std::cout << "Response code: " << res.code << std::endl;
        std::cout << "list: " << res.body << std::endl;
        tinyxml2::XMLDocument doc;
        if (tinyxml2::XML_SUCCESS == doc.Parse(res.body.c_str(), res.body.length())) {

        }
    } else {
        auto res = connection.get("Products('32d27303-8a3c-4088-8a09-4e4816174d6b')/Nodes");
        std::cout << "Response code: " << res.code << std::endl;
        std::cout << res.body << std::endl;
    }
    return 0;
}
