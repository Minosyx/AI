using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace Draughts
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>

    public partial class MainWindow : Window
    {
        private List<Button> ButtonList;

        private Button SelectedButton;
        private bool clicked;

        private (int row, int column) Coords;
        private (int row, int column) PrevCoords;

        private Brush P1Color = Settings.P1;
        private Brush P2Color = Settings.P2;

        private Brush DefaultBorderBrush;
        private Thickness DefaultBorderThickness;

        private Game game;

        public MainWindow()
        {
            InitializeComponent();
            Board.FillBoard(Button_Click);
            Human_id.Fill = P1Color;
            AI_id.Fill = P2Color;

            StartGame();
        }

        private void StartGame()
        {
            ButtonList = Board.Children.Cast<Button>().ToList();

            var button = ButtonList.First();
            DefaultBorderBrush = button.BorderBrush;
            DefaultBorderThickness = button.BorderThickness;

            game = new Game();
            game.OnGameWon += GameOver;
            game.OnPlayerChanged += SetCurrentPlayer;

            PrevCoords = Coords = (-1, -1);
            Board.DrawCheckers(game.Board);
        }

        private void BorderChangeOnClick(Button button)
        {
            button.BorderThickness = new Thickness(5);
            button.BorderBrush = Brushes.GreenYellow;
        }

        private void ResetBorder(Button button)
        {
            button.BorderThickness = DefaultBorderThickness;
            button.BorderBrush = DefaultBorderBrush;
        }

        public void GameOver(object? sender, Player winner)
        {
            if (winner == Player.AI)
            {
                MessageBoxResult result = MessageBox.Show("You lost!", "GAME OVER");
            }
            else
            {
                MessageBoxResult result = MessageBox.Show("You won!", "GAME OVER");
            }
            Close();
        }

        private void SetCurrentPlayer(object? sender, Player player)
        {
            Current.Text = player == Player.Human ? "Human Turn" : "AI Turn";
        }

        private bool IsLegalNormalMove() =>
            game.IsFree(Coords) && !game.PossibleJump(ButtonList) && game.IsForward(Coords, PrevCoords);

        private bool IsLegalJumpMove() => game.IsFree(Coords) && game.IsJump(Coords, PrevCoords);
            

        private bool IsKinged()
        {
            if ((Coords.row != 0 || game.Board[PrevCoords.row, PrevCoords.column] != CheckerStatus.P1) &&
                (Coords.row != game.Board.GetUpperBound(0) ||
                 game.Board[PrevCoords.row, PrevCoords.column] != CheckerStatus.P2)) return false;
            game.Swap(PrevCoords, Coords);
            game.MakeKing(Coords);
            return true;

        }

        private void EndTurn()
        {
            IsKinged();

            clicked = !clicked;
            game.CurrentPlayer = game.CurrentPlayer == Player.AI ? Player.Human : Player.AI;
        }

        private void InvalidChoice()
        {
            if (SelectedButton != null) ResetBorder(SelectedButton);
            clicked = false;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (sender is not Button button) return;
            Coords = (Grid.GetRow(button), Grid.GetColumn(button));

            if (game.CurrentPlayer == Player.Human)
            {
                if (clicked)
                {
                    PrevCoords = (Grid.GetRow(SelectedButton), Grid.GetColumn(SelectedButton));

                    if (game.Board[PrevCoords.row, PrevCoords.column] == CheckerStatus.P1)
                    {
                        if (IsLegalNormalMove())
                        {
                            if (!IsKinged())
                            {
                                game.Swap(PrevCoords, Coords);
                                ResetBorder(SelectedButton);
                            }

                            Board.DrawCheckers(game.Board);
                            EndTurn();
                            ResetBorder(SelectedButton);
                        }
                        else if (IsLegalJumpMove())
                        {
                            game.RemoveEnemyChecker(PrevCoords, Coords);
                            game.Swap(PrevCoords, Coords);
                            Board.DrawCheckers(game.Board);
                            ResetBorder(SelectedButton);
                            if (game.CanJumpPawn(Coords))
                            {
                                SelectedButton = button;
                                BorderChangeOnClick(button);
                            }
                            else
                            {
                                EndTurn();
                                ResetBorder(SelectedButton);
                            }
                        }
                        else if (game.Board[Coords.row, Coords.column] == CheckerStatus.P1 ||
                                 game.Board[Coords.row, Coords.column] == CheckerStatus.P1King)
                        {
                            ResetBorder(SelectedButton);
                            SelectedButton = button;
                            BorderChangeOnClick(button);
                        }
                        else
                        {
                            InvalidChoice();
                        }
                    }
                    else
                    {
                        if (IsLegalNormalMove())
                        {
                            game.Swap(PrevCoords, Coords);
                            Board.DrawCheckers(game.Board);
                            EndTurn();
                        }
                        else if (IsLegalJumpMove())
                        {
                            game.RemoveEnemyChecker(PrevCoords, Coords);
                            game.Swap(PrevCoords, Coords);
                            Board.DrawCheckers(game.Board);
                            ResetBorder(SelectedButton);
                        }
                    }
                }
                else // może się zatrzymywać dalej aniżeli zbił TODO: Poprawić zachowanie dla damki
                {
                    if (game.Board[Coords.row, Coords.column] == CheckerStatus.P1 ||
                        game.Board[Coords.row, Coords.column] == CheckerStatus.P1King)
                    {
                        SelectedButton = button;
                        BorderChangeOnClick(button);
                        clicked = true;
                    }
                }
            }
        }
    }
}
