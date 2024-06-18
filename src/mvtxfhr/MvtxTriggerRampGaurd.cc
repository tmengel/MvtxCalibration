#include "MvtxTriggerRampGaurd.h"

#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/getClass.h>

#include <ffarawobjects/MvtxRawEvtHeader.h>
#include <ffarawobjects/MvtxRawEvtHeaderv1.h>
#include <ffarawobjects/MvtxRawHit.h>
#include <ffarawobjects/MvtxRawHitv1.h>
#include <ffarawobjects/MvtxRawHitContainer.h>
#include <ffarawobjects/MvtxRawHitContainerv1.h>

#include <trackbase/MvtxDefs.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/MvtxEventInfov2.h>

#include <string>
#include <vector>
#include <cstdint>
#include <climits>
#include <memory>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <set>
#include <cstdio>
#include <cstdlib>
#include <cmath>

#include <TH1D.h>

// MvtxTriggerRampGaurd class
//==============================================================================
MvtxTriggerRampGaurd::~MvtxTriggerRampGaurd()
{

}

int MvtxTriggerRampGaurd::InitRun(PHCompositeNode* /*topNode*/)
{

    if(m_output_filename != "")
    {
        PHTFileServer::get().open(m_output_filename, "RECREATE");
    }
    m_h_strobe_delta_bco = new TH1D("h_strobe_delta_bco", "Strobe Delta BCO", 1000, 0, 1000);

    m_strobe_length = (1.0 / m_trigger_rate)*1000000; // microseconds
    m_delta_bco_target = std::floor(m_strobe_length/0.106); // 106 ns per BCO
    std::cout << "Strobe length: " << m_strobe_length << " microseconds" << std::endl;
    std::cout << "Delta BCO target: " << m_delta_bco_target << std::endl;
    
    return Fun4AllReturnCodes::EVENT_OK;
}



int MvtxTriggerRampGaurd::process_event(PHCompositeNode *topNode)
{
    m_current_event++;
    if(m_is_past_trigger_ramp)
    {
        return Fun4AllReturnCodes::EVENT_OK;
    }

    // get dst nodes
    MvtxRawEvtHeaderv1 * m_mvtx_raw_event_header = findNode::getClass<MvtxRawEvtHeaderv1>(topNode, "MVTXRAWEVTHEADER");
    if(!m_mvtx_raw_event_header){ std::cout << PHWHERE << "::" << __func__ << ": Could not get MVTXRAWEVTHEADER from Node Tree" << std::endl; exit(1); }

    MvtxRawHitContainerv1 *m_mvtx_raw_hit_container = findNode::getClass<MvtxRawHitContainerv1>(topNode, "MVTXRAWHIT");
    if (!m_mvtx_raw_hit_container){ std::cout << PHWHERE << "::" << __func__ << ": Could not get MVTXRAWHIT from Node Tree" << std::endl; exit(1); }
    if(Verbosity() > 2) {  m_mvtx_raw_hit_container->identify();  }

    for (unsigned int ihit = 0; ihit < m_mvtx_raw_hit_container->get_nhits(); ihit++)
    {
        // get this hit
        auto mvtx_hit = m_mvtx_raw_hit_container->get_hit(ihit);
        
        // make sure it exists
        if (!mvtx_hit){ std::cout << PHWHERE << "::" << __func__ << ": Could not get MVTX hit from container. Hit index: " << ihit << std::endl; continue; }

        // get the hit info
        uint64_t strobe = mvtx_hit->get_bco();

        if(m_last_strobe_bco == 0)
        {
            m_last_strobe_bco = strobe;
            return Fun4AllReturnCodes::ABORTEVENT;
        }

        m_current_strobe_bco = strobe;
        m_delta_strobe_bco = m_current_strobe_bco - m_last_strobe_bco;
        m_last_strobe_bco = m_current_strobe_bco;

        if((m_delta_strobe_bco > m_delta_bco_target) && (!m_is_past_trigger_ramp))
        {
            m_concurrent_strobe_count = 0;
            return Fun4AllReturnCodes::ABORTEVENT;
        }

        m_concurrent_strobe_count++;
        if((m_concurrent_strobe_count < m_concurrent_strobe_target) && (!m_is_past_trigger_ramp))
        {
            return Fun4AllReturnCodes::ABORTEVENT;
        }

        m_is_past_trigger_ramp = true;

        std::cout << "Past trigger ramp" << std::endl;
        std::cout << "Event: " << m_current_event << std::endl;

        m_h_strobe_delta_bco->Fill(static_cast<double>(m_delta_strobe_bco));

        break;
    }


    return Fun4AllReturnCodes::EVENT_OK;
}

int MvtxTriggerRampGaurd::End(PHCompositeNode * /*topNode*/)
{

    if (m_output_filename == "")
    {
        return Fun4AllReturnCodes::EVENT_OK;
    }
    
    PHTFileServer::get().cd(m_output_filename);
    m_h_strobe_delta_bco->Write();
    PHTFileServer::get().close();

    return Fun4AllReturnCodes::EVENT_OK;

}