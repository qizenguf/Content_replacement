
#include "cache.h"
//#include "log.h"

// Cache class
// constructors/destructors
Cache::Cache(
   String name,
   String cfgname,
   core_id_t core_id,
   UInt32 num_sets,
   UInt32 associativity,
   UInt32 cache_block_size,
   String replacement_policy,
   cache_t cache_type,
   hash_t hash,
   UInt32 cache_sector_size,
   //FaultInjector *fault_injector,
   AddressHomeLookup *ahl):
   CacheBase(name, num_sets, associativity, cache_block_size, hash, cache_sector_size, ahl),
   m_enabled(false),
   m_num_accesses(0),
   m_num_hits(0),
   m_cache_type(cache_type)
   //m_fault_injector(fault_injector)
{
   m_set_info = CacheSet::createCacheSetInfo(name, cfgname, core_id, replacement_policy, m_associativity);
   m_sets = new CacheSet*[m_num_sets];
   for (UInt32 i = 0; i < m_num_sets; i++)
   {
      m_sets[i] = CacheSet::createCacheSet(cfgname, core_id, replacement_policy, m_cache_type, m_associativity, m_blocksize, cache_sector_size, m_set_info);
   }

   #ifdef ENABLE_SET_USAGE_HIST
   m_set_usage_hist = new UInt64[m_num_sets];
   for (UInt32 i = 0; i < m_num_sets; i++)
      m_set_usage_hist[i] = 0;
   #endif
}

Cache::~Cache()
{
   #ifdef ENABLE_SET_USAGE_HIST
   printf("Cache %s set usage:", m_name.c_str());
   for (SInt32 i = 0; i < (SInt32) m_num_sets; i++)
      printf(" %" PRId64, m_set_usage_hist[i]);
   printf("\n");
   delete [] m_set_usage_hist;
   #endif

   if (m_set_info)
      delete m_set_info;

   for (SInt32 i = 0; i < (SInt32) m_num_sets; i++)
      delete m_sets[i];
   delete [] m_sets;
}

/*Lock&
Cache::getSetLock(IntPtr addr)
{
   IntPtr tag;
   UInt32 set_index;

   splitAddress(addr, tag, set_index);
   assert(set_index < m_num_sets);

   return m_sets[set_index]->getLock();
}*/

bool
Cache::invalidateSingleLine(IntPtr addr)
{
   IntPtr tag;
   UInt32 set_index;

   splitAddress(addr, tag, set_index);
   assert(set_index < m_num_sets);

   return m_sets[set_index]->invalidate(tag);
}

int Cache::getSetIndex(IntPtr addr){
	IntPtr tag;
	UInt32 set_index;

	splitAddress(addr, tag, set_index);
	return set_index;
}
CacheBlockInfo*
Cache::accessSingleLine(IntPtr addr, access_t access_type,
      Byte* buff, UInt32 bytes, SubsecondTime now, bool update_replacement)
{
   //assert((buff == NULL) == (bytes == 0));

   IntPtr tag;
   UInt32 set_index;
   UInt32 line_index = -1;
   UInt32 block_offset;

   splitAddress(addr, tag, set_index, block_offset);

   CacheSet* set = m_sets[set_index];
   CacheBlockInfo* cache_block_info = set->find(tag, &line_index);

   if (cache_block_info == NULL)
      return NULL;

   if (access_type == LOAD)
   {
      // NOTE: assumes error occurs in memory. If we want to model bus errors, insert the error into buff instead
     // if (m_fault_injector)
     //    m_fault_injector->preRead(addr, set_index * m_associativity + line_index, bytes, (Byte*)m_sets[set_index]->getDataPtr(line_index, block_offset), now);

      set->read_line(line_index, block_offset, buff, bytes, update_replacement);
   }
   else
   {
      set->write_line(line_index, block_offset, buff, bytes, update_replacement);

      // NOTE: assumes error occurs in memory. If we want to model bus errors, insert the error into buff instead
    //  if (m_fault_injector)
     //    m_fault_injector->postWrite(addr, set_index * m_associativity + line_index, bytes, (Byte*)m_sets[set_index]->getDataPtr(line_index, block_offset), now);
   }

   return cache_block_info;
}

