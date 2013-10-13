using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using Microsoft.Win32.SafeHandles;
using System.Runtime.InteropServices;
using System.Threading;

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace _18F4550USBDemoBoard_VCS {
	
	/////////////////////////////////////////////////////////////////////////////////////////////////
	public partial class Form1 : Form {
		
		// constants //////////////////////////////////////////////////////////////////////////////////
		// modify this to match the VID / PID in your USB device descriptor
		// use the format "Vid_xxxx&Pid_xxxx" where xxxx is a 16-bit hex number
		const String MY_DEVICE_ID = "Vid_04d8&Pid_003F";

		const byte CONNECTION_NOT_SUCCESSFUL = 0;		// for use with g_connectionState
		const byte CONNECTION_SUCCESSFUL = 1;		//

		const byte TURN_LED3_OFF = 0x80;				// use with g_fromHostToDeviceBuffer[1]
		const byte TURN_LED3_ON = 0x81;				//

		const byte DO_NOT_USE_DEBUG_LEDS = 0x00;		// use with g_fromHostToDeviceBuffer[2]
		const byte USE_DEBUG_LEDS = 0x01;		//

		const byte SWITCH1_NOT_PRESSED = 0x00;		// use with g_fromDeviceToHostBuffer[1]
		const byte SWITCH1_PRESSED = 0x01;		//

		const byte SWITCH2_NOT_PRESSED = 0x00;		// use with g_fromDeviceToHostBuffer[2]
		const byte SWITCH2_PRESSED = 0x01;		//

		const byte SWITCH3_NOT_PRESSED = 0x00;		// use with g_fromDeviceToHostBuffer[3]
		const byte SWITCH3_PRESSED = 0x01;		//

		// constant definitions from setupapi.h, which we aren't allowed to include directly since this is C#
		internal const uint DIGCF_PRESENT = 0x02;
		internal const uint DIGCF_DEVICEINTERFACE = 0x10;
		// constants for CreateFile() and other file I/O functions
		internal const short FILE_ATTRIBUTE_NORMAL = 0x80;
		internal const short INVALID_HANDLE_VALUE = -1;
		internal const uint GENERIC_READ = 0x80000000;
		internal const uint GENERIC_WRITE = 0x40000000;
		internal const uint CREATE_NEW = 1;
		internal const uint CREATE_ALWAYS = 2;
		internal const uint OPEN_EXISTING = 3;
		internal const uint FILE_SHARE_READ = 0x00000001;
		internal const uint FILE_SHARE_WRITE = 0x00000002;
		// constant definitions for certain WM_DEVICECHANGE messages
		internal const uint WM_DEVICECHANGE = 0x0219;
		internal const uint DBT_DEVICEARRIVAL = 0x8000;
		internal const uint DBT_DEVICEREMOVEPENDING = 0x8003;
		internal const uint DBT_DEVICEREMOVECOMPLETE = 0x8004;
		internal const uint DBT_CONFIGCHANGED = 0x0018;
		// other constant definitions
		internal const uint DBT_DEVTYP_DEVICEINTERFACE = 0x05;
		internal const uint DEVICE_NOTIFY_WINDOW_HANDLE = 0x00;
		internal const uint ERROR_SUCCESS = 0x00;
		internal const uint ERROR_NO_MORE_ITEMS = 0x00000103;
		internal const uint SPDRP_HARDWAREID = 0x00000001;

		// structs ////////////////////////////////////////////////////////////////////////////////////
		// struct definitions from setupapi.h, which we aren't allowed to include directly since this is C#
		internal struct SP_DEVICE_INTERFACE_DATA {
			internal uint cbSize;               //DWORD
			internal Guid InterfaceClassGuid;   //GUID
			internal uint Flags;                //DWORD
			internal uint Reserved;             //ULONG_PTR MSDN says ULONG_PTR is "typedef unsigned __int3264 ULONG_PTR;"  
		}

		internal struct SP_DEVICE_INTERFACE_DETAIL_DATA {
			internal uint cbSize;               //DWORD
			internal char[] DevicePath;         //TCHAR array of any size
		}

		internal struct SP_DEVINFO_DATA {
			internal uint cbSize;       //DWORD
			internal Guid ClassGuid;    //GUID
			internal uint DevInst;      //DWORD
			internal uint Reserved;     //ULONG_PTR  MSDN says ULONG_PTR is "typedef unsigned __int3264 ULONG_PTR;"  
		}

		internal struct DEV_BROADCAST_DEVICEINTERFACE {
			internal uint dbcc_size;            //DWORD
			internal uint dbcc_devicetype;      //DWORD
			internal uint dbcc_reserved;        //DWORD
			internal Guid dbcc_classguid;       //GUID
			internal char[] dbcc_name;          //TCHAR array
		}

		// Windows API function DLL imports ///////////////////////////////////////////////////////////
		//Returns a HDEVINFO type for a device information set.  We will need the 
		//HDEVINFO as in input parameter for calling many of the other SetupDixxx() functions.
		[DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
		internal static extern IntPtr SetupDiGetClassDevs(ref Guid ClassGuid,     //LPGUID    Input: Need to supply the class GUID. 
																											IntPtr Enumerator,      //PCTSTR    Input: Use NULL here, not important for our purposes
																											IntPtr hwndParent,      //HWND      Input: Use NULL here, not important for our purposes
																											uint Flags);            //DWORD     Input: Flags describing what kind of filtering to use.

		//Gives us "PSP_DEVICE_INTERFACE_DATA" which contains the Interface specific GUID (different
		//from class GUID).  We need the interface GUID to get the device path.
		[DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
		internal static extern bool SetupDiEnumDeviceInterfaces(IntPtr DeviceInfoSet,           //Input: Give it the HDEVINFO we got from SetupDiGetClassDevs()
																														IntPtr DeviceInfoData,          //Input (optional)
																														ref Guid InterfaceClassGuid,    //Input 
																														uint MemberIndex,               //Input: "Index" of the device you are interested in getting the path for.
																														ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData);    //Output: This function fills in an "SP_DEVICE_INTERFACE_DATA" structure.

		//SetupDiDestroyDeviceInfoList() frees up memory by destroying a DeviceInfoList
		[DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
		internal static extern bool SetupDiDestroyDeviceInfoList(IntPtr DeviceInfoSet);					//Input: Give it a handle to a device info list to deallocate from RAM.

		//SetupDiEnumDeviceInfo() fills in an "SP_DEVINFO_DATA" structure, which we need for SetupDiGetDeviceRegistryProperty()
		[DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
		internal static extern bool SetupDiEnumDeviceInfo(IntPtr DeviceInfoSet,
																											uint MemberIndex,
																											ref SP_DEVINFO_DATA DeviceInterfaceData);

		//SetupDiGetDeviceRegistryProperty() gives us the hardware ID, which we use to check to see if it has matching VID/PID
		[DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
		internal static extern bool SetupDiGetDeviceRegistryProperty(IntPtr DeviceInfoSet,
																																 ref SP_DEVINFO_DATA DeviceInfoData,
																																 uint Property,
																																 ref uint PropertyRegDataType,
																																 IntPtr PropertyBuffer,
																																 uint PropertyBufferSize,
																																 ref uint RequiredSize);

		//SetupDiGetDeviceInterfaceDetail() gives us a device path, which is needed before CreateFile() can be used.
		[DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
		internal static extern bool SetupDiGetDeviceInterfaceDetail(IntPtr DeviceInfoSet,                   //Input: Wants HDEVINFO which can be obtained from SetupDiGetClassDevs()
																																ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData,                    //Input: Pointer to an structure which defines the device interface.  
																																IntPtr DeviceInterfaceDetailData,      //Output: Pointer to a SP_DEVICE_INTERFACE_DETAIL_DATA structure, which will receive the device path.
																																uint DeviceInterfaceDetailDataSize,     //Input: Number of bytes to retrieve.
																																ref uint RequiredSize,                  //Output (optional): The number of bytes needed to hold the entire struct 
																																IntPtr DeviceInfoData);                 //Output (optional): Pointer to a SP_DEVINFO_DATA structure

		//Overload for SetupDiGetDeviceInterfaceDetail().  Need this one since we can't pass NULL pointers directly in C#.
		[DllImport("setupapi.dll", SetLastError = true, CharSet = CharSet.Unicode)]
		internal static extern bool SetupDiGetDeviceInterfaceDetail(IntPtr DeviceInfoSet,                   //Input: Wants HDEVINFO which can be obtained from SetupDiGetClassDevs()
																																ref SP_DEVICE_INTERFACE_DATA DeviceInterfaceData,               //Input: Pointer to an structure which defines the device interface.  
																																IntPtr DeviceInterfaceDetailData,       //Output: Pointer to a SP_DEVICE_INTERFACE_DETAIL_DATA structure, which will contain the device path.
																																uint DeviceInterfaceDetailDataSize,     //Input: Number of bytes to retrieve.
																																IntPtr RequiredSize,                    //Output (optional): Pointer to a DWORD to tell you the number of bytes needed to hold the entire struct 
																																IntPtr DeviceInfoData);                 //Output (optional): Pointer to a SP_DEVINFO_DATA structure

		//Need this function for receiving all of the WM_DEVICECHANGE messages.  See MSDN documentation for
		//description of what this function does/how to use it. Note: name is remapped "RegisterDeviceNotificationUM" to
		//avoid possible build error conflicts.
		[DllImport("user32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
		internal static extern IntPtr RegisterDeviceNotification(IntPtr hRecipient,
																														 IntPtr NotificationFilter,
																														 uint Flags);

		//Takes in a device path and opens a handle to the device.
		[DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
		static extern SafeFileHandle CreateFile(string lpFileName,
																						uint dwDesiredAccess,
																						uint dwShareMode,
																						IntPtr lpSecurityAttributes,
																						uint dwCreationDisposition,
																						uint dwFlagsAndAttributes,
																						IntPtr hTemplateFile);

		//Uses a handle (created with CreateFile()), and lets us write USB data to the device.
		[DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
		static extern bool WriteFile(SafeFileHandle hFile,
																 byte[] lpBuffer,
																 uint nNumberOfBytesToWrite,
																 ref uint lpNumberOfBytesWritten,
																 IntPtr lpOverlapped);

		//Uses a handle (created with CreateFile()), and lets us read USB data from the device.
		[DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
		static extern bool ReadFile(SafeFileHandle hFile,
																IntPtr lpBuffer,
																uint nNumberOfBytesToRead,
																ref uint lpNumberOfBytesRead,
																IntPtr lpOverlapped);

		// member variables ///////////////////////////////////////////////////////////////////////////
		int m_connectionState;							// for USB connection state

		byte[] m_fromHostToDeviceBuffer = new byte[65];		// for data transfer to device:
		//		byte 0 => "Report ID", init to zero and never use !!
		//		byte 1 => LED3 state, see #defines
		//		byte 2 => use of debug LEDs to show 4th byte of buffer, see #defines
		//		byte 3 => byte to show on debug LEDs

		byte[] m_fromDeviceToHostBuffer = new byte[65];		// for data transfer to host:
		//		byte 0 => "Report ID", init to zero and never use !!
		//		byte 1 => Switch1 state to show on form, see #defines
		//		byte 2 => Switch2 state to show on form, see #defines
		//		byte 3 => Switch3 state to show on form, see #defines

		bool AttachedState = false;						//Need to keep track of the USB device attachment status for proper plug and play operation.
		SafeFileHandle m_WriteHandleToUSBDevice = null;
		SafeFileHandle m_ReadHandleToUSBDevice = null;
		String DevicePath = null;   //Need the find the proper device path before you can open file handles.

		//Globally Unique Identifier (GUID) for HID class devices.  Windows uses GUIDs to identify things.
		Guid InterfaceClassGuid = new Guid(0x4d1e55b2, 0xf16f, 0x11cf, 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30);
		
		// constructor ////////////////////////////////////////////////////////////////////////////////
		public Form1() {
			InitializeComponent();

			m_fromHostToDeviceBuffer[0] = 0;				// The first byte in the I/O buffers is the "Report ID" and does not get transmitted over the
			m_fromDeviceToHostBuffer[0] = 0;				// USB bus, never change these !!!  Start using the buffers for actual data at index 1.
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		private void Form1_Load(object sender, EventArgs e) {
			int i;

			for (i = 0; i < 256; i++) {						// populate the combo box
				cboDebugLEDsValue.Items.Add(i);
			}

			attemptUSBConnectionFrontEnd();						// attempt USB connection
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		private void lblInfo_Click(object sender, EventArgs e) {						// if user clicks info label to attempt to connect again . . .
			if (m_connectionState == CONNECTION_NOT_SUCCESSFUL) {				// verify not already connected . . .
				attemptUSBConnectionFrontEnd();														// then attempt to connect again
			}
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////////
		void attemptUSBConnectionFrontEnd() {
			lblInfo.Text = "connecting . . . ";

			m_connectionState = attemptUSBConnection();												// attempt to connect to USB board

			if (m_connectionState == CONNECTION_SUCCESSFUL) {								// if connection was successful
				lblInfo.BackColor = System.Drawing.Color.LimeGreen;
				lblInfo.Text = "connection successful";
				tmrUpdateSwitchStates.Enabled = true;
			} else if (m_connectionState == CONNECTION_NOT_SUCCESSFUL) {		// else if connection was not successful
				lblInfo.BackColor = System.Drawing.Color.Red;
				lblInfo.Text = "connection not successful, click here to try again";
			}
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////////
		int attemptUSBConnection() {

			// Register for WM_DEVICECHANGE notifications.  This code uses these messages to detect plug and play connection/disconnection events for USB devices
			DEV_BROADCAST_DEVICEINTERFACE DeviceBroadcastHeader = new DEV_BROADCAST_DEVICEINTERFACE();
			DeviceBroadcastHeader.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
			DeviceBroadcastHeader.dbcc_size = (uint)Marshal.SizeOf(DeviceBroadcastHeader);
			DeviceBroadcastHeader.dbcc_reserved = 0;	//Reserved says not to use...
			DeviceBroadcastHeader.dbcc_classguid = InterfaceClassGuid;
			
			// Need to get the address of the DeviceBroadcastHeader to call RegisterDeviceNotification(), but
			// can't use "&DeviceBroadcastHeader".  Instead, using a roundabout means to get the address by 
			// making a duplicate copy using Marshal.StructureToPtr().
			IntPtr pDeviceBroadcastHeader = IntPtr.Zero;				// Make a pointer.
			pDeviceBroadcastHeader = Marshal.AllocHGlobal(Marshal.SizeOf(DeviceBroadcastHeader));		// allocate memory for a new DEV_BROADCAST_DEVICEINTERFACE structure, and return the address 
			Marshal.StructureToPtr(DeviceBroadcastHeader, pDeviceBroadcastHeader, false);						// Copies the DeviceBroadcastHeader structure into the memory already allocated at DeviceBroadcastHeaderWithPointer
			RegisterDeviceNotification(this.Handle, pDeviceBroadcastHeader, DEVICE_NOTIFY_WINDOW_HANDLE);

			// Now make an initial attempt to find the USB device, if it was already connected to the PC and enumerated prior to launching the application.
			// If it is connected and present, we should open read and write handles to the device so we can communicate with it later.
			// If it was not connected, we will have to wait until the user plugs the device in, and the WM_DEVICECHANGE callback function can process
			// the message and again search for the device.

			IntPtr DeviceInfoTable = IntPtr.Zero;
			SP_DEVICE_INTERFACE_DATA InterfaceDataStructure = new SP_DEVICE_INTERFACE_DATA();
			SP_DEVICE_INTERFACE_DETAIL_DATA DetailedInterfaceDataStructure = new SP_DEVICE_INTERFACE_DETAIL_DATA();
			SP_DEVINFO_DATA DevInfoData = new SP_DEVINFO_DATA();

			uint InterfaceIndex = 0;
			uint dwRegType = 0;
			uint dwRegSize = 0;
			uint dwRegSize2 = 0;
			uint StructureSize = 0;
			IntPtr PropertyValueBuffer = IntPtr.Zero;
			bool MatchFound = false;
			uint ErrorStatus;
			uint loopCounter = 0;

			String DeviceIDToFind = MY_DEVICE_ID;

			// First populate a list of plugged in devices (by specifying "DIGCF_PRESENT"), which are of the specified class GUID. 
			DeviceInfoTable = SetupDiGetClassDevs(ref InterfaceClassGuid, IntPtr.Zero, IntPtr.Zero, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

			if (DeviceInfoTable == IntPtr.Zero) return (CONNECTION_NOT_SUCCESSFUL);

			// Now look through the list we just populated.  We are trying to see if any of them match our device. 
			while (true) {
				InterfaceDataStructure.cbSize = (uint)Marshal.SizeOf(InterfaceDataStructure);
				if (SetupDiEnumDeviceInterfaces(DeviceInfoTable, IntPtr.Zero, ref InterfaceClassGuid, InterfaceIndex, ref InterfaceDataStructure)) {
					ErrorStatus = (uint)Marshal.GetLastWin32Error();
					if (ErrorStatus == ERROR_NO_MORE_ITEMS) {							// did we reach the end of the list of matching devices in the DeviceInfoTable?
						// cound not find the device.  Must not have been attached.
						SetupDiDestroyDeviceInfoList(DeviceInfoTable);			// clean up the old structure we no longer need.
						return (CONNECTION_NOT_SUCCESSFUL);
					}
				} else {																			// else some other kind of unknown error ocurred...
					ErrorStatus = (uint)Marshal.GetLastWin32Error();
					SetupDiDestroyDeviceInfoList(DeviceInfoTable);	// Clean up the old structure we no longer need.
					return (CONNECTION_NOT_SUCCESSFUL);
				}

				// Now retrieve the hardware ID from the registry.  The hardware ID contains the VID and PID, which we will then 
				// check to see if it is the correct device or not.

				// Initialize an appropriate SP_DEVINFO_DATA structure.  We need this structure for SetupDiGetDeviceRegistryProperty().
				DevInfoData.cbSize = (uint)Marshal.SizeOf(DevInfoData);
				SetupDiEnumDeviceInfo(DeviceInfoTable, InterfaceIndex, ref DevInfoData);

				// First query for the size of the hardware ID, so we can know how big a buffer to allocate for the data.
				SetupDiGetDeviceRegistryProperty(DeviceInfoTable, ref DevInfoData, SPDRP_HARDWAREID, ref dwRegType, IntPtr.Zero, 0, ref dwRegSize);

				// Allocate a buffer for the hardware ID.
				// Should normally work, but could throw exception "OutOfMemoryException" if not enough resources available.
				PropertyValueBuffer = Marshal.AllocHGlobal((int)dwRegSize);

				// Retrieve the hardware IDs for the current device we are looking at.  PropertyValueBuffer gets filled with a 
				// REG_MULTI_SZ (array of null terminated strings).  To find a device, we only care about the very first string in the
				// buffer, which will be the "device ID".  The device ID is a string which contains the VID and PID, in the example 
				// format "Vid_04d8&Pid_003f".
				SetupDiGetDeviceRegistryProperty(DeviceInfoTable, ref DevInfoData, SPDRP_HARDWAREID, ref dwRegType, PropertyValueBuffer, dwRegSize, ref dwRegSize2);

				// Now check if the first string in the hardware ID matches the device ID of the USB device we are trying to find.
				String DeviceIDFromRegistry = Marshal.PtrToStringUni(PropertyValueBuffer);	// Make a new string, fill it with the contents from the PropertyValueBuffer

				Marshal.FreeHGlobal(PropertyValueBuffer);		// no longer need the PropertyValueBuffer, free the memory to prevent potential memory leaks

				// Convert both strings to lower case.  This makes the code more robust/portable accross OS Versions
				DeviceIDFromRegistry = DeviceIDFromRegistry.ToLowerInvariant();
				DeviceIDToFind = DeviceIDToFind.ToLowerInvariant();
				// Now check if the hardware ID we are looking at contains the correct VID/PID
				MatchFound = DeviceIDFromRegistry.Contains(DeviceIDToFind);
				if (MatchFound == true) {
					// Device must have been found.  In order to open I/O file handle(s), we will need the actual device path first.
					// We can get the path by calling SetupDiGetDeviceInterfaceDetail(), however, we have to call this function twice:  The first
					// time to get the size of the required structure/buffer to hold the detailed interface data, then a second time to actually 
					// get the structure (after we have allocated enough memory for the structure.)
					DetailedInterfaceDataStructure.cbSize = (uint)Marshal.SizeOf(DetailedInterfaceDataStructure);

					// First call populates "StructureSize" with the correct value
					SetupDiGetDeviceInterfaceDetail(DeviceInfoTable, ref InterfaceDataStructure, IntPtr.Zero, 0, ref StructureSize, IntPtr.Zero);

					// Need to call SetupDiGetDeviceInterfaceDetail() again, this time specifying a pointer to a SP_DEVICE_INTERFACE_DETAIL_DATA buffer with the correct size of RAM allocated.
					// First need to allocate the unmanaged buffer and get a pointer to it.
					IntPtr pUnmanagedDetailedInterfaceDataStructure = IntPtr.Zero;		// Declare a pointer.
					pUnmanagedDetailedInterfaceDataStructure = Marshal.AllocHGlobal((int)StructureSize);    // Reserve some unmanaged memory for the structure.
					DetailedInterfaceDataStructure.cbSize = 6;	// Initialize the cbSize parameter (4 bytes for DWORD + 2 bytes for unicode null terminator)
					Marshal.StructureToPtr(DetailedInterfaceDataStructure, pUnmanagedDetailedInterfaceDataStructure, false); //Copy managed structure contents into the unmanaged memory buffer.

					// Now call SetupDiGetDeviceInterfaceDetail() a second time to receive the device path in the structure at pUnmanagedDetailedInterfaceDataStructure.
					if (SetupDiGetDeviceInterfaceDetail(DeviceInfoTable, ref InterfaceDataStructure, pUnmanagedDetailedInterfaceDataStructure, StructureSize, IntPtr.Zero, IntPtr.Zero)) {
						// Need to extract the path information from the unmanaged "structure".  The path starts at (pUnmanagedDetailedInterfaceDataStructure + sizeof(DWORD)).
						IntPtr pToDevicePath = new IntPtr((uint)pUnmanagedDetailedInterfaceDataStructure.ToInt32() + 4);  //Add 4 to the pointer (to get the pointer to point to the path, instead of the DWORD cbSize parameter)
						DevicePath = Marshal.PtrToStringUni(pToDevicePath); // Now copy the path information into the globally defined DevicePath String.

						// We now have the proper device path, and we can finally use the path to open I/O handle(s) to the device.
						SetupDiDestroyDeviceInfoList(DeviceInfoTable);									// Clean up the old structure we no longer need.
						Marshal.FreeHGlobal(pUnmanagedDetailedInterfaceDataStructure);  // No longer need this unmanaged SP_DEVICE_INTERFACE_DETAIL_DATA buffer.  We already extracted the path information.

						uint ErrorStatusWrite;
						uint ErrorStatusRead;
						// we now have the proper device path, and we can finally open read and write handles to the device.
						m_WriteHandleToUSBDevice = CreateFile(DevicePath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, IntPtr.Zero, OPEN_EXISTING, 0, IntPtr.Zero);
						ErrorStatusWrite = (uint)Marshal.GetLastWin32Error();
						m_ReadHandleToUSBDevice = CreateFile(DevicePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, IntPtr.Zero, OPEN_EXISTING, 0, IntPtr.Zero);
						ErrorStatusRead = (uint)Marshal.GetLastWin32Error();

						if ((ErrorStatusWrite == ERROR_SUCCESS) && (ErrorStatusRead == ERROR_SUCCESS)) {
							return (CONNECTION_SUCCESSFUL);
						} else {									// for some reason the device was physically plugged in, but one or both of the read/write handles didn't open successfully...
							if (ErrorStatusWrite == ERROR_SUCCESS) m_WriteHandleToUSBDevice.Close();
							if (ErrorStatusRead == ERROR_SUCCESS) m_ReadHandleToUSBDevice.Close();
							return (CONNECTION_NOT_SUCCESSFUL);
						}
					} else {																	// Some unknown failure occurred
						uint ErrorCode = (uint)Marshal.GetLastWin32Error();
						SetupDiDestroyDeviceInfoList(DeviceInfoTable);	// Clean up the old structure.
						Marshal.FreeHGlobal(pUnmanagedDetailedInterfaceDataStructure);  // No longer need this unmanaged SP_DEVICE_INTERFACE_DETAIL_DATA buffer.  We already extracted the path information.
						return (CONNECTION_NOT_SUCCESSFUL);
					}
				}
				InterfaceIndex++;
				loopCounter++;
				if (loopCounter > 200) {														// if more than 200 times through the loop . . .
					lblSwitch3State.Text = loopCounter.ToString();		// debug output to label on form
					return (CONNECTION_NOT_SUCCESSFUL);								// give up and bail
				}
			}	// end of while(true)			
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		private void rdoLEDOff_CheckedChanged(object sender, EventArgs e) {										// if LED off radio button state just changed . . .
			uint BytesWritten = 0;

			if (rdoLEDOff.Checked == true) {																										// if LED off radio button was just chosen . . .
				m_fromHostToDeviceBuffer[1] = TURN_LED3_OFF;																			// assign command to turn LED3 off to 2nd byte of buffer
				WriteFile(m_WriteHandleToUSBDevice, m_fromHostToDeviceBuffer, 65, ref BytesWritten, IntPtr.Zero);		// and send buffer to device
			}
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		private void rdoLEDOn_CheckedChanged(object sender, EventArgs e) {									// if LED on radio button state just changed . . .
			uint BytesWritten = 0;

			if (rdoLEDOn.Checked == true) {																// if LED on radio button was just chosen . . .
				m_fromHostToDeviceBuffer[1] = TURN_LED3_ON;																	// assign command to turn LED3 on to 2nd byte of buffer
				WriteFile(m_WriteHandleToUSBDevice, m_fromHostToDeviceBuffer, 65, ref BytesWritten, IntPtr.Zero);			// and send buffer to device
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		private void cbUseComboBox_CheckedChanged(object sender, EventArgs e) {										// if check box to use drop down combo box is checked or unchecked . . .
			uint BytesWritten = 0;

			if (cbUseComboBox.Checked == true)
			{										// if check box was just checked . . .
				cboDebugLEDsValue.Enabled = true;											// enable drop down combo box
				m_fromHostToDeviceBuffer[2] = USE_DEBUG_LEDS;					// assign command to use debug LEDs to show byte to 4rd byte of buffer
			}
			else if (cbUseComboBox.Checked == false)
			{						// else if check box was just unchecked . . .
				cboDebugLEDsValue.Enabled = false;										// disable drop down combo box
				m_fromHostToDeviceBuffer[2] = DO_NOT_USE_DEBUG_LEDS;	// assign command to not use debug LEDs to show byte to 4rd byte of buffer
			}

			WriteFile(m_WriteHandleToUSBDevice, m_fromHostToDeviceBuffer, 65, ref BytesWritten, IntPtr.Zero);			// send buffer to device
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		private void cboDebugLEDsValue_SelectedIndexChanged(object sender, EventArgs e) {					// if new drop down combo box value was just chosen . . .
			uint BytesWritten = 0;

			m_fromHostToDeviceBuffer[3] = Byte.Parse(cboDebugLEDsValue.SelectedItem.ToString());							// assign value from combo box to 4th byte of buffer
			WriteFile(m_WriteHandleToUSBDevice, m_fromHostToDeviceBuffer, 65, ref BytesWritten, IntPtr.Zero);		// send buffer to device
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		private void tmrUpdateSwitchStates_Tick(object sender, EventArgs e) {

			uint BytesRead = 0;

			ReadFileManagedBuffer(m_ReadHandleToUSBDevice, m_fromDeviceToHostBuffer, 65, ref BytesRead, IntPtr.Zero);

			if (m_fromDeviceToHostBuffer[1] == SWITCH1_NOT_PRESSED) {		// if switch 1 is not pressed
				lblSwitch1State.Text = "not pressed";
			} else if (m_fromDeviceToHostBuffer[1] == SWITCH1_PRESSED) {		// else if switch 1 is pressed
				lblSwitch1State.Text = "pressed";
			} else {
				lblSwitch1State.Text = "error";							// should never get here
			}

			if (m_fromDeviceToHostBuffer[2] == SWITCH2_NOT_PRESSED) {		// if switch 2 is not pressed
				lblSwitch2State.Text = "not pressed";
			} else if (m_fromDeviceToHostBuffer[2] == SWITCH2_PRESSED) {		// else if switch 2 is pressed
				lblSwitch2State.Text = "pressed";
			} else {
				lblSwitch2State.Text = "error";							// should never get here
			}

			if (m_fromDeviceToHostBuffer[3] == SWITCH3_NOT_PRESSED) {		// if switch 3 is not pressed
				lblSwitch3State.Text = "not pressed";
			} else if (m_fromDeviceToHostBuffer[3] == SWITCH3_PRESSED) {		// else if switch 3 is pressed
				lblSwitch3State.Text = "pressed";
			} else {
				lblSwitch3State.Text = "error";							// should never get here
			}
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		//FUNCTION:	ReadFileManagedBuffer()
		//PURPOSE:	Wrapper function to call ReadFile()
		//
		//INPUT:	Uses managed versions of the same input parameters as ReadFile() uses.
		//
		//OUTPUT:	Returns boolean indicating if the function call was successful or not.
		//          Also returns data in the byte[] INBuffer, and the number of bytes read. 
		//
		//Notes:    Wrapper function used to call the ReadFile() function.  ReadFile() takes a pointer to an unmanaged buffer and deposits
		//          the bytes read into the buffer.  However, can't pass a pointer to a managed buffer directly to ReadFile().
		//          This ReadFileManagedBuffer() is a wrapper function to make it so application code can call ReadFile() easier
		//          by specifying a managed buffer.
		public unsafe bool ReadFileManagedBuffer(SafeFileHandle hFile, byte[] INBuffer, uint nNumberOfBytesToRead, ref uint lpNumberOfBytesRead, IntPtr lpOverlapped) {
			IntPtr pINBuffer = IntPtr.Zero;

			try {
				pINBuffer = Marshal.AllocHGlobal((int)nNumberOfBytesToRead);    //Allocate some unmanged RAM for the receive data buffer.

				if (ReadFile(hFile, pINBuffer, nNumberOfBytesToRead, ref lpNumberOfBytesRead, lpOverlapped)) {
					Marshal.Copy(pINBuffer, INBuffer, 0, (int)lpNumberOfBytesRead);    //Copy over the data from unmanged memory into the managed byte[] INBuffer
					Marshal.FreeHGlobal(pINBuffer);
					return true;
				} else {
					Marshal.FreeHGlobal(pINBuffer);
					return false;
				}
			} catch {
				if (pINBuffer != IntPtr.Zero) {
					Marshal.FreeHGlobal(pINBuffer);
				} return(false);
			}
		}

	}		// end class
}			// end namespace
