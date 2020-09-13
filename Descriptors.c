#include "Descriptors.h"

// HID Descriptors.
const USB_Descriptor_HIDReport_Datatype_t PROGMEM JoystickReport[] = {
  HID_RI_USAGE_PAGE(8,1), /* Generic Desktop */
  HID_RI_USAGE(8,5), /* Joystick */
  HID_RI_COLLECTION(8,1), /* Application */
    // Buttons (2 bytes)
    HID_RI_LOGICAL_MINIMUM(8,0),
    HID_RI_LOGICAL_MAXIMUM(8,1),
    HID_RI_PHYSICAL_MINIMUM(8,0),
    HID_RI_PHYSICAL_MAXIMUM(8,1),
    // The Switch will allow us to expand the original HORI descriptors to a full 16 buttons.
    // The Switch will make use of 14 of those buttons.
    HID_RI_REPORT_SIZE(8,1),
    HID_RI_REPORT_COUNT(8,16),
    HID_RI_USAGE_PAGE(8,9),
    HID_RI_USAGE_MINIMUM(8,1),
    HID_RI_USAGE_MAXIMUM(8,16),
    HID_RI_INPUT(8,2),
    // HAT Switch (1 nibble)
    HID_RI_USAGE_PAGE(8,1),
    HID_RI_LOGICAL_MAXIMUM(8,7),
    HID_RI_PHYSICAL_MAXIMUM(16,315),
    HID_RI_REPORT_SIZE(8,4),
    HID_RI_REPORT_COUNT(8,1),
    HID_RI_UNIT(8,20),
    HID_RI_USAGE(8,57),
    HID_RI_INPUT(8,66),
    // There's an additional nibble here that's utilized as part of the Switch Pro Controller.
    // I believe this -might- be separate U/D/L/R bits on the Switch Pro Controller, as they're utilized as four button descriptors on the Switch Pro Controller.
    HID_RI_UNIT(8,0),
    HID_RI_REPORT_COUNT(8,1),
    HID_RI_INPUT(8,1),
    // Joystick (4 bytes)
    HID_RI_LOGICAL_MAXIMUM(16,255),
    HID_RI_PHYSICAL_MAXIMUM(16,255),
    HID_RI_USAGE(8,48),
    HID_RI_USAGE(8,49),
    HID_RI_USAGE(8,50),
    HID_RI_USAGE(8,53),
    HID_RI_REPORT_SIZE(8,8),
    HID_RI_REPORT_COUNT(8,4),
    HID_RI_INPUT(8,2),
    // ??? Vendor Specific (1 byte)
    // This byte requires additional investigation.
    HID_RI_USAGE_PAGE(16,65280),
    HID_RI_USAGE(8,32),
    HID_RI_REPORT_COUNT(8,1),
    HID_RI_INPUT(8,2),
    // Output (8 bytes)
    // Original observation of this suggests it to be a mirror of the inputs that we sent.
    // The Switch requires us to have these descriptors available.
    HID_RI_USAGE(16,9761),
    HID_RI_REPORT_COUNT(8,8),
    HID_RI_OUTPUT(8,2),
  HID_RI_END_COLLECTION(0),
};

// Device Descriptor Structure
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
  .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

  .USBSpecification       = VERSION_BCD(2,0,0),
  .Class                  = USB_CSCP_NoDeviceClass,
  .SubClass               = USB_CSCP_NoDeviceSubclass,
  .Protocol               = USB_CSCP_NoDeviceProtocol,

  .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

  .VendorID               = 0x0F0D,
  .ProductID              = 0x0092,
  .ReleaseNumber          = VERSION_BCD(1,0,0),

  .ManufacturerStrIndex   = STRING_ID_Manufacturer,
  .ProductStrIndex        = STRING_ID_Product,
  .SerialNumStrIndex      = NO_DESCRIPTOR,

  .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

// Configuration Descriptor Structure
const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
  .Config =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

      .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
      .TotalInterfaces        = 1,

      .ConfigurationNumber    = 1,
      .ConfigurationStrIndex  = NO_DESCRIPTOR,

      .ConfigAttributes       = 0x80,

      .MaxPowerConsumption    = USB_CONFIG_POWER_MA(500)
    },

  .HID_Interface =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

      .InterfaceNumber        = INTERFACE_ID_Joystick,
      .AlternateSetting       = 0x00,

      .TotalEndpoints         = 2,

      .Class                  = HID_CSCP_HIDClass,
      .SubClass               = HID_CSCP_NonBootSubclass,
      .Protocol               = HID_CSCP_NonBootProtocol,

      .InterfaceStrIndex      = NO_DESCRIPTOR
    },

  .HID_JoystickHID =
    {
      .Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID},

      .HIDSpec                = VERSION_BCD(1,1,1),
      .CountryCode            = 0x00,
      .TotalReportDescriptors = 1,
      .HIDReportType          = HID_DTYPE_Report,
      .HIDReportLength        = sizeof(JoystickReport)
    },

  .HID_ReportINEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = JOYSTICK_IN_EPADDR,
      .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = JOYSTICK_EPSIZE,
      .PollingIntervalMS      = 0x04
    },

  .HID_ReportOUTEndpoint =
    {
      .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

      .EndpointAddress        = JOYSTICK_OUT_EPADDR,
      .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
      .EndpointSize           = JOYSTICK_EPSIZE,
      .PollingIntervalMS      = 0x10
    },
};


