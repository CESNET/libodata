#ifndef TEST_UTILS_H_
#define TEST_UTILS_H_

#include <boost/filesystem/path.hpp>
#include <memory>
#include <string>
#include <vector>

namespace OData {

class Directory;
class FileSystemNode;
class Product;

namespace Test {

std::vector<char> readTestInstance(const std::string& filename);
std::unique_ptr<Product> createProduct(
    std::string id, std::string platform) noexcept;
std::unique_ptr<Directory> createFilesystem(
    std::vector<std::shared_ptr<Product>> products) noexcept;
std::shared_ptr<FileSystemNode> testGetFile(
    const FileSystemNode& instance,
    const boost::filesystem::path& path) noexcept;

} /* namespace Test */
} /* namespace OData */

#endif /* TEST_UTILS_H_ */