int Cache::lowestDiffLine(IntPtr addr, Byte* fill_buff,
      SubsecondTime now, int &select_index, int shiftSize, int flipSize, int range, int weight, CacheCntlr *cntlr)
      {
			//if(range == 0 ) range = m_associativity;
			IntPtr tag;
			UInt32 set_index;
			splitAddress(addr, tag, set_index);
			//int evictDiffBits = lineCompare(fill_buff, evict_buff, 64, shifSize, flipSize);
			CacheSet * set = m_sets[set_index];
			
			int lowestDiffBits = 512;
			
			int temp = 0;
			Byte read_buff[64] = {};
			for(UInt32 i = 0 ; i < m_associativity; i++)
			{	
				int loc = set-> getReplacementInfo(i);
				if(loc < range ) continue;
				//std::cout<<addr <<" with full " << i<<std::endl;
				set->read_line(i, 0, read_buff, 64, false);
				int t = lineCompare(read_buff, fill_buff, 64, shiftSize, flipSize);
				//std::cout<< i <<" diff "<<t<<" bits "<<(uint8_t)read_buff[0]<<std::endl;
				if(t < lowestDiffBits) { 
					//std::cout<< i <<" diff "<<t<<std::endl;
					lowestDiffBits = t; temp = i;
					}
			}
			select_index  = temp;
			//delete read_buff;
			return lowestDiffBits;  
		  
	  }
int Cache::lowestDiffLineLoc(IntPtr addr, Byte* fill_buff,
      SubsecondTime now, int &select_index, int shiftSize, int flipSize, int range, int loc_weight, CacheCntlr *cntlr)
      {
			//if(range == 0 ) range = m_associativity;
			IntPtr tag;
			UInt32 set_index;
			splitAddress(addr, tag, set_index);
			//int evictDiffBits = lineCompare(fill_buff, evict_buff, 64, shifSize, flipSize);
			CacheSet * set = m_sets[set_index];
			
			int lowestDiffBits = 512;
			int temp = 0;
			int retVal = 0;
			Byte read_buff[64] = {};
			for(UInt32 i = 0 ; i < m_associativity; i++)
			{	
				int loc = set-> getReplacementInfo(i);
				if(loc < range ) continue;
				//std::cout<<addr <<" with full " << i<<std::endl;
				set->read_line(i, 0, read_buff, 64, false);
				int t = lineCompare(read_buff, fill_buff, 64, shiftSize, flipSize) - loc *loc_weight;
				//std::cout<< i <<" diff "<<t<<" bits "<<(uint8_t)read_buff[0]<<std::endl;
				if(t < lowestDiffBits) { 
					//std::cout<< i <<" diff "<<t<<std::endl;
					lowestDiffBits = t; temp = i;
					retVal  = t + loc*loc_weight;
					}
			}
			select_index  = temp;
			//delete read_buff;
			return retVal;  
		  
	  }
int Cache::lowestDiffLineEncoding(IntPtr addr, Byte* fill_buff,
      SubsecondTime now, int &select_index, int shiftSize, int flipSize, int range, int loc_weight, CacheCntlr *cntlr)
      {
			//if(range == 0 ) range = m_associativity;
			IntPtr tag;
			UInt32 set_index;
			splitAddress(addr, tag, set_index);
			//int evictDiffBits = lineCompare(fill_buff, evict_buff, 64, shifSize, flipSize);
			CacheSet * set = m_sets[set_index];
			
			int lowestDiffBits = 512;
			int temp = 0;
			int retVal = 0;
			Byte read_buff[64] = {};
			for(UInt32 i = 0 ; i < m_associativity; i++)
			{	
				int loc = set-> getReplacementInfo(i);
				if(loc < range ) continue;
				//std::cout<<addr <<" with full " << i<<std::endl;
				set->read_line(i, 0, read_buff, 64, false);
				int t = encodingCompare(read_buff, fill_buff, 64, shiftSize, flipSize)*64 - loc*loc_weight;
				//std::cout<< i <<" diff "<<t<<" bits "<<(uint8_t)read_buff[0]<<std::endl;
				if(t < lowestDiffBits) { 
					//std::cout<< i <<" diff "<<t<<std::endl;
					lowestDiffBits = t; temp = i;
					}
			}
			select_index  = temp;
			set->read_line(temp, 0, read_buff, 64, false);
			retVal = lineCompare(read_buff, fill_buff, 64, shiftSize, flipSize);
			//codediff = encodingCompare(read_buff, fill_buff, 64, shiftSize, flipSize);
			//delete read_buff;
			return retVal;  
		  
	  }

