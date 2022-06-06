using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace Draughts
{
    public static class DrawingHelper
    {
        private static void DefineGrid(this Grid board)
        {
            int capacity = Settings.N;

            for (int i = 0; i < capacity; i++)
            {
                board.RowDefinitions.Add(new RowDefinition(){Height = new GridLength(1, GridUnitType.Star)});
                board.ColumnDefinitions.Add(new ColumnDefinition(){Width = new GridLength(1, GridUnitType.Star)});
            }
        }

        public static void FillBoard(this Grid board, RoutedEventHandler command)
        {
            board.DefineGrid();

            int boardSize = board.RowDefinitions.Count;
            int counter = 0;

            for (int i = 0; i < boardSize; ++i)
            {
                for (int j = 0; j < boardSize; ++j)
                {
                    if ((i % 2 == 0 && j % 2 == 1) || (i % 2 == 1 && j % 2 == 0))
                    {
                        Button b = new Button()
                        {
                            Background = Brushes.Black,
                            BorderBrush = Brushes.DarkGray,
                            Name = $"btn{counter++}"
                        };
                        b.Click += command;
                        board.Children.Add(b);
                        Grid.SetRow(b, i);
                        Grid.SetColumn(b, j);
                    }
                }
            }
        }

        public static void DrawCheckers(this Grid board, CheckerStatus[,] placement)
        {
            foreach (Button element in board.Children)
            {
                int row = Grid.GetRow(element);
                int column = Grid.GetColumn(element);

                switch (placement[row, column])
                {
                    case CheckerStatus.P1:
                    {
                        Ellipse P1Pawn = new Ellipse() {Width = 75, Height = 75, Fill = Settings.P1};
                        element.Content = P1Pawn;
                    }
                        break;
                    case CheckerStatus.P2:
                    {
                        Ellipse P2Pawn = new Ellipse() {Width = 75, Height = 75, Fill = Settings.P2};
                        element.Content = P2Pawn;
                    }
                        break;
                    case CheckerStatus.P1King:
                    {
                        Grid Wrapper = new Grid();
                        Ellipse P1Pawn = new Ellipse() {Width = 75, Height = 75, Fill = Settings.P1};
                        Ellipse P1King = new Ellipse() {Width = 30, Height = 30, Stroke = Brushes.Gold, StrokeThickness = 5};
                        Wrapper.Children.Add(P1Pawn);
                        Wrapper.Children.Add(P1King);
                        element.Content = Wrapper;
                    }
                        break;
                    case CheckerStatus.P2King:
                    {
                        Grid Wrapper = new Grid();
                        Ellipse P2Pawn = new Ellipse() { Width = 75, Height = 75, Fill = Settings.P2 };
                        Ellipse P2King = new Ellipse() { Width = 30, Height = 30, Stroke = Brushes.Gold, StrokeThickness = 5};
                        Wrapper.Children.Add(P2Pawn);
                        Wrapper.Children.Add(P2King);
                        element.Content = Wrapper;
                    }
                        break;
                    case CheckerStatus.Empty:
                        element.Content = null;
                        break;
                    default:
                        throw new ArgumentOutOfRangeException();
                }
            }
        }
    }

    public static class GameHelper
    {
        public static (int P1Count, int P2Count) CountCheckers(CheckerStatus[,] board)
        {
            int P1Count = 0;
            int P2Count = 0;
            foreach (var el in board)
            {
                switch (el)
                {
                    case CheckerStatus.P1:
                    case CheckerStatus.P1King:
                        P1Count++;
                        break;
                    case CheckerStatus.P2:
                    case CheckerStatus.P2King:
                        P2Count++;
                        break;
                }
            }

            return (P1Count, P2Count);
        }

        public static bool IsFree(this Game game, (int x, int y) coords) =>
            game.Board[coords.x, coords.y] == CheckerStatus.Empty;

        public static bool CanJumpPawn(this Game game, (int x, int y) coords)
        {
            var columns = game.Board.GetLength(0);
            switch (game.CurrentPlayer)
            {
                case Player.Human:
                    if (coords.x - 1 > 0 && coords.y - 1 > 0 && coords.x - 2 > 0 && coords.y - 2 > 0 &&
                        (game.Board[coords.x - 1, coords.y - 1] == CheckerStatus.P2 ||
                         game.Board[coords.x - 1, coords.y - 1] == CheckerStatus.P2King) &&
                        game.Board[coords.x - 2, coords.y - 2] == CheckerStatus.Empty) return true;
                    if (coords.x + 1 < columns && coords.y + 1 < columns && coords.x + 2 < columns &&
                        coords.y + 2 < columns &&
                        (game.Board[coords.x + 1, coords.y + 1] == CheckerStatus.P2 ||
                         game.Board[coords.x + 1, coords.y + 1] == CheckerStatus.P2King) &&
                        game.Board[coords.x + 2, coords.y + 2] == CheckerStatus.Empty) return true;
                    if (coords.x - 1 > 0 && coords.y + 1 < columns && coords.x - 2 > 0 && coords.y + 2 < columns &&
                        (game.Board[coords.x - 1, coords.y + 1] == CheckerStatus.P2 ||
                         game.Board[coords.x - 1, coords.y + 1] == CheckerStatus.P2King) &&
                        game.Board[coords.x - 2, coords.y + 2] == CheckerStatus.Empty) return true;
                    if (coords.x + 1 < columns && coords.y - 1 > 0 && coords.x + 2 < columns && coords.y - 2 > 0 &&
                        (game.Board[coords.x + 1, coords.y - 1] == CheckerStatus.P2 ||
                         game.Board[coords.x + 1, coords.y - 1] == CheckerStatus.P2King) &&
                        game.Board[coords.x + 2, coords.y - 2] == CheckerStatus.Empty) return true;
                    return false;
                case Player.AI:
                    if (coords.x - 1 > 0 && coords.y - 1 > 0 && coords.x - 2 > 0 && coords.y - 2 > 0 &&
                        (game.Board[coords.x - 1, coords.y - 1] == CheckerStatus.P1 ||
                         game.Board[coords.x - 1, coords.y - 1] == CheckerStatus.P1King) &&
                        game.Board[coords.x - 2, coords.y - 2] == CheckerStatus.Empty) return true;
                    if (coords.x + 1 < columns && coords.y + 1 < columns && coords.x + 2 < columns &&
                        coords.y + 2 < columns &&
                        (game.Board[coords.x + 1, coords.y + 1] == CheckerStatus.P1 ||
                         game.Board[coords.x + 1, coords.y + 1] == CheckerStatus.P1King) &&
                        game.Board[coords.x + 2, coords.y + 2] == CheckerStatus.Empty) return true;
                    if (coords.x - 1 > 0 && coords.y + 1 < columns && coords.x - 2 > 0 && coords.y + 2 < columns &&
                        (game.Board[coords.x - 1, coords.y + 1] == CheckerStatus.P1 ||
                         game.Board[coords.x - 1, coords.y + 1] == CheckerStatus.P1King) &&
                        game.Board[coords.x - 2, coords.y + 2] == CheckerStatus.Empty) return true;
                    if (coords.x + 1 < columns && coords.y - 1 > 0 && coords.x + 2 < columns && coords.y - 2 > 0 &&
                        (game.Board[coords.x + 1, coords.y - 1] == CheckerStatus.P1 ||
                         game.Board[coords.x + 1, coords.y - 1] == CheckerStatus.P1King) &&
                        game.Board[coords.x + 2, coords.y - 2] == CheckerStatus.Empty) return true;
                    return false;
                default:
                    throw new ArgumentOutOfRangeException();
            }
 
        }

        private static bool HasOpposingNeighboursPawn(this Game game, List<(int x, int y)> pawnsList)
        {
            if (pawnsList.Count == 0) return false;
            foreach ((int x, int y) coords in pawnsList)
            {
                if (game.CanJumpPawn(coords)) return true;
            }

            return false;
        }

        public static bool CanJumpKing(this Game game, (int x, int y) coords)
        {
            var columns = game.Board.GetLength(0);
            int xCopy, yCopy;
            switch (game.CurrentPlayer)
            {
                case Player.Human:
                    (xCopy, yCopy) = coords;
                    while (xCopy - 2 > 0 && yCopy - 2 > 0)
                    {
                        xCopy--;
                        yCopy--;
                        if (game.Board[xCopy, yCopy] == CheckerStatus.P1 ||
                            game.Board[xCopy, yCopy] == CheckerStatus.P1King) break;

                        if ((game.Board[xCopy, yCopy] == CheckerStatus.P2 ||
                             game.Board[xCopy, yCopy] == CheckerStatus.P2King) &&
                            game.Board[xCopy - 1, yCopy - 1] == CheckerStatus.Empty) return true;
                    }

                    (xCopy, yCopy) = coords;
                    while (xCopy + 2 < columns && yCopy + 2 < columns)
                    {
                        xCopy++;
                        yCopy++;
                        if (game.Board[xCopy, yCopy] == CheckerStatus.P1 ||
                            game.Board[xCopy, yCopy] == CheckerStatus.P1King) break;

                        if ((game.Board[xCopy, yCopy] == CheckerStatus.P2 ||
                             game.Board[xCopy, yCopy] == CheckerStatus.P2King) &&
                            game.Board[xCopy + 1, yCopy + 1] == CheckerStatus.Empty) return true;
                    }

                    (xCopy, yCopy) = coords;
                    while (xCopy - 2 > 0 && yCopy + 2 < columns)
                    {
                        xCopy--;
                        yCopy++;
                        if (game.Board[xCopy, yCopy] == CheckerStatus.P1 ||
                            game.Board[xCopy, yCopy] == CheckerStatus.P1King) break;

                        if ((game.Board[xCopy, yCopy] == CheckerStatus.P2 ||
                             game.Board[xCopy, yCopy] == CheckerStatus.P2King) &&
                            game.Board[xCopy - 1, yCopy + 1] == CheckerStatus.Empty) return true;
                    }

                    (xCopy, yCopy) = coords;
                    while (xCopy + 2 < columns && yCopy - 2 > 0)
                    {
                        xCopy++;
                        yCopy--;
                        if (game.Board[xCopy, yCopy] == CheckerStatus.P1 ||
                            game.Board[xCopy, yCopy] == CheckerStatus.P1King) break;

                        if ((game.Board[xCopy, yCopy] == CheckerStatus.P2 ||
                             game.Board[xCopy, yCopy] == CheckerStatus.P2King) &&
                            game.Board[xCopy + 1, yCopy - 1] == CheckerStatus.Empty) return true;
                    }

                    return false;
                case Player.AI:
                    (xCopy, yCopy) = coords;
                    while (xCopy - 2 > 0 && yCopy - 2 > 0)
                    {
                        xCopy--;
                        yCopy--;
                        if (game.Board[xCopy, yCopy] == CheckerStatus.P2 ||
                            game.Board[xCopy, yCopy] == CheckerStatus.P2King) break;

                        if ((game.Board[xCopy, yCopy] == CheckerStatus.P1 ||
                             game.Board[xCopy, yCopy] == CheckerStatus.P1King) &&
                            game.Board[xCopy - 1, yCopy - 1] == CheckerStatus.Empty) return true;
                    }

                    (xCopy, yCopy) = coords;
                    while (xCopy + 2 < columns && yCopy + 2 < columns)
                    {
                        xCopy++;
                        yCopy++;
                        if (game.Board[xCopy, yCopy] == CheckerStatus.P2 ||
                            game.Board[xCopy, yCopy] == CheckerStatus.P2King) break;

                        if ((game.Board[xCopy, yCopy] == CheckerStatus.P1 ||
                             game.Board[xCopy, yCopy] == CheckerStatus.P1King) &&
                            game.Board[xCopy + 1, yCopy + 1] == CheckerStatus.Empty) return true;
                    }

                    (xCopy, yCopy) = coords;
                    while (xCopy - 2 > 0 && yCopy + 2 < columns)
                    {
                        xCopy--;
                        yCopy++;
                        if (game.Board[xCopy, yCopy] == CheckerStatus.P2 ||
                            game.Board[xCopy, yCopy] == CheckerStatus.P2King) break;

                        if ((game.Board[xCopy, yCopy] == CheckerStatus.P1 ||
                             game.Board[xCopy, yCopy] == CheckerStatus.P1King) &&
                            game.Board[xCopy - 1, yCopy + 1] == CheckerStatus.Empty) return true;
                    }

                    (xCopy, yCopy) = coords;
                    while (xCopy + 2 < columns && yCopy - 2 > 0)
                    {
                        xCopy++;
                        yCopy--;
                        if (game.Board[xCopy, yCopy] == CheckerStatus.P2 ||
                            game.Board[xCopy, yCopy] == CheckerStatus.P2King) break;

                        if ((game.Board[xCopy, yCopy] == CheckerStatus.P1 ||
                             game.Board[xCopy, yCopy] == CheckerStatus.P1King) &&
                            game.Board[xCopy + 1, yCopy - 1] == CheckerStatus.Empty) return true;
                    }

                    return false;
                default:
                    throw new ArgumentOutOfRangeException();
            }

        }
        
        private static bool HasOpposingNeighborsKing(this Game game, List<(int x, int y)> kingsList)
        {
            if (kingsList.Count == 0) return false;
            foreach ((int x, int y) coords in kingsList)
            {
                if (game.CanJumpKing(coords)) return true;
            }

            return false;
        }

        public static bool PossibleJump(this Game game, List<Button> buttons)
        {
            List<(int x, int y)> pawnsToCheck;
            List<(int x, int y)> kingsToCheck;
            if (game.CurrentPlayer == Player.Human)
            {
                pawnsToCheck = buttons.Select(x => (Grid.GetRow(x), Grid.GetColumn(x)))
                    .Where(x => game.Board[x.Item1, x.Item2] == CheckerStatus.P1)
                    .ToList();
                kingsToCheck = buttons.Select(x => (Grid.GetRow(x), Grid.GetColumn(x)))
                    .Where(x => game.Board[x.Item1, x.Item2] == CheckerStatus.P1King)
                    .ToList();

                return game.HasOpposingNeighboursPawn(pawnsToCheck) || game.HasOpposingNeighborsKing(kingsToCheck);
            }
            else
            {
                pawnsToCheck = buttons.Select(x => (Grid.GetRow(x), Grid.GetColumn(x)))
                    .Where(x => game.Board[x.Item1, x.Item2] == CheckerStatus.P2)
                    .ToList();
                kingsToCheck = buttons.Select(x => (Grid.GetRow(x), Grid.GetColumn(x)))
                    .Where(x => game.Board[x.Item1, x.Item2] == CheckerStatus.P2King)
                    .ToList();

                return game.HasOpposingNeighboursPawn(pawnsToCheck) || game.HasOpposingNeighborsKing(kingsToCheck);
            }
        }

        public static bool IsForward(this Game game, (int x, int y) coords, (int x, int y) prevCoords)
        {
            switch (game.CurrentPlayer)
            {
                case Player.Human:
                    return game.Board[prevCoords.x, prevCoords.y] switch
                    {
                        CheckerStatus.P1 => coords.x - prevCoords.x == -1,
                        _ => true
                    };
                case Player.AI:
                    return game.Board[prevCoords.x, prevCoords.y] switch
                    {
                        CheckerStatus.P2 => coords.x - prevCoords.x == 1,
                        _ => true
                    };
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        public static bool IsJump(this Game game, (int x, int y) coords, (int x, int y) prevCoords)
        {
            switch (game.CurrentPlayer)
            {
                case Player.Human:
                    return game.Board[prevCoords.x, prevCoords.y] switch
                    {
                        CheckerStatus.P1 => Math.Abs(coords.x - prevCoords.x) == 2,
                        CheckerStatus.P1King => Math.Abs(coords.x - prevCoords.x) >= 2,
                        _ => throw new ArgumentOutOfRangeException()
                    };
                case Player.AI:
                    return game.Board[prevCoords.x, prevCoords.y] switch
                    {
                        CheckerStatus.P2 => Math.Abs(coords.x - prevCoords.x) == 2,
                        CheckerStatus.P2King => Math.Abs(coords.x - prevCoords.x) >= 2,
                        _ => throw new ArgumentOutOfRangeException()
                    };
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        public static void Swap(this Game game, (int x, int y) prevCoords, (int x, int y) coords)
        {
            (game.Board[prevCoords.x, prevCoords.y], game.Board[coords.x, coords.y]) = (game.Board[coords.x, coords.y],
                game.Board[prevCoords.x, prevCoords.y]);
        }

        public static void MakeKing(this Game game, (int x, int y) coords)
        {
            game.Board[coords.x, coords.y] = game.CurrentPlayer switch
            {
                Player.Human => CheckerStatus.P1King,
                Player.AI => CheckerStatus.P2King,
                _ => game.Board[coords.x, coords.y]
            };
        }

        public static void RemoveEnemyChecker(this Game game, (int x, int y) prevCoords, (int x, int y) coords)
        {
            var (diffX, diffY) = (prevCoords.x - coords.x, prevCoords.y - coords.y);
            int dx = diffX / Math.Abs(diffX);
            int dy = diffY / Math.Abs(diffY);

            game.Board[coords.x + dx, coords.y + dy] = CheckerStatus.Empty;

            game.RemainingCheckers = game.CurrentPlayer switch
            {
                Player.Human => (game.RemainingCheckers.P1Count, game.RemainingCheckers.P2Count - 1),
                Player.AI => (game.RemainingCheckers.P1Count - 1, game.RemainingCheckers.P2Count),
                _ => game.RemainingCheckers
            };
        }
    }
}
