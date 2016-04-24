
-- Copyright (c) 2015 Luke San Antonio
-- All rights reserved.

local ffi = require("ffi")

local rc = {}

function rc:init(config)
    self.engine = self.engine or ffi.gc(ffi.C.redc_init_engine(config),
                                        ffi.C.redc_uninit_engine)

    self.scene = require("scene")
    self.mesh_pool = require("mesh_pool")
    self.shader = require("shader")

    return self.engine
end

function rc:init_client()
    ffi.C.redc_init_client(self.engine)
end
function rc:init_server()
    ffi.C.redc_init_server(self.engine)
end

function rc:step_server()
    ffi.C.redc_server_step(self.engine)
end

function rc:running()
    return ffi.C.redc_running(self.engine)
end
function rc:swap_window()
    ffi.C.redc_window_swap(self.engine)
end

function rc:asset_path()
    -- This memory doesn't have to be freed
    return ffi.C.redc_get_asset_path(self.engine)
end

function rc:load_map(filename)
    return ffi.C.redc_map_load(self.engine, filename)
end

-- Logging functions
function stringify(str, ...)
    for n=1,select('#', ...) do
        local e = select(n, ...)
        str = str .. ' ' .. tostring(e)
    end
    return str
end
function rc:log_d(str, ...)
    ffi.C.redc_log_d(stringify(tostring(str), ...))
end
function rc:log_i(str, ...)
    ffi.C.redc_log_i(stringify(tostring(str), ...))
end
function rc:log_w(str, ...)
    ffi.C.redc_log_w(stringify(tostring(str), ...))
end
function rc:log_e(str, ...)
    ffi.C.redc_log_e(stringify(tostring(str), ...))
end

return rc
