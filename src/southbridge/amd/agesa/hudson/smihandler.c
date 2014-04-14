/*
 * SMI handler for Hudson southbridges
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "smi.h"

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <delay.h>


enum smi_source {
	SMI_SOURCE_SCI = (1 << 0),
	SMI_SOURCE_GPE = (1 << 1),
	SMI_SOURCE_0x84 = (1 << 2),
	SMI_SOURCE_0x88 = (1 << 3),
	SMI_SOURCE_IRQ_TRAP = (1 << 4),
	SMI_SOURCE_0x90 = (1 << 5)
};

int southbridge_io_trap_handler(int smif)
{
	return 0;
}

static void process_smi_sci(void)
{
	const uint32_t status = smi_read32(0x10);

	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(0x10, status);
}

static void process_gpe_smi(void)
{
	const uint32_t status = smi_read32(0x80);
	const uint32_t gevent_mask = (1 << 24) - 1;

	/* Only Bits [23:0] indicate GEVENT SMIs. */
	if (status & gevent_mask) {
		/* A GEVENT SMI occured */
		if (mainboard_smi_gpi)
			mainboard_smi_gpi(status & gevent_mask);
	}

	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(0x80, status);
}

static void process_smi_0x84(void)
{
	const uint32_t status = smi_read32(0x84);

	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(0x84, status);
}

static void process_smi_0x88(void)
{
	const uint32_t status = smi_read32(0x88);

	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(0x88, status);
}

static void process_smi_0x8c(void)
{
	const uint32_t status = smi_read32(0x8c);

	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(0x8c, status);
}

static void process_smi_0x90(void)
{
	const uint32_t status = smi_read32(0x90);

	/* Clear events to prevent re-entering SMI if event isn't handled */
	smi_write32(0x90, status);
}

void southbridge_smi_handler(unsigned int node, smm_state_save_area_t *state_save)
{
	const uint16_t smi_src = smi_read16(0x94);

	if (smi_src & SMI_SOURCE_SCI)
		process_smi_sci();
	if (smi_src & SMI_SOURCE_GPE)
		process_gpe_smi();
	if (smi_src & SMI_SOURCE_0x84)
		process_smi_0x84();
	if (smi_src & SMI_SOURCE_0x88)
		process_smi_0x88();
	if (smi_src & SMI_SOURCE_IRQ_TRAP)
		process_smi_0x8c();
	if (smi_src & SMI_SOURCE_0x90)
		process_smi_0x90();
}

void southbridge_smi_set_eos(void)
{
	uint32_t reg = smi_read32(SMI_REG_SMITRIG0);
	reg |= SMITRG0_EOS;
	smi_write32(SMI_REG_SMITRIG0, reg);
}