/** Device descriptor structure. This descriptor, located in FLASH memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const uint8_t PROGMEM DeviceDescriptorX[] =
{
  0x12,        // bLength
  0x01,        // bDescriptorType (Device)
  0x00, 0x02,  // bcdUSB 2.00
  0xFF,        // bDeviceClass 
  0xFF,        // bDeviceSubClass 
  0xFF,        // bDeviceProtocol 
  0x40,        // bMaxPacketSize0 64
  0x5E, 0x04,  // idVendor 0x045E
  0x8E, 0x02,  // idProduct 0x028E
  0x14, 0x01,  // bcdDevice 2.14
  0x01,        // iManufacturer (String Index)
  0x02,        // iProduct (String Index)
  0x03,        // iSerialNumber (String Index)
  0x01        // bNumConfigurations 1
};

/** Configuration descriptor structure. This descriptor, located in FLASH memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const uint8_t PROGMEM ConfigurationDescriptorX[] =
{
  0x09,        // bLength
  0x02,        // bDescriptorType (Configuration)
  0x99, 0x00,  // wTotalLength 153
  0x04,        // bNumInterfaces 4
  0x01,        // bConfigurationValue
  0x00,        // iConfiguration (String Index)
  0xA0,        // bmAttributes Remote Wakeup
  0xFA,        // bMaxPower 500mA
  
  0x09,        // bLength
  0x04,        // bDescriptorType (Interface)
  0x00,        // bInterfaceNumber 0
  0x00,        // bAlternateSetting
  0x02,        // bNumEndpoints 2
  0xFF,        // bInterfaceClass
  0x5D,        // bInterfaceSubClass
  0x01,        // bInterfaceProtocol
  0x00,        // iInterface (String Index)
  
  0x11,        // bLength
  0x21,        // bDescriptorType (HID)
  0x00, 0x01,  // bcdHID 1.00
  0x01,        // bCountryCode
  0x25,        // bNumDescriptors
  0x81,        // bDescriptorType[0] (Unknown 0x81)
  0x14, 0x00,  // wDescriptorLength[0] 20
  0x00,        // bDescriptorType[1] (Unknown 0x00)
  0x00, 0x00,  // wDescriptorLength[1] 0
  0x13,        // bDescriptorType[2] (Unknown 0x13)
  0x01, 0x08,  // wDescriptorLength[2] 2049
  0x00,        // bDescriptorType[3] (Unknown 0x00)
  0x00, 
  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x81,        // bEndpointAddress (IN/D2H)
  0x03,        // bmAttributes (Interrupt)
  0x20, 0x00,  // wMaxPacketSize 32
  0x04,        // bInterval 4 (unit depends on device speed)
  
  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x01,        // bEndpointAddress (OUT/H2D)
  0x03,        // bmAttributes (Interrupt)
  0x20, 0x00,  // wMaxPacketSize 32
  0x08,        // bInterval 8 (unit depends on device speed)
  
  0x09,        // bLength
  0x04,        // bDescriptorType (Interface)
  0x01,        // bInterfaceNumber 1
  0x00,        // bAlternateSetting
  0x04,        // bNumEndpoints 4
  0xFF,        // bInterfaceClass
  0x5D,        // bInterfaceSubClass
  0x03,        // bInterfaceProtocol
  0x00,        // iInterface (String Index)
  
  0x1B,        // bLength
  0x21,        // bDescriptorType (HID)
  0x00, 0x01,  // bcdHID 1.00
  0x01,        // bCountryCode
  0x01,        // bNumDescriptors
  0x82,        // bDescriptorType[0] (Unknown 0x82)
  0x40, 0x01,  // wDescriptorLength[0] 320
  0x02, 0x20, 0x16, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x82,        // bEndpointAddress (IN/D2H)
  0x03,        // bmAttributes (Interrupt)
  0x20, 0x00,  // wMaxPacketSize 32
  0x02,        // bInterval 2 (unit depends on device speed)
  
  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x02,        // bEndpointAddress (OUT/H2D)
  0x03,        // bmAttributes (Interrupt)
  0x20, 0x00,  // wMaxPacketSize 32
  0x04,        // bInterval 4 (unit depends on device speed)
  
  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x83,        // bEndpointAddress (IN/D2H)
  0x03,        // bmAttributes (Interrupt)
  0x20, 0x00,  // wMaxPacketSize 32
  0x40,        // bInterval 64 (unit depends on device speed)
  
  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x03,        // bEndpointAddress (OUT/H2D)
  0x03,        // bmAttributes (Interrupt)
  0x20, 0x00,  // wMaxPacketSize 32
  0x10,        // bInterval 16 (unit depends on device speed)
  
  0x09,        // bLength
  0x04,        // bDescriptorType (Interface)
  0x02,        // bInterfaceNumber 2
  0x00,        // bAlternateSetting
  0x01,        // bNumEndpoints 1
  0xFF,        // bInterfaceClass
  0x5D,        // bInterfaceSubClass
  0x02,        // bInterfaceProtocol
  0x00,        // iInterface (String Index)
  
  0x09,        // bLength
  0x21,        // bDescriptorType (HID)
  0x00, 0x01,  // bcdHID 1.00
  0x01,        // bCountryCode
  0x22,        // bNumDescriptors
  0x84,        // bDescriptorType[0] (Unknown 0x84)
  0x07, 0x00,  // wDescriptorLength[0] 7
  
  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x84,        // bEndpointAddress (IN/D2H)
  0x03,        // bmAttributes (Interrupt)
  0x20, 0x00,  // wMaxPacketSize 32
  0x10,        // bInterval 16 (unit depends on device speed)
  
  0x09,        // bLength
  0x04,        // bDescriptorType (Interface)
  0x03,        // bInterfaceNumber 3
  0x00,        // bAlternateSetting
  0x00,        // bNumEndpoints 0
  0xFF,        // bInterfaceClass
  0xFD,        // bInterfaceSubClass
  0x13,        // bInterfaceProtocol
  0x04,        // iInterface (String Index)
  
  0x06,        // bLength
  0x41,        // bDescriptorType (Unknown)
  0x00, 0x01, 0x01, 0x03
};

const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
const USB_Descriptor_String_t PROGMEM VersionString = USB_STRING_DESCRIPTOR(L"1.0");

const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"HORI CO.,LTD.");
const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"POKKEN CONTROLLER");
const USB_Descriptor_String_t PROGMEM ManufacturerStringX = USB_STRING_DESCRIPTOR(L"kadevice.net");
const USB_Descriptor_String_t PROGMEM ProductStringX = USB_STRING_DESCRIPTOR(L"KADE - Kick Ass Dynamic Encoder");

// USB Device Callback - Get Descriptor
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
const uint8_t  DescriptorType   = (wValue >> 8);
const uint8_t  DescriptorNumber = (wValue & 0xFF);

const void* Address = NULL;
uint16_t    Size    = NO_DESCRIPTOR;

  switch (DescriptorType)
  {
     case DTYPE_Device:
      Address = xinput? &DeviceDescriptorX:&DeviceDescriptor;
      Size    = xinput? sizeof(DeviceDescriptorX):sizeof(USB_Descriptor_Device_t);
      break;
    case DTYPE_Configuration:
      Address = xinput? &ConfigurationDescriptorX:&ConfigurationDescriptor;
      Size    = xinput? sizeof(ConfigurationDescriptorX):sizeof(USB_Descriptor_Configuration_t);
      break;
    case DTYPE_String:
      switch (DescriptorNumber)
      {
        case STRING_ID_Language:
          Address = &LanguageString;
          Size    = pgm_read_byte(&LanguageString.Header.Size);
          break;
        case STRING_ID_Manufacturer:
          Address = xinput? &ManufacturerStringX:&ManufacturerString;
          Size    = xinput? pgm_read_byte(&ManufacturerStringX.Header.Size):pgm_read_byte(&ManufacturerString.Header.Size);
          break;
        case STRING_ID_Product:
          Address = xinput? &ProductStringX:&ProductString;
          Size    = xinput ? pgm_read_byte(&ProductStringX.Header.Size):pgm_read_byte(&ProductString.Header.Size);
          break;
        case STRING_ID_Version:
          Address = &VersionString;
          Size    = pgm_read_byte(&VersionString.Header.Size);
          break;
    }
      break;
    case DTYPE_HID:
      Address = &ConfigurationDescriptor.HID_JoystickHID;
      Size    = sizeof(USB_HID_Descriptor_HID_t);
      break;
    case DTYPE_Report:
      Address = &JoystickReport;
      Size    = sizeof(JoystickReport);
      break;
  }

  *DescriptorAddress = Address;
  return Size;
 
}

