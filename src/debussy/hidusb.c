#include <avr.h>
#include <chipselect.h>
#include <usbdrv/usbdrv.h>
#include <usbconfig.h>
#include <hidusb.h>

#define USB_PORT    PORTD
#define USB_DDR     DDRD
#define USB_D_PLUS  PD2
#define USB_D_MINUS PD4


PROGMEM const char usbHidReportDescriptor[29] = {    /* USB report descriptor */
        0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
        0x09, 0x01,                    // USAGE (Vendor Usage 1)
        0xa1, 0x01,                    // COLLECTION (Application)
        0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
        0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
        0x75, 0x08,                    //   REPORT_SIZE (8)
        0x95, 0x08,                    //   REPORT_COUNT (8)
        0x09, 0x00,                    //   USAGE (Undefined)
        0x82, 0x02, 0x01,              //   INPUT (Data,Var,Abs,Buf)
        0x95, 0x20,                     //   REPORT_COUNT (32)
        0x09, 0x00,                    //   USAGE (Undefined)
        0xb2, 0x02, 0x01,              //   FEATURE (Data,Var,Abs,Buf)
        0xc0                           // END_COLLECTION
};

/*
 * <hidusb> private
 */
usbMsgLen_t usbFunctionSetup(uchar data[8])
{
        return 0;   /* default for not implemented requests: return no data back to host */
}

/*
 * <hidusb> public
 */
void hidusb_sys_init()
{
        cli();
        USB_DDR = 0;
        USB_PORT = 0;
        usbInit();
        usbDeviceDisconnect();
        avr_wait(300);
        usbDeviceConnect();
        sei();
}

void hidusb_init(struct hidusb* self)
{
}

void hidusb_free(struct hidusb* self)
{
}

void hidusb_puts(struct hidusb* self, const char* s)
{
}

/*
 * <hidusb> test cases
 */
void hidusb_print_test()
{
        while(1) {
                wdt_reset();
                usbPoll();
        }
}