int Cache::lowestDiffLineEncoding_PLRU(IntPtr addr, Byte* fill_buff,
      SubsecondTime now, int &select_index, int shiftSize, int flipSize, int range, int loc_weight, CacheCntlr *cntlr)
      {
			//if(range == 0 ) range = m_associativity;
			IntPtr tag;
			UInt32 set_index;
			splitAddress(addr, tag, set_index);
			//int evictDiffBits = lineCompare(fill_buff, evict_buff, 64, shifSize, flipSize);
			CacheSet * set = m_sets[set_index];
			
			int lowestDiffBits = 512;
			int temp = 0;
			int retVal = 0;
			Byte read_buff[64] = {};
			UInt32 victim = set-> getReplacementIndex(cntlr);
			for(UInt32 i = victim/range*range ; i < victim/range*range + range; i++)
			{	
				set->read_line(i, 0, read_buff, 64, false);
				int t = encodingCompare(read_buff, fill_buff, 64, shiftSize, flipSize);
				//std::cout<< i <<" diff "<<t<<" bits "<<(uint8_t)read_buff[0]<<std::endl;
				if(t < lowestDiffBits) { 
					//std::cout<< i <<" diff "<<t<<std::endl;
					lowestDiffBits = t; temp = i;
					}
			}
			select_index  = temp;
			set->read_line(temp, 0, read_buff, 64, false);
			retVal = lineCompare(read_buff, fill_buff, 64, shiftSize, flipSize);
			//codediff = encodingCompare(read_buff, fill_buff, 64, shiftSize, flipSize);
			//delete read_buff;
			return retVal;  
		  
}

int Cache::lowestDiffLineEncoding_NLRU(IntPtr addr, Byte* fill_buff,
      SubsecondTime now, int &select_index, int shiftSize, int flipSize, int range, int loc_weight, CacheCntlr *cntlr)
      {
			//if(range == 0 ) range = m_associativity;
			IntPtr tag;
			UInt32 set_index;
			splitAddress(addr, tag, set_index);
			//int evictDiffBits = lineCompare(fill_buff, evict_buff, 64, shifSize, flipSize);
			CacheSet * set = m_sets[set_index];
			
			int lowestDiffBits = 512;
			int temp_index = 0;
			int temp_recency = 0;
			int retVal = 0;
			Byte read_buff[64] = {};
			
			if(loc_weight >= 512) {
				select_index = set-> getReplacementIndex(cntlr);
			}
			else{
				for(UInt32 i = 0 ; i < m_associativity; i++)
				{	
					int recency = set-> getReplacementInfo(i);
					if(recency > 0 ){
						set->read_line(i, 0, read_buff, 64, false);
						int t = 64 * encodingCompare(read_buff, fill_buff, 64, shiftSize, flipSize) + recency * loc_weight;
					//std::cout<< i <<" diff "<<t<<" bits "<<(uint8_t)read_buff[0]<<std::endl;
						if(t < lowestDiffBits || (t == lowestDiffBits && recency < temp_recency)) { 
						//std::cout<< i <<" diff "<<t<<std::endl;
						lowestDiffBits = t; temp_index = i;
						temp_recency = recency;
						}
					}
				}
			}
			select_index  = temp_index;
			set->read_line(temp_index, 0, read_buff, 64, false);
			retVal = lineCompare(read_buff, fill_buff, 64, shiftSize, flipSize);
			//codediff = encodingCompare(read_buff, fill_buff, 64, shiftSize, flipSize);
			//delete read_buff;
			return retVal;  
		  
}
int Cache::lowestDiffLinefromSample(IntPtr addr, Byte* fill_buff,
      SubsecondTime now, int &select_index, int shiftSize, int flipSize, int sample_size, int sample_interval, int sample_start, CacheCntlr *cntlr)
      {
			IntPtr tag;
			UInt32 set_index;
			splitAddress(addr, tag, set_index);
			//int evictDiffBits = lineCompare(fill_buff, evict_buff, 64, shifSize, flipSize);
			CacheSet * set = m_sets[set_index];
			Byte read_buff[64] = {};
			int lowestDiffBits = 512;
			int temp = 0;
			for(UInt32 i =0 ; i < m_associativity; i++)
			{
				//std::cout<<addr <<" with sample " << i<<std::endl;
				set->read_line(i, 0, read_buff, 64, false);
				int t = sampleCompare(read_buff, fill_buff, 64, shiftSize, flipSize, sample_size, sample_interval, sample_start);
				if(t < lowestDiffBits) {lowestDiffBits = t; temp = i;}
			}
		set->read_line(select_index, 0, read_buff, 64, false);
		select_index = temp;
		int res = lineCompare(read_buff, fill_buff, 64, shiftSize, flipSize); 
		//delete read_buff;
		return res; 
	  }
