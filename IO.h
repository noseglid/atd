#ifndef __IO_H__
#define __IO_H__

class IO
{
public:
  static std::string
  file_get_contents(std::string file)
  {
    std::ifstream ifs(file.c_str());
    if (!ifs.good())
      throw Exception(std::string("Could not open file: ") + file);

    return std::string(
      (std::istreambuf_iterator<char>(ifs)),
      (std::istreambuf_iterator<char>())
    );
  }
};

#endif
