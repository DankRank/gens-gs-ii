/***************************************************************************
 * libgenscd: Gens/GS II CD-ROM Handler Library.                           *
 * CdDrive.cpp: CD-ROM drive handler class.                                *
 *                                                                         *
 * Copyright (c) 2013 by David Korth.                                      *
 *                                                                         *
 * This program is free software; you can redistribute it and/or modify it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation; either version 2 of the License, or (at your  *
 * option) any later version.                                              *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License along *
 * with this program; if not, write to the Free Software Foundation, Inc., *
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.           *
 ***************************************************************************/

#include "CdDrive.hpp"

// C includes. (C++ namespace)
#include <cstring>
#include <cstdio>

// C++ includes.
#include <string>
using std::string;

// TODO: Byteorder headers from LibGens.
// Assuming LE host for now.
#define __swab16(x) (((x) << 8) | ((x) >> 8))

#define __swab32(x) \
	(((x) << 24) | ((x) >> 24) | \
		(((x) & 0x0000FF00UL) << 8) | \
		(((x) & 0x00FF0000UL) >> 8))

#define be16_to_cpu(x)	__swab16(x)
#define be32_to_cpu(x)	__swab32(x)
#define le16_to_cpu(x)	(x)
#define le32_to_cpu(x)	(x)

#define cpu_to_be16(x)	__swab16(x)
#define cpu_to_be32(x)	__swab32(x)
#define cpu_to_le16(x)	(x)
#define cpu_to_le32(x)	(x)

// TODO: Factory class?
#include "ScsiBase.hpp"
#if defined(_WIN32)
#include "ScsiSpti.hpp"
#elif defined(__linux__)
#include "ScsiLinux.hpp"
#else
#error CdDrive: Only Win32 and Linux are supported right now.
#endif

#define PRINT_SCSI_ERROR(op, err) \
	do { \
		fprintf(stderr, "%s(): SCSI error: OP=%02X, SK=%01X ASC=%02X ASCQ=%02X\n", \
			__func__, op, SK(err), ASC(err), ASCQ(err)); \
	} while (0)

namespace LibGensCD
{

class CdDrivePrivate
{
	public:
		CdDrivePrivate(CdDrive *q, string filename);
		~CdDrivePrivate();

	private:
		CdDrive *const q;
		
		// Q_DISABLE_COPY() equivalent.
		// TODO: Add LibGensCD-specific version of Q_DISABLE_COPY().
		CdDrivePrivate(const CdDrivePrivate &);
		CdDrivePrivate &operator=(const CdDrivePrivate &);

	public:
		// Device filename.
		std::string filename;

		// Scsi handler.
		ScsiBase *p_scsi;

		enum InquiryStatus
		{
			INQ_NOT_DONE,
			INQ_SUCCESSFUL,
			INQ_FAILED
		};

		// INQUIRY results.
		struct {
			// Inquiry status.
			InquiryStatus inq_status;

			// Peripheral device type.
			// Should be 0x05. (DTYPE_CDROM)
			uint8_t scsi_device_type;

			std::string vendor;
			std::string model;
			std::string firmware;

			// Drive type. 
			CD_DriveType_t driveType;
		} inq_data;

		int inquiry(void);
		bool isInquirySuccessful(void);

		/**
		 * Get the current feature profile, aka disc type.
		 * Uses the MMC-1 READ DISC INFORMATION command.
		 * @return Current feature profile, or 0xFFFF on error.
		 */
		uint16_t getCurrentFeatureProfile_mmc1(void);

		/**
		 * Get the current feature profile, aka disc type.
		 * Uses the MMC-2 GET CONFIGURATION command.
		 * If the command isn't supported, falls back to MMC-1.
		 * @return Current feature profile, or 0xFFFF on error.
		 */
		uint16_t getCurrentFeatureProfile(void);

		/**
		 * Get a bitfield of supported disc types.
		 * @return Bitfield of supported disc types.
		 */
		uint32_t getSupportedDiscTypes(void);

