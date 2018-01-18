#include <iostream>
#include <restclient-cpp/restclient.h>

int main(int, char**) {
    RestClient::Response res = RestClient::get("https://dhr1.cesnet.cz/odata/v1/Products('32d27303-8a3c-4088-8a09-4e4816174d6b')/Nodes");
    std::cout << "Response code: " << res.code << std::endl;
    return 0;
}
