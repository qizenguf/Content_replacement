//#include "./core/memory_subsystem/parametric_dram_directory_msi/memory_manager.h"
//#include "core_manager.h"
//#include "simulator.h"
#include "subsecond_time.h"
//#include "./config/config.hpp"
//#include "./core/memory_subsystem/parametric_dram_directory_msi/cache_atd.h"
//#include "./core/core.h"
#include "./core/memory_subsystem/cache/cache.h"
#include "./core/memory_subsystem/cache/shared_cache_block_info.h"
#include "./core/memory_subsystem/address_home_lookup.h"
//#include "./core/memory_subsystem/parametric_dram_directory_msi/shmem_msg.h"
#include "./misc/fixed_types.h"
#include "./core/memory_subsystem/cache/cache_base.h"
#include <iostream>
#include "boost/tuple/tuple.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <fstream>


using namespace std;
int bits_count( Byte* ablock, int size){
		Byte temp;
	
		int bits = 0;
		for(int j = 0; j < size; j++){
			temp = ablock[j];
			for(int k = 0; k<8; k++)  {
				if (temp & (1<<k) ) bits++; // count every bit in a byte
				}	
		}
		
	   return bits / 8;	
	}
int main(int argc, char* argv[]){
	if(argc < 3) {
		std::cout<<"need more paras" << endl; 
		return 0;
		}
	int cacheSize = atoi(argv[2]);
	
	int associativity = atoi(argv[3]);
	int num_sets = cacheSize*1024*1024/associativity/64;
	int m_cache_block_size = 64;
	int sector_size = 4;
	int length = 0; // for million
	if(argc > 4) sector_size = atoi(argv[4]);
	if(argc > 5)  length = atoi(argv[5]);
	cout<<"sec size = " <<sector_size <<endl;
	//m_master = new CacheMasterCntlr("cache", 0, 0);
    Cache * m_cache = new Cache("L3",
            "perf_model/l3_cache",
            0,
            num_sets,
            associativity,
            m_cache_block_size,
            "bc",
            Cache::SHARED_CACHE,
            Cache::HASH_MASK,
            sector_size );
   

     // m_shmem_perf_global = new ShmemPerf();
      //m_shmem_perf_totaltime = SubsecondTime::Zero();
      //m_shmem_perf_numrequests = 0;
      int m_shiftSize = 64, m_flipSize = 2 , m_sample_size = 2 , m_sample_interval = 32, m_sample_start = 0;
	  uint64_t evicted_bits = 0;
	  uint64_t optimal_bits = 0;
	  uint64_t evicted_is_optimal = 0;
	  uint64_t eviction_times = 0;
	  uint64_t evicted_nearby_bits = 0;
	  uint64_t sample_bits = 0;
	  uint64_t evicted_nearby = 0;
	  uint64_t sample_hit = 0;
	  uint64_t evicted_bits_cnt[513];
	  uint64_t optimal_bits_cnt[513];
	  uint64_t optimal_lru_bits[64];
	  for(int i =0 ; i<513; i++){
		  evicted_bits_cnt[i] = 0;
		  optimal_bits_cnt[i] = 0;
	  }
	  
	  //malloc(optimal_lru_bits, m_cache->getAssociativity());
	  for(unsigned int i =0 ; i<m_cache->getAssociativity(); i++) {
		  //String numi = itostr(i);
		 // if(i<10) numi = "0" + numi;
		  optimal_lru_bits[i] = 0;
		 // String res =  "optimal_lru_bits" + numi;
		  //registerStatsMetric(namez, core_id, res, &evicted_bits_cnt[i]);
	  }
   
   ifstream fin;
   string workload(argv[1]);
   unsigned found = workload.find_last_of("/");
   workload = workload.substr(found+1);
   found = workload.find(".");
   workload = workload.substr(0,found);
   cout<<"Reading trace from:"<<workload<<endl;
   //cout<<cacheSize<<" MB "<<asso<<" way cache"<<endl;

    fin.open(argv[1], ios::in);

   if(!fin.good()){
	   std::cout<< "Exception opening/reading/closing file\n";
		return 0;
	}
 
	uint64_t data[8] = {0};
	Byte zero[64] = {0};
	uint64_t data_8byte;
    string addr, insns_str;
    uint64_t addrValue = 0;
	int marker = 0;
	Byte evict_buf[64]= {0};
	uint64_t evict_addr = 0;
	bool eviction = false;
	uint64_t count = 0;
	CacheBlockInfo evict_block_info;
	char name[256];
	uint64_t insns; //instrucitons count 
	uint64_t hit_in_miss = 0;
	uint64_t hit_in_hit = 0;
	uint64_t miss_in_miss = 0;
	uint64_t miss_in_hit = 0;
	uint64_t wb_request = 0;
	uint64_t wb_request_old = 0;
	uint64_t optimal_nearby_bits = 0;
	uint64_t zero_diff_cnt = 0;
	uint64_t zero_input_cnt = 0;
	uint64_t zero_optimal_diff_cnt =0;
	uint64_t start = 0;
    while(!fin.eof()){
		count ++;
		fin>>insns_str;
		
	    fin>>addr;
	    if(insns_str[0]<'0' || insns_str[0]>'9') {
			fin.getline(name, 256); 
			cout<<"bad input here"<<endl;
			continue;
			
		}
		if(start == 0 ) start = stoull(insns_str, nullptr, 10);
		else if(length != 0){
			insns = stoull(insns_str, nullptr, 10);
			if( (insns - start)/1000000 > length) break;
		}
			//cout<<addr<<endl;
			//if(count >67280000) cout<<insns<<" "<<addr<<endl;
		if(addr[0]=='n'){ // write back hit
				//int bitdiff = 0;
				wb_request ++;
				addrValue = stoull(addr.substr(3),nullptr,16);
				for(int i=0;i<8;i++){
					fin>>hex>>data_8byte;
					data[i] = data_8byte;
				}
				m_cache->accessSingleLine(addrValue, Cache::STORE, (Byte*)data, 64, SubsecondTime::Zero(), false);
				//fin>>insns_str;
				//while(insns_str[0]<'0'||insns_str[0]>'9') {fin.getline(name, 256); fin>>insns_str;}
				//if(!fin.eof()) insns = stoull(insns_str, nullptr, 10);
				//insnsValue = stoull(insns, nullptr, 10);
				//fin>>addr;
				//if(addr[0]!='o') {wb_request_error++;continue;} // old line should follow new line
				//for(int i=0;i<8;i++){
				//	fin>>hex>>data_8byte;
				//}
		}else if(addr[0]=='h' || addr[0] =='m'){
				addrValue = stoull(addr.substr(3),nullptr,16);
				for(int i=0;i<8;i++){
					fin>>hex>>data_8byte;
					data[i] = data_8byte;
				}			
				if(m_cache->accessSingleLine(addrValue, Cache::STORE, (Byte*)data, 64, SubsecondTime::Zero(), true) == NULL){ // miss
					if(addr[0]=='h') miss_in_hit ++;
					else miss_in_miss++; 
					 // miss
					int optimal_t = 0, sample_t = 0, evicted_t = 0, input_t = 0;
					int optimal_index = 0;
					int sample_index = 0;
					int optimal_lru =0, sample_lru = 0;
					if(m_cache->isFull(addrValue)){ // before insertion happens, find the optimal first
					  // cout<<"aa";
					   optimal_t = m_cache->lowestDiffLine(addrValue, (Byte*)data, SubsecondTime::Zero(), optimal_index, m_shiftSize, m_flipSize);
						//std::cout<<address <<" with sample2 " << sample_t<<std::endl;
					   sample_t = m_cache->lowestDiffLinefromSample(addrValue, (Byte*)data, SubsecondTime::Zero(), sample_index, m_shiftSize, m_flipSize, m_sample_size, m_sample_interval, m_sample_start);
					  //optimal_lru_bits[optimal_index]++;
					   if(sample_index == optimal_index) sample_hit++;
					   
					   //else 
					   //sample_bits += sample_t;	 
					   optimal_lru = m_cache->getReplacementInfo(addrValue, optimal_index);
					   sample_lru = m_cache->getReplacementInfo(addrValue, sample_index);
					    
						//std::cout<<address <<" with sample4 " << sample_t<<std::endl;
					}
					//actual insertion here!!!!!!
					eviction = false;
					m_cache->insertSingleLine(addrValue, (Byte*)data, &eviction, &evict_addr, &evict_block_info, evict_buf, SubsecondTime::Zero());	
					
		 
					 //IntPtr tag, tag2;
					// UInt32 set_index, set_index2;
					//  m_cache->splitAddress(address,tag, set_index);
					//  m_cache->splitAddress(evict_address,tag2, set_index2);
					// assert(set_index == set_index2);        
					 if(eviction) {
						 eviction_times ++;
						 evicted_t =  m_cache->lineCompare((Byte*)data, evict_buf, 64, m_shiftSize, m_flipSize);
						 input_t = m_cache->lineCompare((Byte*)data, zero, 64, 64, 0);
						 if(input_t == 0) zero_input_cnt++;
						 if(evicted_t == 0) zero_diff_cnt++;
						 if(optimal_t == 0 ) zero_optimal_diff_cnt ++;
						 //std::cerr<<addrValue <<" 0=MRU: optimal_i= " << optimal_lru <<" w "<< optimal_t<<" ; but sample_i= "<<sample_lru<<" w "<<sample_t <<" lru "<<evicted_t<<std::endl;
						 if(optimal_t == evicted_t) evicted_is_optimal++;
						 evicted_bits_cnt[evicted_t]++;
					     optimal_bits_cnt[optimal_t]++;
					     if(bits_count((Byte *)data, 64) /8 == bits_count((Byte *)evict_buf, 64) /8) {
							evicted_nearby++;
							evicted_nearby_bits += evicted_t;
							optimal_nearby_bits += optimal_t;//??
						}
						optimal_bits += optimal_t;
						evicted_bits += evicted_t;
						sample_bits += sample_t;	 
						if(optimal_lru < m_cache->getAssociativity())
								optimal_lru_bits[optimal_lru]++; 
					 }
					 
					 //std::cout<<"effected here: optimal = "<< optimal_t<<" and evicted = " << evicted_t<<std::endl;
			}else{
				if(addr[0]=='h') hit_in_hit ++;
				else hit_in_miss ++; 
			}		
		}else if(addr[0] =='o') {
			wb_request_old++;
			for(int i=0;i<8;i++){
					fin>>hex>>data_8byte;
				}
		}
		if(count % 1000000 == 0) std::cout <<"count "<< count <<std::endl; 
	}
	
	cout<<"done"<<endl;
	ofstream fout;
	char foo[256];
    //int period = 0;
    sprintf(foo,"bc_%s_%dM_%dwayL%dm.txt", workload.c_str(), cacheSize, associativity, length);
    fout.open(foo, ios::out);
    fout<< "total times "<<eviction_times <<std::endl;
    if(eviction_times == 0) eviction_times = 1;
    fout<<"evicted_bits "<< evicted_bits << " "<< evicted_bits/eviction_times <<std::endl;
	fout<<"optimal_bits "<< optimal_bits << " "<< optimal_bits/eviction_times<<std::endl;
	fout<<"sample_bits "<< sample_bits << " "<< sample_bits/eviction_times<< endl;
	fout<<"sample_hit "<< sample_hit <<std::endl;
	fout<<"evicted_is_optimal "<< evicted_is_optimal <<std::endl;
	
	
	if(evicted_nearby == 0) evicted_nearby = 1;
	fout<<"evicted_nearby_bits "<<evicted_nearby_bits << " "<<evicted_nearby_bits/evicted_nearby<<std::endl;
	fout<<"optimal_nearby_bits "<<optimal_nearby_bits << " "<<optimal_nearby_bits/evicted_nearby<<std::endl;
	//fout<< sample_bits <<std::endl;
	
	fout<< "nearby_times "<<evicted_nearby <<std::endl;
	fout<<"hit_in_miss "<< hit_in_miss <<endl;
	fout<<"hit_in_hit "<< hit_in_hit <<endl;
	fout<<"miss_in_miss "<< miss_in_miss <<endl;
	fout<<"miss_in_hit "<< miss_in_hit <<endl;
	
	fout<<"hit rate = "<< (double)(hit_in_miss + hit_in_hit)/ (hit_in_miss + hit_in_hit+ miss_in_miss + miss_in_hit) << endl;
	
	fout<<"zero input " <<zero_input_cnt << endl;
	fout<<"zero diff " <<zero_diff_cnt <<endl;
	fout<<"zero optimal "<<zero_optimal_diff_cnt <<endl;
	fout<<"wb times " <<wb_request <<" old: " << wb_request_old<<endl;
	for(unsigned int i =0 ; i<m_cache->getAssociativity(); i++) {
		  //String numi = itostr(i);
		 // if(i<10) numi = "0" + numi;
		  fout<<i<<" "<<optimal_lru_bits[i] <<endl;
		 // String res =  "optimal_lru_bits" + numi;
		  //registerStatsMetric(namez, core_id, res, &evicted_bits_cnt[i]);
	  }
	
	  
	fout.close();
	  
	  return 0;	

}
