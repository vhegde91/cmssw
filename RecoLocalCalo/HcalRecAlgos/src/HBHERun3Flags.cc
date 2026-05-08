#include "RecoLocalCalo/HcalRecAlgos/interface/HBHERun3Flags.h"
#include <cstdint>
#include <iostream>
#include "DataFormats/HcalRecHit/interface/CaloRecHitAuxSetter.h"
#include "DataFormats/HcalRecHit/interface/HBHERecHitAuxSetter.h"

HBHERun3Flags::HBHERun3Flags() {}

HBHERun3Flags::~HBHERun3Flags() {}

bool HBHERun3Flags::isStuckADC(const QIE11DataFrame& digi){
uint8_t nSamples = digi.samples();
if (nSamples < 2)
  return false;
const uint8_t firstAdc = digi[0].adc();
if (firstAdc < stuckADC_min_)
  return false;

for (int ts = 1; ts < nSamples; ++ts) {
  if (digi[ts].adc() != firstAdc)
    return false;
  }
return true;
}

bool HBHERun3Flags::repeatedADCblock(const QIE11DataFrame& digi){ //looks for patterns like A B C D A B C D and requires exaclty 8 samples
  uint8_t nSamples = digi.samples();
  if (nSamples != 8) return false;
  int maxADC = 0;
  for(int i = 0; i < 4; i++){
    if(digi[i].adc() == digi[i+4].adc()){
      if(digi[i].adc() > maxADC) maxADC = digi[i].adc();
    }
    else{
      return false;
    }
  }
  if(maxADC < repeatedADCblock_min_) return false;
  return true;
}

bool HBHERun3Flags::isBadCapId(const QIE11DataFrame& digi, const uint32_t bx){
  short soi = -1;
  for (int i = 0; i < digi.samples(); i++) {
    if (digi[i].soi()) {
      soi = i;
      break;
    }
  }
  HcalDetId const& did = digi.detid();
  short this_capidmbx = (digi[soi].capid() - bx) % 4;
  if (this_capidmbx < 0) {
    this_capidmbx += 4;
  }
  
  bool good_capidmbx = true;
  if(did.subdet() == HcalBarrel) good_capidmbx = (capidmbx_HB_ == this_capidmbx);
  else if(did.subdet() == HcalEndcap) good_capidmbx = (capidmbx_HE_ == this_capidmbx);
  else{
    throw cms::Exception("HBHERun3Flags")
          << "Failed to get either HcalBarrel or HcalEndcap as Hcal subdet Id while setting HBHERun3 BadCapId flag" << std::endl;
  }
  return !good_capidmbx;
}

void HBHERun3Flags::setRecHitFlagRun3(HBHERecHit *rechit, const uint32_t bitPosition) {
  // now set the flag
  uint32_t aux_hbhe = rechit->auxHBHE(), aux_phase1 = rechit->auxPhase1();
  CaloRecHitAuxSetter::setBit(&aux_hbhe, bitPosition, true);
  CaloRecHitAuxSetter::setBit(&aux_phase1, bitPosition, true);
  rechit->setFlagField(1U, bitPosition);
  //rechit->setAuxHBHE(aux_hbhe);
  //rechit->setAuxPhase1(aux_phase1);
  return;
}