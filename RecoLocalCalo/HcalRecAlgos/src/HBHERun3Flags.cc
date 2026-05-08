#include "RecoLocalCalo/HcalRecAlgos/interface/HBHERun3Flags.h"
#include <cstdint>
#include <iostream>
#include "DataFormats/HcalRecHit/interface/CaloRecHitAuxSetter.h"
#include "DataFormats/HcalRecHit/interface/HBHERecHitAuxSetter.h"

HBHERun3Flags::HBHERun3Flags() {}

HBHERun3Flags::~HBHERun3Flags() {}

void HBHERun3Flags::setStuckADCflagRun3(HBHERecHit& rechit, const QIE11DataFrame& digi, const uint8_t nSamples) {
  //setFlagStuckADC<HBHERecHit, QIE11DataFrame>(rechit, digi, nSamples, stuckADC_min_);

  const uint8_t tsShift = 0;
  if (nSamples < 2)
    return;
  const uint8_t firstAdc = digi[tsShift].adc();
  if (firstAdc < stuckADC_min_)
    return;

  for (int ts = 1; ts < nSamples; ++ts) {
    if (digi[ts + tsShift].adc() != firstAdc)
      return;
  }

  // now set the flag
  uint32_t aux_hbhe = rechit.auxHBHE(), aux_phase1 = rechit.auxPhase1();
  CaloRecHitAuxSetter::setBit(&aux_hbhe, bitForStuckADC_, true);
  CaloRecHitAuxSetter::setBit(&aux_phase1, bitForStuckADC_, true);
  rechit.setFlagField(1, bitForStuckADC_);
  rechit.setAuxHBHE(aux_hbhe);
  rechit.setAuxPhase1(aux_phase1);
  return;
}