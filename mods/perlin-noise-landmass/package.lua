-- Copyright (C) 2015 Luke San Antonio
-- Released under the 3-clause BSD License found in the mod root (LICENSE.txt)

terrain_gen = require('terrain_gen')

rc = require('redcrane')

mod = rc.register_terrain_algorithm(terrain_gen, {
  octaves = 8
});
