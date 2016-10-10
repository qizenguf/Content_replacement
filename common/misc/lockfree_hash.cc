#include "lockfree_hash.h"

LockFreeHash::LockFreeHash(UInt64 size) : BasicHash::BasicHash(size)
{
}

LockFreeHash::~LockFreeHash()
{
}


UInt64 LockFreeHash::bucket_size(UInt64 key)
{
   UInt64 index = key % size;
   Bucket& bucket = array[index];
   return bucket.size();
}

std::pair<bool, UInt64> LockFreeHash::find(UInt64 key)
{
   std::pair<bool, UInt64> res = BasicHash::find(key);
   assert(bucket_size(key) <= 1);

   return res;
}

bool LockFreeHash::insert(UInt64 key, UInt64 value)
{
   bool res = BasicHash::insert(key, value);
   assert(bucket_size(key) <= 1);

   return res;
}


