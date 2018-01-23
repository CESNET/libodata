#include <iostream>
#include "Connection.h"
#include "Product.h"

int main(int argc, char** argv) {
    if (argc < 4) {
        return 1;
    }
    OData::Connection connection("https://dhr1.cesnet.cz/", argv[1], argv[2]);
    const auto files = connection.listProducts("Sentinel-1", 6);
    for (const auto& file: files) {
        std::cout << file << std::endl;
    }
    return 0;
}
