#ifndef __HASH_H__
#define __HASH_H__

#include <boost/uuid/sha1.hpp>
#include <sstream>

class Hash
{
public:
  static std::string sha1(std::string data)
  {
    boost::uuids::detail::sha1 sha1;
    sha1.process_bytes(data.c_str(), data.size());

    unsigned int hash[5];
    sha1.get_digest(hash);

    std::stringstream ret;
    for (size_t i = 0; i < sizeof(hash) / sizeof(*hash); ++i) {
      ret << std::hex << hash[i];
    }

    return ret.str();
  }
};

#endif
