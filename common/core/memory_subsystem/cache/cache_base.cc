#include "cache_base.h"
#include "utils.h"
//#include "log.h"
#include "rng.h"
#include "address_home_lookup.h"

CacheBase::CacheBase(
   String name, UInt32 num_sets, UInt32 associativity, UInt32 cache_block_size,
   CacheBase::hash_t hash, UInt32 cache_sector_size, AddressHomeLookup *ahl)
:
   m_name(name),
   m_cache_size(UInt64(num_sets) * associativity * cache_block_size),
   m_associativity(associativity),
   m_blocksize(cache_block_size), 
   m_sectorsize(cache_sector_size),
   m_hash(hash),
   m_num_sets(num_sets),
   m_ahl(ahl)
{
   m_log_blocksize = floorLog2(m_blocksize);
   //m_sectorsize = Sim()->getCfg()->getInt("perf_model/STT/sector_size");
   //LOG_ASSERT_ERROR((m_num_sets == (1UL << floorLog2(m_num_sets))) || (hash != CacheBase::HASH_MASK),
    //  "Caches of non-power of 2 size need funky hash function");
}

CacheBase::~CacheBase()
{}

// utilities
CacheBase::hash_t
CacheBase::parseAddressHash(String hash_name)
{
   if (hash_name == "mask")
      return CacheBase::HASH_MASK;
   else if (hash_name == "mod")
      return CacheBase::HASH_MOD;
   else if (hash_name == "rng1_mod")
      return CacheBase::HASH_RNG1_MOD;
   else if (hash_name == "rng2_mod")
      return CacheBase::HASH_RNG2_MOD;
   else if (hash_name == "sec")
	  return CacheBase::SEC;
	else if (hash_name == "rand")
	  return CacheBase::RAND;
	/*else if (hash_name == "sec_4")
		return CacheBase::SEC_4; 
	else if (hash_name == "sec_8")
		return CacheBase::SEC_8;  
	else if (hash_name == "sec_16")
		return CacheBase::SEC_16;  
	else if (hash_name == "sec_32")
		return CacheBase::SEC_32;  
	else if (hash_name == "sec_64")
		return CacheBase::SEC_64;    */
   else
		return CacheBase::HASH_MASK;
   //   LOG_PRINT_ERROR("Invalid address hash function %s", hash_name.c_str());
}

void
CacheBase::splitAddress(const IntPtr addr, IntPtr& tag, UInt32& set_index) const
{
   tag = addr >> m_log_blocksize;

   IntPtr linearAddress = m_ahl ? m_ahl->getLinearAddress(addr) : addr;
   IntPtr block_num = linearAddress >> m_log_blocksize;

   switch(m_hash)
   {
      case CacheBase::HASH_MASK:
         set_index = block_num & (m_num_sets-1);
         break;
      case CacheBase::HASH_MOD:
         set_index = block_num % m_num_sets;
         break;
      case CacheBase::HASH_RNG1_MOD:
      {
         UInt64 state = rng_seed(block_num);
         set_index = rng_next(state) % m_num_sets;
         break;
      }
      case CacheBase::HASH_RNG2_MOD:
      {
         UInt64 state = rng_seed(block_num);
         rng_next(state);
         set_index = rng_next(state) % m_num_sets;
         break;
      }
      case CacheBase::SEC:
      {
		set_index = (block_num/m_sectorsize) % m_num_sets;
		//tag = tag ;
		break;
	  }
	  case CacheBase::RAND:
      {
		UInt32 temp = (block_num /m_num_sets) % m_num_sets;
		set_index = (block_num) % m_num_sets;
		set_index = set_index ^ temp;
		//tag = tag ;
		break;
	  }
    /*  case CacheBase::SEC_4:
      {
		set_index = (block_num/2) % m_num_sets;
		tag = tag >>2;
		break;
	  }
      case CacheBase::SEC_8:
      {
		set_index = (block_num/8) % m_num_sets;
		tag = tag >> 3;
		break;
	  }
      case CacheBase::SEC_16:
      {
		set_index = (block_num/16) % m_num_sets;
		break;
	  }
      case CacheBase::SEC_32:
      {
		set_index = (block_num/32) % m_num_sets;
		break;
	  }
      case CacheBase::SEC_64: // add by QI to support sector indexing
      {
		set_index = (block_num/64) % m_num_sets;
		break;
	  }*/
      default: return;
		//	LOG_PRINT_ERROR("Invalid hash function %d", m_hash);
   }
}

void
CacheBase::splitAddress(const IntPtr addr, IntPtr& tag, UInt32& set_index,
                  UInt32& block_offset) const
{
   block_offset = addr & (m_blocksize-1);
   splitAddress(addr, tag, set_index);
}

IntPtr
CacheBase::tagToAddress(const IntPtr tag)
{
   return tag << m_log_blocksize;
}
