using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Draughts
{
    public class CountCheckersEventArgs : EventArgs
    {
        public CheckerStatus[,] Board { get; set; }
        delegate void OnCounted(int p1, int p2);
    }
}
