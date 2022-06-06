using System;

namespace Draughts
{
    public class Game
    {
        
        public CheckerStatus[,] Board { get; }
        //public bool P1Turn { get; }

        public event EventHandler<Player>? OnGameWon;
        public event EventHandler<Player>? OnPlayerChanged;

        private (int P1Count, int P2Count) remainingCheckers;
        private Player currentPlayer;

        public Player CurrentPlayer
        {
            get => currentPlayer;
            set
            {
                currentPlayer = value;
                OnPlayerChanged?.Invoke(this, currentPlayer);
            }
        }


        public (int P1Count, int P2Count) RemainingCheckers
        {
            get => remainingCheckers;
            set
            {
                if (value.P1Count == 0)
                    OnGameWon?.Invoke(this, Player.AI);
                else if (value.P2Count == 0)
                    OnGameWon?.Invoke(this, Player.Human);
                else remainingCheckers = value;
            }
        }

        public Game()
        {
            Board = Settings.StartPosition;
            CurrentPlayer = Settings.StartingPlayer;
            RemainingCheckers = GameHelper.CountCheckers(Board);
        }

    }
}
