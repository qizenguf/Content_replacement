#include "cache_set_plru.h"
//#include "log.h"

// Tree LRU for 4 and 8 way caches

CacheSetPLRU::CacheSetPLRU(
      CacheBase::cache_t cache_type,
      UInt32 associativity, UInt32 blocksize) :
   CacheSet(cache_type, associativity, blocksize)
{
  // LOG_ASSERT_ERROR(associativity == 4 || associativity == 8 || associativity == 16,
  //    "PLRU not implemted for associativity %d (only 4, 8, 16)", associativity);
   for(int i = 0; i < 32; ++i)
      b[i] = 0;
}

CacheSetPLRU::~CacheSetPLRU()
{
}

int
CacheSetPLRU::possibleIndexCheck( int range)
{		
		for(int i = 0; i<32; i++) m_cache_block_info_array[i]->setreplacementInfo(0);
		int retValue = -1;
	if(range == 8){
		int plru[16] = {0};
		plru[0] = b[0];
		plru[1] = b[1];
		plru[2] = b[16];
		plru[3] = b[2];
		plru[4] = b[9];
		plru[5] = b[17];
		plru[6] = b[24];
		for(int i = 7; i<15; i++){
			int temp = 0;
			int p = i;
			int level = 0;
			while( p != 0){
				if(p%2==0 && plru[p/2-1] ==1) temp = temp+ (1<<level);
				else if( p%2 == 1 && plru[p/2] == 0) temp = temp + (1<<level);
				level ++;
				p = (p-1)/2;
			}
			if(temp == 7 ) // temp samller, more mru
				plru[i] = 1;
			else if(temp == 6) 
				plru[i] = 2;
			else if(temp > 3)	
				plru[i] = 3;
			else 
				plru[i] = 4;
		}
		
					 if (b[3] == 0)
					 {
						if (b[4] == 0) retValue= 0;
						else           retValue= 1;  // b2==1
					 }
					 else
					 {                            // b1==1
						if (b[5] == 0) retValue = 2;
						else           retValue = 3;  // b3==1
					 }
				     m_cache_block_info_array[retValue]->setreplacementInfo(plru[7]);                         // b0==1
					 if (b[6] == 0)
					 {
						if (b[7] == 0) retValue = 4;
						else           retValue = 5;  // b5==1
					 }
					 else
					 {                            // b4==1
						if (b[8] == 0) retValue = 6;
						else           retValue = 7;  // b6==1
					}
					m_cache_block_info_array[retValue]->setreplacementInfo(plru[8]);   
				 
					 if (b[10] == 0)
					 {
						if (b[11] == 0) retValue= 8;
						else           retValue= 9;  // b2==1
					 }
					 else
					 {                            // b1==1
						if (b[12] == 0) retValue = 10;
						else           retValue = 11;  // b3==1
					 }
						m_cache_block_info_array[retValue]->setreplacementInfo(plru[9]);   // b0==1
					 if (b[13] == 0)
					 {
						if (b[14] == 0) retValue = 12;
						else           retValue = 13;  // b5==1
					 }
					 else
					 {                            // b4==1
						if (b[15] == 0) retValue = 14;
						else           retValue = 15;  // b6==1
					 }
					m_cache_block_info_array[retValue]->setreplacementInfo(plru[10]);   
				  
					 if (b[18] == 0)
					 {
						if (b[19] == 0) retValue= 16;
						else            retValue= 17;  // b2==1
					 }
					 else
					 {                            // b1==1
						if (b[20] == 0) retValue = 18;
						else           retValue = 19;  // b3==1
					 }
				    m_cache_block_info_array[retValue]->setreplacementInfo(plru[11]);                               // b0==1
					 if (b[21] == 0)
					 {
						if (b[22] == 0) retValue = 20;
						else           retValue = 21;  // b5==1
					 }
					 else
					 {                            // b4==1
						if (b[23] == 0) retValue = 22;
						else           retValue = 23;  // b6==1
					 }
					m_cache_block_info_array[retValue]->setreplacementInfo(plru[12]);    
				 
					 if (b[25] == 0)
					 {
						if (b[26] == 0) retValue= 24;
						else           retValue= 25;  // b2==1
					 }
					 else
					 {                            // b1==1
						if (b[27] == 0) retValue = 26;
						else           retValue = 27;  // b3==1
					 }
					m_cache_block_info_array[retValue]->setreplacementInfo(plru[13]);    
				                               // b0==1
					 if (b[28] == 0)
					 {
						if (b[29] == 0) retValue = 28;
						else           retValue = 29;  // b5==1
					 }
					 else
					 {                            // b4==1
						if (b[30] == 0) retValue = 30;
						else           retValue = 31;  // b6==1
					 }	
					m_cache_block_info_array[retValue]->setreplacementInfo(plru[14]);  
			} 
			if(range == 4){
				int plru[8] = {0};
				plru[0] = b[0];
				plru[1] = b[1];
				plru[2] = b[16];

				for(int i = 3; i<7; i++){
					int temp = 0;
					int p = i;
					int level = 0;
					while( p != 0){
						if(p%2==0 && plru[p/2-1] ==1) temp = temp+ (1<<level);
						else if( p%2 == 1 && plru[p/2] == 0) temp = temp + (1<<level);
						level ++;
						p = (p-1)/2;
					}
					if(temp == 3 )
						plru[i] = 1;
					else if(temp == 3) 
						plru[i] = 3;
					else	
					plru[i] = 4;
				}
		
			  
				  if (b[2] == 0)
				  {
					 if (b[3] == 0)
					 {
						if (b[4] == 0) retValue= 0;
						else           retValue= 1;  // b2==1
					 }
					 else
					 {                            // b1==1
						if (b[5] == 0) retValue = 2;
						else           retValue = 3;  // b3==1
					 }
				  }
				  else
				  {                               // b0==1
					 if (b[6] == 0)
					 {
						if (b[7] == 0) retValue = 4;
						else           retValue = 5;  // b5==1
					 }
					 else
					 {                            // b4==1
						if (b[8] == 0) retValue = 6;
						else           retValue = 7;  // b6==1
					 }
				  }
				m_cache_block_info_array[retValue]->setreplacementInfo(plru[3]);  
				
				
				  if (b[9] == 0)
				  {
					 if (b[10] == 0)
					 {
						if (b[11] == 0) retValue= 8;
						else           retValue= 9;  // b2==1
					 }
					 else
					 {                            // b1==1
						if (b[12] == 0) retValue = 10;
						else           retValue = 11;  // b3==1
					 }
				  }
				  else
				  {                               // b0==1
					 if (b[13] == 0)
					 {
						if (b[14] == 0) retValue = 12;
						else           retValue = 13;  // b5==1
					 }
					 else
					 {                            // b4==1
						if (b[15] == 0) retValue = 14;
						else           retValue = 15;  // b6==1
					 }
				  }
				m_cache_block_info_array[retValue]->setreplacementInfo(plru[4]);  
				
				  if (b[17] == 0)
				  {
					 if (b[18] == 0)
					 {
						if (b[19] == 0) retValue= 16;
						else            retValue= 17;  // b2==1
					 }
					 else
					 {                            // b1==1
						if (b[20] == 0) retValue = 18;
						else           retValue = 19;  // b3==1
					 }
				  }
				  else
				  {                               // b0==1
					 if (b[21] == 0)
					 {
						if (b[22] == 0) retValue = 20;
						else           retValue = 21;  // b5==1
					 }
					 else
					 {                            // b4==1
						if (b[23] == 0) retValue = 22;
						else           retValue = 23;  // b6==1
					 }
				  }
				
				m_cache_block_info_array[retValue]->setreplacementInfo(plru[5]);  
				  if (b[24] == 0)
				  {
					 if (b[25] == 0)
					 {
						if (b[26] == 0) retValue= 24;
						else           retValue= 25;  // b2==1
					 }
					 else
					 {                            // b1==1
						if (b[27] == 0) retValue = 26;
						else           retValue = 27;  // b3==1
					 }
				  }
				  else
				  {                               // b0==1
					 if (b[28] == 0)
					 {
						if (b[29] == 0) retValue = 28;
						else           retValue = 29;  // b5==1
					 }
					 else
					 {                            // b4==1
						if (b[30] == 0) retValue = 30;
						else           retValue = 31;  // b6==1
					 }
				  }
			m_cache_block_info_array[retValue]->setreplacementInfo(plru[6]);  
				
			}
			return 0;
}
UInt32
CacheSetPLRU::getReplacementIndex(CacheCntlr *cntlr)
{
   // Invalidations may mess up the LRU bits

   for (UInt32 i = 0; i < m_associativity; i++)
   {
      if (!m_cache_block_info_array[i]->isValid())
      {
         updateReplacementIndex(i);
         return i;
      }
   }

   UInt32 retValue = -1;
   if(m_associativity == 1 ) return 0;
   if (m_associativity == 4)
   {
      if (b[0] == 0)
      {
         if (b[1] == 0) retValue = 0;
         else           retValue = 1;   // b1==1
      }
      else
      {
         if (b[2] == 0) retValue = 2;
         else           retValue = 3;   // b2==1
      }
   }
   else if (m_associativity == 8)
   {
      if (b[0] == 0)
      {  
         if (b[1] == 0)
         {
            if (b[2] == 0) retValue= 0;
            else           retValue= 1;  // b2==1
         }
         else
         {                            // b1==1
            if (b[3] == 0) retValue = 2;
            else           retValue = 3;  // b3==1
         }
      }
      else
      {                               // b0==1
         if (b[4] == 0)
         {
            if (b[5] == 0) retValue = 4;
            else           retValue = 5;  // b5==1
         }
         else
         {                            // b4==1
            if (b[6] == 0) retValue = 6;
            else           retValue = 7;  // b6==1
         }
      }
   }
   else if (m_associativity == 16)
   {
      if (b[0] == 0)
		  {
			  if (b[1] == 0)
			  {
				 if (b[2] == 0)
				 {
					if (b[3] == 0) retValue= 0;
					else           retValue= 1;  // b2==1
				 }
				 else
				 {                            // b1==1
					if (b[4] == 0) retValue = 2;
					else           retValue = 3;  // b3==1
				 }
			  }
			  else
			  {                               // b0==1
				 if (b[5] == 0)
				 {
					if (b[6] == 0) retValue = 4;
					else           retValue = 5;  // b5==1
				 }
				 else
				 {                            // b4==1
					if (b[7] == 0) retValue = 6;
					else           retValue = 7;  // b6==1
				 }
			  }
			}
			else{
			  if (b[8] == 0)
			  {
				 if (b[9] == 0)
				 {
					if (b[10] == 0) retValue= 8;
					else           retValue= 9;  // b2==1
				 }
				 else
				 {                            // b1==1
					if (b[11] == 0) retValue = 10;
					else           retValue = 11;  // b3==1
				 }
			  }
			  else
			  {                               // b0==1
				 if (b[12] == 0)
				 {
					if (b[13] == 0) retValue = 12;
					else           retValue = 13;  // b5==1
				 }
				 else
				 {                            // b4==1
					if (b[14] == 0) retValue = 14;
					else           retValue = 15;  // b6==1
				 }
			  }
			}
		}
	else if (m_associativity == 32)
	{
		  if( b[0] == 0 ){
			  
			  if( b[1] == 0 )
			  {
				  if (b[2] == 0)
				  {
					 if (b[3] == 0)
					 {
						if (b[4] == 0) retValue= 0;
						else           retValue= 1;  // b2==1
					 }
					 else
					 {                            // b1==1
						if (b[5] == 0) retValue = 2;
						else           retValue = 3;  // b3==1
					 }
				  }
				  else
				  {                               // b0==1
					 if (b[6] == 0)
					 {
						if (b[7] == 0) retValue = 4;
						else           retValue = 5;  // b5==1
					 }
					 else
					 {                            // b4==1
						if (b[8] == 0) retValue = 6;
						else           retValue = 7;  // b6==1
					 }
				  }
				}
				else{
				  if (b[9] == 0)
				  {
					 if (b[10] == 0)
					 {
						if (b[11] == 0) retValue= 8;
						else           retValue= 9;  // b2==1
					 }
					 else
					 {                            // b1==1
						if (b[12] == 0) retValue = 10;
						else           retValue = 11;  // b3==1
					 }
				  }
				  else
				  {                               // b0==1
					 if (b[13] == 0)
					 {
						if (b[14] == 0) retValue = 12;
						else           retValue = 13;  // b5==1
					 }
					 else
					 {                            // b4==1
						if (b[15] == 0) retValue = 14;
						else           retValue = 15;  // b6==1
					 }
				  }
				}
			}
			else{
				if( b[16] == 0 )
				{
				  if (b[17] == 0)
				  {
					 if (b[18] == 0)
					 {
						if (b[19] == 0) retValue= 16;
						else            retValue= 17;  // b2==1
					 }
					 else
					 {                            // b1==1
						if (b[20] == 0) retValue = 18;
						else           retValue = 19;  // b3==1
					 }
				  }
				  else
				  {                               // b0==1
					 if (b[21] == 0)
					 {
						if (b[22] == 0) retValue = 20;
						else           retValue = 21;  // b5==1
					 }
					 else
					 {                            // b4==1
						if (b[23] == 0) retValue = 22;
						else           retValue = 23;  // b6==1
					 }
				  }
				}
				else{
				  if (b[24] == 0)
				  {
					 if (b[25] == 0)
					 {
						if (b[26] == 0) retValue= 24;
						else           retValue= 25;  // b2==1
					 }
					 else
					 {                            // b1==1
						if (b[27] == 0) retValue = 26;
						else           retValue = 27;  // b3==1
					 }
				  }
				  else
				  {                               // b0==1
					 if (b[28] == 0)
					 {
						if (b[29] == 0) retValue = 28;
						else           retValue = 29;  // b5==1
					 }
					 else
					 {                            // b4==1
						if (b[30] == 0) retValue = 30;
						else           retValue = 31;  // b6==1
					 }
				  }
				}
			}
	}
   else
   {
     return 99;
     // LOG_PRINT_ERROR("PLRU doesn't support associativity %d", m_associativity);
   }


  // LOG_ASSERT_ERROR(isValidReplacement(retValue), "PLRU selected an invalid replacement candidate" );
   updateReplacementIndex(retValue);
   return retValue;

}

