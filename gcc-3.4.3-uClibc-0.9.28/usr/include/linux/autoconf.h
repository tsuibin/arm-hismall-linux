/*
 * Automatically generated C config: don't edit
 * Linux kernel version: 2.6.14-rc5-rt7
 * Mon Nov  5 18:40:12 2007
 */
#define AUTOCONF_INCLUDED
#define CONFIG_ARM 1
#define CONFIG_MMU 1
#define CONFIG_UID16 1
#define CONFIG_GENERIC_HARDIRQS 1
#define CONFIG_RWSEM_GENERIC_SPINLOCK 1
#define CONFIG_GENERIC_CALIBRATE_DELAY 1

/*
 * Code maturity level options
 */
#define CONFIG_EXPERIMENTAL 1
#define CONFIG_CLEAN_COMPILE 1
#define CONFIG_BROKEN_ON_SMP 1
#define CONFIG_LOCK_KERNEL 1
#define CONFIG_INIT_ENV_ARG_LIMIT 32

/*
 * General setup
 */
#define CONFIG_LOCALVERSION ""
#define CONFIG_LOCALVERSION_AUTO 1
#define CONFIG_SWAP 1
#define CONFIG_SYSVIPC 1
#undef CONFIG_POSIX_MQUEUE
#undef CONFIG_BSD_PROCESS_ACCT
#define CONFIG_SYSCTL 1
#undef CONFIG_AUDIT
#undef CONFIG_HOTPLUG
#define CONFIG_KOBJECT_UEVENT 1
#undef CONFIG_IKCONFIG
#define CONFIG_INITRAMFS_SOURCE ""
#define CONFIG_EMBEDDED 1
#define CONFIG_KALLSYMS 1
#undef CONFIG_KALLSYMS_EXTRA_PASS
#define CONFIG_PRINTK 1
#define CONFIG_BUG 1
#define CONFIG_BASE_FULL 1
#define CONFIG_FUTEX 1
#define CONFIG_EPOLL 1
#define CONFIG_CC_OPTIMIZE_FOR_SIZE 1
#define CONFIG_SHMEM 1
#define CONFIG_CC_ALIGN_FUNCTIONS 0
#define CONFIG_CC_ALIGN_LABELS 0
#define CONFIG_CC_ALIGN_LOOPS 0
#define CONFIG_CC_ALIGN_JUMPS 0
#undef CONFIG_TINY_SHMEM
#define CONFIG_BASE_SMALL 0

/*
 * Loadable module support
 */
#undef CONFIG_MODULES

/*
 * System Type
 */
#undef CONFIG_ARCH_CLPS7500
#define CONFIG_ARCH_CLPS711X 1
#undef CONFIG_ARCH_CO285
#undef CONFIG_ARCH_EBSA110
#undef CONFIG_ARCH_CAMELOT
#undef CONFIG_ARCH_FOOTBRIDGE
#undef CONFIG_ARCH_INTEGRATOR
#undef CONFIG_ARCH_IOP3XX
#undef CONFIG_ARCH_IXP4XX
#undef CONFIG_ARCH_IXP2000
#undef CONFIG_ARCH_L7200
#undef CONFIG_ARCH_PXA
#undef CONFIG_ARCH_RPC
#undef CONFIG_ARCH_SA1100
#undef CONFIG_ARCH_S3C2410
#undef CONFIG_ARCH_SHARK
#undef CONFIG_ARCH_LH7A40X
#undef CONFIG_ARCH_OMAP
#undef CONFIG_ARCH_VERSATILE
#undef CONFIG_ARCH_IMX
#undef CONFIG_ARCH_H720X
#undef CONFIG_ARCH_AAEC2000

/*
 * CLPS711X/EP721X Implementations
 */
#undef CONFIG_ARCH_AUTCPU12
#undef CONFIG_ARCH_CDB89712
#undef CONFIG_ARCH_CEIVA
#undef CONFIG_ARCH_CLEP7312
#define CONFIG_ARCH_EDB7211 1
#undef CONFIG_ARCH_P720T
#undef CONFIG_ARCH_FORTUNET
#define CONFIG_ARCH_EP7211 1
#undef CONFIG_EP72XX_ROM_BOOT

/*
 * Processor Type
 */
#define CONFIG_CPU_32 1
#define CONFIG_CPU_ARM720T 1
#define CONFIG_CPU_32v4 1
#define CONFIG_CPU_ABRT_LV4T 1
#define CONFIG_CPU_CACHE_V4 1
#define CONFIG_CPU_CACHE_VIVT 1
#define CONFIG_CPU_COPY_V4WT 1
#define CONFIG_CPU_TLB_V4WT 1

/*
 * Processor Features
 */
#define CONFIG_ARM_THUMB 1

