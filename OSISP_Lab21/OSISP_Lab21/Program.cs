
using System.Collections.Concurrent;

public class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("============Output Args================");

        foreach (string arg in args)
        {
            Console.WriteLine(arg);
        }

        Console.WriteLine("======================================");
    }
}
