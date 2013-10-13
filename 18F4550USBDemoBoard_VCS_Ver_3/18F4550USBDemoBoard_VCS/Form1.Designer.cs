namespace _18F4550USBDemoBoard_VCS
{
	partial class Form1
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.lblInfo = new System.Windows.Forms.Label();
			this.lblSwitchStates = new System.Windows.Forms.Label();
			this.lblLabel1 = new System.Windows.Forms.Label();
			this.lblSwitch1State = new System.Windows.Forms.Label();
			this.lblLabel2 = new System.Windows.Forms.Label();
			this.lblSwitch2State = new System.Windows.Forms.Label();
			this.lblLabel3 = new System.Windows.Forms.Label();
			this.lblSwitch3State = new System.Windows.Forms.Label();
			this.lblLine = new System.Windows.Forms.Label();
			this.gbLED3 = new System.Windows.Forms.GroupBox();
			this.rdoLEDOff = new System.Windows.Forms.RadioButton();
			this.rdoLEDOn = new System.Windows.Forms.RadioButton();
			this.cbUseComboBox = new System.Windows.Forms.CheckBox();
			this.cboDebugLEDsValue = new System.Windows.Forms.ComboBox();
			this.tmrUpdateSwitchStates = new System.Windows.Forms.Timer(this.components);
			this.gbLED3.SuspendLayout();
			this.SuspendLayout();
			// 
			// lblInfo
			// 
			this.lblInfo.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblInfo.Location = new System.Drawing.Point(8, 8);
			this.lblInfo.Name = "lblInfo";
			this.lblInfo.Size = new System.Drawing.Size(472, 32);
			this.lblInfo.TabIndex = 0;
			this.lblInfo.Text = "info label";
			this.lblInfo.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			this.lblInfo.Click += new System.EventHandler(this.lblInfo_Click);
			// 
			// lblSwitchStates
			// 
			this.lblSwitchStates.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblSwitchStates.Location = new System.Drawing.Point(40, 48);
			this.lblSwitchStates.Name = "lblSwitchStates";
			this.lblSwitchStates.Size = new System.Drawing.Size(160, 32);
			this.lblSwitchStates.TabIndex = 1;
			this.lblSwitchStates.Text = "switch states:";
			// 
			// lblLabel1
			// 
			this.lblLabel1.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblLabel1.Location = new System.Drawing.Point(8, 88);
			this.lblLabel1.Name = "lblLabel1";
			this.lblLabel1.Size = new System.Drawing.Size(136, 32);
			this.lblLabel1.TabIndex = 2;
			this.lblLabel1.Text = "switch 1 state:";
			this.lblLabel1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// lblSwitch1State
			// 
			this.lblSwitch1State.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblSwitch1State.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblSwitch1State.Location = new System.Drawing.Point(144, 88);
			this.lblSwitch1State.Name = "lblSwitch1State";
			this.lblSwitch1State.Size = new System.Drawing.Size(120, 32);
			this.lblSwitch1State.TabIndex = 3;
			this.lblSwitch1State.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// lblLabel2
			// 
			this.lblLabel2.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblLabel2.Location = new System.Drawing.Point(8, 128);
			this.lblLabel2.Name = "lblLabel2";
			this.lblLabel2.Size = new System.Drawing.Size(136, 32);
			this.lblLabel2.TabIndex = 4;
			this.lblLabel2.Text = "switch 2 state:";
			this.lblLabel2.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// lblSwitch2State
			// 
			this.lblSwitch2State.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblSwitch2State.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblSwitch2State.Location = new System.Drawing.Point(144, 128);
			this.lblSwitch2State.Name = "lblSwitch2State";
			this.lblSwitch2State.Size = new System.Drawing.Size(120, 32);
			this.lblSwitch2State.TabIndex = 5;
			this.lblSwitch2State.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// lblLabel3
			// 
			this.lblLabel3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblLabel3.Location = new System.Drawing.Point(8, 168);
			this.lblLabel3.Name = "lblLabel3";
			this.lblLabel3.Size = new System.Drawing.Size(136, 32);
			this.lblLabel3.TabIndex = 6;
			this.lblLabel3.Text = "switch 3 state:";
			this.lblLabel3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// lblSwitch3State
			// 
			this.lblSwitch3State.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.lblSwitch3State.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblSwitch3State.Location = new System.Drawing.Point(144, 168);
			this.lblSwitch3State.Name = "lblSwitch3State";
			this.lblSwitch3State.Size = new System.Drawing.Size(120, 32);
			this.lblSwitch3State.TabIndex = 7;
			this.lblSwitch3State.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
			// 
			// lblLine
			// 
			this.lblLine.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.lblLine.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.lblLine.Location = new System.Drawing.Point(8, 216);
			this.lblLine.Name = "lblLine";
			this.lblLine.Size = new System.Drawing.Size(472, 1);
			this.lblLine.TabIndex = 8;
			// 
			// gbLED3
			// 
			this.gbLED3.Controls.Add(this.rdoLEDOn);
			this.gbLED3.Controls.Add(this.rdoLEDOff);
			this.gbLED3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.gbLED3.Location = new System.Drawing.Point(280, 48);
			this.gbLED3.Name = "gbLED3";
			this.gbLED3.Size = new System.Drawing.Size(200, 136);
			this.gbLED3.TabIndex = 9;
			this.gbLED3.TabStop = false;
			this.gbLED3.Text = "choose LED3 state";
			// 
			// rdoLEDOff
			// 
			this.rdoLEDOff.Location = new System.Drawing.Point(32, 48);
			this.rdoLEDOff.Name = "rdoLEDOff";
			this.rdoLEDOff.Size = new System.Drawing.Size(120, 32);
			this.rdoLEDOff.TabIndex = 0;
			this.rdoLEDOff.TabStop = true;
			this.rdoLEDOff.Text = "LED3 off";
			this.rdoLEDOff.UseVisualStyleBackColor = true;
			this.rdoLEDOff.CheckedChanged += new System.EventHandler(this.rdoLEDOff_CheckedChanged);
			// 
			// rdoLEDOn
			// 
			this.rdoLEDOn.Location = new System.Drawing.Point(32, 88);
			this.rdoLEDOn.Name = "rdoLEDOn";
			this.rdoLEDOn.Size = new System.Drawing.Size(120, 32);
			this.rdoLEDOn.TabIndex = 1;
			this.rdoLEDOn.TabStop = true;
			this.rdoLEDOn.Text = "LED3 on";
			this.rdoLEDOn.UseVisualStyleBackColor = true;
			this.rdoLEDOn.CheckedChanged += new System.EventHandler(this.rdoLEDOn_CheckedChanged);
			// 
			// cbUseComboBox
			// 
			this.cbUseComboBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cbUseComboBox.Location = new System.Drawing.Point(64, 232);
			this.cbUseComboBox.Name = "cbUseComboBox";
			this.cbUseComboBox.Size = new System.Drawing.Size(352, 56);
			this.cbUseComboBox.TabIndex = 10;
			this.cbUseComboBox.Text = "check here to use debug LEDs to show value in drop down combo box";
			this.cbUseComboBox.UseVisualStyleBackColor = true;
			this.cbUseComboBox.CheckedChanged += new System.EventHandler(this.cbUseComboBox_CheckedChanged);
			// 
			// cboDebugLEDsValue
			// 
			this.cboDebugLEDsValue.Enabled = false;
			this.cboDebugLEDsValue.Font = new System.Drawing.Font("Microsoft Sans Serif", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.cboDebugLEDsValue.FormattingEnabled = true;
			this.cboDebugLEDsValue.Location = new System.Drawing.Point(168, 296);
			this.cboDebugLEDsValue.Name = "cboDebugLEDsValue";
			this.cboDebugLEDsValue.Size = new System.Drawing.Size(144, 30);
			this.cboDebugLEDsValue.TabIndex = 11;
			this.cboDebugLEDsValue.SelectedIndexChanged += new System.EventHandler(this.cboDebugLEDsValue_SelectedIndexChanged);
			// 
			// tmrUpdateSwitchStates
			// 
			this.tmrUpdateSwitchStates.Interval = 50;
			this.tmrUpdateSwitchStates.Tick += new System.EventHandler(this.tmrUpdateSwitchStates_Tick);
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(489, 335);
			this.Controls.Add(this.cboDebugLEDsValue);
			this.Controls.Add(this.cbUseComboBox);
			this.Controls.Add(this.gbLED3);
			this.Controls.Add(this.lblLine);
			this.Controls.Add(this.lblSwitch3State);
			this.Controls.Add(this.lblLabel3);
			this.Controls.Add(this.lblSwitch2State);
			this.Controls.Add(this.lblLabel2);
			this.Controls.Add(this.lblSwitch1State);
			this.Controls.Add(this.lblLabel1);
			this.Controls.Add(this.lblSwitchStates);
			this.Controls.Add(this.lblInfo);
			this.Name = "Form1";
			this.Text = "go to www.18F4550.com for more !!";
			this.Load += new System.EventHandler(this.Form1_Load);
			this.gbLED3.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Label lblInfo;
		private System.Windows.Forms.Label lblSwitchStates;
		private System.Windows.Forms.Label lblLabel1;
		private System.Windows.Forms.Label lblSwitch1State;
		private System.Windows.Forms.Label lblLabel2;
		private System.Windows.Forms.Label lblSwitch2State;
		private System.Windows.Forms.Label lblLabel3;
		private System.Windows.Forms.Label lblSwitch3State;
		private System.Windows.Forms.Label lblLine;
		private System.Windows.Forms.GroupBox gbLED3;
		private System.Windows.Forms.RadioButton rdoLEDOn;
		private System.Windows.Forms.RadioButton rdoLEDOff;
		private System.Windows.Forms.CheckBox cbUseComboBox;
		private System.Windows.Forms.ComboBox cboDebugLEDsValue;
		private System.Windows.Forms.Timer tmrUpdateSwitchStates;
	}
}