/*
 * Bus support
 */
#define CONFIG_ISA 1
#define CONFIG_ISA_DMA_API 1

/*
 * PCCARD (PCMCIA/CardBus) support
 */
#undef CONFIG_PCCARD

/*
 * Kernel Features
 */
#undef CONFIG_PREEMPT_NONE
#undef CONFIG_PREEMPT_VOLUNTARY
#undef CONFIG_PREEMPT_DESKTOP
#define CONFIG_PREEMPT_RT 1
#define CONFIG_PREEMPT 1
#define CONFIG_PREEMPT_SOFTIRQS 1
#define CONFIG_PREEMPT_HARDIRQS 1
#define CONFIG_PREEMPT_BKL 1
#define CONFIG_PREEMPT_RCU 1
#define CONFIG_RCU_STATS 1
#define CONFIG_RCU_TORTURE_TEST 1
#undef CONFIG_NO_IDLE_HZ
#define CONFIG_ARCH_DISCONTIGMEM_ENABLE 1
#define CONFIG_SELECT_MEMORY_MODEL 1
#undef CONFIG_FLATMEM_MANUAL
#define CONFIG_DISCONTIGMEM_MANUAL 1
#undef CONFIG_SPARSEMEM_MANUAL
#define CONFIG_DISCONTIGMEM 1
#define CONFIG_FLAT_NODE_MEM_MAP 1
#define CONFIG_NEED_MULTIPLE_NODES 1
#undef CONFIG_SPARSEMEM_STATIC
#define CONFIG_ALIGNMENT_TRAP 1

/*
 * Boot options
 */
#define CONFIG_ZBOOT_ROM_TEXT 0x0
#define CONFIG_ZBOOT_ROM_BSS 0x0
#define CONFIG_CMDLINE ""
#undef CONFIG_XIP_KERNEL

/*
 * Floating point emulation
 */

/*
 * At least one emulation must be selected
 */
#undef CONFIG_FPE_NWFPE
#undef CONFIG_FPE_FASTFPE

/*
 * Userspace binary formats
 */
#define CONFIG_BINFMT_ELF 1
#undef CONFIG_BINFMT_AOUT
#undef CONFIG_BINFMT_MISC
#undef CONFIG_ARTHUR

/*
 * Power management options
 */
#undef CONFIG_PM

/*
 * Networking
 */
#define CONFIG_NET 1

/*
 * Networking options
 */
#define CONFIG_PACKET 1
#undef CONFIG_PACKET_MMAP
#define CONFIG_UNIX 1
#undef CONFIG_NET_KEY
#define CONFIG_INET 1
#undef CONFIG_IP_MULTICAST
#undef CONFIG_IP_ADVANCED_ROUTER
#define CONFIG_IP_FIB_HASH 1
#undef CONFIG_IP_PNP
#undef CONFIG_NET_IPIP
#undef CONFIG_NET_IPGRE
#undef CONFIG_ARPD
#undef CONFIG_SYN_COOKIES
#undef CONFIG_INET_AH
#undef CONFIG_INET_ESP
#undef CONFIG_INET_IPCOMP
#undef CONFIG_INET_TUNNEL
#define CONFIG_INET_DIAG 1
#define CONFIG_INET_TCP_DIAG 1
#undef CONFIG_TCP_CONG_ADVANCED
#define CONFIG_TCP_CONG_BIC 1
#undef CONFIG_IPV6
#undef CONFIG_NETFILTER

/*
 * DCCP Configuration (EXPERIMENTAL)
 */
#undef CONFIG_IP_DCCP

/*
 * SCTP Configuration (EXPERIMENTAL)
 */
#undef CONFIG_IP_SCTP
#undef CONFIG_ATM
#undef CONFIG_BRIDGE
#undef CONFIG_VLAN_8021Q
#undef CONFIG_DECNET
#undef CONFIG_LLC2
#undef CONFIG_IPX
#undef CONFIG_ATALK
#undef CONFIG_X25
#undef CONFIG_LAPB
#undef CONFIG_NET_DIVERT
#undef CONFIG_ECONET
#undef CONFIG_WAN_ROUTER
#undef CONFIG_NET_SCHED
#undef CONFIG_NET_CLS_ROUTE

/*
 * Network testing
 */
#undef CONFIG_NET_PKTGEN
#undef CONFIG_HAMRADIO
#undef CONFIG_IRDA
#undef CONFIG_BT
#undef CONFIG_IEEE80211

/*
 * Device Drivers
 */

/*
 * Generic Driver Options
 */
#define CONFIG_STANDALONE 1
#define CONFIG_PREVENT_FIRMWARE_BUILD 1
#undef CONFIG_FW_LOADER

/*
 * Memory Technology Devices (MTD)
 */
#undef CONFIG_MTD

