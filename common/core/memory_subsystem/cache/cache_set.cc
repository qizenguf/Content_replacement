#include "cache_set.h"
#include "cache_set_lru.h"
//#include "cache_set_mru.h"
//#include "cache_set_nmru.h"
//#include "cache_set_nru.h"
#include "cache_set_plru.h"
//#include "cache_set_random.h"
//#include "cache_set_round_robin.h"
//#include "cache_set_srrip.h"
#include "cache_set_sttsec.h"
#include "cache_set_sec.h"
#include "cache_set_bitcount.h"
#include "cache_base.h"
#include <iostream>
//#include "log.h"
//#include "simulator.h"
//#include "config.h"
//#include "config.hpp"
CacheBase::ReplacementPolicy CacheSet::m_policy = CacheBase::LRU;

CacheSet::CacheSet(CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize):
      m_associativity(associativity), m_blocksize(blocksize)
{
   m_cache_block_info_array = new CacheBlockInfo*[m_associativity];
  // m_policy = CacheBase::LRU ;
   for (UInt32 i = 0; i < m_associativity; i++)
   {
      m_cache_block_info_array[i] = CacheBlockInfo::create(cache_type);
   }

      m_blocks = new char[m_associativity * m_blocksize];
      memset(m_blocks, 0x00, m_associativity * m_blocksize);

}

CacheSet::~CacheSet()
{
   for (UInt32 i = 0; i < m_associativity; i++)
      delete m_cache_block_info_array[i];
   delete [] m_cache_block_info_array;
   delete [] m_blocks;
}

void
CacheSet::read_line(UInt32 line_index, UInt32 offset, Byte *out_buff, UInt32 bytes, bool update_replacement)
{
   assert(offset + bytes <= m_blocksize);
   //assert((out_buff == NULL) == (bytes == 0));

   if (out_buff != NULL && m_blocks != NULL)
      memcpy((void*) out_buff, &m_blocks[line_index * m_blocksize + offset], bytes);

   if (update_replacement)
      updateReplacementIndex(line_index);
}

void
CacheSet::write_line(UInt32 line_index, UInt32 offset, Byte *in_buff, UInt32 bytes, bool update_replacement)
{
   assert(offset + bytes <= m_blocksize);
   //assert((in_buff == NULL) == (bytes == 0));

   if (in_buff != NULL && m_blocks != NULL)
      memcpy(&m_blocks[line_index * m_blocksize + offset], (void*) in_buff, bytes);

   if (update_replacement)
      updateReplacementIndex(line_index);
}

CacheBlockInfo*
CacheSet::find(IntPtr tag, UInt32* line_index)
{
   for (SInt32 index = m_associativity-1; index >= 0; index--)
   {
      if (m_cache_block_info_array[index]->getTag() == tag)
      {
         if (line_index != NULL)
            *line_index = index;
         return (m_cache_block_info_array[index]);
      }
   }
   return NULL;
}

bool
CacheSet::invalidate(IntPtr& tag)
{
   for (SInt32 index = m_associativity-1; index >= 0; index--)
   {
      if (m_cache_block_info_array[index]->getTag() == tag)
      {
         m_cache_block_info_array[index]->invalidate();
         return true;
      }
   }
   return false;
}

bool CacheSet::isFull(){
	//std::cout<<"hhh"<<std::endl;
	for (UInt32 i = 0; i < m_associativity; i++)
	{
      if (!m_cache_block_info_array[i]->isValid())
      {
         return false;
      }
   }
  // std::cout<<"hhh2"<<std::endl;
   return true;
}
void
CacheSet::insert(CacheBlockInfo* cache_block_info, Byte* fill_buff, bool* eviction, CacheBlockInfo* evict_block_info, Byte* evict_buff, CacheCntlr *cntlr)
{
   // This replacement strategy does not take into account the fact that
   // cache blocks can be voluntarily flushed or invalidated due to another write request
   accessing_tag = cache_block_info->getTag();
   accessing_para = 0;
   if(m_policy == CacheBase::BC) accessing_para = bits_count(fill_buff , 64);
   const UInt32 index = getReplacementIndex(cntlr);
   //std::cout<<"wtf 99 " << index << std::endl;;
   assert(index < m_associativity);

   assert(eviction != NULL);
   
   if (m_cache_block_info_array[index]->isValid())
   {
      *eviction = true;
      // FIXME: This is a hack. I dont know if this is the best way to do
      evict_block_info->clone(m_cache_block_info_array[index]);
      if (evict_buff != NULL && m_blocks != NULL)
         memcpy((void*) evict_buff, &m_blocks[index * m_blocksize], m_blocksize);
   }
   else
   {
      *eviction = false;
   }

   // FIXME: This is a hack. I dont know if this is the best way to do
   m_cache_block_info_array[index]->clone(cache_block_info);

   if (fill_buff != NULL && m_blocks != NULL)
      memcpy(&m_blocks[index * m_blocksize], (void*) fill_buff, m_blocksize);
      m_cache_block_info_array[index]->setreplacementInfo(accessing_para);
}

