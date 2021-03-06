/* This file was generated automatically by the Snowball to ISO C++ compiler */

#include "steminternal.h"

namespace Xapian {

class InternalStemSwedish : public SnowballStemImplementation {
    int I_x;
    int I_p1;
  public:
    int r_other_suffix();
    int r_consonant_pair();
    int r_main_suffix();
    int r_mark_regions();

    InternalStemSwedish();
    ~InternalStemSwedish();
    int stem();
    std::string get_description() const;
};

}