/*
 * Parallel port support
 */
#undef CONFIG_PARPORT

/*
 * Plug and Play support
 */
#undef CONFIG_PNP

/*
 * Block devices
 */
#undef CONFIG_BLK_DEV_XD
#undef CONFIG_BLK_DEV_COW_COMMON
#undef CONFIG_BLK_DEV_LOOP
#undef CONFIG_BLK_DEV_NBD
#define CONFIG_BLK_DEV_RAM 1
#define CONFIG_BLK_DEV_RAM_COUNT 16
#define CONFIG_BLK_DEV_RAM_SIZE 4096
#define CONFIG_BLK_DEV_INITRD 1
#undef CONFIG_CDROM_PKTCDVD

/*
 * IO Schedulers
 */
#define CONFIG_IOSCHED_NOOP 1
#define CONFIG_IOSCHED_AS 1
#define CONFIG_IOSCHED_DEADLINE 1
#define CONFIG_IOSCHED_CFQ 1
#undef CONFIG_ATA_OVER_ETH

/*
 * SCSI device support
 */
#undef CONFIG_RAID_ATTRS
#undef CONFIG_SCSI

/*
 * Multi-device support (RAID and LVM)
 */
#undef CONFIG_MD

/*
 * Fusion MPT device support
 */
#undef CONFIG_FUSION

/*
 * IEEE 1394 (FireWire) support
 */

/*
 * I2O device support
 */

/*
 * Network device support
 */
#define CONFIG_NETDEVICES 1
#undef CONFIG_DUMMY
#undef CONFIG_BONDING
#undef CONFIG_EQUALIZER
#undef CONFIG_TUN

/*
 * ARCnet devices
 */
#undef CONFIG_ARCNET

/*
 * PHY device support
 */

/*
 * Ethernet (10 or 100Mbit)
 */
#undef CONFIG_NET_ETHERNET

/*
 * Ethernet (1000 Mbit)
 */

/*
 * Ethernet (10000 Mbit)
 */

/*
 * Token Ring devices
 */
#undef CONFIG_TR

/*
 * Wireless LAN (non-hamradio)
 */
#undef CONFIG_NET_RADIO

/*
 * Wan interfaces
 */
#undef CONFIG_WAN
#undef CONFIG_PPP
#undef CONFIG_SLIP
#undef CONFIG_SHAPER
#undef CONFIG_NETCONSOLE
#undef CONFIG_NETPOLL
#undef CONFIG_NET_POLL_CONTROLLER

/*
 * ISDN subsystem
 */
#undef CONFIG_ISDN

/*
 * Input device support
 */
#undef CONFIG_INPUT

/*
 * Hardware I/O ports
 */
#define CONFIG_SERIO 1
#define CONFIG_SERIO_SERPORT 1
#define CONFIG_SERIO_LIBPS2 1
#undef CONFIG_SERIO_RAW
#undef CONFIG_GAMEPORT

/*
 * Character devices
 */
#undef CONFIG_VT
#undef CONFIG_SERIAL_NONSTANDARD

/*
 * Serial drivers
 */
#undef CONFIG_SERIAL_8250

/*
 * Non-8250 serial port support
 */
#define CONFIG_SERIAL_CLPS711X 1
#define CONFIG_SERIAL_CLPS711X_CONSOLE 1
#define CONFIG_SERIAL_CORE 1
#define CONFIG_SERIAL_CORE_CONSOLE 1
#define CONFIG_UNIX98_PTYS 1
#define CONFIG_LEGACY_PTYS 1
#define CONFIG_LEGACY_PTY_COUNT 256

/*
 * IPMI
 */
#undef CONFIG_IPMI_HANDLER

/*
 * Watchdog Cards
 */
#undef CONFIG_WATCHDOG
#undef CONFIG_NVRAM
#undef CONFIG_RTC
#define CONFIG_BLOCKER 1
#define CONFIG_LPPTEST 1
#undef CONFIG_DTLK
#undef CONFIG_R3964

/*
 * Ftape, the floppy tape device driver
 */
#undef CONFIG_RAW_DRIVER

/*
 * TPM devices
 */

/*
 * I2C support
 */
#undef CONFIG_I2C

/*
 * Hardware Monitoring support
 */
#define CONFIG_HWMON 1
#undef CONFIG_HWMON_VID
#undef CONFIG_HWMON_DEBUG_CHIP

/*
 * Misc devices
 */

/*
 * Multimedia Capabilities Port drivers
 */

/*
 * Multimedia devices
 */
#undef CONFIG_VIDEO_DEV

/*
 * Digital Video Broadcasting Devices
 */
#undef CONFIG_DVB

/*
 * Graphics support
 */
#undef CONFIG_FB

/*
 * Sound
 */
