using ImageMagick;
using System.Text.Json;

Console.WriteLine("path to the folder containing all the folders (:D)");
string path = Console.ReadLine();

DateTime lenght = DateTime.Now;

Directory.CreateDirectory($"output");

string[] directories = { "APESPIL", "Bisam", "FILIFJO", "GROTTAN", "HATIFN1", "HATIFNA", "HEMUL", "INFOKAR", "INTRO", "Kake",
    "KART", "LANGUAGE", "Memoar", "OMMUMIN", "Rubin", "SLUTT", "Snusmum", "TofsVif", "TROLLIN", "TROLOUT" };

int xi = 0;

foreach (string directory in directories)
{
    Directory.CreateDirectory($"output/{directory}");
    string path2 = $"{path}\\{directories[xi]}";
    string[] files = Directory.GetFiles(@path2);

    foreach (string file in files)
    {
        Console.WriteLine(file);
        rmBackground(file);
    }

    string jpath = $"json/{directories[xi]}.json";

    if (File.Exists(jpath))
    {
        using StreamReader reader = new(jpath);
        var pixels = JsonSerializer.Deserialize<List<whitePixel>>(reader.ReadToEnd());

        foreach (whitePixel white in pixels)
        {
            rmWithJson("output/"+white.file, white.x, white.y);
        }
    }

    xi++;
}

Console.WriteLine($"Took: {DateTime.Now - lenght}");
Console.ReadKey();

void rmBackground (string fpath)
{
    MagickImage png = new MagickImage(fpath);

    png.ColorFuzz = new Percentage(1);

    whiteFloodFill(png, 0, 0);

    string output = $"output/{directories[xi]}/{Path.GetFileName(fpath)}";
    png.Write(output);
}

void rmWithJson(string fpath, int x, int y)
{
    MagickImage png = new MagickImage(fpath);

    png.ColorFuzz = new Percentage(1);

    if (x == 0 && y == 0)
    {
        png.Transparent(MagickColors.White);
    } else
    {
        png.FloodFill(MagickColors.Transparent, x, y);
    }

    string output = $"output/{directories[xi]}/{Path.GetFileName(fpath)}";
    png.Write(output);
}

void whiteFloodFill(MagickImage png, int x, int y)
{
    int w = png.Width;
    int h = png.Height;

    for (int i = 0; i < w; ++i)
    {
        if ((MagickColor)png.GetPixels().GetPixel(i, 0).ToColor() == MagickColors.White)
        {
            png.FloodFill(MagickColors.Transparent, i, 0);
        }

        if ((MagickColor)png.GetPixels().GetPixel(i, h-1).ToColor() == MagickColors.White)
        {
            png.FloodFill(MagickColors.Transparent, i, h-1);
        }
    }

    for (int i = 0; i < h; ++i)
    {
        if ((MagickColor)png.GetPixels().GetPixel(0, i).ToColor() == MagickColors.White)
        {
            png.FloodFill(MagickColors.Transparent, 0, i);
        }

        if ((MagickColor)png.GetPixels().GetPixel(w-1, i).ToColor() == MagickColors.White)
        {
            png.FloodFill(MagickColors.Transparent, w-1, i);
        }
    }
}
class whitePixel
{
    public string file { get; set; }
    public int x { get; set; }
    public int y { get; set; }
}