// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 2024-09-12
// 23:04:22
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include   "App.h"


static UX_SLAVE_CLASS_HID_PARAMETER hid_parameter;

//-----------------------------------------------------------------------------------------------
// USB HID Report Descriptor
//---------------------------------------------------------------------------------------------

static const UCHAR hid_report_descriptor_keyboard[] =
{ 
  0x05, 0x01,                           /* Usage Page (Generic Desktop)                    */
  0x09, 0x06,                           /* Usage (Keyboard)                                */
  0xA1, 0x01,                           /* Collection (Application)                        */
  0x05, 0x07,                           /* Usage Page (Key Codes)                          */
  0x19, 0xE0,                           /* Usage Minimum (Keyboard LeftControl)            */
  0x29, 0xE7,                           /* Usage Maximun (Keyboard Right GUI)              */
  0x15, 0x00,                           /* Logical Minimum (0)                             */
  0x25, 0x01,                           /* Logical Maximum (1)                             */
  0x75, 0x01,                           /* Report Size (1)                                 */
  0x95, 0x08,                           /* Report Count (8)                                */
  0x81, 0x02,                           /* Input (Data, Variable, Absolute); Modifier byte */
  0x95, 0x01,                           /* Report Count (1)                                */
  0x75, 0x08,                           /* Report Size (8)                                 */
  0x81, 0x03,                           /* Input (Constant); Reserved byte                 */
  0x95, 0x05,                           /* Report Count (5)                                */
  0x75, 0x01,                           /* Report Size (1)                                 */
  0x05, 0x08,                           /* Usage Page (Page# for LEDs)                     */
  0x19, 0x01,                           /* Usage Minimum (1)                               */
  0x29, 0x05,                           /* Usage Maximum (5)                               */
  0x91, 0x02,                           /* Output (Data, Variable, Absolute); LED report   */
  0x95, 0x01,                           /* Report Count (1)                                */
  0x75, 0x03,                           /* Report Size (3)                                 */
  0x91, 0x03,                           /* Output (Constant) ;LED report padding           */
  0x95, 0x06,                           /* Report Count (6)                                */
  0x75, 0x08,                           /* Report Size (8)                                 */
  0x15, 0x00,                           /* Logical Minimum (0)                             */
  0x25, 0x65,                           /* Logical Maximum(101)                            */
  0x05, 0x07,                           /* Usage Page (Key Codes)                          */
  0x19, 0x00,                           /* Usage Minimum (0)                               */
  0x29, 0x65,                           /* Usage Maximum (101)                             */
  0x81, 0x00,                           /* Input (Data, Array); Key arrays (6 bytes)       */
  0xc0                                  /* End Collection                                  */
};

static const UCHAR hid_report_descriptor_mouse[] =
{ 
  0x05, 0x01,                           /* Usage Page (Generic Desktop)                    */
  0x09, 0x02,                           /* Usage (Mouse)                                   */
  0xA1, 0x01,                           /* Collection (Application)                        */
  0x09, 0x01,                           /* Usage (Pointer)                                 */
  0xA1, 0x00,                           /* Collection (Physical)                           */
  0x05, 0x09,                           /* Usage Page (Buttons)                            */
  0x19, 0x01,                           /* Usage Minimum (01)                              */
  0x29, 0x03,                           /* Usage Maximun (03)                              */
  0x15, 0x00,                           /* Logical Minimum (0)                             */
  0x25, 0x01,                           /* Logical Maximum (1)                             */
  0x95, 0x03,                           /* Report Count (3)                                */
  0x75, 0x01,                           /* Report Size (1)                                 */
  0x81, 0x02,                           /* Input (Data, Variable, Absolute); 3 button bits */
  0x95, 0x01,                           /* Report Count (1)                                */
  0x75, 0x05,                           /* Report Size (5)                                 */
  0x81, 0x01,                           /* Input (Constant), ;5 bit padding                */
  0x05, 0x01,                           /* Usage Page (Generic Desktop)                    */
  0x09, 0x30,                           /* Usage (X)                                       */
  0x09, 0x31,                           /* Usage (Y)                                       */
  0x15, 0x81,                           /* Logical Minimum (-127)                          */
  0x25, 0x7F,                           /* Logical Maximum (127)                           */
  0x75, 0x08,                           /* Report Size (8)                                 */
  0x95, 0x02,                           /* Report Count (2)                                */
  0x81, 0x06,                           /* Input (Data, Variable, Relative); 2position bytes(X & Y) */
  0xC0,                                 /* End Collection                                  */
  0xC0,                                 /* End Collection                                  */
};


