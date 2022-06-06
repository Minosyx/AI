using System;
using System.Windows.Media;

namespace Draughts
{
    public enum Player
    {
        Human,
        AI
    }
    public enum CheckerStatus
    {
        Empty,
        P1,
        P1King,
        P2,
        P2King
    }

    public static class Settings
    {
        public static int N { get; } = 8;
        public static Brush P1 { get; } = Brushes.CornflowerBlue;
        public static Brush P2 { get; } = Brushes.Red;
        public static Player StartingPlayer = Player.Human;

        public static CheckerStatus[,] StartPosition
        {
            get
            {
                CheckerStatus[,] board = InitializeEmptyBoard();
                for (int i = 0; i < board.GetLength(0) / 2 - 1; i++)
                {
                    int j = i % 2 == 0 ? 1 : 0;
                    int step = j == 0 ? 1 : -1;
                    int cols = board.GetLength(1);
                    for (; j < cols; j += 2)
                    {
                        //board[i, j] = CheckerStatus.P2;
                        board[cols - i - 1, j + step] = CheckerStatus.P1;
                    }
                }

                //board[4, 1] = CheckerStatus.P2;
                board[2, 7] = CheckerStatus.P2;
                board[2, 1] = CheckerStatus.P1;
                board[1, 2] = CheckerStatus.P2;
                board[1, 4] = CheckerStatus.P2;


                //board[1, 4] = CheckerStatus.Empty;

                return board;
            }
        }

        private static CheckerStatus[,] InitializeEmptyBoard()
        {
            var board = new CheckerStatus[N, N];

            for (int i = 0; i < board.GetLength(0); i++)
                for (int j = 0; j < board.GetLength(1); j++)
                    board[i, j] = CheckerStatus.Empty;
            return board;
        }
    }
}