void
CacheSet::insertwIndex(CacheBlockInfo* cache_block_info, Byte* fill_buff, bool* eviction, CacheBlockInfo* evict_block_info, Byte* evict_buff, int evicted_index, CacheCntlr *cntlr)
{
   // This replacement strategy does not take into account the fact that
   // cache blocks can be voluntarily flushed or invalidated due to another write request
   accessing_tag = cache_block_info->getTag();
  // std::cout<<"wtf gong " << evicted_index << std::endl;
    UInt32 index = 0;
    if(evicted_index == -1)
		index = getReplacementIndex(cntlr);
	else {
		index = (UInt32)evicted_index;
		updateReplacementIndex(index);
	}
   //std::cout<<"wtf 99 " << index << std::endl;
   assert(index < m_associativity);

   assert(eviction != NULL);
   
   if (m_cache_block_info_array[index]->isValid())
   {
      *eviction = true;
      // FIXME: This is a hack. I dont know if this is the best way to do
      evict_block_info->clone(m_cache_block_info_array[index]);
      if (evict_buff != NULL && m_blocks != NULL)
         memcpy((void*) evict_buff, &m_blocks[index * m_blocksize], m_blocksize);
   }
   else
   {
      *eviction = false;
   }

   // FIXME: This is a hack. I dont know if this is the best way to do
   m_cache_block_info_array[index]->clone(cache_block_info);

   if (fill_buff != NULL && m_blocks != NULL)
      memcpy(&m_blocks[index * m_blocksize], (void*) fill_buff, m_blocksize);
      
   //std::cout<<*eviction<<" wtf done insert " << index << std::endl;
}

void
CacheSet::insert_Para(CacheBlockInfo* cache_block_info, Byte* fill_buff, bool* eviction, CacheBlockInfo* evict_block_info, Byte* evict_buff, int para, CacheCntlr *cntlr)
{
   // This replacement strategy does not take into account the fact that
   // cache blocks can be voluntarily flushed or invalidated due to another write request
   accessing_para = para;
  // std::cout<<"wtf gong " << evicted_index << std::endl;
    UInt32 index = 0;

	index = getReplacementIndex(cntlr);
	
   //std::cout<<"wtf 99 " << index << std::endl;
   assert(index < m_associativity);
   assert(eviction != NULL);
   
   if (m_cache_block_info_array[index]->isValid())
   {
      *eviction = true;
      // FIXME: This is a hack. I dont know if this is the best way to do
      evict_block_info->clone(m_cache_block_info_array[index]);
      if (evict_buff != NULL && m_blocks != NULL)
         memcpy((void*) evict_buff, &m_blocks[index * m_blocksize], m_blocksize);
   }
   else
   {
      *eviction = false;
   }

   // FIXME: This is a hack. I dont know if this is the best way to do
   m_cache_block_info_array[index]->clone(cache_block_info);

   if (fill_buff != NULL && m_blocks != NULL)
      memcpy(&m_blocks[index * m_blocksize], (void*) fill_buff, m_blocksize);
      
   //std::cout<<*eviction<<" wtf done insert " << index << std::endl;
}


char*
CacheSet::getDataPtr(UInt32 line_index, UInt32 offset)
{
   return &m_blocks[line_index * m_blocksize + offset];
}

