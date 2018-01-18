#include <iostream>
#include <restclient-cpp/connection.h>

int main(int argc, char** argv) {
    if(argc < 3) {
        return 1;
    }
    RestClient::Connection connection("https://dhr1.cesnet.cz/odata/v1/");
    connection.SetBasicAuth(argv[1], argv[2]);
    auto res = connection.get("Products('32d27303-8a3c-4088-8a09-4e4816174d6b')/$value");
    std::cout << "Response code: " << res.code << std::endl;
    std::cout << res.body << std::endl;
    return 0;
}
