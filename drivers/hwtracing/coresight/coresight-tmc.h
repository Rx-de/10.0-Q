/*
 * Copyright(C) 2015 Linaro Limited. All rights reserved.
 * Author: Mathieu Poirier <mathieu.poirier@linaro.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CORESIGHT_TMC_H
#define _CORESIGHT_TMC_H

#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/cacheflush.h>
#include <linux/of_address.h>
#include <linux/amba/bus.h>
#include <linux/usb_bam.h>
#include <linux/msm-sps.h>
#include <linux/usb/usb_qdss.h>
#include <linux/coresight-cti.h>

#include "coresight-byte-cntr.h"

#define TMC_RSZ			0x004
#define TMC_STS			0x00c
#define TMC_RRD			0x010
#define TMC_RRP			0x014
#define TMC_RWP			0x018
#define TMC_TRG			0x01c
#define TMC_CTL			0x020
#define TMC_RWD			0x024
#define TMC_MODE		0x028
#define TMC_LBUFLEVEL		0x02c
#define TMC_CBUFLEVEL		0x030
#define TMC_BUFWM		0x034
#define TMC_RRPHI		0x038
#define TMC_RWPHI		0x03c
#define TMC_AXICTL		0x110
#define TMC_DBALO		0x118
#define TMC_DBAHI		0x11c
#define TMC_FFSR		0x300
#define TMC_FFCR		0x304
#define TMC_PSCR		0x308
#define TMC_ITMISCOP0		0xee0
#define TMC_ITTRFLIN		0xee8
#define TMC_ITATBDATA0		0xeec
#define TMC_ITATBCTR2		0xef0
#define TMC_ITATBCTR1		0xef4
#define TMC_ITATBCTR0		0xef8

/* register description */
/* TMC_CTL - 0x020 */
#define TMC_CTL_CAPT_EN		BIT(0)
/* TMC_STS - 0x00C */
#define TMC_STS_TMCREADY_BIT	2
#define TMC_STS_FULL		BIT(0)
#define TMC_STS_TRIGGERED	BIT(1)
/*
 * TMC_AXICTL - 0x110
 *
 * TMC AXICTL format for SoC-400
 *	Bits [0-1]	: ProtCtrlBit0-1
 *	Bits [2-5]	: CacheCtrlBits 0-3 (AXCACHE)
 *	Bit  6		: Reserved
 *	Bit  7		: ScatterGatherMode
 *	Bits [8-11]	: WrBurstLen
 *	Bits [12-31]	: Reserved.
 * TMC AXICTL format for SoC-600, as above except:
 *	Bits [2-5]	: AXI WCACHE
 *	Bits [16-19]	: AXI RCACHE
 *	Bits [20-31]	: Reserved
 */
#define TMC_AXICTL_CLEAR_MASK 0xfbf
#define TMC_AXICTL_ARCACHE_MASK (0xf << 16)

#define TMC_AXICTL_PROT_CTL_B0	BIT(0)
#define TMC_AXICTL_PROT_CTL_B1	BIT(1)
#define TMC_AXICTL_CACHE_CTL_B0	BIT(2)
#define TMC_AXICTL_CACHE_CTL_B1	BIT(3)
#define TMC_AXICTL_CACHE_CTL_B2	BIT(4)
#define TMC_AXICTL_CACHE_CTL_B3	BIT(5)
#define TMC_AXICTL_SCT_GAT_MODE	BIT(7)
#define TMC_AXICTL_WR_BURST_16	0xF00
/* Write-back Read and Write-allocate */
#define TMC_AXICTL_AXCACHE_OS	(0xf << 2)
#define TMC_AXICTL_ARCACHE_OS	(0xf << 16)

/* TMC_FFCR - 0x304 */
#define TMC_FFCR_FLUSHMAN_BIT	6
#define TMC_FFCR_EN_FMT		BIT(0)
#define TMC_FFCR_EN_TI		BIT(1)
#define TMC_FFCR_FON_FLIN	BIT(4)
#define TMC_FFCR_FON_TRIG_EVT	BIT(5)
#define TMC_FFCR_TRIGON_TRIGIN	BIT(8)
#define TMC_FFCR_STOP_ON_FLUSH	BIT(12)

#define TMC_ETR_SG_ENT_TO_BLK(phys_pte)	(((phys_addr_t)phys_pte >> 4)	\
					 << PAGE_SHIFT)
#define TMC_ETR_SG_ENT(phys_pte)	(((phys_pte >> PAGE_SHIFT) << 4) | 0x2)
#define TMC_ETR_SG_NXT_TBL(phys_pte)	(((phys_pte >> PAGE_SHIFT) << 4) | 0x3)
#define TMC_ETR_SG_LST_ENT(phys_pte)	(((phys_pte >> PAGE_SHIFT) << 4) | 0x1)

