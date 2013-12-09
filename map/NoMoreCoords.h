#ifndef __MAP__NOMORECOORDS_H__
#define __MAP__NOMORECOORDS_H__

namespace map {

class NoMoreCoords : public Exception
{
public:
  NoMoreCoords() : Exception("No more coordinates in path.") {}
};

}

#endif
