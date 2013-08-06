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
            "resources/specs/towers/sages/hero.json",
            "resources/specs/towers/sages/arcane.json"
          };
        case Faction::BRUTALS:
          return {
            "resources/specs/towers/brutals_hero.json",
            "resources/specs/towers/brutals_axe.json"
          };
      }
    }
  };

};

#endif
