#ifndef HBHERUN3FLAGS_H
#define HBHERUN3FLAGS_H

#include <cstdint>
#include "DataFormats/HcalRecHit/interface/HBHERecHit.h"
#include "DataFormats/HcalDigi/interface/QIE11DataFrame.h"
#include "DataFormats/METReco/interface/HcalCaloFlagLabels.h"

class HBHERun3Flags {
public:
  HBHERun3Flags();
  ~HBHERun3Flags();

  void setStuckADCflagRun3(HBHERecHit& rechit, const QIE11DataFrame& digi, const uint8_t nSamples);

private:
  static constexpr uint32_t stuckADC_min_ = 50;
  static constexpr uint8_t bitForStuckADC_ = HcalCaloFlagLabels::HBHERun3StuckADC;
};

#endif