CacheSet*
CacheSet::createCacheSet(String cfgname, core_id_t core_id,
      String replacement_policy,
      CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize, UInt32 sectorsize, CacheSetInfo* set_info)
{
   
   CacheBase::ReplacementPolicy policy = parsePolicyType(replacement_policy);
   CacheSet::m_policy = policy;
   switch(policy)
   {  
	  case CacheBase::BC:	 
		 return new CacheSetBC(cache_type, associativity, blocksize, dynamic_cast<CacheSetInfoBC*>(set_info), getNumQBSAttempts(policy, cfgname, core_id));
       case CacheBase::SECTOR:	 
		 return new CacheSetSEC(cache_type, associativity, blocksize, sectorsize, dynamic_cast<CacheSetInfoSEC*>(set_info), getNumQBSAttempts(policy, cfgname, core_id));   
      case CacheBase::STTSEC:	 
		 return new CacheSetSTTSEC(cache_type, associativity, blocksize, sectorsize, dynamic_cast<CacheSetInfoSTTSEC*>(set_info), getNumQBSAttempts(policy, cfgname, core_id));
      case CacheBase::LRU:
		 return new CacheSetLRU(cache_type, associativity, blocksize, dynamic_cast<CacheSetInfoLRU*>(set_info), getNumQBSAttempts(policy, cfgname, core_id));
      case CacheBase::PLRU:
         return new CacheSetPLRU(cache_type, associativity, blocksize);
      default:
         //LOG_PRINT_ERROR("Unrecognized Cache Replacement Policy: %i",
         //      policy);
         break;
   }

   return (CacheSet*) NULL;
}

CacheSetInfo*
CacheSet::createCacheSetInfo(String name, String cfgname, core_id_t core_id, String replacement_policy, UInt32 associativity)
{
   CacheBase::ReplacementPolicy policy = parsePolicyType(replacement_policy);
   switch(policy)
   {
      case CacheBase::LRU:
         return new CacheSetInfoLRU(name, cfgname, core_id, associativity, getNumQBSAttempts(policy, cfgname, core_id));
      case CacheBase::STTSEC:
		 return new CacheSetInfoSTTSEC(name, cfgname, core_id, associativity, getNumQBSAttempts(policy, cfgname, core_id));
	  case CacheBase::SECTOR:
		 return new CacheSetInfoSEC(name, cfgname, core_id, associativity, getNumQBSAttempts(policy, cfgname, core_id));
      case CacheBase::BC:
		 return new CacheSetInfoBC(name, cfgname, core_id, associativity, getNumQBSAttempts(policy, cfgname, core_id));
      default:
         return NULL;
   }
}

UInt8
CacheSet::getNumQBSAttempts(CacheBase::ReplacementPolicy policy, String cfgname, core_id_t core_id)
{
   switch(policy)
   {
      case CacheBase::LRU_QBS:
      case CacheBase::SRRIP_QBS:
         return 0;
      default:
         return 1;
   }
}

CacheBase::ReplacementPolicy
CacheSet::parsePolicyType(String policy)
{
   
   if (policy == "lru")
      return CacheBase::LRU;
   if (policy == "plru")
      return CacheBase::PLRU;
   if (policy == "sttsec")
      return CacheBase::STTSEC;
   if (policy == "sec")
      return CacheBase::SECTOR;
   if (policy == "bc")
      return CacheBase::BC;
      return CacheBase::LRU;
  // LOG_PRINT_ERROR("Unknown replacement policy %s", policy.c_str());
}

bool CacheSet::isValidReplacement(UInt32 index)
{
   if (m_cache_block_info_array[index]->getCState() == CacheState::SHARED_UPGRADING)
   {
      return false;
   }
   else
   {
      return true;
   }
}
UInt8 CacheSet::getReplacementInfo(UInt32 line_index){
	//std::cout<<line_index <<" cacheset" <<std::endl;
	/*if (CacheSet::m_policy == CacheBase::LRU || CacheSet::m_policy == CacheBase::STTSEC)
      return getReplacementInfo(line_index);
    else return 0;*/
    return m_cache_block_info_array[line_index]->getreplacementInfo();
}


int CacheSet::bits_count( Byte* ablock, int size){
		Byte temp;
	
		int bits = 0;
		for(int j = 0; j < size; j++){
			temp = ablock[j];
			for(int k = 0; k<8; k++)  {
				if (temp & (1<<k) ) bits++; // count every bit in a byte
				}	
		}
		
	   return (bits+7) / 8;	
	}
	

