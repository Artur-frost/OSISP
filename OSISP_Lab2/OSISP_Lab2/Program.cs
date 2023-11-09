using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;


//Path C:\Users\MyPC\source\c#\OSISP_Lab21\OSISP_Lab21\bin\Debug\net7.0\OSISP_Lab21.exe$03.10.2023 21:50$dfgdf 3 fg
//C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe$03.10.2023 21:57$ 

class Program
{
    static List<AppInfo> appList = new List<AppInfo>();
    static string dataFilePath = "appData.json"; // Путь к файлу данных

    static async Task Main(string[] args)
    {
        // Загрузка данных из файла (если он существует)
        LoadAppData();

        // Запуск асинхронной функции для проверки приложений
        Task checkingTask = new(() => CheckAppsPeriodically());
        checkingTask.Start();

        // Основной цикл для ввода приложений
        while (true)
        {
            Console.WriteLine("Введите информацию о приложении (путь$дата/время$аргументы), либо нажмите Enter для завершения:");
            string? input = Console.ReadLine();

            if (string.IsNullOrWhiteSpace(input))
            {
                break;
            }

            var l = appList;

            string[] parts = input.Split(new char[] { '$' }, StringSplitOptions.RemoveEmptyEntries);

            if (parts.Length != 3)
            {
                Console.WriteLine("Неверный формат ввода. Попробуйте снова.");
                continue;
            }

            string appPath = parts[0].Trim();
            if (!DateTime.TryParseExact(parts[1].Trim(), "dd.MM.yyyy HH:mm", null, System.Globalization.DateTimeStyles.None, out DateTime startTime))
            {
                Console.WriteLine("Неверный формат даты/времени. Попробуйте снова.");
                continue;
            }

            string appArguments = parts[2].Trim();

            appList.Add(new AppInfo
            {
                AppPath = appPath,
                StartTime = startTime,
                AppArguments = appArguments
            });

            Console.WriteLine("Приложение добавлено в очередь для запуска.");
        }

        // Сериализация данных перед завершением программы
        SaveAppData();

        // Ожидание завершения асинхронной функции
        //await checkingTask;
        checkingTask.Dispose();
        Console.WriteLine("Метод закончили принудительно");
    }

    static async Task CheckAppsPeriodically()
    {
        Console.WriteLine("Метод стартанул");
        while (true)
        {
            DateTime currentTime = DateTime.Now;

            //Console.WriteLine("Метод работает");

            for (int i = 0; i < appList.Count; i++)
            {
                if (currentTime >= appList[i].StartTime)
                {
                    Console.WriteLine($"Время запуска приложения '{appList[i].AppPath}' пришло({appList[i].StartTime}). Запускаем...");

                    try
                    {
                        Process process = new Process();
                        process.StartInfo.FileName = appList[i].AppPath;
                        process.StartInfo.Arguments = appList[i].AppArguments;
                        process.Start();
                        Console.WriteLine($"Приложение '{appList[i].AppPath}' успешно запущено.");
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine($"Ошибка при запуске приложения '{appList[i].AppPath}': {ex.Message}");
                    }

                    // Удаление приложения из списка, так как оно уже было запущено
                    appList.Remove(appList[i]);
                }
            }

            // Пауза перед следующей проверкой
            await Task.Delay(1000); // Проверяем каждую секунду
        }
    }

    static void SaveAppData()
    {
        try
        {
            string json = JsonSerializer.Serialize(appList);
            File.WriteAllText(dataFilePath, json);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Ошибка при сохранении данных: {ex.Message}");
        }
    }

    static void LoadAppData()
    {
        try
        {
            if (File.Exists(dataFilePath))
            {
                string json = File.ReadAllText(dataFilePath);
                appList = JsonSerializer.Deserialize<List<AppInfo>>(json);
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Ошибка при загрузке данных: {ex.Message}");
        }
    }

    class AppInfo
    {
        public string AppPath { get; set; }
        public DateTime StartTime { get; set; }
        public string AppArguments { get; set; }
    }
}