#define TMC_DEVID_NOSCAT	BIT(24)

#define TMC_DEVID_AXIAW_VALID	BIT(16)
#define TMC_DEVID_AXIAW_SHIFT	17
#define TMC_DEVID_AXIAW_MASK	0x7f
#define TMC_ETR_BAM_PIPE_INDEX	0
#define TMC_ETR_BAM_NR_PIPES	2

#define TMC_ETR_PCIE_MEM_SIZE	0x400000

enum tmc_config_type {
	TMC_CONFIG_TYPE_ETB,
	TMC_CONFIG_TYPE_ETR,
	TMC_CONFIG_TYPE_ETF,
};

enum tmc_mode {
	TMC_MODE_CIRCULAR_BUFFER,
	TMC_MODE_SOFTWARE_FIFO,
	TMC_MODE_HARDWARE_FIFO,
};

enum tmc_mem_intf_width {
	TMC_MEM_INTF_WIDTH_32BITS	= 1,
	TMC_MEM_INTF_WIDTH_64BITS	= 2,
	TMC_MEM_INTF_WIDTH_128BITS	= 4,
	TMC_MEM_INTF_WIDTH_256BITS	= 8,
};

/* TMC ETR Capability bit definitions */
#define TMC_ETR_SG			(0x1U << 0)
/* ETR has separate read/write cache encodings */
#define TMC_ETR_AXI_ARCACHE		(0x1U << 1)
/*
 * TMC_ETR_SAVE_RESTORE - Values of RRP/RWP/STS.Full are
 * retained when TMC leaves Disabled state, allowing us to continue
 * the tracing from a point where we stopped. This also implies that
 * the RRP/RWP/STS.Full should always be programmed to the correct
 * value. Unfortunately this is not advertised by the hardware,
 * so we have to rely on PID of the IP to detect the functionality.
 */
#define TMC_ETR_SAVE_RESTORE		(0x1U << 2)

/* Coresight SoC-600 TMC-ETR unadvertised capabilities */
#define CORESIGHT_SOC_600_ETR_CAPS	\
	(TMC_ETR_SAVE_RESTORE | TMC_ETR_AXI_ARCACHE)

enum tmc_etr_mem_type {
	TMC_ETR_MEM_TYPE_CONTIG,
	TMC_ETR_MEM_TYPE_SG,
};

static const char * const str_tmc_etr_mem_type[] = {
	[TMC_ETR_MEM_TYPE_CONTIG]	= "contig",
	[TMC_ETR_MEM_TYPE_SG]		= "sg",
};
enum tmc_etr_out_mode {
	TMC_ETR_OUT_MODE_NONE,
	TMC_ETR_OUT_MODE_MEM,
	TMC_ETR_OUT_MODE_USB,
	TMC_ETR_OUT_MODE_PCIE,
};

static const char * const str_tmc_etr_out_mode[] = {
	[TMC_ETR_OUT_MODE_NONE]		= "none",
	[TMC_ETR_OUT_MODE_MEM]		= "mem",
	[TMC_ETR_OUT_MODE_USB]		= "usb",
	[TMC_ETR_OUT_MODE_PCIE]		= "pcie",
};

struct tmc_etr_bam_data {
	struct sps_bam_props	props;
	unsigned long		handle;
	struct sps_pipe		*pipe;
	struct sps_connect	connect;
	uint32_t		src_pipe_idx;
	unsigned long		dest;
	uint32_t		dest_pipe_idx;
	struct sps_mem_buffer	desc_fifo;
	struct sps_mem_buffer	data_fifo;
	bool			enable;
	enum usb_pipe_mem_type	mem_type;
};

/**
 * struct tmc_drvdata - specifics associated to an TMC component
 * @base:	memory mapped base address for this component.
 * @dev:	the device entity associated to this component.
 * @csdev:	component vitals needed by the framework.
 * @miscdev:	specifics to handle "/dev/xyz.tmc" entry.
 * @spinlock:	only one at a time pls.
 * @buf:	area of memory where trace data get sent.
 * @paddr:	DMA start location in RAM.
 * @vaddr:	virtual representation of @paddr.
 * @size:	trace buffer size.
 * @len:	size of the available trace.
 * @mode:	how this TMC is being used.
 * @config_type: TMC variant, must be of type @tmc_config_type.
 * @memwidth:	width of the memory interface databus, in bytes.
 * @trigger_cntr: amount of words to store after a trigger.
 * @etr_caps:	Bitmask of capabilities of the TMC ETR, inferred from the
 *		device configuration register (DEVID)
 */
