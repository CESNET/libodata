# libodata
Library provides access to products stored in [Copernicus Data Hub](https://github.com/SentinelDataHub/dhus-distribution). It creates virtual filesystem containing product archives and also content of those archives. Information retrieved from data hub is cached in local database.

## Building from source
Following libraries are needed:

  * [libcurl4](https://github.com/curl/curl)
  * [glog](https://github.com/google/glog)
  * [tinyxml2](https://github.com/leethomason/tinyxml2)
  * [libdb++](https://github.com/berkeleydb/libdb)
  * boost (filesystem, thread, serialization, iostreams program_options)
  * optionally google test (needed only for unit tests)

i.e. on debian buster just run:

    # apt-get install libcurl4-openssl-dev libtinyxml2-dev libdb++-dev libgoogle-glog-dev libboost-all-dev    
    $ cd libodata
    $ cmake . && make