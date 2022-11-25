using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Configuration;
using System.Runtime.InteropServices;

namespace BD
{
    public partial class WorkerForm : Form
    {
        public const int WM_NCLBUTTONDOWN = 0xA1;
        public const int HT_CAPTION = 0x2;

        [DllImportAttribute("user32.dll")]
        public static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);
        [DllImportAttribute("user32.dll")]
        public static extern bool ReleaseCapture();

        MainForm mainForm;
        DataBaseInformation dbi;

        string SearchRequest = "";
        string SortRequest = "";

        public WorkerForm()
        {
            InitializeComponent();
        }

        public WorkerForm(MainForm GetBackForm, string TableName)
        {
            const int captionHeight = 25;

            InitializeComponent();

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
            capt.Text = TableName;
            capt.ForeColor = Color.White;
            capt.BackColor = topPanel.BackColor;
            capt.MouseDown += TitleMouseDown;

            Controls.Add(capt);
            Controls.Add(exitBtn);
            Controls.Add(topPanel);

            mainForm = GetBackForm;
            dbi = new DataBaseInformation(TableName);
            UpdateAllSQL("Select * from ["+TableName+"]");
        }

        private void WorkerForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            mainForm.Show();
        }

        private void UpdateAllSQL(string commandText)
        {
            UpdateDataGridViewSQL(commandText);

            searchCheckedList.Items.Clear();
            SortFieldSelect.Items.Clear();

            foreach(var el in dbi.Headers)
            {
                if(el.cellType == HeaderCellType.Common)
                {
                    searchCheckedList.Items.Add(el.value);
                    SortFieldSelect.Items.Add(el.value);
                }
            }
        }

        private void UpdateDataGridViewSQL(string commandText)
        {
            dbi.Update(commandText);

            gridOutput.Rows.Clear();
            gridOutput.Columns.Clear();

            foreach(var el in dbi.Headers)
            {
                if (el.cellType == HeaderCellType.Foreign)
                {
                    DataGridViewButtonColumn btn = new DataGridViewButtonColumn();
                    btn.HeaderText = el.value;
                    btn.Name = el.value;
                    btn.FlatStyle = FlatStyle.Flat;
                    gridOutput.Columns.Add(btn);
                }
                else
                gridOutput.Columns.Add(el.value, el.value);
            }

            for(int i=0;i<dbi.Table.Rows.Count;i++)
            {
                gridOutput.Rows.Add(dbi.Table.Rows[i].ItemArray);
            }

            gridOutput.Columns[dbi.PrimaryKeys[0]].Visible = false;  
        }

        private void CombineQuerySortSearch()
        {
            UpdateDataGridViewSQL("select * from [" + dbi.TableName + "] " + SearchRequest + " " + SortRequest);
        }

        private void SortButt_Click(object sender, EventArgs e)
        {
            if(SortFieldSelect.SelectedIndex==-1)
            {
                MessageBox.Show("Необходимо выбрать поле");
                return;
            }
            if (SortTypeSelector.SelectedIndex == -1)
            {
                MessageBox.Show("Необходимо выбрать тип сортировки");
                return;
            }

            SortRequest = "order by [" + SortFieldSelect.Text +
                (SortTypeSelector.SelectedIndex == 1 ? "] DESC" : "] ASC");

            CombineQuerySortSearch();
        }

        private void searchInput_TextChanged(object sender, EventArgs e)
        {
            SearchRequest = "where ";
            foreach (var el in searchCheckedList.CheckedItems)
                SearchRequest += "["+el.ToString() + "] like \'%" + searchInput.Text + "%\' or ";
            SearchRequest = SearchRequest.Substring(0, SearchRequest.Length - 3);

            CombineQuerySortSearch();
        }

        private void DeleteButton_Click(object sender, EventArgs e)
        {
            if (gridOutput.SelectedRows.Count == 0) return;
            var msg = MessageBox.Show("Вы уверены что хотите удалить запись?", "Удаление", MessageBoxButtons.YesNo);

            if (msg == DialogResult.No) return;

            string temp = dbi.PrimaryKeys[0];

            string command = "Delete from [" + dbi.TableName + "] where [" +
                temp + "] = \'" + gridOutput.SelectedRows[0].Cells[temp].Value.ToString()+"\'";

            DBWorker.DoScalarSQL(command);

            CombineQuerySortSearch();
        }

        private void EditButton_Click(object sender, EventArgs e)
        {
            if (gridOutput.SelectedRows.Count == 0) return;

            string temp = dbi.PrimaryKeys[0];

            (new EditAddForm(dbi, EditAddOperation.Edit, gridOutput.SelectedRows[0].Cells[temp].Value.ToString())).ShowDialog();

            CombineQuerySortSearch();
        }

        private void AddButton_Click(object sender, EventArgs e)
        {
            (new EditAddForm(dbi, EditAddOperation.Add)).ShowDialog();
            CombineQuerySortSearch();
        }

        private void gridOutput_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            var senderGrid = (DataGridView)sender;

            if (senderGrid.Columns[e.ColumnIndex] is DataGridViewButtonColumn &&
                e.RowIndex >= 0)
            {
                string colName = dbi.Headers[e.ColumnIndex].toColumnName;
                string keyValue = dbi.Table.Rows[e.RowIndex].ItemArray[e.ColumnIndex].ToString();
                string tableName = dbi.Headers[e.ColumnIndex].toTableName;

                string command = "select * from [" + tableName + "] where [" + colName + "] = \'" + keyValue + "\'";

                var foreignForm = new CheckForeignForm(tableName, command);
                
                foreignForm.ShowDialog();
            }
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
