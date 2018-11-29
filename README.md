# libodata
Library provides access to products stored in [Copernicus Data Hub](https://github.com/SentinelDataHub/dhus-distribution). It creates virtual filesystem containing product archives and also content of those archives. Information retrieved from data hub is cached in local database.

## Building from source
Following libraries are needed:

  * [libcurl4](https://github.com/curl/curl)
  * [glog](https://github.com/google/glog)
  * [tinyxml2](https://github.com/leethomason/tinyxml2)
  * [libdb++](https://github.com/berkeleydb/libdb)
  * boost (filesystem, thread, serialization, iostreams, program_options)
  * optionally google test (needed only for unit tests)
  * optionally fuse (needed only for fuse filesystem implementation)

i.e. on debian just run:

    # apt-get install libcurl4-openssl-dev libtinyxml2-dev libdb++-dev libgoogle-glog-dev libboost-all-dev libfuse-dev
    $ cd libodata
    $ cmake . && make
    $ cpack
    # dpkg -i *.deb

## ODatafs
ODatafs combines libodata nad fuse library to create unix filesystem containing data stored in data hub. Filesystem metadata are stored in local database to improve performance. Last access files are also stored locally.

Filesystem options are provided in configuration file. All options are written as key value pairs (key=value).
  * url - data hub address
  * username - data hub username
  * password - data hub password
  * missions - comma separated list of missions to be downloaded (i.e. Sentinel-1,Sentinel-2)
  * db_path - database file location for storing products metadata
  * tmp_path - folder where temporary files will be stored
  * tmp_size - maximum temporary files count
  * custom_path - template defining filesystem structure i.e. /${platformname}/${year}/${mont}/${day}. 
    Product attributes are specified in "${}" and you can use any product attribute or additional attributes computed by odata (date, year, month, day).
  * validate_certificate - enable or disable server certificate validation