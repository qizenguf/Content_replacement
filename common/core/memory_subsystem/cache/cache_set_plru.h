#ifndef CACHE_SET_PLRU_H
#define CACHE_SET_PLRU_H

#include "cache_set.h"

class CacheSetPLRU : public CacheSet
{
   public:
      CacheSetPLRU(CacheBase::cache_t cache_type,
            UInt32 associativity, UInt32 blocksize);
      ~CacheSetPLRU();

      UInt32 getReplacementIndex(CacheCntlr *cntlr);
      void updateReplacementIndex(UInt32 accessed_index);
      int possibleIndexCheck(int range);
   private:
      UInt8 b[32];
};

#endif /* CACHE_SET_PLRU_H */
