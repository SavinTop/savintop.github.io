using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ChessGame
{
    enum Figure
    {
        noFigure,
        whitePawn ,
        whiteKnight,
        whiteBishop,
        whiteRook,
        whiteKing,
        whiteQueen,
        blackPawn,
        blackKnight,
        blackBishop,
        blackRook,
        blackKing,
        blackQueen,
    }

    struct slctdFig
    {
        public Figure fig;
        public bool isWhite;
        public byte i;
        public byte j;
    }

    static class Chess
    {
        static readonly byte[,] defaultField = {
        {10, 08, 09, 12, 11, 09, 08, 10},
        {07, 07, 07, 07, 07, 07, 07, 07},
        {00, 00, 00, 00, 00, 00, 00, 00},
        {00, 00, 00, 00, 00, 00, 00, 00},
        {00, 00, 00, 00, 00, 00, 00, 00},
        {00, 00, 00, 00, 00, 00, 00, 00},
        {01, 01, 01, 01, 01, 01, 01, 01},
        {04, 02, 03, 06, 05, 03, 02, 04}};

        static byte[,] FigField = new byte[8,8];
        static byte[,] CurrentMovementMap  = new byte[8,8];

        static public slctdFig SelectedFigure;

        static bool whiteTurn = true;
        public static bool figureSelected = false;

        public static void InitField()
        {
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                    FigField[i, j] = defaultField[i, j];
        }

        public static void Init()
        {
            InitField();
        }

        public static byte[,] GetField()
        {
            return FigField;
        }

        public static byte[,] GetMovementField()
        {
            return CurrentMovementMap;
        }

        public static void RotateField()
        {
            whiteTurn = !whiteTurn;
            for(int i=0;i<4;i++)
                for(int j=0;j<8;j++)
                {
                    byte temp = FigField[i, j];
                    FigField[i,j] = FigField[7-i, 7-j];
                    FigField[7 - i, 7 - j] = temp;
                }
        }

        public static slctdFig getFigureInfo(byte i, byte j)
        {
            slctdFig temp = new slctdFig();
            temp.fig = (Figure)FigField[i, j];
            temp.i = i;
            temp.j = j;
            temp.isWhite = (int)temp.fig <= 6;
            return temp;
        }

        static void Win()
        {
            if (whiteTurn)
                MessageBox.Show("Белые Победили");
            else
                MessageBox.Show("Черные Победили");

            MapBuilder.ClearField();
            InitField();   
        }

        public static bool MoveTo(byte i, byte j)
        {
            if (CurrentMovementMap[i, j] == 0) return false;
            FigField[SelectedFigure.i, SelectedFigure.j] = 0;

            if((Figure)FigField[i,j]==Figure.blackKing || (Figure)FigField[i, j] == Figure.whiteKing)
            {
                Win();
                return true;
            }

            FigField[i, j] = (byte)SelectedFigure.fig;
            figureSelected = false;
            MapBuilder.ClearField();
            RotateField();
            return true;
        }

        public static bool SelectFigure(byte i, byte j)
        {
            slctdFig temp = getFigureInfo(i,j);
            MapBuilder.ClearField();

            if (temp.fig == Figure.noFigure || temp.isWhite != whiteTurn)
            {
                return false;
            }

            figureSelected = true;
            SelectedFigure = temp;
            CurrentMovementMap = MapBuilder.BuildFigureMovementMap(temp);
            return true;
        }
    }

    static class MapBuilder
    {
        static byte[,] MovementMap = new byte[8, 8];
        static slctdFig CurrentFig;

        static readonly sbyte[,] KnightMovement =
          {
            {1, -2},
            {2, -1},
            {2, 1},
            {1, 2},
            {-1, 2},
            {-2, 1},
            {-2, -1},
            {-1, -2}
          };

        static readonly sbyte[,] KingMovement =
          {
            {-1,-1},
            {0,-1},
            {1,-1},
            {-1,0},
            {1,0},
            {-1,1},
            {0,1},
            {1,1},
          };

        public static void ClearField()
        {
            Array.Clear(MovementMap, 0, MovementMap.Length);
        }

        public static byte[,] BuildFigureMovementMap(slctdFig figure)
        {
            CurrentFig = figure;

            ClearField();

            switch(figure.fig)
            {
                case Figure.whitePawn:
                case Figure.blackPawn:
                    BuildPawnMap();
                    break;
                case Figure.whiteKnight:
                case Figure.blackKnight:
                    BuildKnightMap();
                    break;
                case Figure.whiteBishop:
                case Figure.blackBishop:
                    BuildBishopMap();
                    break;
                case Figure.whiteRook:
                case Figure.blackRook:
                    BuildRookMap();
                    break;
                case Figure.whiteKing:
                case Figure.blackKing:
                    BuildKingMap();
                    break;
                case Figure.whiteQueen:
                case Figure.blackQueen:
                    BuildQueenMap();
                    break;
            }

            return MovementMap;
        }

        static void BuildPawnMap()
        {
            if (CurrentFig.i == 0) return;

            slctdFig FirstStep = Chess.getFigureInfo((byte)(CurrentFig.i - 1),CurrentFig.j);

            if(CurrentFig.j!=0)
            {
                slctdFig UpLeft = Chess.getFigureInfo((byte)(CurrentFig.i - 1), (byte)(CurrentFig.j - 1));
                if (UpLeft.isWhite != CurrentFig.isWhite && UpLeft.fig!=Figure.noFigure)
                    MovementMap[UpLeft.i, UpLeft.j] = 2;
            }        

            if(CurrentFig.j!=7)
            {
                slctdFig UpRight = Chess.getFigureInfo((byte)(CurrentFig.i - 1), (byte)(CurrentFig.j + 1));

                if (UpRight.isWhite != CurrentFig.isWhite && UpRight.fig != Figure.noFigure)
                    MovementMap[UpRight.i, UpRight.j] = 2;
            }

            if (FirstStep.fig != Figure.noFigure) return;
            MovementMap[FirstStep.i, FirstStep.j] = 1;

            slctdFig SecondStep = Chess.getFigureInfo((byte)(CurrentFig.i - 2), CurrentFig.j);

            if (CurrentFig.i==6 && SecondStep.fig==Figure.noFigure)
                MovementMap[SecondStep.i, SecondStep.j] = 1;
        }

        static void BuildKnightMap()
        {
            BuildMapFromArray(KnightMovement);
        }

        static void BuildBishopMap()
        {
            DiagonalMap();
        }

        static void BuildRookMap()
        {
            VerticalAndHorizontalMap();
        }

        static void BuildKingMap()
        {
            BuildMapFromArray(KingMovement);
        }

        static void BuildQueenMap()
        {
            VerticalAndHorizontalMap();
            DiagonalMap();
        }

        static void BuildMapFromArray(sbyte[,] array)
        {
            for (int k = 0; k < array.GetLength(0); k++)
            {
                sbyte i = (sbyte)(array[k, 1] + CurrentFig.i);
                sbyte j = (sbyte)(array[k, 0] + CurrentFig.j);

                if (i < 0 || i > 7 || j < 0 || j > 7) continue;

                slctdFig tempFig = Chess.getFigureInfo((byte)i, (byte)j);

                if (tempFig.fig == Figure.noFigure) MovementMap[i, j] = 1;
                else
                if (tempFig.isWhite != CurrentFig.isWhite) MovementMap[i, j] = 2;
            }
        }

        static void MakeRay(sbyte stepI, sbyte stepJ)
        {
            sbyte i = (sbyte)(CurrentFig.i+stepI), j = (sbyte)(CurrentFig.j+stepJ);

            for (; i >= 0 && i <= 7 && j >= 0 && j <= 7; i += stepI, j+=stepJ)
            {
                slctdFig temp = Chess.getFigureInfo((byte)i, (byte)j);
                if (temp.fig != Figure.noFigure)
                {
                    if (temp.isWhite != CurrentFig.isWhite) MovementMap[i, j] = 2;
                    break;
                }
                MovementMap[i, j] = 1;
            }
        }

        static void VerticalAndHorizontalMap()
        {
            MakeRay(0, 1);
            MakeRay(0, -1);
            MakeRay(1, 0);
            MakeRay(-1, 0);
        }

        static void DiagonalMap()
        {
            MakeRay(1, 1);
            MakeRay(1, -1);
            MakeRay(-1, 1);
            MakeRay(-1, -1);
        }
    }
}
