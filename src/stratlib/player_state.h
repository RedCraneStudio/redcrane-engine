/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
namespace game { namespace strat
{
  enum class Player_State_Type
  {
    Nothing,
    Building
  };

  struct Nothing_State { };

  struct Structure;
  struct Building_State
  {
    Structure* to_build;
  };

  struct Player_State
  {
    Player_State_Type type;
    union
    {
      Nothing_State nothing;
      Building_State building;
    };
  };
} }