		/**
		 * Convert an MMC feature profile to a CD_DiscType_t.
		 * @param featureProfile MMC feature profile.
		 * @return CD_DiscType_t.
		 */
		static CD_DiscType_t mmcFeatureProfileToDiscType(uint16_t featureProfile);

		/**
		 * Convert a bitfield of CD_DiscType_t to a CD_DriveType_t.
		 * @param discTypes Bitfield of all CD_DiscType_t disc types.
		 * @return CD_DriveType_t.
		 */
		static CD_DriveType_t discTypesToDriveType(uint32_t discTypes);

		// Cached disc information.
		struct {
			// If true, cache is stale and needs to be updated.
			bool stale;

			// Disc type.
			CD_DiscType_t discType;

			// Table of Contents.
			struct {
				int num_tracks;
				SCSI_CDROM_TOC toc;
			} toc;
		} cache;

		/**
		 * Update the cached disc information.
		 * @param force If true, force a cache update, even if it's not stale.
		 */
		void updateCache(bool force = false);

		/**
		 * Read the Table of Contents into the cache.
		 * @return 0 on success; non-zero on error.
		 */
		int readToc(void);
};

/** CdDrivePrivate **/

CdDrivePrivate::CdDrivePrivate(CdDrive *q, string filename)
	: q(q)
	, filename(filename)
{
	// Initialize the SCSI device handler.
	// TODO: Factory class?
#if defined(_WIN32)
	p_scsi = new ScsiSpti(filename);
#elif defined(__linux__)
	p_scsi = new ScsiLinux(filename);
#else
#error CdDrive: Only Win32 and Linux are supported right now.
#endif

	// Run the SCSI INQUIRY command.
	inquiry();

	// Mark the cache as stale.
	cache.stale = true;
}

CdDrivePrivate::~CdDrivePrivate()
{
	delete p_scsi;
}

/**
 * Run a SCSI INQUIRY command.
 * @return 0 on success; nonzero on error.
 */
int CdDrivePrivate::inquiry(void)
{
	SCSI_RESP_INQUIRY_STD resp;
	int err = p_scsi->inquiry(&resp);
	if (err != 0) {
		// Inquiry failed.
		PRINT_SCSI_ERROR(SCSI_OP_INQUIRY, err);
		inq_data.inq_status = CdDrivePrivate::INQ_FAILED;
		return -1;
	}

	// Drive inquiry successful.
	// Get the information.
	// TODO: Validate that device type is 0x05. (DTYPE_CDROM)
	// TODO: Trim spaces in vendor, model, and firmware.
	inq_data.scsi_device_type = resp.PeripheralDeviceType;
	inq_data.vendor = string(resp.vendor_id, sizeof(resp.vendor_id));
	inq_data.model = string(resp.product_id, sizeof(resp.product_id));
	inq_data.firmware = string(resp.product_revision_level, sizeof(resp.product_revision_level));

	// SCSI_INQUIRY completed successfully.
	inq_data.inq_status = CdDrivePrivate::INQ_SUCCESSFUL;

	// Get the CD_DriveType_t.
	inq_data.driveType = discTypesToDriveType(getSupportedDiscTypes());

	return 0;
}

/**
 * Check if the inquiry was successful.
 * @return True if successful; false if not.
 */
bool CdDrivePrivate::isInquirySuccessful(void)
{
	if (inq_data.inq_status == INQ_NOT_DONE)
		inquiry();
	return (inq_data.inq_status == INQ_SUCCESSFUL);
}

/**
 * Update the cached disc information.
 * @param force If true, force a cache update, even if it's not stale.
 */
void CdDrivePrivate::updateCache(bool force)
{
	if (!p_scsi->isDiscPresent()) {
		// No disc.
		cache.stale = true;
		cache.discType = DISC_TYPE_NONE;
		printf("update cache: no disc\n");
		return;
	}

	// Check if the disc has been changed.
	// NOTE: We always do this, even if cache.stale = true,
	// since it clears the OS's "disc has changed" state.
	if (p_scsi->hasDiscChanged()) {
		// Disc has changed. Update the cache.
		cache.stale = true;
	}

	// Check if we need to update the cache.
	if (!cache.stale && !force) {
		printf("update cache: not stale\n");
		return;
	}

	printf("update cache: running...\n");
	// Update the disc type.
	cache.discType = mmcFeatureProfileToDiscType(getCurrentFeatureProfile());

	// Update the TOC.
	int err = readToc();
	if (err != 0) {
		// Error reading the TOC.
		cache.stale = true;
		return;
	}

	// Cache is updated.
	cache.stale = false;
}

/**
 * Get the current feature profile, aka disc type.
 * Uses the MMC-1 READ DISC INFORMATION command.
 * @return Current feature profile, or 0xFFFF on error.
 */
uint16_t CdDrivePrivate::getCurrentFeatureProfile_mmc1(void)
{
	// Make sure a disc is present.
	if (!p_scsi->isOpen() || !p_scsi->isDiscPresent())
		return 0;

	SCSI_RESP_READ_DISC_INFORMATION_STANDARD resp;
	int err = p_scsi->readDiscInformation(&resp);
	if (err != 0) {
		// An error occurred executing READ DISC INFORMATION.
		// This usually means that we have a CD-ROM.
		return SCSI_MMC_PROFILE_CDROM;
	}

	// Determine the current profile based on the disc information.
	// MMC-1 devices don't support DVDs, so we can rule out
	// everything except CD-ROM, CD-R, and CD-RW.
	// (We're not counting MO here.)
	if (resp.DiscStatusFlags & 0x10) {
		// Disc is rewritable.
		return SCSI_MMC_PROFILE_CD_RW;
	} else if ((resp.DiscStatusFlags & 0x03) < 2) {
		// Disc is either empty, incomplete, or finalized.
		// This is a CD-R.
		return SCSI_MMC_PROFILE_CD_R;
	}

	// Assume this is a CD-ROM.
	return SCSI_MMC_PROFILE_CDROM;
}

/**
 * Get the current feature profile, aka disc type.
 * Uses the MMC-2 GET CONFIGURATION command.
 * If the command isn't supported, falls back to MMC-1.
 * @return Current feature profile, or 0xFFFF on error.
 */
uint16_t CdDrivePrivate::getCurrentFeatureProfile(void)
{
	SCSI_RESP_GET_CONFIGURATION_HEADER resp;
	int err = p_scsi->getConfiguration(&resp);
	if (err != 0) {
		// An error occurred executing GET CONFIGURATION.
		// If the command wasn't supported, try the
		// MMC-1 command READ DISC INFORMATION.
		if (SK(err) == 0x5 && ASC(err) == 0x20) {
			// Drive does not support MMC-2 commands.
			// Try the MMC-1 fallback.
			return getCurrentFeatureProfile_mmc1();
		}

		// Other error.
		PRINT_SCSI_ERROR(SCSI_OP_GET_CONFIGURATION, err);
	}

	// Return the current profile.
	return resp.CurrentProfile;
}

/**
 * Get a bitfield of supported disc types.
 * @return Bitfield of supported disc types.
 */
uint32_t CdDrivePrivate::getSupportedDiscTypes(void)
{
	// TODO: Figure out some way to move this to ScsiBase.

	// Make sure the device file is open.
	if (!p_scsi->isOpen())
		return 0;

	SCSI_CDB_GET_CONFIGURATION cdb;
	memset(&cdb, 0x00, sizeof(cdb));

	// Response buffer.
	uint8_t resp[65530];
	memset(resp, 0x00, sizeof(resp));

	// Get all available feature descriptors.
	cdb.OpCode = SCSI_OP_GET_CONFIGURATION;
	cdb.AllocLen = (uint16_t)cpu_to_be16(sizeof(resp));
	cdb.Control = 0;

	int err = p_scsi->scsi_send_cdb(&cdb, sizeof(cdb), &resp, sizeof(resp), ScsiBase::SCSI_DATA_IN);
	if (err != 0) {
		// An error occurred.
		if (SK(err) == 0x5 && ASC(err) == 0x20) {
			// Drive does not support MMC-2 commands.
			// Determine drive type based on current disc type.
			switch (getCurrentFeatureProfile_mmc1()) {
				case SCSI_MMC_PROFILE_CD_RW:
					return DRIVE_TYPE_CD_RW;
				case SCSI_MMC_PROFILE_CD_R:
					return DRIVE_TYPE_CD_R;
				case SCSI_MMC_PROFILE_CDROM:
				default:
					return DRIVE_TYPE_CDROM;
			}
		}

		// Some other error occurred.
		PRINT_SCSI_ERROR(cdb.OpCode, err);
		return 0;
	}

	// Check how many feature descriptors we received.
	uint32_t len = (resp[0] << 24 | resp[1] << 16 | resp[2] << 8 | resp[3]);
	if (len > sizeof(resp)) {
		// Too many feature descriptors. Truncate the list
		len = sizeof(resp);
	}

	// Go through all of the feature descriptors.
	int discTypes = 0;
	for (unsigned int i = 8; i+4 < len; i += (4 + resp[i+3])) {
		const unsigned int featureCode = (resp[i] << 8 | resp[i+1]);
		if (featureCode == SCSI_MMC_FEATURE_PROFILE_LIST) {
			// Feature profile.
			const uint8_t *ptr = &resp[i+4];
			const uint8_t *const ptr_end = ptr + resp[i+3];

			for (; ptr < ptr_end; ptr += 4) {
				const uint16_t featureProfile = (ptr[0] << 8 | ptr[1]);
				printf("Feature Profile: %04X\n", featureProfile);
				discTypes |= mmcFeatureProfileToDiscType(featureProfile);
			}
		}
	}

	// Return the bitfield of supported disc types.
	return discTypes;
}

/**
 * Convert an MMC feature profile to a CD_DiscType_t.
 * @param featureProfile MMC feature profile.
 * @return CD_DiscType_t.
 */
CD_DiscType_t CdDrivePrivate::mmcFeatureProfileToDiscType(uint16_t featureProfile)
{
	switch (featureProfile) {
		case SCSI_MMC_PROFILE_MO_ERASABLE:	return DISC_TYPE_MO;
		case SCSI_MMC_PROFILE_MO_WRITE_ONCE:	return DISC_TYPE_MO;
		case SCSI_MMC_PROFILE_AS_MO:		return DISC_TYPE_MO;
		case SCSI_MMC_PROFILE_CDROM:		return DISC_TYPE_CDROM;
		case SCSI_MMC_PROFILE_CD_R:		return DISC_TYPE_CD_R;
		case SCSI_MMC_PROFILE_CD_RW:		return DISC_TYPE_CD_RW;
		case SCSI_MMC_PROFILE_DVDROM:		return DISC_TYPE_DVDROM;
		case SCSI_MMC_PROFILE_DVD_R:		return DISC_TYPE_DVD_R;
		case SCSI_MMC_PROFILE_DVD_RAM:		return DISC_TYPE_DVD_RAM;
		case SCSI_MMC_PROFILE_DVD_RW_RO:	return DISC_TYPE_DVD_RW;
		case SCSI_MMC_PROFILE_DVD_RW_SEQ:	return DISC_TYPE_DVD_RW;
		case SCSI_MMC_PROFILE_DVD_R_DL_SEQ:	return DISC_TYPE_DVD_R_DL;
		case SCSI_MMC_PROFILE_DVD_R_DL_JUMP:	return DISC_TYPE_DVD_R_DL;
		case SCSI_MMC_PROFILE_DVD_RW_DL:	return DISC_TYPE_DVD_RW_DL;
		case SCSI_MMC_PROFILE_DVD_PLUS_RW:	return DISC_TYPE_DVD_PLUS_RW;
		case SCSI_MMC_PROFILE_DVD_PLUS_R:	return DISC_TYPE_DVD_PLUS_R;
		case SCSI_MMC_PROFILE_DDCDROM:		return DISC_TYPE_DDCDROM;
		case SCSI_MMC_PROFILE_DDCD_R:		return DISC_TYPE_DDCD_R;
		case SCSI_MMC_PROFILE_DDCD_RW:		return DISC_TYPE_DDCD_RW;
		case SCSI_MMC_PROFILE_DVD_PLUS_RW_DL:	return DISC_TYPE_DVD_PLUS_RW_DL;
		case SCSI_MMC_PROFILE_DVD_PLUS_R_DL:	return DISC_TYPE_DVD_PLUS_R_DL;
		case SCSI_MMC_PROFILE_BDROM:		return DISC_TYPE_BDROM;
		case SCSI_MMC_PROFILE_BD_R_SEQ:		return DISC_TYPE_BD_R;
		case SCSI_MMC_PROFILE_BD_R_RND:		return DISC_TYPE_BD_R;
		case SCSI_MMC_PROFILE_BD_RE:		return DISC_TYPE_BD_RE;
		case SCSI_MMC_PROFILE_HDDVD:		return DISC_TYPE_HDDVD;
		case SCSI_MMC_PROFILE_HDDVD_R:		return DISC_TYPE_HDDVD_R;
		case SCSI_MMC_PROFILE_HDDVD_RAM:	return DISC_TYPE_HDDVD_RAM;
		case SCSI_MMC_PROFILE_HDDVD_RW:		return DISC_TYPE_HDDVD_RW;
		case SCSI_MMC_PROFILE_HDDVD_R_DL:	return DISC_TYPE_HDDVD_R_DL;
		case SCSI_MMC_PROFILE_HDDVD_RW_DL:	return DISC_TYPE_HDDVD_RW_DL;

		case 0x00:
		default:
			return DISC_TYPE_NONE;
	}
}

/**
 * Convert a bitfield of CD_DiscType_t to a CD_DriveType_t.
 * @param discTypes Bitfield of all CD_DiscType_t disc types.
 * @return CD_DriveType_t.
 */
CD_DriveType_t CdDrivePrivate::discTypesToDriveType(uint32_t discTypes)
{
	// TODO: Find various permutations like DVD/CD-RW.
	// Also, check for multi-format DVD±RW drives.
	// For now, just get the maximum disc type.
	if (discTypes & DISC_TYPE_MO)
		return DRIVE_TYPE_MO;
	else if (discTypes & DISC_TYPE_HDDVD_RW)
		return DRIVE_TYPE_HDDVD_RW;
	else if (discTypes & DISC_TYPE_HDDVD_R)
		return DRIVE_TYPE_HDDVD_R;
	else if (discTypes & DISC_TYPE_HDDVD) {
		// TODO: Check for CD/DVD writing capabilities.
		return DRIVE_TYPE_HDDVD;
	} else if (discTypes & DISC_TYPE_BD_RE)
		return DRIVE_TYPE_BD_RE;
	else if (discTypes & DISC_TYPE_BD_R)
		return DRIVE_TYPE_BD_R;
	else if (discTypes & DISC_TYPE_BDROM) {
		// TODO: Check for CD/DVD writing capabilities.
		return DRIVE_TYPE_BDROM;
	} else if (discTypes & DISC_TYPE_DVD_PLUS_RW_DL)
		return DRIVE_TYPE_DVD_PLUS_RW_DL;
	else if (discTypes & DISC_TYPE_DVD_PLUS_R_DL)
		return DRIVE_TYPE_DVD_PLUS_R_DL;
	else if (discTypes & (DISC_TYPE_DVD_PLUS_RW | DISC_TYPE_DVD_PLUS_R)) {
		// DVD+RW was released before DVD+R.
		// Hence, there's no such thing as a DVD+R-only drive.
		return DRIVE_TYPE_DVD_PLUS_RW;
	} else if (discTypes & DISC_TYPE_DVD_RAM)
		return DRIVE_TYPE_DVD_RAM;
	else if (discTypes & DISC_TYPE_DVD_RW)
		return DRIVE_TYPE_DVD_RW;
	else if (discTypes & DISC_TYPE_DVD_R)
		return DRIVE_TYPE_DVD_R;
	else if (discTypes & DISC_TYPE_DVDROM) {
		if (discTypes & (DISC_TYPE_CD_R | DISC_TYPE_CD_RW))
			return DRIVE_TYPE_DVD_CD_RW;
		else
			return DRIVE_TYPE_DVD;
	} else if (discTypes & DISC_TYPE_DDCD_RW)
		return DRIVE_TYPE_DDCD_RW;
	else if (discTypes & DISC_TYPE_DDCD_R)
		return DRIVE_TYPE_DDCD_R;
	else if (discTypes & DISC_TYPE_DDCDROM)
		return DRIVE_TYPE_DDCDROM;
	else if (discTypes & DISC_TYPE_CD_RW)
		return DRIVE_TYPE_CD_RW;
	else if (discTypes & DISC_TYPE_CD_R)
		return DRIVE_TYPE_CD_R;
	else if (discTypes & DISC_TYPE_CDROM)
		return DRIVE_TYPE_CDROM;
	else
		return DRIVE_TYPE_NONE;
}

/**
 * Read the Table of Contents into the cache.
 * @return 0 on success; non-zero on error.
 */
int CdDrivePrivate::readToc(void)
{
	// TODO: Check if the medium has been changed.
	if (!p_scsi->isDiscPresent()) {
		// No disc present.
		cache.toc.num_tracks = 0;
		cache.toc.toc.FirstTrackNumber = 0;
		cache.toc.toc.LastTrackNumber = 0;
		return 0;
	}

	// Read the TOC.
	int err = p_scsi->readToc(&cache.toc.toc, &cache.toc.num_tracks);
	if (err != 0) {
		// An error occurred.
		PRINT_SCSI_ERROR(SCSI_OP_READ_TOC, err);
		cache.toc.num_tracks = 0;
		cache.toc.toc.FirstTrackNumber = 0;
		cache.toc.toc.LastTrackNumber = 0;
		return -1;
	}

	// TOC has been read.
	return 0;
}

/** CdDrive **/

CdDrive::CdDrive(const string& filename)
	: d(new CdDrivePrivate(this, filename))
{ }

CdDrive::~CdDrive()
{
	delete d;
}

bool CdDrive::isOpen(void) const
{
	return d->p_scsi->isOpen();
}

void CdDrive::close(void)
{
	d->p_scsi->close();
}

std::string CdDrive::dev_vendor(void)
{
	if (!d->isInquirySuccessful())
		return "";
	return d->inq_data.vendor;
}

std::string CdDrive::dev_model(void)
{
	if (!d->isInquirySuccessful())
		return "";
	return d->inq_data.model;
}

std::string CdDrive::dev_firmware(void)
{
	if (!d->isInquirySuccessful())
		return "";
	return d->inq_data.firmware;
}

/**
 * Force a cache update.
 * NOTE: Currently required for SPTI, since the
 * MMC GET_EVENT_STATUS_NOTIFICATION command
 * isn't working properly, and WM_DEVICECHANGE
 * requires a window to receive notifications.
 */
void CdDrive::forceCacheUpdate(void)
{
	d->updateCache(true);
}

/**
 * Check if a disc is present.
 * @return True if a disc is present; false if not.
 */
bool CdDrive::isDiscPresent(void)
{
	return d->p_scsi->isDiscPresent();
}

/**
 * Get the current disc type.
 * @return Disc type.
 */
CD_DiscType_t CdDrive::getDiscType(void)
{
	d->updateCache();
	return d->cache.discType;
}

/**
 * Get the current drive type.
 * @return Drive type.
 */
CD_DriveType_t CdDrive::getDriveType(void)
{
	if (!d->isInquirySuccessful())
		return DRIVE_TYPE_NONE;
	return d->inq_data.driveType;
}

}