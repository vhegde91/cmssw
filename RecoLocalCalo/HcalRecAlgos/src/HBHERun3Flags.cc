/*
Identify bad recHits based on ADC values and capID misalignment.
Relevant for Run 3 HBHE channels which use QIE11.
*/
#include "RecoLocalCalo/HcalRecAlgos/interface/HBHERun3Flags.h"
#include <cstdint>
#include <iostream>
#include "DataFormats/HcalRecHit/interface/CaloRecHitAuxSetter.h"
#include "DataFormats/HcalRecHit/interface/HBHERecHitAuxSetter.h"

HBHERun3Flags::HBHERun3Flags() {}

HBHERun3Flags::~HBHERun3Flags() {}

bool HBHERun3Flags::isStuckADC(const QIE11DataFrame& digi) {  // All ADC values are the same and are above some threshold
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

bool HBHERun3Flags::repeatedADCblock(const QIE11DataFrame& digi, const int soi) {  //looks for patterns like A B C D A B C D. ACD value should be > threshold in soi. Requires exaclty 8 samples
  if (isStuckADC(digi))
    return false;

  uint8_t nSamples = digi.samples();
  if (nSamples != 8 && nSamples != 10)
    return false;

  if (soi >= nSamples || digi[soi].adc() <= repeatedADCblock_min_)
    return false;
  
  int nHalf = nSamples / 2;
  for (int i = 0; i < nHalf; i++) {
    if (digi[i].adc() != digi[i + nHalf].adc())
      return false;
  }
  return true;
}

bool HBHERun3Flags::isBadCapId(
    const QIE11DataFrame& digi,
    const int soi,
    const uint32_t bx) {  //Does not pass (capId - bunchCrossing)%nCapsQIE11_ = a known value for the soi, where nCapsQIE11_ = 4
  if (soi >= digi.samples()) return false;

  short this_capidmbx = (digi[soi].capid() - bx) % nCapsQIE11_;
  if (this_capidmbx < 0)
    this_capidmbx += nCapsQIE11_;

  return (expCapIdInSOI_ != this_capidmbx);
}

bool HBHERun3Flags::nonRotatingCapId(
    const QIE11DataFrame& digi,
    const int soi,
    const uint32_t bx) {  //Does not pass (capId - bunchCrossing)%nCapsQIE11_ = a known value for at least one TS
  uint8_t nSamples = digi.samples();

  if (soi >= nSamples) return false;
  
  for (int i = 0; i < nSamples; i++) {
    short this_capidmbx = (digi[i].capid() - bx) % nCapsQIE11_;
    if (this_capidmbx < 0)
      this_capidmbx += nCapsQIE11_;
    short this_exp_capidmbx = (expCapIdInSOI_ + (i - soi)) % nCapsQIE11_;
    if (this_exp_capidmbx < 0)
      this_exp_capidmbx += nCapsQIE11_;
    if (this_capidmbx != this_exp_capidmbx)
      return true;
  }
  return false;
}
