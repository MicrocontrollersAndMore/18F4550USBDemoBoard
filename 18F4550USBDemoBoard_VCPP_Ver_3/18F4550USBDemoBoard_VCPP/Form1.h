#pragma once


// includes ///////////////////////////////////////////////////////////////////////////////////////
#include<Windows.h>			// definitions for types, including DWORD, PCHAR, HANDLE, etc
#include<setupapi.h>		// from platform SDK, definitions needed for SetupDixxx() functions, used to find plug and play device

// defines ////////////////////////////////////////////////////////////////////////////////////////
// modify this to match the VID / PID in your USB device descriptor
// use the format    "Vid_xxxx&Pid_xxxx" where xxxx is a 16-bit hex number
#define MY_DEVICE_ID "Vid_04d8&Pid_003F"

#define CONNECTION_NOT_SUCCESSFUL	0		// for use with g_connectionState
#define CONNECTION_SUCCESSFUL		1

#define TURN_LED3_OFF	0x80		// use with g_fromHostToDeviceBuffer[1]
#define TURN_LED3_ON	0x81		//

#define DO_NOT_USE_DEBUG_LEDS	0x00	// use with g_fromHostToDeviceBuffer[2]
#define USE_DEBUG_LEDS			0x01	//

#define SWITCH1_NOT_PRESSED		0x00	// use with g_fromDeviceToHostBuffer[1]
#define SWITCH1_PRESSED			0x01	//

#define SWITCH2_NOT_PRESSED		0x00	// use with g_fromDeviceToHostBuffer[2]
#define SWITCH2_PRESSED			0x01	//

#define SWITCH3_NOT_PRESSED		0x00	// use with g_fromDeviceToHostBuffer[3]
#define SWITCH3_PRESSED			0x01	//

// global variables ///////////////////////////////////////////////////////////////////////////////
int g_connectionState;							// for USB connection state

unsigned char g_fromHostToDeviceBuffer[65];		// for data transfer to device:
												//		byte 0 => "Report ID", init to zero and never use !!
												//		byte 1 => LED3 state, see #defines
												//		byte 2 => use of debug LEDs to show 4th byte of buffer, see #defines
												//		byte 3 => byte to show on debug LEDs

unsigned char g_fromDeviceToHostBuffer[65];		// for data transfer to host:
												//		byte 0 => "Report ID", init to zero and never use !!
												//		byte 1 => Switch1 state to show on form, see #defines
												//		byte 2 => Switch2 state to show on form, see #defines
												//		byte 3 => Switch3 state to show on form, see #defines

///////////////////////////////////////////////////////////////////////////////////////////////////
namespace My18F4550USBDemoBoard_VCPP {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	using namespace System::Runtime::InteropServices;		// for support of unmanaged code
	
	/*
	To use unmanaged functions from within .NET, we have to explicitly import the functions from .DLLs,
	simply including the header files is not enough.

	To avoid potential name conflicts in the header files we have named the imported functions by
	adding UM (unmanaged) onto the end of the function names.
	To find documentation for the functions in MSDN, search for the function without the UM on the end

	In the header files (ex. setupapi.h), normally the function names are remapped, depending on if
	UNICODE is defined or not.  For example, two versions of the function SetupDiGetDeviceInterfaceDetail()
	exist, one for UNICODE and one for ANSI.  To make sure the correct function gets called we should
	specify the CharSet when doing the DLL import.
	*/

	#ifdef UNICODE
	#define Seeifdef Unicode
	#else
	#define Seeifdef Ansi
	#endif
								// function imports

	/* Returns a HDEVINFO type for a device information set (USB HID device in our case).  We will
	   use the HDEVINFO returned as an input for other SetupDixxx() functions. */
	[DllImport("setupapi.dll", CharSet = CharSet::Seeifdef, EntryPoint = "SetupDiGetClassDevs")]
	extern "C" HDEVINFO SetupDiGetClassDevsUM(LPGUID ClassGuid,			// class Globally Unique Identifier
											  PCTSTR Enumerator,		// use NULL, not for import purposes
											  HWND hwndParent,			// use NULL, not for import purposes
											  DWORD Flags);				// flags describing kind of filtering to use
	
