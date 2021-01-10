# This file is part of libsamsung-ipc.
#
# Copyright (C) 2021 Denis 'GNUtoo' Carikli <GNUtoo@cyberdimension.org>
#
# libsamsung-ipc is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libsamsung-ipc is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libsamsung-ipc.  If not, see <http://www.gnu.org/licenses/>.

# We need to keep the compatibility with older Replicant versions, at least for
# testing purposes: some of the devices we support in libsamsung-ipc are not
# supported (yet) in newer Android versions or recent GNU/Linux distributions,
# and doing tests on older devices might enable us to also understand better
# the modem protocol and/or the EFS.

# To limit the amount of work, we need to avoid modifying older Android versions
# to be able to use recent libsamsung-ril and libsamsung-ipc.

# Here's how different Android distributions behave with
# LOCAL_PROPRIETARY_MODULE := true in libsamsung-ipc's Android.mk:
# Replicant  4.2: It probably breaks the modem support
# Replicant  6.0: It break the modem support
# Replicant  9.0: The modem integration doesn't work because it was not finished
# Replicant 10.0: The modem integration doesn't work because it was not finished
# Replicant 11.0: It is required for the modem to work

# Set a default value for build systems like Guix
PLATFORM_VERSION ?= 7
ifneq (1,$(filter 1,$(shell echo "$$(( $$(echo $(PLATFORM_VERSION) | sed 's/\..*//g') < 7 ))" )))
	LOCAL_PROPRIETARY_MODULE := true
endif
