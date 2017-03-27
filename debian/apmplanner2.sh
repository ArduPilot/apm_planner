#!/bin/bash
#variable needed to prevent CPU hogging in Intel video chipsets.
export QSG_RENDER_LOOP=threaded
./apmplanner2