	/* We call this to get PSP_DEVICE_INTERFACE_DATA which contains the interface GUID (note this is different than the class GUID).
	   The interface GUID is necessary to get the device path. */
	[DllImport("setupapi.dll", CharSet = CharSet::Seeifdef, EntryPoint = "SetupDiEnumDeviceInterfaces")]
	extern "C" WINSETUPAPI BOOL WINAPI SetupDiEnumDeviceInterfacesUM(HDEVINFO DeviceInfoSet,							// use the HDEVINFO from SetupDiGetClassDevs() here
																	 PSP_DEVINFO_DATA DeviceInfoData,					// ??
																	 LPGUID InterfaceClassGuid,							// ??
																	 DWORD MemberIndex,									// index of the device we are getting the path for
																	 PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData);	// this is the output, the function will fill this structure
	
	/* Destroys (deallocates memory from) a DeviceInfoList. */
	[DllImport("setupapi.dll", CharSet = CharSet::Seeifdef, EntryPoint = "SetupDiDestroyDeviceInfoList")]
	extern "C" WINSETUPAPI BOOL WINAPI SetupDiDestroyDeviceInfoListUM(HDEVINFO DeviceInfoSet);		// pass in the DeviceInfoList to be deallocated
	
	/* Fills in a SP_DEVINFO_DATA structure, which we need for SetupDiGetDeviceRegistryProperty(). */
	[DllImport("setupapi.dll", CharSet = CharSet::Seeifdef, EntryPoint = "SetupDiEnumDeviceInfo")]
	extern "C" WINSETUPAPI BOOL WINAPI SetupDiEnumDeviceInfoUM(HDEVINFO DeviceInfoSet,					// ??
															   DWORD MemberIndex,						// ??
															   PSP_DEVINFO_DATA DeviceInfoData);		// this is the output, the function will fill in this structure
	
	/* Gets us the hardware ID, which we use to check if the VID / PID matches. */
	[DllImport("setupapi.dll", CharSet = CharSet::Seeifdef, EntryPoint = "SetupDiGetDeviceRegistryProperty")]
	extern "C" WINSETUPAPI BOOL WINAPI SetupDiGetDeviceRegistryPropertyUM(HDEVINFO DeviceInfoSet,
																		  PSP_DEVINFO_DATA DeviceInfoData,
																		  DWORD Property,
																		  PDWORD PropertyRegDataType,
																		  PBYTE PropertyBuffer,
																		  DWORD PropertyBufferSize,
																		  PDWORD RequiredSize);
	
	/* give us a device path, which is used in CreateFile()	*/
	[DllImport("setupapi.dll", CharSet = CharSet::Seeifdef, EntryPoint = "SetupDiGetDeviceInterfaceDetail")]
	extern "C" BOOL SetupDiGetDeviceInterfaceDetailUM(HDEVINFO DeviceInfoSet,										// we got this from SetupDiGetClassDevs()
													  PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,				// pointer to a struct that defines the device interface
													  PSP_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData,	// output, pointer to a struct that will contain the device path
													  DWORD DeviceInterfaceDetailDataSize,							// number of bytes to retrieve
													  PDWORD RequiredSize,											// output, number of bytes to hold the entire struct
													  PSP_DEVINFO_DATA DeviceInfoData);								// output, ??
	
	// member variables ///////////////////////////////////////////////////////////////////////////
	HANDLE WriteHandle = INVALID_HANDLE_VALUE;
	HANDLE ReadHandle = INVALID_HANDLE_VALUE;

	/// <summary>
	/// Summary for Form1
	/// </summary>

