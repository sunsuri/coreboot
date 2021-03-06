#
# This file is part of the coreboot project.
#
# Copyright (C) 2009 coresystems GmbH
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc.
#
subdirs-y += loaders

bootblock-y += assets.c
bootblock-y += prog_loaders.c
bootblock-y += prog_ops.c
bootblock-y += cbfs.c
bootblock-y += cbfs_boot_props.c
bootblock-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
bootblock-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c
bootblock-y += libgcc.c
bootblock-$(CONFIG_GENERIC_UDELAY) += timer.c

bootblock-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
bootblock-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
bootblock-y += memchr.c
bootblock-y += memcmp.c
bootblock-y += mem_pool.c
bootblock-y += region.c
bootblock-y += boot_device.c
bootblock-y += fmap.c

verstage-y += assets.c
verstage-y += prog_loaders.c
verstage-y += prog_ops.c
verstage-y += delay.c
verstage-y += cbfs.c
verstage-y += halt.c
verstage-y += fmap.c
verstage-y += cbfs_boot_props.c
verstage-y += libgcc.c
verstage-y += memcmp.c
verstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
verstage-y += region.c
verstage-y += boot_device.c
verstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
verstage-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c

ifeq ($(MOCK_TPM),1)
libverstage-y += mocked_tlcl.c
else
libverstage-y += tlcl.c
endif

verstage-$(CONFIG_GENERIC_UDELAY) += timer.c
verstage-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c
verstage-y += mem_pool.c

romstage-y += assets.c
romstage-y += prog_loaders.c
romstage-y += prog_ops.c
romstage-y += memchr.c
romstage-y += memcmp.c
$(foreach arch,$(ARCH_SUPPORTED),\
	    $(eval rmodules_$(arch)-y += memcmp.c) \
	    $(eval rmodules_$(arch)-y += rmodule.ld))

romstage-y += fmap.c
romstage-$(CONFIG_I2C_TPM) += delay.c
romstage-y += cbfs.c
romstage-y += cbfs_boot_props.c
romstage-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
romstage-$(CONFIG_COMPRESS_RAMSTAGE) += lzma.c lzmadecode.c
romstage-y += libgcc.c
romstage-$(CONFIG_PRIMITIVE_MEMTEST) += primitive_memtest.c
ramstage-$(CONFIG_PRIMITIVE_MEMTEST) += primitive_memtest.c
romstage-$(CONFIG_CACHE_AS_RAM) += ramtest.c
romstage-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c

ifeq ($(CONFIG_EARLY_CBMEM_INIT),y)
romstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
romstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
endif

romstage-y += compute_ip_checksum.c
ifeq ($(CONFIG_COMPILER_GCC),y)
romstage-$(CONFIG_ARCH_ROMSTAGE_X86_32) += gcc.c
ramstage-$(CONFIG_ARCH_RAMSTAGE_X86_32) += gcc.c
endif

romstage-$(CONFIG_GENERIC_UDELAY) += timer.c

ramstage-y += assets.c
ramstage-y += prog_loaders.c
ramstage-y += prog_ops.c
ramstage-y += hardwaremain.c
ramstage-y += selfboot.c
ramstage-y += coreboot_table.c
ramstage-y += bootmem.c
ramstage-y += fmap.c
ramstage-y += memchr.c
ramstage-y += memcmp.c
ramstage-y += malloc.c
smm-$(CONFIG_SMM_TSEG) += malloc.c
ramstage-y += delay.c
ramstage-y += fallback_boot.c
ramstage-y += compute_ip_checksum.c
ramstage-y += cbfs.c
ramstage-y += cbfs_boot_props.c
ramstage-$(CONFIG_COMMON_CBFS_SPI_WRAPPER) += cbfs_spi.c
ramstage-y += lzma.c lzmadecode.c
ramstage-y += stack.c
ramstage-$(CONFIG_CONSOLE_CBMEM) += cbmem_console.c
ramstage-$(CONFIG_BOOTSPLASH) += jpeg.c
ramstage-$(CONFIG_TRACE) += trace.c
ramstage-$(CONFIG_COLLECT_TIMESTAMPS) += timestamp.c
ramstage-$(CONFIG_COVERAGE) += libgcov.c
ramstage-$(CONFIG_MAINBOARD_DO_NATIVE_VGA_INIT) += edid.c
ramstage-y += memrange.c
ramstage-$(CONFIG_COOP_MULTITASKING) += thread.c
ramstage-$(CONFIG_TIMER_QUEUE) += timer_queue.c
ramstage-$(CONFIG_GENERIC_GPIO_LIB) += gpio.c
ramstage-$(CONFIG_GENERIC_UDELAY) += timer.c
ramstage-y += b64_decode.c