UInt8 Cache::getReplacementInfo(IntPtr addr, UInt32 index){
	IntPtr tag;
	UInt32 set_index;
	splitAddress(addr, tag, set_index);
	//std::cout<<set_index <<" cache " <<index <<std::endl;
	return m_sets[set_index]->getReplacementInfo(index);
}
int Cache::sampleCompare(Byte* ablock, Byte* bblock, int size, int shiftSize, int flipSize, int sample_size, int sample_interval, int sample_start){
	if(flipSize ==0 ) flipSize = size+1;
	int minimal_bits = 512;
	for (int i =0; i<size; i += shiftSize){
		Byte temp;
		int total_bits = 0;
		int flipbits = 0;
		for(int j = sample_start; j<size; j++){
				temp = ablock[j] ^ bblock[(j+i)%size];
				for(int k = 0; k<8; k++)  {
					if (temp & (1<<k) ) flipbits++; // count every bit in a byte
					}
					if(((j+1)%sample_interval)%flipSize == 0 ){ // to decide if flip
						if(flipbits > flipSize*4) flipbits = flipSize*8 - flipbits;
						total_bits += flipbits;
						flipbits = 0;
					}
					if(((j+1)%sample_interval)%sample_size == 0) j += sample_interval-sample_size; // flipsize should be smaller than sample_size
		}
		total_bits += flipbits;  // if no flip..
		if(total_bits < minimal_bits) minimal_bits = total_bits;		
	}
	return minimal_bits;	
}

int Cache::encodingCompare(Byte* ablock, Byte* bblock, int size, int shiftSize, int flipSize){
	if(flipSize == 0 ) flipSize = size+1;
	int minimal_diff = size/8;
	for (int i =0; i<size; i += shiftSize) {
		//Byte temp;
		int total_diff = 0;
		int abits = 0, bbits = 0;
		for(int j = 0; j<size; j++){
				for(int k = 0; k<8; k++)  {
					if (ablock[j] & (1<<k) ) abits++; // count every bit in a byte
					if (bblock[j] & (1<<k) ) bbits++;
					}
				if( (j+1)%8 == 0 ) {
					if(abits > 43 or abits < 21) abits = 1;
					else abits = 0;
					if(bbits > 43 or bbits < 21) bbits = 1;
					else bbits = 0;
					if(abits != bbits) total_diff++;
					abits = 0;
					bbits = 0;
				}					
		}
		//total_bits += flipbits;  // if no flip..
		if(total_diff < minimal_diff) minimal_diff = total_diff;		
	}
	assert(minimal_diff <= size/8);
	return minimal_diff;	
}
int Cache::lineCompare( Byte* ablock, Byte* bblock, int size, int shiftSize, int flipSize){
	if( flipSize == 0 ) flipSize = size+1; // no flip
	int minimal_bits = 512;
	for (int i =0; i<size; i += shiftSize){
		Byte temp;
		int total_bits = 0;
		int flipbits = 0;
		for(int j = 0; j < size; j++){
			temp = ablock[j] ^ bblock[(j+i)%size];
			for(int k = 0; k<8; k++)  {
				if (temp & (1<<k) ) flipbits++; // count every bit in a byte
				}
				if((j+1)% flipSize == 0 ){ // to decide if flip
					if(flipbits > flipSize*4) flipbits = flipSize*8 - flipbits;
					total_bits += flipbits;
					flipbits = 0;
				}		
		}
		total_bits += flipbits ; 
		if(total_bits < minimal_bits) minimal_bits=total_bits;		
	}
	return minimal_bits;	
}