	///////////////////////////////////////////////////////////////////////////////////////////////
	public ref class Form1 : public System::Windows::Forms::Form {
	public:
		
		// constructor ////////////////////////////////////////////////////////////////////////////
		Form1(void) {
			InitializeComponent();

			g_fromHostToDeviceBuffer[0] = 0;	// The first byte in the I/O buffers is the "Report ID" and does not get transmitted over the
			g_fromDeviceToHostBuffer[0] = 0;	// USB bus, never change these !!!  Start using the buffers for actual data at index 1.
		}
		
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		
		// destructor /////////////////////////////////////////////////////////////////////////////
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  lblInfo;
	protected: 
	private: System::Windows::Forms::Label^  lblSwitchStates;
	private: System::Windows::Forms::Label^  lblLabel1;
	private: System::Windows::Forms::Label^  lblSwitch1State;
	private: System::Windows::Forms::Label^  lblLabel2;
	private: System::Windows::Forms::Label^  lblSwitch2State;
	private: System::Windows::Forms::Label^  lblLabel3;
	private: System::Windows::Forms::Label^  lblSwitch3State;
	private: System::Windows::Forms::Label^  lblLine;
	private: System::Windows::Forms::GroupBox^  gbLED3;
	private: System::Windows::Forms::RadioButton^  rdoLEDOn;
	private: System::Windows::Forms::RadioButton^  rdoLEDOff;
	private: System::Windows::Forms::CheckBox^  cbUseComboBox;
	private: System::Windows::Forms::ComboBox^  cboDebugLEDsValue;
	private: System::Windows::Forms::Timer^  tmrUpdateSwitchStates;
	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->lblInfo = (gcnew System::Windows::Forms::Label());
			this->lblSwitchStates = (gcnew System::Windows::Forms::Label());
			this->lblLabel1 = (gcnew System::Windows::Forms::Label());
			this->lblSwitch1State = (gcnew System::Windows::Forms::Label());
			this->lblLabel2 = (gcnew System::Windows::Forms::Label());
			this->lblSwitch2State = (gcnew System::Windows::Forms::Label());
			this->lblLabel3 = (gcnew System::Windows::Forms::Label());
			this->lblSwitch3State = (gcnew System::Windows::Forms::Label());
			this->lblLine = (gcnew System::Windows::Forms::Label());
			this->gbLED3 = (gcnew System::Windows::Forms::GroupBox());
			this->rdoLEDOn = (gcnew System::Windows::Forms::RadioButton());
			this->rdoLEDOff = (gcnew System::Windows::Forms::RadioButton());
			this->cbUseComboBox = (gcnew System::Windows::Forms::CheckBox());
			this->cboDebugLEDsValue = (gcnew System::Windows::Forms::ComboBox());
			this->tmrUpdateSwitchStates = (gcnew System::Windows::Forms::Timer(this->components));
			this->gbLED3->SuspendLayout();
			this->SuspendLayout();
			// 
			// lblInfo
			// 
			this->lblInfo->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->lblInfo->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblInfo->Location = System::Drawing::Point(8, 8);
			this->lblInfo->Name = L"lblInfo";
			this->lblInfo->Size = System::Drawing::Size(472, 32);
			this->lblInfo->TabIndex = 0;
			this->lblInfo->Text = L"info label";
			this->lblInfo->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			this->lblInfo->Click += gcnew System::EventHandler(this, &Form1::lblInfo_Click);
			// 
			// lblSwitchStates
			// 
			this->lblSwitchStates->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Underline, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblSwitchStates->Location = System::Drawing::Point(40, 48);
			this->lblSwitchStates->Name = L"lblSwitchStates";
			this->lblSwitchStates->Size = System::Drawing::Size(160, 32);
			this->lblSwitchStates->TabIndex = 1;
			this->lblSwitchStates->Text = L"switch states:";
			// 
			// lblLabel1
			// 
			this->lblLabel1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblLabel1->Location = System::Drawing::Point(8, 88);
			this->lblLabel1->Name = L"lblLabel1";
			this->lblLabel1->Size = System::Drawing::Size(136, 32);
			this->lblLabel1->TabIndex = 2;
			this->lblLabel1->Text = L"switch 1 state:";
			this->lblLabel1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// lblSwitch1State
			// 
			this->lblSwitch1State->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->lblSwitch1State->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblSwitch1State->Location = System::Drawing::Point(144, 88);
			this->lblSwitch1State->Name = L"lblSwitch1State";
			this->lblSwitch1State->Size = System::Drawing::Size(120, 32);
			this->lblSwitch1State->TabIndex = 3;
			this->lblSwitch1State->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// lblLabel2
			// 
			this->lblLabel2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblLabel2->Location = System::Drawing::Point(8, 128);
			this->lblLabel2->Name = L"lblLabel2";
			this->lblLabel2->Size = System::Drawing::Size(136, 32);
			this->lblLabel2->TabIndex = 4;
			this->lblLabel2->Text = L"switch 2 state:";
			this->lblLabel2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// lblSwitch2State
			// 
			this->lblSwitch2State->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->lblSwitch2State->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblSwitch2State->Location = System::Drawing::Point(144, 128);
			this->lblSwitch2State->Name = L"lblSwitch2State";
			this->lblSwitch2State->Size = System::Drawing::Size(120, 32);
			this->lblSwitch2State->TabIndex = 5;
			this->lblSwitch2State->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// lblLabel3
			// 
			this->lblLabel3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblLabel3->Location = System::Drawing::Point(8, 168);
			this->lblLabel3->Name = L"lblLabel3";
			this->lblLabel3->Size = System::Drawing::Size(136, 32);
			this->lblLabel3->TabIndex = 6;
			this->lblLabel3->Text = L"switch 3 state:";
			this->lblLabel3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// lblSwitch3State
			// 
			this->lblSwitch3State->BorderStyle = System::Windows::Forms::BorderStyle::Fixed3D;
			this->lblSwitch3State->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblSwitch3State->Location = System::Drawing::Point(144, 168);
			this->lblSwitch3State->Name = L"lblSwitch3State";
			this->lblSwitch3State->Size = System::Drawing::Size(120, 32);
			this->lblSwitch3State->TabIndex = 7;
			this->lblSwitch3State->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// lblLine
			// 
			this->lblLine->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->lblLine->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->lblLine->Location = System::Drawing::Point(8, 216);
			this->lblLine->Name = L"lblLine";
			this->lblLine->Size = System::Drawing::Size(472, 1);
			this->lblLine->TabIndex = 8;
			// 
			// gbLED3
			// 
			this->gbLED3->Controls->Add(this->rdoLEDOn);
			this->gbLED3->Controls->Add(this->rdoLEDOff);
			this->gbLED3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->gbLED3->Location = System::Drawing::Point(280, 48);
			this->gbLED3->Name = L"gbLED3";
			this->gbLED3->Size = System::Drawing::Size(200, 136);
			this->gbLED3->TabIndex = 9;
			this->gbLED3->TabStop = false;
			this->gbLED3->Text = L"choose LED3 state:";
			// 
			// rdoLEDOn
			// 
			this->rdoLEDOn->Location = System::Drawing::Point(32, 88);
			this->rdoLEDOn->Name = L"rdoLEDOn";
			this->rdoLEDOn->Size = System::Drawing::Size(120, 29);
			this->rdoLEDOn->TabIndex = 1;
			this->rdoLEDOn->TabStop = true;
			this->rdoLEDOn->Text = L"LED3 on";
			this->rdoLEDOn->UseVisualStyleBackColor = true;
			this->rdoLEDOn->CheckedChanged += gcnew System::EventHandler(this, &Form1::rdoLEDOn_CheckedChanged);
			// 
			// rdoLEDOff
			// 
			this->rdoLEDOff->Location = System::Drawing::Point(32, 48);
			this->rdoLEDOff->Name = L"rdoLEDOff";
			this->rdoLEDOff->Size = System::Drawing::Size(120, 29);
			this->rdoLEDOff->TabIndex = 0;
			this->rdoLEDOff->TabStop = true;
			this->rdoLEDOff->Text = L"LED3 off";
			this->rdoLEDOff->UseVisualStyleBackColor = true;
			this->rdoLEDOff->CheckedChanged += gcnew System::EventHandler(this, &Form1::rdoLEDOff_CheckedChanged);
			// 
			// cbUseComboBox
			// 
			this->cbUseComboBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->cbUseComboBox->Location = System::Drawing::Point(64, 232);
			this->cbUseComboBox->Name = L"cbUseComboBox";
			this->cbUseComboBox->Size = System::Drawing::Size(352, 56);
			this->cbUseComboBox->TabIndex = 10;
			this->cbUseComboBox->Text = L"check here to use debug LEDs to show value in drop down combo box";
			this->cbUseComboBox->UseVisualStyleBackColor = true;
			this->cbUseComboBox->CheckedChanged += gcnew System::EventHandler(this, &Form1::cbUseComboBox_CheckedChanged);
			// 
			// cboDebugLEDsValue
			// 
			this->cboDebugLEDsValue->Enabled = false;
			this->cboDebugLEDsValue->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.8F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->cboDebugLEDsValue->FormattingEnabled = true;
			this->cboDebugLEDsValue->Location = System::Drawing::Point(168, 296);
			this->cboDebugLEDsValue->Name = L"cboDebugLEDsValue";
			this->cboDebugLEDsValue->Size = System::Drawing::Size(144, 30);
			this->cboDebugLEDsValue->TabIndex = 11;
			this->cboDebugLEDsValue->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::cboDebugLEDsValue_SelectedIndexChanged);
			// 
			// tmrUpdateSwitchStates
			// 
			this->tmrUpdateSwitchStates->Interval = 50;
			this->tmrUpdateSwitchStates->Tick += gcnew System::EventHandler(this, &Form1::tmrUpdateSwitchStates_Tick);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(489, 335);
			this->Controls->Add(this->cboDebugLEDsValue);
			this->Controls->Add(this->cbUseComboBox);
			this->Controls->Add(this->gbLED3);
			this->Controls->Add(this->lblLine);
			this->Controls->Add(this->lblSwitch3State);
			this->Controls->Add(this->lblLabel3);
			this->Controls->Add(this->lblSwitch2State);
			this->Controls->Add(this->lblLabel2);
			this->Controls->Add(this->lblSwitch1State);
			this->Controls->Add(this->lblLabel1);
			this->Controls->Add(this->lblSwitchStates);
			this->Controls->Add(this->lblInfo);
			this->Name = L"Form1";
			this->Text = L"go to www.18F4550.com for more !!";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->gbLED3->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion			// end of Windows Form Designer generated code

///////////////////////////////////////////////////////////////////////////////////////////////////
private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
	int i;
	
