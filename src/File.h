#ifndef SRC_FILE_H_
#define SRC_FILE_H_

#include "FileSystemNode.h"
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <string>
#include <vector>

namespace OData {

class File : public FileSystemNode {
public:
  File() = default;
  File(std::string name, std::vector<char> data) noexcept;
  ~File() = default;
  File(const File&) = delete;
  File& operator=(const File&) = delete;

  void toString(std::ostream& ostr, unsigned indent_level = 0) const
      noexcept override;
  bool compare(const FileSystemNode& node) const noexcept override;
  std::string getName() const noexcept;
  const FileSystemNode* getFile(
      boost::filesystem::path::const_iterator begin,
      boost::filesystem::path::const_iterator end) const noexcept override;
  std::vector<std::string> readDir() const noexcept override;
  bool isDirectory() const noexcept override;

  const std::vector<char>& getData() const noexcept;

private:
  friend class boost::serialization::access;
  template <typename Archive> void serialize(Archive& ar, const unsigned int) {
    ar& boost::serialization::base_object<FileSystemNode>(*this);
    ar& name;
    ar& data;
  }

  std::string name;
  std::vector<char> data;
};

} /* namespace OData */

BOOST_CLASS_EXPORT_KEY(OData::File)

#endif /* SRC_FILE_H_ */
