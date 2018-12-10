#ifndef SRC_PRODUCTATTRIBUTE_H_
#define SRC_PRODUCTATTRIBUTE_H_

namespace OData {
namespace Attribute {
// common product attributes
constexpr const char* CREATION_DATE = "beginposition";
constexpr const char* CLOUD_COVERAGE = "cloudcoverpercentage";
constexpr const char* COLLECTION = "collection";
constexpr const char* END_POSITION = "endposition";
constexpr const char* FILENAME = "filename";
constexpr const char* FOOTPRINT = "footprint";
constexpr const char* ID = "uuid";
constexpr const char* INGESTION_DATE = "ingestiondate";
constexpr const char* LAST_ORBIT_NUMBER = "lastorbitnumber";
constexpr const char* LAST_RELATIVE_ORBIT_NUMBER = "lastrelativeorbitnumber";
constexpr const char* NAME = "identifier";
constexpr const char* ORBIT_DIRECTION = "orbitdirection";
constexpr const char* ORBIT_NUMBER = "orbitnumber";
constexpr const char* PLATFORM = "platformname";
constexpr const char* POLARISATION_MODE = "polarisationmode";
constexpr const char* PRODUCT_TYPE = "producttype";
constexpr const char* RELATIVE_ORBIT_NUMBER = "relativeorbitnumber";
constexpr const char* SENSOR_OPER_MODE = "sensoroperationalmode";
constexpr const char* SIZE = "size";
constexpr const char* SWATH_ID = "swathidentifier";

} // namespace Attribute
} // namespace OData

#endif /* SRC_PRODUCTATTRIBUTE_H_ */