struct tmc_drvdata {
	void __iomem		*base;
	struct device		*dev;
	struct coresight_device	*csdev;
	struct miscdevice	miscdev;
	spinlock_t		spinlock;
	bool			reading;
	bool			enable;
	char			*buf;
	dma_addr_t		paddr;
	void			*vaddr;
	u32			size;
	u32			len;
	u32			mode;
	enum tmc_config_type	config_type;
	enum tmc_mem_intf_width	memwidth;
	struct mutex		mem_lock;
	u32			mem_size;
	u32			trigger_cntr;
	enum tmc_etr_mem_type	mem_type;
	enum tmc_etr_mem_type	memtype;
	u32			etr_caps;
	u32			delta_bottom;
	int			sg_blk_num;
	enum tmc_etr_out_mode	out_mode;
	struct usb_qdss_ch	*usbch;
	struct tmc_etr_bam_data	*bamdata;
	bool			sticky_enable;
	bool			enable_to_bam;
	struct coresight_cti	*cti_flush;
	struct coresight_cti	*cti_reset;
	struct coresight_csr	*csr;
	const char		*csr_name;
	struct byte_cntr	*byte_cntr;
	struct dma_iommu_mapping *iommu_mapping;
	bool			force_reg_dump;
};

/* Generic functions */
void tmc_wait_for_tmcready(struct tmc_drvdata *drvdata);
void tmc_flush_and_stop(struct tmc_drvdata *drvdata);
void tmc_enable_hw(struct tmc_drvdata *drvdata);
void tmc_disable_hw(struct tmc_drvdata *drvdata);

/* ETB/ETF functions */
int tmc_read_prepare_etb(struct tmc_drvdata *drvdata);
int tmc_read_unprepare_etb(struct tmc_drvdata *drvdata);
extern const struct coresight_ops tmc_etb_cs_ops;
extern const struct coresight_ops tmc_etf_cs_ops;

/* ETR functions */
void tmc_etr_sg_compute_read(struct tmc_drvdata *drvdata, loff_t *ppos,
			     char **bufpp, size_t *len);
int tmc_read_prepare_etr(struct tmc_drvdata *drvdata);
int tmc_read_unprepare_etr(struct tmc_drvdata *drvdata);
void __tmc_etr_disable_to_bam(struct tmc_drvdata *drvdata);
void tmc_etr_bam_disable(struct tmc_drvdata *drvdata);
void tmc_etr_enable_hw(struct tmc_drvdata *drvdata);
void tmc_etr_disable_hw(struct tmc_drvdata *drvdata);
void usb_notifier(void *priv, unsigned int event, struct qdss_request *d_req,
		  struct usb_qdss_ch *ch);
int tmc_etr_bam_init(struct amba_device *adev,
		     struct tmc_drvdata *drvdata);
extern struct byte_cntr *byte_cntr_init(struct amba_device *adev,
					struct tmc_drvdata *drvdata);
extern const struct coresight_ops tmc_etr_cs_ops;
extern void tmc_etr_sg_rwp_pos(struct tmc_drvdata *drvdata, phys_addr_t rwp);
extern void tmc_etr_free_mem(struct tmc_drvdata *drvdata);
extern int tmc_etr_alloc_mem(struct tmc_drvdata *drvdata);

extern const struct coresight_ops tmc_etr_cs_ops;
int tmc_etr_switch_mode(struct tmc_drvdata *drvdata, const char *out_mode);

#define TMC_REG_PAIR(name, lo_off, hi_off)				\
static inline u64							\
tmc_read_##name(struct tmc_drvdata *drvdata)				\
{									\
	return coresight_read_reg_pair(drvdata->base, lo_off, hi_off);	\
}									\
static inline void							\
tmc_write_##name(struct tmc_drvdata *drvdata, u64 val)			\
{									\
	coresight_write_reg_pair(drvdata->base, val, lo_off, hi_off);	\
}

TMC_REG_PAIR(rrp, TMC_RRP, TMC_RRPHI)
TMC_REG_PAIR(rwp, TMC_RWP, TMC_RWPHI)
TMC_REG_PAIR(dba, TMC_DBALO, TMC_DBAHI)

/* Initialise the caps from unadvertised static capabilities of the device */
static inline void tmc_etr_init_caps(struct tmc_drvdata *drvdata, u32 dev_caps)
{
	WARN_ON(drvdata->etr_caps);
	drvdata->etr_caps = dev_caps;
}

static inline void tmc_etr_set_cap(struct tmc_drvdata *drvdata, u32 cap)
{
	drvdata->etr_caps |= cap;
}

static inline bool tmc_etr_has_cap(struct tmc_drvdata *drvdata, u32 cap)
{
	return !!(drvdata->etr_caps & cap);
}

#endif
