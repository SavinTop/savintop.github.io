using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Drawing;

namespace ChessGame
{
    static class Visual
    {
        public static readonly float sqr = (float)(450.0/8.0);

        public static Bitmap fieldPic = new Bitmap(@"sprites/fieldwood.jpg");
        public static Bitmap greenFramePic = new Bitmap(@"sprites/greenframe.png");
        public static Bitmap redFramePic = new Bitmap(@"sprites/redframe.png");
        public static Bitmap buf = new Bitmap(450, 450);

        static public Bitmap[] figuresPics =
        {
            new Bitmap(@"sprites/whitePawn.png"),
            new Bitmap(@"sprites/whiteKnight.png"),
            new Bitmap(@"sprites/whiteBishop.png"),
            new Bitmap(@"sprites/whiteRook.png"),
            new Bitmap(@"sprites/whiteKing.png"),
            new Bitmap(@"sprites/whiteQueen.png"),
            new Bitmap(@"sprites/blackPawn.png"),
            new Bitmap(@"sprites/blackKnight.png"),
            new Bitmap(@"sprites/blackBishop.png"),
            new Bitmap(@"sprites/blackRook.png"),
            new Bitmap(@"sprites/blackKing.png"),
            new Bitmap(@"sprites/blackQueen.png")
        };

        public static Graphics buffer;

        static Graphics graph;

        public static void RedrawFigField()
        {
            byte[,] field = Chess.GetField();

            buffer.DrawImage(fieldPic,0,0);

            for(int i=0;i<8;i++)
                for(int j=0;j<8;j++)
                {
                    if (field[i, j] == 0) continue;
                    buffer.DrawImage(figuresPics[field[i, j]-1], j * sqr, i * sqr, sqr, sqr);
                }
        }

        public static void RedrawMovementField()
        {
            byte[,] field = Chess.GetMovementField();

            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                {
                    if (field[i, j] == 0) continue;
                    if (field[i, j] == 1)
                    buffer.DrawImage(greenFramePic, j * sqr, i * sqr, sqr, sqr);
                    else
                    buffer.DrawImage(redFramePic, j * sqr, i * sqr, sqr, sqr);
                }
        }

        public static void SetGraph(Graphics graphics)
        {
            graph = graphics;
            buffer = Graphics.FromImage(buf);
        }

        public static void Redraw()
        {
            graph.DrawImage(buf, 0, 0);
        }
    }

}
