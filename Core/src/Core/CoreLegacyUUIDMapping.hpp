#pragma once

#include <map>
#include <string>

// clang-format off
#include "Core/Filters/FindNeighborhoodsFilter.hpp"
// @@__HEADER__TOKEN__DO__NOT__DELETE__@@

namespace complex
{
  static const std::map<complex::Uuid, complex::Uuid> k_SIMPL_to_Core
  {
    // syntax std::make_pair {Dream3d UUID , Dream3dnx UUID}, // dream3d-class-name
    {complex::Uuid::FromString("697ed3de-db33-5dd1-a64b-04fb71e7d63e").value(), complex::FilterTraits<FindNeighborhoodsFilter>::uuid}, // FindNeighborhoods
    // @@__MAP__UPDATE__TOKEN__DO__NOT__DELETE__@@
  };

} // namespace complex
// clang-format on