/*-----------------------------------------------------------------------------------------------------
  
  
  \param hid  
  \param hid_event  
  
  \return uint32_t 
-----------------------------------------------------------------------------------------------------*/
static uint32_t ux_hid_device_callback(UX_SLAVE_CLASS_HID *hid, UX_SLAVE_CLASS_HID_EVENT *hid_event)
{

  return 0;
}

/*-----------------------------------------------------------------------------------------------------
  
  
  \param hid_instance  
-----------------------------------------------------------------------------------------------------*/
static void  HID_keyboard_instance_activate_callback(VOID *hid_instance)
{

}

/*-----------------------------------------------------------------------------------------------------
  
  
  \param hid_instance  
-----------------------------------------------------------------------------------------------------*/
static void  HID_keyboard_instance_deactivate_callback(VOID *hid_instance)
{

}

/*-----------------------------------------------------------------------------------------------------
  
  
  \param hid_instance  
-----------------------------------------------------------------------------------------------------*/
static void  HID_mouse_instance_activate_callback(VOID *hid_instance)
{

}

/*-----------------------------------------------------------------------------------------------------
  
  
  \param hid_instance  
-----------------------------------------------------------------------------------------------------*/
static void  HID_mouse_instance_deactivate_callback(VOID *hid_instance)
{

}

/*-----------------------------------------------------------------------------------------------------
  
  
  \param void  
-----------------------------------------------------------------------------------------------------*/
void USB_hid_setup(void)
{
  /* Initialize the HID class parameters for a keyboard.  */
  hid_parameter.ux_device_class_hid_parameter_report_address = (UCHAR *) hid_report_descriptor_keyboard;
  hid_parameter.ux_device_class_hid_parameter_report_length = sizeof(hid_report_descriptor_keyboard);
  hid_parameter.ux_device_class_hid_parameter_callback      = ux_hid_device_callback;
  hid_parameter.ux_device_class_hid_parameter_report_id     = UX_TRUE;
  hid_parameter.ux_device_class_hid_parameter_get_callback  = NULL;

  /* Register user callback functions.  */
  hid_parameter.ux_slave_class_hid_instance_activate   = HID_keyboard_instance_activate_callback;
  hid_parameter.ux_slave_class_hid_instance_deactivate = HID_keyboard_instance_deactivate_callback;

  /* Initialize the device HID class. This class owns both interfaces starting with 1. */
  ux_device_stack_class_register(_ux_system_slave_class_hid_name, ux_device_class_hid_entry, 1, 0x00,(VOID *)&hid_parameter);
  /* Initialize the HID class parameters for a mouse.  */
  hid_parameter.ux_device_class_hid_parameter_report_address = (UCHAR *) hid_report_descriptor_mouse;
  hid_parameter.ux_device_class_hid_parameter_report_length  = sizeof(hid_report_descriptor_mouse);
  hid_parameter.ux_device_class_hid_parameter_callback       = ux_hid_device_callback;
  hid_parameter.ux_device_class_hid_parameter_report_id      = UX_TRUE;
  hid_parameter.ux_device_class_hid_parameter_get_callback   = NULL;

  /* Register user callback functions.  */
  hid_parameter.ux_slave_class_hid_instance_activate   = HID_mouse_instance_activate_callback;
  hid_parameter.ux_slave_class_hid_instance_deactivate = HID_mouse_instance_deactivate_callback;

  /* Initialize the device HID class. This class owns both interfaces starting with 1. */
  ux_device_stack_class_register(_ux_system_slave_class_hid_name, ux_device_class_hid_entry, 1, 0x01,(VOID *)&hid_parameter);
}

