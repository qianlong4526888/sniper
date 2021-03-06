#ifndef __SIMPLE_PREFETCHER_H
#define __SIMPLE_PREFETCHER_H

#include "prefetcher.h"

class SimplePrefetcher : public Prefetcher
{
   public:
      SimplePrefetcher(String configName, core_id_t core_id, UInt32 shared_cores, void *cache_cntlr);
      virtual std::vector<IntPtr> getNextAddress(IntPtr current_address, UInt32 offset ,core_id_t core_id, DynamicInstruction *dynins, UInt64 *pointer_loads, UInt64* multi_consumer_loads, UInt64* pointer_stores, IntPtr target_reg);

   private:
      const core_id_t core_id;
      const UInt32 shared_cores;
      const UInt32 n_flows;
      const bool flows_per_core;
      const UInt32 num_prefetches;
      const bool stop_at_page;
      UInt32 n_flow_next;
      std::vector<std::vector<IntPtr> > m_prev_address;
};

#endif // __SIMPLE_PREFETCHER_H
