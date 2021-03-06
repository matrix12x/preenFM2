
#include "usbd_midi_core.h"
#include "usb_dcd.h"
#include "PreenFM.h"


// Usb midi buffer..
uint8_t midiBuff[4];


/*********************************************
 midi Device library callbacks
 *********************************************/
static uint8_t usbd_midi_Init(void *pdev, uint8_t cfgidx);
static uint8_t usbd_midi_DeInit(void *pdev, uint8_t cfgidx);
static uint8_t usbd_midi_Setup(void *pdev, USB_SETUP_REQ *req);
static uint8_t usbd_midi_EP0_RxReady(void *pdev);
static uint8_t usbd_midi_DataIn(void *pdev, uint8_t epnum);
static uint8_t usbd_midi_DataOut(void *pdev, uint8_t epnum);
static uint8_t usbd_midi_SOF(void *pdev);
static uint8_t usbd_midi_OUT_Incplt(void *pdev);
static uint8_t *usbd_midi_GetCfgDesc(uint8_t speed, uint16_t *length);

USBD_Class_cb_TypeDef midiCallback = {
		usbd_midi_Init,
		usbd_midi_DeInit,
		usbd_midi_Setup,
		(void*) 0, /* EP0_TxSent */
		usbd_midi_EP0_RxReady,
		usbd_midi_DataIn,
		usbd_midi_DataOut,
		usbd_midi_SOF,
		(void*) 0,
		usbd_midi_OUT_Incplt,
		usbd_midi_GetCfgDesc,
		usbd_midi_GetCfgDesc
};


#define CLASS_SPECIFIC_DESC_SIZE                7+6+9+9+5+9+5
#define MIDI_CONFIG_DESC_SIZE 					9+9+9+9+CLASS_SPECIFIC_DESC_SIZE
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_DEVICE_CLASS_AUDIO                  0x01
#define USB_DEVICE_SUBCLASS_MIDISTREAMING       0x03

/* USB AUDIO device Configuration Descriptor */
static uint8_t usbd_audio_CfgDesc[MIDI_CONFIG_DESC_SIZE] =
{
	/* Configuration 1 */
	0x09,                                 /* bLength */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,    /* bDescriptorType */
	LOBYTE(MIDI_CONFIG_DESC_SIZE),        /* wTotalLength  */
	HIBYTE(MIDI_CONFIG_DESC_SIZE),
	0x02,                                 /* bNumInterfaces */
	0x01,                                 /* bConfigurationValue */
	0x00,                                 /* iConfiguration */
	0xC0,                                 /* bmAttributes  = b6 + b7 : self powered */
	0x32,                                 /* bMaxPower = 150 mA*/
	/* 09 byte*/

	// B.3.1 Standard AC Interface Descriptor
	/* Audio Control standard */
	0x9,         /* sizeof(usbDescrInterface): length of descriptor in bytes */
	0x04,         /* interface descriptor type */
	0x00,         /* index of this interface */
	0x00,         /* alternate setting for this interface */
	0x00,         /* endpoints excl 0: number of endpoint descriptors to follow */
	0x01,         /* AUDIO */
	0x01,         /* AUDIO_Control*/
	0x00,         /* */
	0x00,         /* string index for interface */

	// B.3.2 Class-specific AC Interface Descriptor
	/* AudioControl   Class-Specific descriptor */
	0x09,         /* sizeof(usbDescrCDC_HeaderFn): length of descriptor in bytes */
	0x24,         /* descriptor type */
	0x01,         /* header functional descriptor */
	0x0, 0x01,      /* bcdADC */
	0x09, 0x00,         /* wTotalLength */
	0x01,         /* */
	0x01,         /* */


	// B.4 MIDIStreaming Interface Descriptors

	// B.4.1 Standard MS Interface Descriptor
	/* PreenFM Standard interface descriptor */
	0x09,            /* bLength */
	0x04,        /* bDescriptorType */
	0x01,                                 /* bInterfaceNumber */
	0x00,                                 /* bAlternateSetting */
	0x02,                                 /* bNumEndpoints */
	USB_DEVICE_CLASS_AUDIO,               /* bInterfaceClass */
	USB_DEVICE_SUBCLASS_MIDISTREAMING,	/* bInterfaceSubClass : MIDIStreaming*/
	0x00,             					/* InterfaceProtocol: NOT USED */
	0x00,                                 /* iInterface : NOT USED*/
	/* 09 byte*/

	// B.4.2 Class-specific MS Interface Descriptor
	/* MS Class-Specific descriptor */
	0x07,         /* length of descriptor in bytes */
	0x24,         /* descriptor type */
	0x01,         /* header functional descriptor */
	0x0, 0x01,      /* bcdADC */
	CLASS_SPECIFIC_DESC_SIZE, 0,         /* wTotalLength : Why 0x41 ?? XH : don't know... */

	// B.4.3 MIDI IN Jack Descriptor
	// Midi in Jack Descriptor (Embedded)
	0x06,         /* bLength */
	0x24,         /* descriptor type */
	0x02,         /* MIDI_IN_JACK desc subtype */
	0x01,         /* EMBEDDED bJackType */
	0x01,         /* bJackID */
	0x00,         /* UNUSEED */


	// Table B4.4
	// Midi Out Jack Descriptor (Embedded)
	0x09,         /* length of descriptor in bytes */
	0x24,         /* descriptor type */
	0x03,         /* MIDI_OUT_JACK descriptor */
	0x01,         /* EMBEDDED bJackType */
	0x02,         /* bJackID */
	0x01,         /* No of input pins */
	0x01,         /* ID of the Entity to which this Pin is connected. */
	0x01,         /* Output Pin number of the Entity to which this Input Pin is connected. */
	0X00,         /* iJack : UNUSED */


	// ===== B.5 Bulk OUT Endpoint Descriptors
	//B.5.1 Standard Bulk OUT Endpoint Descriptor
	0x09,      /* bLenght */
	0x05,   	/* bDescriptorType = endpoint */
	0x01,      /* bEndpointAddress OUT endpoint number 1 */
	0x02,         /* bmAttributes: 2:Bulk, 3:Interrupt endpoint */
	0x40, 0X00,         /* wMaxPacketSize 64 bytes per packet. */
	0x00,         /* bIntervall in ms : ignored for bulk*/
	0x00,         /* bRefresh Unused */
	0x00,         /* bSyncAddress Unused */

	// B.5.2 Class-specific MS Bulk OUT Endpoint Descriptor
	0x05,         /* bLength of descriptor in bytes */
	0x25,         /* bDescriptorType */
	0x01,         /* bDescriptorSubtype : GENERAL */
	0x01,         /* bNumEmbMIDIJack  */
	0x01,         /* baAssocJackID (0) ID of the Embedded MIDI IN Jack. */


	//B.6 Bulk IN Endpoint Descriptors
	//B.6.1 Standard Bulk IN Endpoint Descriptor
	0x09,         /* bLenght */
	0x05,   		/* bDescriptorType = endpoint */
	0x81,         /* bEndpointAddress IN endpoint number 1 */
	0X02,         /* bmAttributes: 2: Bulk, 3: Interrupt endpoint */
	0x40, 0X00,         /* wMaxPacketSize */
	0X00,         /* bIntervall in ms */
	0X00,         /* bRefresh */
	0X00,         /* bSyncAddress */

	// B.6.2 Class-specific MS Bulk IN Endpoint Descriptor
	0X05,         /* bLength of descriptor in bytes */
	0X25,         /* bDescriptorType */
	0x01,         /* bDescriptorSubtype */
	0X01,         /* bNumEmbMIDIJack (0) */
	0X02,         /* baAssocJackID (0) ID of the Embedded MIDI OUT Jack	*/

};



