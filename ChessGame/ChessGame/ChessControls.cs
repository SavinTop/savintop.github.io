using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ChessGame
{
    static class ChessControls
    {
        public static void MouseDown(object sender, MouseEventArgs e)
        {
            byte i = (byte)(e.Y / Visual.sqr);
            byte j = (byte)(e.X / Visual.sqr);

            if (!(Chess.figureSelected & Chess.MoveTo(i, j)))
            {
                Chess.SelectFigure(i, j);
            }

            Visual.RedrawFigField();
            Visual.RedrawMovementField();
            Visual.Redraw();
        }
    }
}
