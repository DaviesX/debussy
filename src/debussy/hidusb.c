#ifndef ARCH_X86_64
#  include <stdio.h>
#  include <string.h>
#  include <avr.h>
#  include <chipselect.h>
#  include <usbdrv/usbdrv.h>
#  include <usbconfig.h>
#  include <hidusb.h>

#define USB_PORT    PORTD
#define USB_DDR     DDRD
#define USB_D_PLUS  PD2
#define USB_D_MINUS PD4


/*
 * <hidusb_request> public
 */
void hidusb_request_init(struct hidusb_request* self, uint8_t type, uint8_t len)
{
        self->report_id = 0x01;
        self->type = type;
        self->len = len;
}

/*
 * <hidusb> private
 */
PROGMEM const char usbHidReportDescriptor[] = {    /* USB report descriptor */
        0x06, 0x00, 0xFF,               // (GLOBAL) USAGE_PAGE         0xFF00 Vendor-defined
        0xA1, 0x01,                     // (MAIN)   COLLECTION         0x01 Application
        0x15, 0x00,                     //   (GLOBAL) LOGICAL_MINIMUM    0x00 (0)
        0x26, 0xFF, 0x00,               //   (GLOBAL) LOGICAL_MAXIMUM    0x00FF (255)
        0x75, 0x08,                     //   (GLOBAL) REPORT_SIZE        0x08 (8) Number of bits per field
        0x85, 0x01,                     //   (GLOBAL) REPORT_ID          0x01 (1)
        0x95, REPORT_BYTE_COUNT,        //   (GLOBAL) REPORT_COUNT       Number of fields
        0x09, 0x01,                     //   (LOCAL)  USAGE              0xFF000001
        0x81, 0x82,                     //   (MAIN)   INPUT              0x00000002 (64 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap
        0x85, 0x02,                     //   (GLOBAL) REPORT_ID          0x02 (2)
        0x09, 0x01,                     //   (LOCAL)  USAGE              0xFF000001
        0x91, 0x82,                     //   (MAIN)   OUTPUT             0x00000002 (64 fields x 8 bits) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap
        0xC0,                           // (MAIN)   END_COLLECTION     Application
};

static uint8_t                  g_consumed = 0;
static uint8_t                  g_num_bytes = 0;
static struct hidusb_request    g_request;


// usbFunctionRead() is called when the host requests a chunk of data from the device.
uint8_t usbFunctionRead(uint8_t *data, uchar len)
{
        if(len > g_num_bytes)
                len = g_num_bytes;

        uint8_t* local_buf = (uint8_t*) &g_request;
        uint8_t i;
        for (i = 0; i < len; i ++) {
                data[i] = local_buf[g_consumed + i];
        }

        g_consumed += len;
        g_num_bytes -= len;
        return len;
}

// usbFunctionWrite() is called when the host sends a chunk of data to the device.
uint8_t usbFunctionWrite(uint8_t *data, uint8_t len)
{
        if(g_num_bytes == 0)
                // end of transfer
                return 1;
        if(len > g_num_bytes)
                len = g_num_bytes;

        uint8_t* local_buf = (uint8_t*) &g_request;
        uint8_t i;
        for (i = 0; i < len; i ++) {
                local_buf[g_consumed + i] = data[i];
        }

        g_consumed += len;
        g_num_bytes -= len;

        // return 1 if this was the last chunk
        return g_num_bytes == 0;
}

usbMsgLen_t usbFunctionSetup(uint8_t data[8])
{
        g_consumed = 0;
        g_num_bytes = REPORT_BUFFER_SIZE;
        return USB_NO_MSG;
}

/*
 * <hidusb> public
 */
static bool g_dev_enabled;
static bool g_is_connected = false;

void hidusb_sys_init(bool dev_enable)
{
        g_dev_enabled = dev_enable;
        if (!hidusb_is_enabled())
                return ;
        cli();
        USB_DDR = 0;
        USB_PORT = 0;
        usbInit();
        sei();
}

void hidusb_connect()
{
        if (hidusb_is_connected() || !hidusb_is_enabled())
                return ;
        usbDeviceDisconnect(), avr_wait(300), usbDeviceConnect();
}

void hidusb_disconnect()
{
        if (!hidusb_is_connected() || !hidusb_is_enabled())
                return ;
        usbDeviceDisconnect();
}

bool hidusb_is_enabled()
{
        return g_dev_enabled;
}

bool hidusb_is_connected()
{
        return g_is_connected;
}

void hidusb_put_request(struct hidusb_request* request)
{
        if (!hidusb_is_connected() || !hidusb_is_enabled())
                return ;

        uint8_t* request_buf = (uint8_t*) &g_request;

        uint8_t consumed;
        uint8_t total;
        for (consumed = 0, total = sizeof(struct hidusb_request); consumed < total; ) {
                wdt_reset();
                hidusb_tick();
                if(usbInterruptIsReady()) {
                        uint8_t rest = total - consumed;
                        uint8_t len = rest > 8 ? 8 : rest;
                        usbSetInterrupt(&request_buf[consumed], len);
                        consumed += len;
                }
        }
        // Make sure the message is terminated.
        while (usbInterruptIsReady())
                usbSetInterrupt(&request_buf[consumed], 0);
}

void hidusb_tick()
{
        usbPoll();
}


#endif // ARCH_X86_64
