/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

/* This file was generated by getpir.c, do not modify!
   (but if you do, please run checkpir on it to verify)
   Contains the IRQ Routing Table dumped directly from your memory , wich BIOS sets up

   Documentation at : http://www.microsoft.com/hwdev/busbios/PCIIRQ.HTM
*/
#include <console/console.h>
#include <device/pci.h>
#include <string.h>
#include <stdint.h>
#include <arch/pirq_routing.h>

#include <cpu/amd/amdk8_sysconf.h>

static void write_pirq_info(struct irq_info *pirq_info, uint8_t bus, uint8_t devfn, uint8_t link0, uint16_t bitmap0,
		uint8_t link1, uint16_t bitmap1, uint8_t link2, uint16_t bitmap2,uint8_t link3, uint16_t bitmap3,
		uint8_t slot, uint8_t rfu)
{
        pirq_info->bus = bus;
        pirq_info->devfn = devfn;
                pirq_info->irq[0].link = link0;
                pirq_info->irq[0].bitmap = bitmap0;
                pirq_info->irq[1].link = link1;
                pirq_info->irq[1].bitmap = bitmap1;
                pirq_info->irq[2].link = link2;
                pirq_info->irq[2].bitmap = bitmap2;
                pirq_info->irq[3].link = link3;
                pirq_info->irq[3].bitmap = bitmap3;
        pirq_info->slot = slot;
        pirq_info->rfu = rfu;
}
extern unsigned char bus_mcp55[8]; //1



unsigned long write_pirq_routing_table(unsigned long addr)
{

	struct irq_routing_table *pirq;
	struct irq_info *pirq_info;
	unsigned slot_num;
	uint8_t *v;
	unsigned sbdn;

        uint8_t sum=0;
        int i;

        get_bus_conf(); // it will find out all bus num and apic that share with mptable.c and mptable.c and acpi_tables.c
	sbdn = sysconf.sbdn;

        /* Align the table to be 16 byte aligned. */
        addr += 15;
        addr &= ~15;

        /* This table must be between 0xf0000 & 0x100000 */
        printk(BIOS_INFO, "Writing IRQ routing tables to 0x%lx...", addr);

	pirq = (void *)(addr);
	v = (uint8_t *)(addr);

	pirq->signature = PIRQ_SIGNATURE;
	pirq->version  = PIRQ_VERSION;

	pirq->rtr_bus = bus_mcp55[0];
	pirq->rtr_devfn = ((sbdn+6)<<3)|0;

	pirq->exclusive_irqs = 0;

	pirq->rtr_vendor = 0x10de;
	pirq->rtr_device = 0x0370;

	pirq->miniport_data = 0;

	memset(pirq->rfu, 0, sizeof(pirq->rfu));

	pirq_info = (void *) ( &pirq->checksum + 1);
	slot_num = 0;
//pci bridge
	write_pirq_info(pirq_info, bus_mcp55[0], ((sbdn+6)<<3)|0, 0x1, 0xdef8, 0x2, 0xdef8, 0x3, 0xdef8, 0x4, 0xdef8, 0, 0);
	pirq_info++; slot_num++;

	pirq->size = 32 + 16 * slot_num;

        for (i = 0; i < pirq->size; i++)
                sum += v[i];

	sum = pirq->checksum - sum;

        if (sum != pirq->checksum) {
                pirq->checksum = sum;
        }

	printk(BIOS_INFO, "done.\n");

	return	(unsigned long) pirq_info;

}