static uint8_t usbd_midi_Init(void *pdev, uint8_t cfgidx) {
	/* Open EP IN */
	DCD_EP_Open((USB_OTG_CORE_HANDLE *)pdev,
			  0x81,
			  128,
			  0x02);

	/* Open EP OUT */
	DCD_EP_Open((USB_OTG_CORE_HANDLE *)pdev,
			  0x1,
			  128,
			  0x02);

	// Prepare for next midi information
	DCD_EP_PrepareRx((USB_OTG_CORE_HANDLE *)pdev,
			0x1,
			(uint8_t*)midiBuff,
			4);

	return USBD_OK;
}

static uint8_t usbd_midi_DeInit(void *pdev, uint8_t cfgidx) {
	  /* Close MIDI EPs */
	  DCD_EP_Close ((USB_OTG_CORE_HANDLE *)pdev , 0x81);
	  DCD_EP_Close ((USB_OTG_CORE_HANDLE *)pdev , 0x1);

	  return USBD_OK;
}

static uint8_t usbd_midi_Setup(void *pdev, USB_SETUP_REQ *req) {
	return 0;
}
static uint8_t usbd_midi_EP0_RxReady(void *pdev) {
	return 0;
}

extern bool usbReady;
static uint8_t usbd_midi_DataIn(void *pdev, uint8_t epnum) {
    DCD_EP_Flush((USB_OTG_CORE_HANDLE *)pdev, 0x81);
    usbReady = true;
    return USBD_OK;
}

static uint8_t usbd_midi_DataOut(void *pdev, uint8_t epnum) {
    switch (midiBuff[0] & 0xf) {
    case 0x9:
    case 0x8:
    case 0xb:
    case 0xe:
    case 0x3:
// Sysex :
//    case 0x4:
//    case 0x7:
	    usartBuffer.insert(midiBuff[1]);
	    usartBuffer.insert(midiBuff[2]);
	    usartBuffer.insert(midiBuff[3]);
	    break;
// Sysex :
//    case 0x6:
    case 0x2:
        usartBuffer.insert(midiBuff[1]);
        usartBuffer.insert(midiBuff[2]);
        break;
    case 0x5:
    case 0xF:
        usartBuffer.insert(midiBuff[1]);
        break;
    }

	// Prepare for next midi information
	DCD_EP_PrepareRx((USB_OTG_CORE_HANDLE *)pdev,
			0x1,
			(uint8_t*)midiBuff,
			4);

	return USBD_OK;
}
static uint8_t usbd_midi_SOF(void *pdev) {
	return 0;
}
static uint8_t usbd_midi_OUT_Incplt(void *pdev) {
	return 0;
}
static uint8_t *usbd_midi_GetCfgDesc(uint8_t speed, uint16_t *length) {
	*length = sizeof (usbd_audio_CfgDesc);
	return usbd_audio_CfgDesc;
}


