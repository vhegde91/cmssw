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

  void setRecHitFlagRun3(HBHERecHit *rechit, const uint32_t bitPosition);
  bool repeatedADCblock(const QIE11DataFrame& digi);
  bool isStuckADC(const QIE11DataFrame& digi);

  bool isBadCapId(const QIE11DataFrame& digi, const uint32_t bunchCrossing);

private:
  static constexpr uint32_t stuckADC_min_ = 50;
  static constexpr int repeatedADCblock_min_ = 10;

  static constexpr short capidmbx_HB_ = 1;
  static constexpr short capidmbx_HE_ = 1;
  
};

#endif
