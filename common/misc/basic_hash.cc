#include "basic_hash.h"

BasicHash::BasicHash(UInt64 size): array(new Bucket[size]), size(size)
{
}

BasicHash::~BasicHash()
{
   delete[] array;
}


std::pair<bool, UInt64> BasicHash::find(UInt64 key)
{
   UInt64 index = key % size;
   Bucket& bucket = array[index];
   Bucket::iterator it = bucket.find(key);
   if (it == bucket.end())
   {
      // condition to assert no collision
      assert(bucket.size() == 0);
      return std::make_pair(false, ~0);
   }
   return std::make_pair(true, it->second);
}

bool BasicHash::insert(UInt64 key, UInt64 value)
{
   UInt64 index = key % size;
   Bucket& bucket = array[index];
   std::pair<Bucket::iterator, bool> res = bucket.insert(std::make_pair(key, value));

   // condition to assert no collision
   assert(bucket.size() == 1);

   return res.second;
}
