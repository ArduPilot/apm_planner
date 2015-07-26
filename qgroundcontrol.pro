# -------------------------------------------------
# APM Planner - Micro Air Vehicle Groundstation
# Please see our website at <http://ardupilot.com>
# Maintainer:
# Bill Bonney <billbonney@communitech.com>
# Based of QGroundControl by Lorenz Meier <lm@inf.ethz.ch>
#
# (c) 2009-2011 QGroundControl Developers
# (c) 2013 Bill Bonney <billbonney@communistech.com>
# (c) 2013 Michael Carpenter <malcom2073@gmail.com>
#
# This file is part of the open groundstation project
# APM Planner is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# APM Planner is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with QGroundControl. If not, see <http://www.gnu.org/licenses/>.
# -------------------------------------------------

include(apmplanner_core.pro)

test {
   message("Building unit tests")

   TEMPLATE = app
   TARGET = apmplanner2_test

   include(qgcunittest.pro)
} else: {
   message("Building main application")

   TEMPLATE = app
   TARGET = apmplanner2

   SOURCES += src/main.cc
}
