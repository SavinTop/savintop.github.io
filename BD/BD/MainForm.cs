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
using System.IO;
using System.Reflection;

namespace BD
{
    public partial class MainForm : Form
    {
        public const int WM_NCLBUTTONDOWN = 0xA1;
        public const int HT_CAPTION = 0x2;

        [DllImportAttribute("user32.dll")]
        public static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, int lParam);
        [DllImportAttribute("user32.dll")]
        public static extern bool ReleaseCapture();

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            const int captionHeight = 25;
            const int leftOffset = 10;
            const int topOffset = 10+captionHeight;
            const int downOffset = 10;
            const int destBetweenH = 5;
            const int defWidth = 300;

            string connectionString = ConfigurationManager.ConnectionStrings["DefaultConnection"].ConnectionString;

            SqlConnectionStringBuilder builder = new SqlConnectionStringBuilder(connectionString);

            string database = builder.InitialCatalog;

            string sqlExpression = "SELECT TABLE_NAME FROM ["+database+"].INFORMATION_SCHEMA.TABLES WHERE TABLE_TYPE = 'BASE TABLE'";
            using (SqlConnection connection = new SqlConnection(connectionString))
            {
                connection.Open();
                SqlCommand command = new SqlCommand(sqlExpression, connection);
                SqlDataReader reader = command.ExecuteReader();

                List<Button> testButtons = new List<Button>();
                Button temp = null;

                for (int i=0; reader.Read(); i++)
                {
                    temp = new Button();
                    temp.Text = reader.GetString(0);
                    temp.Width = defWidth;
                    temp.Left = leftOffset;
                    temp.Top = topOffset + (temp.Height + destBetweenH) * i;
                    temp.Click += ButtonClickHandler;
                    temp.FlatStyle = FlatStyle.Flat;
                    temp.FlatAppearance.BorderSize = 0;
                    temp.BackColor = Color.FromArgb(66, 69, 73);
                    temp.ForeColor = Color.FromArgb(255, 255, 255);
                    testButtons.Add(temp);
                }

                BackColor = Color.FromArgb(30, 33, 36);
                ClientSize = new Size(leftOffset * 2 + temp.Width, temp.Top + temp.Height + downOffset);


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
                exitBtn.Left = ClientSize.Width- exitBtn.Width - 5;
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
                capt.Text = builder.InitialCatalog;
                capt.ForeColor = Color.White;
                capt.BackColor = topPanel.BackColor;
                capt.MouseDown += TitleMouseDown;

                var trackBar = new TrackBar();
                trackBar.Left = 0;
                trackBar.Top = ClientSize.Height;
                trackBar.Width = ClientSize.Width;
                trackBar.Minimum = ClientSize.Width;
                trackBar.Maximum = ClientSize.Width*2;
                trackBar.ValueChanged += tbvc;

                var img = new PictureBox();
                img.Left = ClientSize.Width;
                img.Top = 0;
                img.Height = ClientSize.Height;
                img.Width = ClientSize.Width;
                img.SizeMode = PictureBoxSizeMode.Zoom;

                Assembly myAssembly = Assembly.GetExecutingAssembly();
                Stream myStream = myAssembly.GetManifestResourceStream("BD.5.jpg");
                Bitmap bmp = new Bitmap(myStream);

                img.Image = bmp;

                Controls.Add(img);

                Controls.Add(trackBar);
                Controls.Add(capt);

                Controls.Add(exitBtn);
                Controls.Add(topPanel);
                Controls.AddRange(testButtons.ToArray());

                reader.Close();

                DoubleBuffered = true;
            }
        }


        private void ButtonClickHandler(object sender, EventArgs e)
        {
            Hide();
            (new WorkerForm(this,((Button)sender).Text)).Show();
        }

        private void TitleMouseDown(object sender,MouseEventArgs e)
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

        bool swither = false;

        private void MainForm_DoubleClick(object sender, EventArgs e)
        {
            if (swither)
            ClientSize = new Size(ClientSize.Width,ClientSize.Height -(new TrackBar()).Height);
            else
            ClientSize = new Size(ClientSize.Width, ClientSize.Height + (new TrackBar()).Height);
            swither = !swither;
        }

        private void tbvc(object sender, EventArgs e)
        {
            TrackBar temp = ((TrackBar)sender);
            Width = temp.Value;
        }
    }
}