#undef CONFIG_SOUND

/*
 * USB support
 */
#define CONFIG_USB_ARCH_HAS_HCD 1
#undef CONFIG_USB_ARCH_HAS_OHCI
#undef CONFIG_USB

/*
 * USB Gadget Support
 */
#undef CONFIG_USB_GADGET

/*
 * MMC/SD Card support
 */
#undef CONFIG_MMC

/*
 * File systems
 */
#define CONFIG_EXT2_FS 1
#undef CONFIG_EXT2_FS_XATTR
#undef CONFIG_EXT2_FS_XIP
#undef CONFIG_EXT3_FS
#undef CONFIG_JBD
#undef CONFIG_REISERFS_FS
#undef CONFIG_JFS_FS
#undef CONFIG_FS_POSIX_ACL
#undef CONFIG_XFS_FS
#define CONFIG_MINIX_FS 1
#undef CONFIG_ROMFS_FS
#define CONFIG_INOTIFY 1
#undef CONFIG_QUOTA
#define CONFIG_DNOTIFY 1
#undef CONFIG_AUTOFS_FS
#undef CONFIG_AUTOFS4_FS
#undef CONFIG_FUSE_FS

/*
 * CD-ROM/DVD Filesystems
 */
#undef CONFIG_ISO9660_FS
#undef CONFIG_UDF_FS

/*
 * DOS/FAT/NT Filesystems
 */
#undef CONFIG_MSDOS_FS
#undef CONFIG_VFAT_FS
#undef CONFIG_NTFS_FS

/*
 * Pseudo filesystems
 */
#define CONFIG_PROC_FS 1
#define CONFIG_SYSFS 1
#undef CONFIG_TMPFS
#undef CONFIG_HUGETLB_PAGE
#define CONFIG_RAMFS 1
#undef CONFIG_RELAYFS_FS

/*
 * Miscellaneous filesystems
 */
#undef CONFIG_ADFS_FS
#undef CONFIG_AFFS_FS
#undef CONFIG_HFS_FS
#undef CONFIG_HFSPLUS_FS
#undef CONFIG_BEFS_FS
#undef CONFIG_BFS_FS
#undef CONFIG_EFS_FS
#undef CONFIG_CRAMFS
#undef CONFIG_VXFS_FS
#undef CONFIG_HPFS_FS
#undef CONFIG_QNX4FS_FS
#undef CONFIG_SYSV_FS
#undef CONFIG_UFS_FS

/*
 * Network File Systems
 */
#undef CONFIG_NFS_FS
#undef CONFIG_NFSD
#undef CONFIG_SMB_FS
#undef CONFIG_CIFS
#undef CONFIG_NCP_FS
#undef CONFIG_CODA_FS
#undef CONFIG_AFS_FS
#undef CONFIG_9P_FS

/*
 * Partition Types
 */
#define CONFIG_PARTITION_ADVANCED 1
#undef CONFIG_ACORN_PARTITION
#undef CONFIG_OSF_PARTITION
#undef CONFIG_AMIGA_PARTITION
#undef CONFIG_ATARI_PARTITION
#undef CONFIG_MAC_PARTITION
#undef CONFIG_MSDOS_PARTITION
#undef CONFIG_LDM_PARTITION
#undef CONFIG_SGI_PARTITION
#undef CONFIG_ULTRIX_PARTITION
#undef CONFIG_SUN_PARTITION
#undef CONFIG_EFI_PARTITION

/*
 * Native Language Support
 */
#undef CONFIG_NLS

/*
 * Profiling support
 */
#undef CONFIG_PROFILING

/*
 * Kernel hacking
 */
#undef CONFIG_PRINTK_TIME
#undef CONFIG_PRINTK_IGNORE_LOGLEVEL
#undef CONFIG_DEBUG_KERNEL
#define CONFIG_LOG_BUF_SHIFT 14
#undef CONFIG_WAKEUP_TIMING
#undef CONFIG_CRITICAL_PREEMPT_TIMING
#undef CONFIG_CRITICAL_IRQSOFF_TIMING
#undef CONFIG_RT_DEADLOCK_DETECT
#undef CONFIG_DEBUG_RT_LOCKING_MODE
#undef CONFIG_DEBUG_BUGVERBOSE
#define CONFIG_FRAME_POINTER 1
#define CONFIG_DEBUG_USER 1

/*
 * Security options
 */
#undef CONFIG_KEYS
#undef CONFIG_SECURITY

/*
 * Cryptographic options
 */
#undef CONFIG_CRYPTO

/*
 * Hardware crypto devices
 */

/*
 * Library routines
 */
#undef CONFIG_CRC_CCITT
#undef CONFIG_CRC16
#define CONFIG_CRC32 1
#undef CONFIG_LIBCRC32C