	for(i=0;i<256;i++) {						// populate the combo box
		cboDebugLEDsValue->Items->Add(i);
	}
	
	attemptUSBConnectionFrontEnd();						// attempt USB connection
}

///////////////////////////////////////////////////////////////////////////////////////////////////
private: System::Void lblInfo_Click(System::Object^  sender, System::EventArgs^  e) {
	if(g_connectionState == CONNECTION_NOT_SUCCESSFUL) {				// verify not already connected . . .
		attemptUSBConnectionFrontEnd();									// then attempt to connect again
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void attemptUSBConnectionFrontEnd(void) {
	lblInfo->Text = "connecting . . . ";
	
	g_connectionState = attemptUSBConnection();								// attempt to connect to USB board
	
	if(g_connectionState == CONNECTION_SUCCESSFUL) {				// if connection was successful
		lblInfo->BackColor = System::Drawing::Color::LimeGreen;
		lblInfo->Text = "connection successful";
		tmrUpdateSwitchStates->Enabled = true;
	} else if(g_connectionState == CONNECTION_NOT_SUCCESSFUL) {		// else if connection was not successful
		lblInfo->BackColor = System::Drawing::Color::Red;
		lblInfo->Text = "connection not successful, click here to try again";
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int attemptUSBConnection(void) {
	int countNumLoops = 0;
	GUID InterfaceClassGuid = { 0x4d1e55b2, 0xf16f, 0x11cf, 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 };	// GUID for HID class devices
	HDEVINFO DeviceInfoTable = INVALID_HANDLE_VALUE;
	PSP_DEVICE_INTERFACE_DATA InterfaceDataStructure = new SP_DEVICE_INTERFACE_DATA;
	PSP_DEVICE_INTERFACE_DETAIL_DATA DetailedInterfaceDataStructure = new SP_DEVICE_INTERFACE_DETAIL_DATA;
	SP_DEVINFO_DATA DevInfoData;
	DWORD InterfaceIndex = 0;
	DWORD StatusLastError = 0;
	DWORD dwRegType;
	DWORD dwRegSize;
	DWORD StructureSize = 0;
	PBYTE PropertyValueBuffer;
	bool MatchFound = false;
	DWORD ErrorStatus;
	String^ DeviceIDToFind = MY_DEVICE_ID;
	
															// first populate a list of plugged in devices of the class GUID
	DeviceInfoTable = SetupDiGetClassDevsUM(&InterfaceClassGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

															// now look through the list just populated, see if any match our device . . .
	while(true) {
		InterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		SetupDiEnumDeviceInterfacesUM(DeviceInfoTable, NULL, &InterfaceClassGuid, InterfaceIndex, InterfaceDataStructure);
		ErrorStatus = GetLastError();
		if(ERROR_NO_MORE_ITEMS == GetLastError()) {					// if we reached the end of the list of matching devices in DeviceInfoTable . . .
			SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);		// deallocate the list of devices
			return(CONNECTION_NOT_SUCCESSFUL);						// and return unsuccessful
		}

														// retrieve the hardware ID from the registry, the hardware ID contains the VID & PID
														// which we will check to see if we found the correct device
		DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);				// init a SP_DEVINFO_DATA struct, we will use in call to SetupDiGetDeviceRegistryProperty()
		SetupDiEnumDeviceInfoUM(DeviceInfoTable, InterfaceIndex, &DevInfoData);
		
																	// query for the size of the hardware ID, so we can know how big a buffer to allocate for the data
		SetupDiGetDeviceRegistryPropertyUM(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType, NULL, 0, &dwRegSize);
		PropertyValueBuffer = (BYTE *)malloc(dwRegSize);			// allocate a buffer for the hardware ID
		if(PropertyValueBuffer == NULL) {							// if NULL, error, couldn't allocate enough memory
			SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);		// deallocate the list of devices
			return(CONNECTION_NOT_SUCCESSFUL);						// and return unsuccessful
		}
		
				/* Retrieve the hardware IDs for the current device we are looking at.  PropertyValueBuffer gets filled with a REG_MULTI_SZ
				   (array of null terminated strings).  To find a device we check the first string in the buffer, which will be the device ID.
				   The device ID is a string in the format Vid_xxxx&Pid_xxxx. */
		SetupDiGetDeviceRegistryPropertyUM(DeviceInfoTable, &DevInfoData, SPDRP_HARDWAREID, &dwRegType, PropertyValueBuffer, dwRegSize, NULL);
		
											// check if the first string in the hardware ID matches the device ID of our USB device
		#ifdef UNICODE
		String^ DeviceIDFromRegistry = gcnew String((wchar_t*)PropertyValueBuffer);
		#else
		String^ DeviceIDFromRegistry = gcnew String((char*)PropertyValueBuffer);
		#endif

		free(PropertyValueBuffer);		// we no longer need PropertyValueBuffer, deallocate to prevent memory leaks

		DeviceIDFromRegistry = DeviceIDFromRegistry->ToLowerInvariant();		// convert both strings to lower case
		DeviceIDToFind = DeviceIDToFind->ToLowerInvariant();

		MatchFound = DeviceIDFromRegistry->Contains(DeviceIDToFind);			// check if the hardware ID we are on contains the correct VID / PID
		if(MatchFound == true) {												// if device was found . . .

										/* Next we open read and write handles.  First we call SetupDiGetDeviceInterfaceDetail() to get the device path.
										   We have to call SetupDiGetDeviceInterfaceDetail() twice, first to get the size of the required structure / buffer
										   to hold the detailed interface data, then a second time to actually get the structure after allocating memory */
			DetailedInterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			SetupDiGetDeviceInterfaceDetailUM(DeviceInfoTable, InterfaceDataStructure, NULL, NULL, &StructureSize, NULL);	// 1st call, get size of required struct
			DetailedInterfaceDataStructure = (PSP_DEVICE_INTERFACE_DETAIL_DATA)(malloc(StructureSize));						// allocate memory
			if(DetailedInterfaceDataStructure == NULL) {				// if NULL, couldn't allocate enough memory
				SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);		// deallocate list of devices
				return(CONNECTION_NOT_SUCCESSFUL);						// and bail
			}
			DetailedInterfaceDataStructure->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
																															// 2nd call to get the struct
			SetupDiGetDeviceInterfaceDetailUM(DeviceInfoTable, InterfaceDataStructure, DetailedInterfaceDataStructure, StructureSize, NULL, NULL);
			
									// open read an write handles to global variables
			WriteHandle = CreateFile((DetailedInterfaceDataStructure->DevicePath), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			ErrorStatus = GetLastError();
			if(ErrorStatus == ERROR_SUCCESS) {
				// error handling code here ??
			}
			ReadHandle = CreateFile((DetailedInterfaceDataStructure->DevicePath), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			ErrorStatus = GetLastError();
			if(ErrorStatus == ERROR_SUCCESS) {
				// error handling code here ??
			}
			SetupDiDestroyDeviceInfoListUM(DeviceInfoTable);			// deallocate list of devices
			return(CONNECTION_SUCCESSFUL);								// if we get here connection was successful, wooo hooo !! 
		}	// end if(MatchFound == true)
		InterfaceIndex++;											// increment InterfaceIndex
		countNumLoops++;											// increment loop counter
		if(countNumLoops > 100) {									// if more than 100 times through loop . . .
			lblSwitch3State->Text = countNumLoops.ToString();		// debug output to label on form
			return(CONNECTION_NOT_SUCCESSFUL);						// give up and bail
		}
	}	// end while(true)
}

///////////////////////////////////////////////////////////////////////////////////////////////////
private: System::Void rdoLEDOff_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {	// if LED off radio button state just changed . . .
	DWORD bytesWritten = 0;
	
	if(rdoLEDOff->Checked == true) {												// if LED off radio button was just chosen . . .
		g_fromHostToDeviceBuffer[1] = TURN_LED3_OFF;								// assign command to turn LED3 off to 2nd byte of buffer
		WriteFile(WriteHandle, &g_fromHostToDeviceBuffer, 65, &bytesWritten, 0);	// and send buffer to device
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
private: System::Void rdoLEDOn_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {		// if LED on radio button state just changed . . .
	DWORD bytesWritten = 0;

	if(rdoLEDOn->Checked == true) {													// if LED on radio button was just chosen . . .
		g_fromHostToDeviceBuffer[1] = TURN_LED3_ON;									// assign command to turn LED3 on to 2nd byte of buffer
		WriteFile(WriteHandle, &g_fromHostToDeviceBuffer, 65, &bytesWritten, 0);	// and send buffer to device
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
private: System::Void cbUseComboBox_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {	// if check box to use drop down combo box is checked or unchecked . . .
	DWORD bytesWritten = 0;
	
	if(cbUseComboBox->Checked == true) {					// if check box was just checked . . .
		cboDebugLEDsValue->Enabled = true;						// enable drop down combo box
		g_fromHostToDeviceBuffer[2] = USE_DEBUG_LEDS;			// assign command to use debug LEDs to show byte to 4rd byte of buffer
	} else if(cbUseComboBox->Checked == false) {			// else if check box was just unchecked . . .
		cboDebugLEDsValue->Enabled = false;						// disable drop down combo box
		g_fromHostToDeviceBuffer[2] = DO_NOT_USE_DEBUG_LEDS;	// assign command to not use debug LEDs to show byte to 4rd byte of buffer
	}
	WriteFile(WriteHandle, &g_fromHostToDeviceBuffer, 65, &bytesWritten, 0);		// send buffer to device
}

///////////////////////////////////////////////////////////////////////////////////////////////////
private: System::Void cboDebugLEDsValue_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {	// if new drop down combo box value was just chosen . . .
	DWORD bytesWritten = 0;
	
	g_fromHostToDeviceBuffer[3] = Int32::Parse(cboDebugLEDsValue->SelectedItem->ToString());	// assign value from combo box to 4th byte of buffer
	WriteFile(WriteHandle, &g_fromHostToDeviceBuffer, 65, &bytesWritten, 0);					// send buffer to device
}

///////////////////////////////////////////////////////////////////////////////////////////////////
private: System::Void tmrUpdateSwitchStates_Tick(System::Object^  sender, System::EventArgs^  e) {		// every timer cycle . . .
	DWORD BytesRead = 0;
	
	ReadFile(ReadHandle, &g_fromDeviceToHostBuffer, 65, &BytesRead, 0);			// read buffer from device
	
	if(g_fromDeviceToHostBuffer[1] == SWITCH1_NOT_PRESSED) {		// if switch 1 is not pressed
		lblSwitch1State->Text = "not pressed";
	} else if(g_fromDeviceToHostBuffer[1] == SWITCH1_PRESSED) {		// else if switch 1 is pressed
		lblSwitch1State->Text = "pressed";
	} else {
		lblSwitch1State->Text = "error";							// should never get here
	}

	if(g_fromDeviceToHostBuffer[2] == SWITCH2_NOT_PRESSED) {		// if switch 2 is not pressed
		lblSwitch2State->Text = "not pressed";
	} else if(g_fromDeviceToHostBuffer[2] == SWITCH2_PRESSED) {		// else if switch 2 is pressed
		lblSwitch2State->Text = "pressed";
	} else {
		lblSwitch2State->Text = "error";							// should never get here
	}

	if(g_fromDeviceToHostBuffer[3] == SWITCH3_NOT_PRESSED) {		// if switch 3 is not pressed
		lblSwitch3State->Text = "not pressed";
	} else if(g_fromDeviceToHostBuffer[3] == SWITCH3_PRESSED) {		// else if switch 3 is pressed
		lblSwitch3State->Text = "pressed";
	} else {
		lblSwitch3State->Text = "error";							// should never get here
	}
}

};		// end class
}		// end namespace