bool Cache::isFull(IntPtr addr){
	IntPtr tag;
    UInt32 set_index;
    splitAddress(addr, tag, set_index);
    //std::cout<<"is_full"<<std::endl;
	return m_sets[set_index]->isFull();
}
void
Cache::insertSingleLine(IntPtr addr, Byte* fill_buff,
      bool* eviction, IntPtr* evict_addr,
      CacheBlockInfo* evict_block_info, Byte* evict_buff,
      SubsecondTime now, CacheCntlr *cntlr)
{
   IntPtr tag;
   UInt32 set_index;
   splitAddress(addr, tag, set_index);

   CacheBlockInfo* cache_block_info = CacheBlockInfo::create(m_cache_type);
   cache_block_info->setTag(tag);

   m_sets[set_index]->insert(cache_block_info, fill_buff,
         eviction, evict_block_info, evict_buff, cntlr);
   *evict_addr = tagToAddress(evict_block_info->getTag());

  // if (m_fault_injector) {
      // NOTE: no callback is generated for read of evicted data
   //   UInt32 line_index = -1;
   //   __attribute__((unused)) CacheBlockInfo* res = m_sets[set_index]->find(tag, &line_index);
   //   LOG_ASSERT_ERROR(res != NULL, "Inserted line no longer there?");

   //   m_fault_injector->postWrite(addr, set_index * m_associativity + line_index, m_sets[set_index]->getBlockSize(), (Byte*)m_sets[set_index]->getDataPtr(line_index, 0), now);
  // }

   #ifdef ENABLE_SET_USAGE_HIST
   ++m_set_usage_hist[set_index];
   #endif

   delete cache_block_info;
}
void
Cache::insertSingleLinewIndex(IntPtr addr, Byte* fill_buff,
      bool* eviction, IntPtr* evict_addr,
      CacheBlockInfo* evict_block_info, Byte* evict_buff,
      SubsecondTime now, int evicted_index, CacheCntlr *cntlr){
		IntPtr tag;
		UInt32 set_index;
		splitAddress(addr, tag, set_index);

		CacheBlockInfo* cache_block_info = CacheBlockInfo::create(m_cache_type);
		cache_block_info->setTag(tag);
		//std::cout<<"wtf 99 " << evicted_index << std::endl;
		   m_sets[set_index]->insertwIndex(cache_block_info, fill_buff,
				 eviction, evict_block_info, evict_buff, evicted_index, cntlr);
		   *evict_addr = tagToAddress(evict_block_info->getTag());
		 //std::cout<<"wtf 99 " << evicted_index << std::endl;;  
		   #ifdef ENABLE_SET_USAGE_HIST
		   ++m_set_usage_hist[set_index];
		   #endif

		   delete cache_block_info;
		}

// Single line cache access at addr
CacheBlockInfo*
Cache::peekSingleLine(IntPtr addr)
{
   IntPtr tag;
   UInt32 set_index;
   splitAddress(addr, tag, set_index);

   return m_sets[set_index]->find(tag);
}

void
Cache::updateCounters(bool cache_hit)
{
   if (m_enabled)
   {
      m_num_accesses ++;
      if (cache_hit)
         m_num_hits ++;
   }
}

void
Cache::updateHits(UInt64 hits)
{
   if (m_enabled)
   {
      m_num_accesses += hits;
      m_num_hits += hits;
   }
}
