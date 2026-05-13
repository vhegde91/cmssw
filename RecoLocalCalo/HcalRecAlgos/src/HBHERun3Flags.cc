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

bool HBHERun3Flags::repeatedADCblock(
    const QIE11DataFrame&
        digi) {  //looks for patterns like A B C D (E) A B C D (E) and requires exaclty 8 (or 10) samples
  uint8_t nSamples = digi.samples();
  if (nSamples != 8 && nSamples != 10)
    return false;
  int maxADC = 0;
  int nHalf = nSamples / 2;
  for (int i = 0; i < nHalf; i++) {
    if (digi[i].adc() == digi[i + nHalf].adc()) {
      if (digi[i].adc() > maxADC)
        maxADC = digi[i].adc();
    } else {
      return false;
    }
  }
  if (maxADC < repeatedADCblock_min_)
    return false;
  return true;
}

bool HBHERun3Flags::isBadCapId(
    const QIE11DataFrame& digi,
    const int soi,
    const uint32_t bx) {  //Does not pass (capId - bunchCrossing)%4 = a known value for the soi
  HcalDetId const& did = digi.detid();
  short this_capidmbx = (digi[soi].capid() - bx) % 4;
  if (this_capidmbx < 0)
    this_capidmbx += 4;

  bool good_capidmbx = true;
  if (did.subdet() == HcalBarrel)
    good_capidmbx = (capidmbx_HB_ == this_capidmbx);
  else if (did.subdet() == HcalEndcap)
    good_capidmbx = (capidmbx_HE_ == this_capidmbx);
  else {
    throw cms::Exception("HBHERun3Flags")
        << "Failed to get either HcalBarrel or HcalEndcap as Hcal subdet Id while setting HBHERun3 BadCapId flag"
        << std::endl;
  }
  return !good_capidmbx;
}

bool HBHERun3Flags::nonRotatingCapId(
    const QIE11DataFrame& digi,
    const int soi,
    const uint32_t bx) {  //Does not pass (capId - bunchCrossing)%4 = a known value for at least one TS
  uint8_t nSamples = digi.samples();

  HcalDetId const& did = digi.detid();
  short expCapidInSOI = -10;
  if (did.subdet() == HcalBarrel)
    expCapidInSOI = capidmbx_HB_;
  else if (did.subdet() == HcalEndcap)
    expCapidInSOI = capidmbx_HE_;
  else {
    throw cms::Exception("HBHERun3Flags") << "Failed to get either HcalBarrel or HcalEndcap as Hcal subdet Id while "
                                             "setting HBHERun3 nonRotatingCapId flag"
                                          << std::endl;
  }  //got expected CapId in soi

  for (int i = 0; i < nSamples; i++) {
    short this_capidmbx = (digi[i].capid() - bx) % 4;
    if (this_capidmbx < 0)
      this_capidmbx += 4;
    short this_exp_capidmbx = (expCapidInSOI + (i - soi)) % 4;
    if (this_exp_capidmbx < 0)
      this_exp_capidmbx += 4;
    if (this_capidmbx != this_exp_capidmbx)
      return true;
  }
  return false;
}
