// gpioctl.h    Include file for Generic Port I/O Example Driver
//
// Define the IOCTL codes we will use.  The IOCTL code contains a command
// identifier, plus other information about the device, the type of access
// with which the file must have been opened, and the type of buffering.
//
// Robert B. Nelson (Microsoft)     March 1, 1993

// Device type           -- in the "User Defined" range."
#define JAVOTE_TYPE 40000

// The IOCTL function codes from 0x800 to 0xFFF are for customer use.

#define IOCTL_JAVOTE_READ_PORT_UCHAR \
    CTL_CODE( JAVOTE_TYPE, 0x900, METHOD_BUFFERED, FILE_READ_ACCESS )

#define IOCTL_JAVOTE_READ_PORT_USHORT \
    CTL_CODE( JAVOTE_TYPE, 0x901, METHOD_BUFFERED, FILE_READ_ACCESS )

#define IOCTL_JAVOTE_READ_PORT_ULONG \
    CTL_CODE( JAVOTE_TYPE, 0x902, METHOD_BUFFERED, FILE_READ_ACCESS )

#define IOCTL_JAVOTE_WRITE_PORT_UCHAR \
    CTL_CODE(JAVOTE_TYPE,  0x910, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_JAVOTE_WRITE_PORT_USHORT \
    CTL_CODE(JAVOTE_TYPE,  0x911, METHOD_BUFFERED, FILE_WRITE_ACCESS)

#define IOCTL_JAVOTE_WRITE_PORT_ULONG \
    CTL_CODE(JAVOTE_TYPE,  0x912, METHOD_BUFFERED, FILE_WRITE_ACCESS)

typedef struct  _JAVOTE_WRITE_INPUT {
    ULONG   PortNumber;     // Port # to write to
    union   {               // Data to be output to port
        ULONG   LongData;
        USHORT  ShortData;
        UCHAR   CharData;
    };
}   JAVOTE_WRITE_INPUT;
