//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace Tetris;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409


struct Tetrimino
{
	uint8 rotation;
	bool fill[4][4];
	uint8 color;
	Tetrimino(char l1[4], char l2[4], char l3[4], char l4[4], uint8 c)
	{
		fill[0][0] = l1[0];
		fill[0][0] = l1[1];
		fill[0][0] = l1[2];
		fill[0][0] = l1[3];
		fill[0][1] = l2[0];
		fill[0][1] = l2[1];
		fill[0][1] = l2[2];
		fill[0][1] = l2[3];
		fill[0][2] = l3[0];
		fill[0][2] = l3[1];
		fill[0][2] = l3[2];
		fill[0][2] = l3[3];
		fill[0][3] = l4[0];
		fill[0][3] = l4[1];
		fill[0][3] = l4[2];
		fill[0][3] = l4[3];

		rotation = 0;

		color = c;
	}
	Tetrimino() {}
	bool** get() {
		bool** r - new bool*[4];
		for (int i = 0; i < 4; i++)
		{
			r[i] = new bool[4];
			for (int j = 0; j < 4; j++)
			{
				switch (rotation) {
				case 0:
					r[i][j] = fill[i][j];
					break;
				case 1:
					r[i][j] = fill[j][i];
					break;
				case 2:
					r[i][j] = fill[3 - i][j];
					break;
				case 3:
					r[i][j] = fill[j][3 - i];
					break;
				}
			}
		}
	}
};

MainPage::MainPage()
{
	InitializeComponent();
}

static SolidColorBrush^ colors[8];
static Tetrimino Tetriminos[7];

static Grid^ FieldG;
static int field[10][22];

 
Windows::UI::Xaml::Shapes::Rectangle^** FieldGrid;

static void initField() {
	FieldGrid = new Windows::UI::Xaml::Shapes::Rectangle^* [10];

	for (int i = 0; i < 10; i++)
	{
		FieldGrid[i] = new Windows::UI::Xaml::Shapes::Rectangle^ [20];
		for (int j = 2; j < 22; j++)
		{
			auto rect = ref new Windows::UI::Xaml::Shapes::Rectangle();
			rect->Fill = colors[rand() % 7];
			FieldG->Children->Append(rect);
			FieldG->SetColumn(rect, i);
			FieldG->SetRow(rect, j - 2);
			FieldGrid[i][j - 2] = rect;
		}
	}
}

static void DrawField()
{

	for (int i = 0; i < 10; i++)
	{
		for (int j = 2; j < 22; j++)
		{
			FieldGrid[i][j - 2]->Fill = colors[rand() % 7];
		}
	}
}

static bool inject(int x, int y, Tetrimino t)
{
	for (int i = x; i < x + 4; i++)
	{
		for (int j = y; j < y + 4; j++)
		{
			bool** tetr = t.get();
			int8 color = t.color;

		}
	}
}

void Tetris::MainPage::GamePage_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	colors[0] = ref new SolidColorBrush(Windows::UI::Colors::Cyan);
	colors[1] = ref new SolidColorBrush(Windows::UI::Colors::Yellow);
	colors[2] = ref new SolidColorBrush(Windows::UI::Colors::Purple);
	colors[3] = ref new SolidColorBrush(Windows::UI::Colors::Green);
	colors[4] = ref new SolidColorBrush(Windows::UI::Colors::Red);
	colors[5] = ref new SolidColorBrush(Windows::UI::Colors::Blue);
	colors[6] = ref new SolidColorBrush(Windows::UI::Colors::Orange);
	colors[7] = ref new SolidColorBrush(Windows::UI::Colors::Black);

	Tetriminos[0] = Tetrimino(
		"0000",
		"1111",
		"0000",
		"0000",
		0);

	FieldG = this->Field;
	initField();


	startTimer();
	//DrawField();
}

void MainPage::startTimer()
{
	DispatcherTimer^ refreshTimer = ref new DispatcherTimer;
	auto registrationtoken = refreshTimer->Tick += ref new Windows::Foundation::EventHandler<Platform::Object^>(this, &MainPage::refreshFunc);
	TimeSpan t;

	t.Duration = 10000000;
	refreshTimer->Interval = t;
	refreshTimer->Start();
}


void MainPage::refreshFunc(Platform::Object^ sender, Platform::Object^ e)
{
	DrawField();
}