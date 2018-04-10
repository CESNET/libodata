#include "BerkeleyDBStorage.h"

#include "Directory.h"
#include "File.h"
#include "Product.h"
#include "RemoteFile.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <vector>

namespace OData {
namespace {
Dbt createProductKey(const std::string& product_id) {
  const void* key = product_id.c_str();
  return Dbt(const_cast<void*>(key), product_id.length() + 1);
}

std::vector<char> createProductRecord(const Product& product) {
  std::vector<char> v;
  boost::iostreams::stream<
      boost::iostreams::back_insert_device<std::vector<char>>>
      os(v);
  boost::archive::binary_oarchive out(os);
  out.register_type<Directory>();
  out.register_type<RemoteFile>();
  out.register_type<File>();
  out.register_type<Product>();
  out& product;
  os.flush();
  return v;
}

std::shared_ptr<Product> decodeProductRecord(const Dbt& record) {
  auto product(std::make_shared<Product>());
  boost::iostreams::stream<boost::iostreams::array_source> sstream(
      reinterpret_cast<char*>(record.get_data()), record.get_size());
  boost::archive::binary_iarchive in(sstream);
  in.register_type<Directory>();
  in.register_type<RemoteFile>();
  in.register_type<File>();
  in.register_type<Product>();
  in&(*product);
  return product;
}
} // namespace

BerkeleyDBStorage::BerkeleyDBStorage() : database(nullptr, 0) {
  database.open(nullptr, "products.db", nullptr, DB_HASH, DB_CREATE, 0);
}

BerkeleyDBStorage::~BerkeleyDBStorage() {
  database.close(0);
}

void BerkeleyDBStorage::storeProduct(std::shared_ptr<Product> product) {
  std::unique_lock<std::mutex> lock(db_access_mutex);
  auto key = createProductKey(product->getId());
  auto raw_data = createProductRecord(*product);
  Dbt data(raw_data.data(), raw_data.size());
  /*int ret =*/database.put(nullptr, &key, &data, 0);
  // TODO handle error
}

bool BerkeleyDBStorage::productExists(const std::string& product_id) {
  std::unique_lock<std::mutex> lock(db_access_mutex);
  auto key = createProductKey(product_id);
  return 0 == database.exists(nullptr, &key, 0);
}

std::shared_ptr<Product> BerkeleyDBStorage::getProduct(
    const std::string& product_id) {
  std::unique_lock<std::mutex> lock(db_access_mutex);
  auto key = createProductKey(product_id);
  Dbt data;
  // TODO error handling
  database.get(nullptr, &key, &data, 0);
  return decodeProductRecord(data);
}

} /* namespace OData */