romstage-y += cbmem_common.c
romstage-y += imd_cbmem.c

ramstage-y += cbmem_common.c
ramstage-y += imd_cbmem.c

romstage-y += imd.c
ramstage-y += imd.c

ramstage-y += hexdump.c
romstage-y += hexdump.c

romstage-$(CONFIG_REG_SCRIPT) += reg_script.c
ramstage-$(CONFIG_REG_SCRIPT) += reg_script.c

ifeq ($(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM),y)
ramstage-y += ext_stage_cache.c
romstage-y += ext_stage_cache.c
else
ramstage-$(CONFIG_RELOCATABLE_RAMSTAGE) += cbmem_stage_cache.c
romstage-$(CONFIG_RELOCATABLE_RAMSTAGE) += cbmem_stage_cache.c
endif

romstage-y += mem_pool.c
ramstage-y += mem_pool.c

romstage-y += region.c
ramstage-y += region.c
romstage-y += boot_device.c
ramstage-y += boot_device.c

smm-y += region.c
smm-y += boot_device.c
smm-y += fmap.c
smm-y += cbfs.c memcmp.c
smm-$(CONFIG_COMPILER_GCC) += gcc.c

bootblock-y += version.c
romstage-y += version.c
ramstage-y += version.c
smm-y += version.c
secmon-y += version.c
verstage-y += version.c

$(obj)/lib/version.bootblock.o : $(obj)/build.h
$(obj)/lib/version.romstage.o : $(obj)/build.h
$(obj)/lib/version.ramstage.o : $(obj)/build.h
$(obj)/lib/version.smm.o : $(obj)/build.h
$(obj)/lib/version.secmon.o : $(obj)/build.h
$(obj)/lib/version.verstage.o : $(obj)/build.h

romstage-y += bootmode.c
ramstage-y += bootmode.c

bootblock-y += halt.c
romstage-y += halt.c
ramstage-y += halt.c
smm-y += halt.c
secmon-y += halt.c

ifneq ($(CONFIG_ARCH_X86),y)
# X86 bootblock and romstage use custom ldscripts that are all glued together,
# so we need to exclude it here or it would pick these up as well
bootblock-y += bootblock.ld
romstage-y += romstage.ld
endif
ramstage-y += ramstage.ld

ifeq ($(CONFIG_RELOCATABLE_MODULES),y)
ramstage-y += rmodule.c
romstage-$(CONFIG_RELOCATABLE_RAMSTAGE) += rmodule.c

RMODULE_LDFLAGS  := -nostartfiles --gc-sections --emit-relocs -z defs -Bsymbolic

# rmodule_link_rules is a function that should be called with:
# (1) the object name to link
# (2) the dependencies
# (3) heap size of the relocatable module
# (4) arch for which the rmodules are to be linked
# It will create the necessary Make rules to create a rmodule. The resulting
# rmdoule is named $(1).rmod
define rmodule_link
$(strip $(1)): $(strip $(2)) $$(COMPILER_RT_rmodules_$(4)) $(obj)/lib/rmodule.rmodules_$(4).ld | $$(RMODTOOL)
	$$(LD_rmodules_$(4)) $(RMODULE_LDFLAGS) -T $(obj)/lib/rmodule.rmodules_$(4).ld --defsym=__heap_size=$(strip $(3)) -o $$@ --whole-archive --start-group $(filter-out %.ld,$(2)) --end-group
	$$(NM_rmodules_$(4)) -n $$@ > $$(basename $$@).map

$(strip $(1)).rmod: $(strip $(1))
	$$(RMODTOOL) -i $$^ -o $$@
endef

endif
