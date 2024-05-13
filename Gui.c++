#include <Windows.h>
#include <winuser.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <ctime>
#include <map>
#include <vector>

//Bibliotheken für die Bild verarbeitung
#include <gdiplus.h>
#include <gdiplusheaders.h>
#include <objidl.h>
#include <cstdlib>
#include <cwchar>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

using namespace std;

const wchar_t* ConvertToWideChar(const char* input) {
    size_t length = mbstowcs(NULL, input, 0);
    if (length == static_cast<size_t>(-1)) {
        // Fehler bei der Umwandlung
        return nullptr;
    }

    wchar_t* buffer = new wchar_t[length + 1];
    mbstowcs(buffer, input, length + 1);

    return buffer;
}

// File Handler zum Öffnen von Dateien
string OpenFile(HWND hwnd)
{
    // Festlegen der maximalen Länge des Dateinamens
    char filename[777]; // Lucky Number Seven ^^
    string fileNameNew;
    string directoryPath;
    // Initialisieren der OPENFILENAME-Struktur und des Dateinamens
    OPENFILENAME ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = filename;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilter = "TXT\0*.txt\0CSV\0*.csv\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

    // Öffnen des Dateidialogs und Prüfen auf Erfolg
    if (GetOpenFileName(&ofn))
    {
        // Initialisieren von Variablen und Vektoren
        vector<string> fileNames;
        char *p = ofn.lpstrFile;
        bool firstString = true;
        // Iteration durch die Dateinamen
        while (*p)
        {
            fileNameNew += p;
            p += fileNameNew.size() + 1;

            // Unterscheidung zwischen Verzeichnis und Dateien
            if (firstString)
            {
                directoryPath = fileNameNew;
                firstString = false;
            }
            else
            {
                fileNames.push_back(fileNameNew);
            }
        }
        return fileNameNew;
    }
    else
    {
        string fehler;
        // Fehlerbehandlung beim Öffnen von Dateien
        switch (CommDlgExtendedError())
        {
        case CDERR_DIALOGFAILURE:
            fehler = "CDERR_DIALOGFAILURE\n";
            break;
        case CDERR_FINDRESFAILURE:
            fehler = "CDERR_FINDRESFAILURE\n";
            break;
        case CDERR_INITIALIZATION:
            fehler = "CDERR_INITIALIZATION\n";
            break;
        case CDERR_LOADRESFAILURE:
            fehler = "CDERR_LOADRESFAILURE\n";
            break;
        case CDERR_LOADSTRFAILURE:
            fehler = "CDERR_LOADSTRFAILURE\n";
            break;
        case CDERR_LOCKRESFAILURE:
            fehler = "CDERR_LOCKRESFAILURE\n";
            break;
        case CDERR_MEMALLOCFAILURE:
            fehler = "CDERR_MEMALLOCFAILURE\n";
            break;
        case CDERR_MEMLOCKFAILURE:
            fehler = "CDERR_MEMLOCKFAILURE\n";
            break;
        case CDERR_NOHINSTANCE:
            fehler = "CDERR_NOHINSTANCE\n";
            break;
        case CDERR_NOHOOK:
            fehler = "CDERR_NOHOOK\n";
            break;
        case CDERR_NOTEMPLATE:
            fehler = "CDERR_NOTEMPLATE\n";
            break;
        case CDERR_STRUCTSIZE:
            fehler = "CDERR_STRUCTSIZE\n";
            break;
        case FNERR_BUFFERTOOSMALL:
            fehler = "FNERR_BUFFERTOOSMALL\n";
            break;
        case FNERR_INVALIDFILENAME:
            fehler = "FNERR_INVALIDFILENAME\n";
            break;
        case FNERR_SUBCLASSFAILURE:
            fehler = "FNERR_SUBCLASSFAILURE\n";
            break;
        default:
            fehler = "";
        }
        if (fehler != "")
        {
            MessageBox(NULL, TEXT(fehler.c_str()),
                       "ERROR", MB_ICONERROR);
        }
        return " ";
    }
}

/*
Methode zum setzen eines icons mit eiem PNG
*/
HICON LoadIconFromFile(const char* filename) {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    const wchar_t* buffer = ConvertToWideChar(filename);
    
    Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromFile(buffer);
    //Fehler Behandung
    if (bmp == NULL) {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return NULL;
    }


    // Erstellen eines HICON aus dem GDI+ Bitmap
    HICON hIcon;
    bmp->GetHICON(&hIcon);
    // Freigeben von Ressourcen
    delete bmp;

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return hIcon;
}

/*
Methode Zum stetzen des Hintergrundes mit einem PNG
*/
HBRUSH SetPngBackground(const char* filename) {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    const wchar_t* buffer = ConvertToWideChar(filename);
    Gdiplus::Bitmap* bmp = Gdiplus::Bitmap::FromFile(buffer);
    //Fehler Behandlung
    if (bmp == NULL) {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return NULL;
    }

    // Erstellen eines GDI+ Grafikobjekts
    Gdiplus::Graphics* graphics = new Gdiplus::Graphics(GetDC(NULL));

    // Erstellen eines HBITMAP aus dem GDI+ Bitmap
    HBITMAP hBitmap;
    bmp->GetHBITMAP(Gdiplus::Color(), &hBitmap);

    // Erstellen einer Bürste aus dem HBITMAP
    HBRUSH hBrush = CreatePatternBrush(hBitmap);

    // Freigeben von Ressourcen
    delete bmp;
    delete graphics;

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return hBrush;
}