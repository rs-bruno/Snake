using System.Data.SqlTypes;
using System.Diagnostics;
using System.Net.Sockets;
using System.Runtime.CompilerServices;
using System.Text;

namespace Snake;
class Program
{
    enum PixelType
    {
        Empty,
        Fruit,
        SnakeBody,
        SnakeHead
    }
    struct Pair
    {
        public Pair(int x, int y)
        {
            this.x = x;
            this.y = y;
        }
        public int x, y;
        public static bool operator==(Pair a, Pair b)
        {
            return a.x == b.x && a.y == b.y;
        }
        public static bool operator!=(Pair a, Pair b)
        {
            return a.x != b.x || a.y != b.y;
        }
    }
    enum Direction
    {
        Up,
        Right,
        Down,
        Left
    }
    enum GameState
    {
        Paused,
        Moving,
        Exit
    }
    static string RenderWorld(PixelType[] world, int wWidth, int wHeigth)
    {
        StringBuilder sb = new StringBuilder();
        sb.EnsureCapacity(wWidth * wHeigth);
        for (int i = 0; i < wHeigth; ++i)
        {
            for (int j = 0; j < wWidth; ++j)
            {
                switch (world[i*wWidth + j]) 
                {
                    case PixelType.Empty:
                        sb.Append('.');
                        break;
                    case PixelType.SnakeBody:
                        sb.Append('O');
                        break;
                    case PixelType.Fruit:
                        sb.Append('(');
                        break;
                    case PixelType.SnakeHead:
                        sb.Append('X');
                        break;
                }
            }
            sb.Append('\n');
        }
        return sb.ToString();
    }
    public static void Main(string[] args)
    {
        int wWidth = 15;
        int wHeight = 15;
        int maxFrameRate = 16;
        int stepsBySec = 5;
        long frameInterval = 1000 / maxFrameRate; // frame length in ms
        long moveInterval = 1000 / stepsBySec;
        long timeAcum = 0;

        Console.WindowWidth = wWidth;
        Console.WindowHeight = wHeight + 1;

        GameState state = GameState.Paused;
        Semaphore stateMutex = new Semaphore(1, 1);

        PixelType[] world = new PixelType[wWidth * wHeight];
        Pair nextFruit;

        LinkedList<Pair> snake = new LinkedList<Pair>();
        snake.AddFirst(new Pair(wWidth / 2, wHeight / 2));
        snake.AddFirst(new Pair(wWidth / 2 + 1, wHeight / 2)); // snake head
        Direction looking = Direction.Right;

        Random rnd = new Random(DateTime.Now.Millisecond);
        nextFruit = new Pair(rnd.Next(wWidth), rnd.Next(wHeight));

        // Input management thread
        Thread inputThrd = new Thread(() =>
        {
            while (true)
            {
                var inputKey = Console.ReadKey().Key;

                stateMutex.WaitOne();
                if (state == GameState.Paused)
                {
                    if (inputKey == ConsoleKey.P || inputKey == ConsoleKey.Spacebar)
                        state = GameState.Moving;
                    else if (inputKey == ConsoleKey.Escape)
                        state = GameState.Exit;
                }
                else if (state == GameState.Moving)
                {
                    if (inputKey == ConsoleKey.P || inputKey == ConsoleKey.Spacebar)
                        state = GameState.Paused;
                    else if (inputKey == ConsoleKey.Escape)
                        state = GameState.Exit;
                    else
                    {
                        switch (inputKey)
                        {
                            case ConsoleKey.A:
                            case ConsoleKey.LeftArrow:
                                if (looking != Direction.Right)
                                    looking = Direction.Left;
                                break;
                            case ConsoleKey.D:
                            case ConsoleKey.RightArrow:
                                if (looking != Direction.Left)
                                    looking = Direction.Right;
                                break;
                            case ConsoleKey.W:
                            case ConsoleKey.UpArrow:
                                if (looking != Direction.Down)
                                    looking = Direction.Up;
                                break;
                            case ConsoleKey.S:
                            case ConsoleKey.DownArrow:
                                if (looking != Direction.Up)
                                    looking = Direction.Down;
                                break;
                            default:
                                break;
                        }
                    }
                }
                stateMutex.Release();

            }
        });
        inputThrd.IsBackground = true;
        inputThrd.Start();
        
        // Main loop (world processing + rendering)
        Stopwatch sw = Stopwatch.StartNew();
        long previousPoint = sw.ElapsedMilliseconds;
        while (true) {
            var frameStart = sw.ElapsedMilliseconds;
            stateMutex.WaitOne();
            if (state == GameState.Moving)
            {
                long nextPoint = sw.ElapsedMilliseconds;
                timeAcum += nextPoint - previousPoint;
                previousPoint = nextPoint;
                if (timeAcum > moveInterval)
                {
                    timeAcum = 0;
                    var head = snake.First.Value;
                    switch (looking)
                    {
                        case Direction.Up:
                            head.y = (head.y - 1) >= 0 ? (head.y - 1) : wHeight - 1;
                            break;
                        case Direction.Right:
                            head.x = (head.x + 1) % wWidth;
                            break;
                        case Direction.Down:
                            head.y = (head.y + 1) % wHeight;
                            break;
                        case Direction.Left:
                            head.x = (head.x - 1) >= 0 ? (head.x - 1) : wWidth - 1;
                            break;
                        default:
                            break;
                    }
                    snake.AddFirst(head);
                    if (head == nextFruit)
                    {
                        while (head == nextFruit)
                        {
                            nextFruit = new Pair(rnd.Next(wWidth), rnd.Next(wHeight));
                        }
                    }
                    else
                    {
                        snake.RemoveLast();
                    }
                }
            }
            else if (state == GameState.Exit)
            {
                break;
            }
            stateMutex.Release();
            // update world representation
            for (int i = 0; i < world.Length; ++i)
            {
                world[i] = PixelType.Empty;
            }
            foreach (var node in snake)
            {
                world[node.x + node.y * wWidth] = PixelType.SnakeBody;
            }
            world[nextFruit.x + nextFruit.y * wWidth] = PixelType.Fruit;
            // print world representation
            Console.Write(RenderWorld(world, wWidth, wHeight));
            var frameLen = sw.ElapsedMilliseconds - frameStart;
            Thread.Sleep((int)Math.Max(0, frameInterval - frameLen));
        }
    }
}