void
CacheSetPLRU::updateReplacementIndex(UInt32 accessed_index)
{
   if (m_associativity == 4)
   {
      if      (accessed_index==0) { b[0]=1;b[1]=1;     }
      else if (accessed_index==1) { b[0]=1;b[1]=0;     }
      else if (accessed_index==2) { b[0]=0;       b[2]=1;}
      else if (accessed_index==3) { b[0]=0;       b[2]=0;}
   }
   else if (m_associativity == 8)
   {
      if      (accessed_index==0) { b[0]=1;b[1]=1;b[2]=1;                            }
      else if (accessed_index==1) { b[0]=1;b[1]=1;b[2]=0;                            }
      else if (accessed_index==2) { b[0]=1;b[1]=0;       b[3]=1;                     }
      else if (accessed_index==3) { b[0]=1;b[1]=0;       b[3]=0;                     }
      else if (accessed_index==4) { b[0]=0;                     b[4]=1;b[5]=1;       }
      else if (accessed_index==5) { b[0]=0;                     b[4]=1;b[5]=0;       }
      else if (accessed_index==6) { b[0]=0;                     b[4]=0;       b[6]=1;}
      else if (accessed_index==7) { b[0]=0;                     b[4]=0;       b[6]=0;}
   }
   else if (m_associativity == 16) // added by Qi
   {
      if      (accessed_index==0) { b[0]=1;b[1]=1;b[2]=1;b[3]=1;                           }
      else if (accessed_index==1) { b[0]=1;b[1]=1;b[2]=1;b[3]=0;                            }
      else if (accessed_index==2) { b[0]=1;b[1]=1;b[2]=0;       b[4]=1;                     }
      else if (accessed_index==3) { b[0]=1;b[1]=1;b[2]=0;       b[4]=0;                     }
      else if (accessed_index==4) { b[0]=1;b[1]=1;                     b[5]=1;b[6]=1;       }
      else if (accessed_index==5) { b[0]=1;b[1]=1;                     b[5]=1;b[6]=0;       }
      else if (accessed_index==6) { b[0]=1;b[1]=1;                     b[5]=0;       b[7]=1;}
      else if (accessed_index==7) { b[0]=1;b[1]=0;                     b[5]=0;       b[7]=0;}
      else if (accessed_index==8) { b[0]=0; b[8]=1;b[9]=1;b[10]=1;                            }
      else if (accessed_index==9) { b[0]=0; b[8]=1;b[9]=1;b[10]=0;                            }
      else if (accessed_index==10) { b[0]=0; b[8]=1;b[9]=0;       b[11]=1;                     }
      else if (accessed_index==11) { b[0]=0; b[8]=1;b[9]=0;       b[11]=0;                     }
      else if (accessed_index==12) { b[0]=0; b[8]=0;                     b[12]=1;b[13]=1;       }
      else if (accessed_index==13) { b[0]=0; b[8]=0;                     b[12]=1;b[13]=0;       }
      else if (accessed_index==14) { b[0]=0; b[8]=0;                     b[12]=0;       b[14]=1;}
      else if (accessed_index==15) { b[0]=0; b[8]=0;                     b[12]=0;       b[14]=0;}
   }
   else if (m_associativity == 32) // added by Qi
   {
      if      (accessed_index==0) { b[0]=1; b[1]=1;b[2]=1;b[3]=1;b[4]=1;                           } // 0000 00001
      else if (accessed_index==1) { b[0]=1; b[1]=1;b[2]=1;b[3]=1;b[4]=0;                            }
      else if (accessed_index==2) { b[0]=1; b[1]=1;b[2]=1;b[3]=0;       b[5]=1;                     }
      else if (accessed_index==3) { b[0]=1; b[1]=1;b[2]=1;b[3]=0;       b[5]=0;                     }
      else if (accessed_index==4) { b[0]=1; b[1]=1;b[2]=1;                     b[6]=1;b[7]=1;       }
      else if (accessed_index==5) { b[0]=1; b[1]=1;b[2]=1;                     b[6]=1;b[7]=0;       }
      else if (accessed_index==6) { b[0]=1; b[1]=1;b[2]=1;                     b[6]=0;       b[8]=1;}
      else if (accessed_index==7) { b[0]=1; b[1]=1;b[2]=0;                     b[6]=0;       b[8]=0;}
      else if (accessed_index==8) { b[0]=1; b[1]=0; b[9]=1;b[10]=1;b[11]=1;                            }
      else if (accessed_index==9) { b[0]=1; b[1]=0; b[9]=1;b[10]=1;b[11]=0;                            }
      else if (accessed_index==10) { b[0]=1; b[1]=0; b[9]=1;b[10]=0;       b[12]=1;                     }
      else if (accessed_index==11) { b[0]=1; b[1]=0; b[9]=1;b[10]=0;       b[12]=0;                     }
      else if (accessed_index==12) { b[0]=1; b[1]=0; b[9]=0;                     b[13]=1;b[14]=1;       }
      else if (accessed_index==13) { b[0]=1; b[1]=0; b[9]=0;                     b[13]=1;b[14]=0;       }
      else if (accessed_index==14) { b[0]=1; b[1]=0; b[9]=0;                     b[13]=0;       b[15]=1;}
      else if (accessed_index==15) { b[0]=1; b[1]=0; b[9]=0;                     b[13]=0;       b[15]=0;}
      
      else if (accessed_index== 16) { b[0]=0; b[16]=1;b[17]=1;b[18]=1;b[19]=1;                           } // 0000 00001
      else if (accessed_index== 17) { b[0]=0; b[16]=1;b[17]=1;b[18]=1;b[19]=0;                            }
      else if (accessed_index== 18) { b[0]=0; b[16]=1;b[17]=1;b[18]=0;       b[20]=1;                     }
      else if (accessed_index== 19) { b[0]=0; b[16]=1;b[17]=1;b[18]=0;       b[20]=0;                     }
      else if (accessed_index== 20) { b[0]=0; b[16]=1;b[17]=1;                     b[21]=1;b[22]=1;       }
      else if (accessed_index==21) { b[0]=0; b[16]=1;b[17]=1;                     b[21]=1;b[22]=0;       }
      else if (accessed_index==22) { b[0]=0; b[16]=1;b[17]=1;                     b[21]=0;       b[23]=1;}
      else if (accessed_index==23) { b[0]=0; b[16]=1;b[17]=0;                     b[21]=0;       b[23]=0;}
      else if (accessed_index==24) { b[0]=0; b[16]=0; b[24]=1;b[25]=1;b[26]=1;                            }
      else if (accessed_index==25) { b[0]=0; b[16]=0; b[24]=1;b[25]=1;b[26]=0;                            }
      else if (accessed_index==26) { b[0]=0; b[16]=0; b[24]=1;b[25]=0;       b[27]=1;                     }
      else if (accessed_index==27) { b[0]=0; b[16]=0; b[24]=1;b[25]=0;       b[27]=0;                     }
      else if (accessed_index==28) { b[0]=0; b[16]=0; b[24]=0;                     b[28]=1;b[29]=1;       }
      else if (accessed_index==29) { b[0]=0; b[16]=0; b[24]=0;                     b[28]=1;b[29]=0;       }
      else if (accessed_index==30) { b[0]=0; b[16]=0; b[24]=0;                     b[28]=0;       b[30]=1;}
      else if (accessed_index==31) { b[0]=0; b[16]=0; b[24]=0;                     b[28]=0;       b[30]=0;}
   }
   else
   { 
	   return;
     // LOG_PRINT_ERROR("PLRU doesn't support associativity %d", m_associativity);
   }
   possibleIndexCheck(4);
}
