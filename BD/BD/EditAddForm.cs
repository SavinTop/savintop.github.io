using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace BD
{
    public enum EditAddOperation { Edit, Add}

    public struct SqlInputLine
    {
        public Control control;
        public HeaderCell hc;
        public string code;
    }

    public partial class EditAddForm : Form
    {
        public const int WM_NCLBUTTONDOWN = 0xA1;
        public const int HT_CAPTION = 0x2;

        [DllImportAttribute("user32.dll")]
        public static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);
        [DllImportAttribute("user32.dll")]
        public static extern bool ReleaseCapture();

        DataBaseInformation dbi;
        EditAddOperation operation;
        string PrimaryKeyValue;

        List<SqlInputLine> sil = new List<SqlInputLine>();

        public EditAddForm()
        {
            InitializeComponent();
        }

        public EditAddForm(DataBaseInformation dbi, EditAddOperation operation,
            string PrimaryKeyValue = "")
        {
            InitializeComponent();
            this.operation = operation;
            this.PrimaryKeyValue = PrimaryKeyValue;
            this.Text = dbi.TableName;
            this.dbi = dbi;

            this.dbi.Update("Select * from [" + dbi.TableName + "] where [" + dbi.PrimaryKeys[0] +
                "] = \'" + PrimaryKeyValue + "\'");

            initInputs();
            Generate();
            if (operation == EditAddOperation.Edit)
            {
                FillFields();
            }
        }

        private void initInputs()
        {
            int fuckingShitCounterForFuckingTag = 0;
            foreach (var el in dbi.Headers)
            {
                if(el.cellType!=HeaderCellType.Primary)
                {
                    var temp = new SqlInputLine();
                    temp.hc = el;

                    if(el.cellType==HeaderCellType.Common)
                    {
                        if (el.dataType == typeof(DateTime))
                        {
                            temp.control = new DateTimePicker();
                            ((DateTimePicker)temp.control).Format = DateTimePickerFormat.Custom;
                            ((DateTimePicker)temp.control).CustomFormat = "dd.MM.yyyy HH:mm:ss";
                        }
                        else
                        if (el.dataType == typeof(int))
                        {
                            temp.control = new NumericUpDown();
                            ((NumericUpDown)temp.control).Minimum = int.MinValue;
                            ((NumericUpDown)temp.control).Maximum = int.MaxValue;
                        }
                        else
                        if (el.dataType == typeof(bool))
                            temp.control = new CheckBox();
                        else
                        {
                            temp.control = new TextBox();
                            ((TextBox)temp.control).BorderStyle = BorderStyle.FixedSingle;
                        }

                        temp.control.BackColor = Color.FromArgb(66, 69, 73);
                        temp.control.ForeColor = Color.FromArgb(255, 255, 255);
                    } else
                    {
                        var butt = new Button();
                        butt.FlatStyle = FlatStyle.Flat;
                        butt.FlatAppearance.BorderSize = 0;
                        butt.BackColor = Color.FromArgb(66, 69, 73);
                        butt.ForeColor = Color.FromArgb(255, 255, 255);
                        butt.Tag = fuckingShitCounterForFuckingTag;
                        butt.Click += ForeignButtClick;
                        temp.control = butt;
                    }

                    sil.Add(temp);
                    fuckingShitCounterForFuckingTag++;
                }
            }
        }

        private void Generate()
        {
            const int captionHeight = 25;
            const int leftOffset = 20;
            const int topOffset = 20 + captionHeight;
            const int downOffset = 20;
            const int destBetweenH = 5;
            const int textBoxW = 150;
            int maxLabelWidth = 0;

            int labelHeight = (new Label()).Height;

            for(int i=0;i<sil.Count;i++)
            {
                var tempLabel = new Label();
                tempLabel.AutoSize = true;
                tempLabel.Text = sil[i].hc.value;
                tempLabel.Left = leftOffset;
                tempLabel.Top = topOffset + (labelHeight + destBetweenH) * i;
                tempLabel.ForeColor = Color.White;
                Controls.Add(tempLabel);
                if (tempLabel.Width > maxLabelWidth) maxLabelWidth = tempLabel.Width;
            }

            for (int i = 0; i < sil.Count; i++)
            {
                sil[i].control.Left = maxLabelWidth + leftOffset;
                sil[i].control.Top = topOffset + (labelHeight + destBetweenH) * i;
                sil[i].control.Width = textBoxW;

                sil[i].control.BackColor = Color.FromArgb(66, 69, 73);
                sil[i].control.ForeColor = Color.FromArgb(255, 255, 255);

                Controls.Add(sil[i].control);
            }

            var butt = new Button();
            butt.Left = leftOffset;
            butt.Top = topOffset + (labelHeight + destBetweenH) * dbi.Headers.Length;
            butt.Text = operation == EditAddOperation.Add ? "Добавить" : "Изменить";
            butt.FlatStyle = FlatStyle.Flat;
            butt.FlatAppearance.BorderSize = 0;
            butt.BackColor = Color.FromArgb(66, 69, 73);
            butt.ForeColor = Color.FromArgb(255, 255, 255);

            if (operation == EditAddOperation.Edit)
                butt.Click += EditButton;
            else
                butt.Click += AddButton;

            Controls.Add(butt);

            BackColor = Color.FromArgb(30, 33, 36);

            ClientSize = new Size(leftOffset * 2 + maxLabelWidth+textBoxW,
                butt.Top + butt.Height + downOffset);

            //Shitty:dddd
            var topPanel = new Panel();
            topPanel.Height = captionHeight;
            topPanel.Width = ClientSize.Width;
            topPanel.BackColor = Color.FromArgb(20, 23, 26);
            topPanel.MouseDown += TitleMouseDown;

            var exitBtn = new Button();
            exitBtn.Text = "";
            exitBtn.TextAlign = ContentAlignment.MiddleCenter;
            exitBtn.Height = 15;
            exitBtn.Width = 15;
            exitBtn.Left = ClientSize.Width - exitBtn.Width - 5;
            exitBtn.Top = 5;
            exitBtn.Click += exitButton;
            exitBtn.FlatStyle = FlatStyle.Flat;
            exitBtn.FlatAppearance.BorderSize = 0;
            exitBtn.BackColor = Color.FromArgb(30, 31, 32);
            exitBtn.ForeColor = Color.FromArgb(255, 255, 255);
            //

            var capt = new Label();
            capt.Left = 5;
            capt.Top = 5;
            capt.Height = topPanel.Height - capt.Top;
            capt.Width = exitBtn.Left - capt.Left;
            capt.Text = dbi.TableName + (operation == EditAddOperation.Add ? " Add" : " Edit");
            capt.ForeColor = Color.White;
            capt.BackColor = topPanel.BackColor;
            capt.MouseDown += TitleMouseDown;

            Controls.Add(capt);

            Controls.Add(exitBtn);
            Controls.Add(topPanel);
        }

        private void FillFields()
        {
            for (int i = 0; i < sil.Count; i++)
            {
                
                int colInd = dbi.Table.Columns[sil[i].hc.value].Ordinal;
                if (sil[i].hc.cellType==HeaderCellType.Common)
                {
                    if (sil[i].hc.dataType == typeof(bool))
                    {
                        var t = dbi.Table.Rows[0].ItemArray[colInd].ToString().Trim();
                        ((CheckBox)sil[i].control).Checked = (t == "True");
                    }
                    else
                    if (sil[i].hc.dataType == typeof(double) || sil[i].hc.dataType == typeof(float)
                        || sil[i].hc.dataType == typeof(decimal))
                    {
                        sil[i].control.Text = dbi.Table.Rows[0].ItemArray[colInd].ToString().Trim();
                        sil[i].control.Text = sil[i].control.Text.Replace(',', '.');
                    }
                    else
                        sil[i].control.Text = dbi.Table.Rows[0].ItemArray[colInd].ToString().Trim();
                } 
                else
                {
                    var el = sil[i];
                    el.code = dbi.Table.Rows[0].ItemArray[colInd].ToString().Trim();
                    sil[i] = el;
                    sil[i].control.Text = el.code;
                }
            }
        }

        private void EditButton(object sender, EventArgs e)
        {
            string command = "update [" + dbi.TableName + "] set ";

            foreach (var el in sil)
            {
                string t;

                if(el.hc.cellType == HeaderCellType.Foreign)
                {
                    t = sil[((int)el.control.Tag)].code;
                }else
                {
                    if (el.hc.dataType == typeof(DateTime))
                        t = ((DateTimePicker)el.control).Value.ToString("dd.MM.yyyy HH:mm:ss");
                    else if (el.hc.dataType == typeof(bool))
                        t = ((CheckBox)el.control).Checked ? "1" : "0";
                    else
                        t = el.control.Text;
                }
                

                command += "["+el.hc.value+"] = \'" + t +"\', ";
            }
            command = command.Substring(0, command.Length - 2);

            command += " where [" + dbi.PrimaryKeys[0] + "] = \'" + PrimaryKeyValue+"\'";

            DBWorker.DoScalarSQL(command);

            Close();
        }

        private void AddButton(object sender, EventArgs e)
        {
            string command = "insert into [" + dbi.TableName + "](";

            foreach (var el in sil)
                command += "["+el.hc.value+"], ";

            command = command.Substring(0, command.Length - 2)+") values (";

            foreach (var el in sil)
            {
                string t;

                if(el.hc.cellType == HeaderCellType.Common)
                {
                    if (el.hc.dataType == typeof(DateTime))
                        t = ((DateTimePicker)el.control).Value.ToString("dd.MM.yyyy HH:mm:ss");
                    else if (el.hc.dataType == typeof(bool))
                        t = ((CheckBox)el.control).Checked ? "1" : "0";
                    else
                        t = el.control.Text;
                } else
                {
                    t = sil[((int)el.control.Tag)].code;
                }
                

                command +="\'"+ t + "\', ";
            }
            command = command.Substring(0, command.Length - 2)+")";

            DBWorker.DoScalarSQL(command);

            Close();
        }

        private void ForeignButtClick(object sender, EventArgs e)
        {
            int ind = (int)((Button)sender).Tag;
            string neededTable = sil[ind].hc.toTableName;

            var dialog = new CheckForeignForm(neededTable, "select * from ["+ neededTable + "]", sil[ind].code);

            dialog.ShowDialog();

            var el = sil[ind];
            el.code = dialog.key;
            el.control.Text = el.code;
            sil[ind] = el;

        }

        private void EditAddForm_Load(object sender, EventArgs e)
        {
            
        }

        private void TitleMouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                ReleaseCapture();
                SendMessage(Handle, WM_NCLBUTTONDOWN, HT_CAPTION, 0);
            }
        }

        private void exitButton(object sender, EventArgs e)
        {
            Close();
        }
    }
}
