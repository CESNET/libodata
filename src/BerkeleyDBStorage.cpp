#include "BerkeleyDBStorage.h"

#include "DataHubException.h"
#include "Directory.h"
#include "File.h"
#include "Product.h"
#include "RemoteFile.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <sstream>
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

class Cursor : public ProductIterator {
public:
  explicit Cursor(Dbc* cursor, std::mutex& db_access_mutex)
      : cursor(cursor), db_access_mutex(db_access_mutex) {
  }
  virtual ~Cursor() {
    cursor->close();
  }

  std::shared_ptr<Product> next() override {
    std::unique_lock<std::mutex> lock(db_access_mutex);
    Dbt key;
    Dbt data;
    if (cursor->get(&key, &data, DB_NEXT) == 0) {
      return decodeProductRecord(data);
    } else {
      // TODO handle error
      return nullptr;
    }
  }

private:
  Dbc* cursor;
  std::mutex& db_access_mutex;
};

std::shared_ptr<Product> throwException(
    const std::string& product_id, const std::string& operation, int error) {
  std::stringstream str;
  str << "DB:";
  if (!product_id.empty()) {
    str << " Product '" << product_id << "'";
  }
  str << " " << operation << " failed: " << error;
  throw DataHubException(str.str());
}
} // namespace

BerkeleyDBStorage::BerkeleyDBStorage(boost::filesystem::path db_path)
    : database(nullptr, 0) {
  database.open(nullptr, db_path.c_str(), nullptr, DB_HASH, DB_CREATE, 0);
}

BerkeleyDBStorage::~BerkeleyDBStorage() {
  database.close(0);
}

void BerkeleyDBStorage::storeProduct(std::shared_ptr<Product> product) {
  const auto product_id = product->getId();
  auto key = createProductKey(product_id);
  auto raw_data = createProductRecord(*product);
  int ret;
  Dbt data(raw_data.data(), raw_data.size());
  {
    std::unique_lock<std::mutex> lock(db_access_mutex);
    ret = database.put(nullptr, &key, &data, 0);
  }
  if (ret != 0) {
    throwException(product_id, "save ", ret);
  }
}

bool BerkeleyDBStorage::productExists(const std::string& product_id) noexcept {
  auto key = createProductKey(product_id);
  {
    std::unique_lock<std::mutex> lock(db_access_mutex);
    return 0 == database.exists(nullptr, &key, 0);
  }
}

std::shared_ptr<Product> BerkeleyDBStorage::getProduct(
    const std::string& product_id) {
  auto key = createProductKey(product_id);
  Dbt data;
  {
    std::unique_lock<std::mutex> lock(db_access_mutex);
    const auto ret = database.get(nullptr, &key, &data, 0);
    if (ret == 0) {
      return decodeProductRecord(data);
    } else {
      return throwException(product_id, "load ", ret);
    }
  }
}

void BerkeleyDBStorage::deleteProduct(const std::string& product_id) {
  int ret;
  auto key = createProductKey(product_id);
  {
    std::unique_lock<std::mutex> lock(db_access_mutex);
    ret = database.del(nullptr, &key, 0);
  }
  if (ret != 0) {
    throwException(product_id, "delete ", ret);
  }
} // namespace OData

std::unique_ptr<ProductIterator> BerkeleyDBStorage::iterator() {
  std::unique_lock<std::mutex> lock(db_access_mutex);
  Dbc* cursor = nullptr;
  const auto ret = database.cursor(nullptr, &cursor, 0);
  if (ret != 0 || cursor == nullptr) {
    throwException("", "create cursor", ret);
  }
  return std::unique_ptr<ProductIterator>(new Cursor(cursor, db_access_mutex));
} // namespace OData

} /* namespace OData */
