#include "cache_set_lru.h"
//#include "log.h"
//#include "stats.h"
#include <iostream>
// Implements LRU replacement, optionally augmented with Query-Based Selection [Jaleel et al., MICRO'10]

CacheSetLRU::CacheSetLRU(
      CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize, CacheSetInfoLRU* set_info, UInt8 num_attempts)
   : CacheSet(cache_type, associativity, blocksize)
   , m_num_attempts(num_attempts)
   , m_set_info(set_info)
{
   m_lru_bits = new UInt8[m_associativity];
   for (UInt32 i = 0; i < m_associativity; i++){
      m_lru_bits[i] = i;
      //m_cache_block_info_array[i]->setreplacementInfo(i);
  }
      
}

CacheSetLRU::~CacheSetLRU()
{
   delete [] m_lru_bits;
}

UInt8 CacheSetLRU::getReplacementInfo(UInt32 line_index)
{
   std::cout<<line_index <<" cacheset lru " <<std::endl;
   return m_lru_bits[line_index];
}

UInt32
CacheSetLRU::getReplacementIndex(CacheCntlr *cntlr)
{
   // First try to find an invalid block
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (!m_cache_block_info_array[i]->isValid())
      {
         // Mark our newly-inserted line as most-recently used
         moveToMRU(i);
         return i;
      }
   }

  
      UInt32 index = 0;
      int max_bits = -1;
      for (UInt32 i = 0; i < m_associativity; i++)
      {
         if (m_lru_bits[i] > max_bits && isValidReplacement(i))
         {
            index = i;
            max_bits = m_lru_bits[i];
         }
      }
      moveToMRU(index);
     // m_set_info->incrementAttempt(attempt);
      return index;
  // LOG_PRINT_ERROR("Should not reach here");
}

void
CacheSetLRU::updateReplacementIndex(UInt32 accessed_index)
{
   m_set_info->increment(m_lru_bits[accessed_index]);
   moveToMRU(accessed_index);
}

void
CacheSetLRU::moveToMRU(UInt32 accessed_index)
{
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (m_lru_bits[i] < m_lru_bits[accessed_index])
         m_lru_bits[i] ++;
      m_cache_block_info_array[i]->setreplacementInfo(m_lru_bits[i]);
   }
   m_lru_bits[accessed_index] = 0;
   m_cache_block_info_array[accessed_index]->setreplacementInfo(0);
}

CacheSetInfoLRU::CacheSetInfoLRU(String name, String cfgname, core_id_t core_id, UInt32 associativity, UInt8 num_attempts)
   : m_associativity(associativity)
   , m_attempts(NULL)
{
   m_access = new UInt64[m_associativity];
   for(UInt32 i = 0; i < m_associativity; ++i)
   {
      m_access[i] = 0;
     // registerStatsMetric(name, core_id, String("access-mru-")+itostr(i), &m_access[i]);
   }

   if (num_attempts > 1)
   {
      m_attempts = new UInt64[num_attempts];
      for(UInt32 i = 0; i < num_attempts; ++i)
      {
         m_attempts[i] = 0;
       //  registerStatsMetric(name, core_id, String("qbs-attempt-")+itostr(i), &m_attempts[i]);
      }
   }
};

CacheSetInfoLRU::~CacheSetInfoLRU()
{
   delete [] m_access;
   if (m_attempts)
      delete [] m_attempts;
}
