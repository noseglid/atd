#ifndef __FACTION_H__
#define __FACTION_H__

namespace Faction {
  enum Faction {
    SAGES,
    BRUTALS
  };

  class Loader
  {
  public:
    static std::list<std::string>
    tower_specs(Faction faction)
    {
      switch (faction) {
        case Faction::SAGES:
          return {
            "specs/towers/sages_hero.twr",
            "specs/towers/sages_arcane.twr"
          };
        case Faction::BRUTALS:
          return {
            "specs/towers/brutals_hero.twr",
            "specs/towers/brutals_axe.twr"
          };
      }
    }
  };

};

#endif
