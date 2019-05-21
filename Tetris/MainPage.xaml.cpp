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
	bool fill[4][4][4];
	Tetrimino(char r1l1[4], char r1l2[4], char r1l3[4], char r1l4[4], char r2l1[4], char r2l2[4], char r2l3[4], char r2l4[4], char r3l1[4], char r3l2[4], char r3l3[4], char r3l4[4], char r4l1[4], char r4l2[4], char r4l3[4], char r4l4[4])
	{
		fill[0][0][0] = r1l1[0];
		fill[0][0][1] = r1l1[1];
		fill[0][0][2] = r1l1[2];
		fill[0][0][3] = r1l1[3];
		fill[0][1][0] = r1l2[0];
		fill[0][1][1] = r1l2[1];
		fill[0][1][2] = r1l2[2];
		fill[0][1][3] = r1l2[3];
		fill[0][2][0] = r1l3[0];
		fill[0][2][1] = r1l3[1];
		fill[0][2][2] = r1l3[2];
		fill[0][2][3] = r1l3[3];
		fill[0][3][0] = r1l4[0];
		fill[0][3][1] = r1l4[1];
		fill[0][3][2] = r1l4[2];
		fill[0][3][3] = r1l4[3];

		fill[1][0][0] = r2l1[0];
		fill[1][0][1] = r2l1[1];
		fill[1][0][2] = r2l1[2];
		fill[1][0][3] = r2l1[3];
		fill[1][1][0] = r2l2[0];
		fill[1][1][1] = r2l2[1];
		fill[1][1][2] = r2l2[2];
		fill[1][1][3] = r2l2[3];
		fill[1][2][0] = r2l3[0];
		fill[1][2][1] = r2l3[1];
		fill[1][2][2] = r2l3[2];
		fill[1][2][3] = r2l3[3];
		fill[1][3][0] = r2l4[0];
		fill[1][3][1] = r2l4[1];
		fill[1][3][2] = r2l4[2];
		fill[1][3][3] = r2l4[3];

		fill[2][0][0] = r3l1[0];
		fill[2][0][1] = r3l1[1];
		fill[2][0][2] = r3l1[2];
		fill[2][0][3] = r3l1[3];
		fill[2][1][0] = r3l2[0];
		fill[2][1][1] = r3l2[1];
		fill[2][1][2] = r3l2[2];
		fill[2][1][3] = r3l2[3];
		fill[2][2][0] = r3l3[0];
		fill[2][2][1] = r3l3[1];
		fill[2][2][2] = r3l3[2];
		fill[2][2][3] = r3l3[3];
		fill[2][3][0] = r3l4[0];
		fill[2][3][1] = r3l4[1];
		fill[2][3][2] = r3l4[2];
		fill[2][3][3] = r3l4[3];

		fill[3][0][0] = r4l1[0];
		fill[3][0][1] = r4l1[1];
		fill[3][0][2] = r4l1[2];
		fill[3][0][3] = r4l1[3];
		fill[3][1][0] = r4l2[0];
		fill[3][1][1] = r4l2[1];
		fill[3][1][2] = r4l2[2];
		fill[3][1][3] = r4l2[3];
		fill[3][2][0] = r4l3[0];
		fill[3][2][1] = r4l3[1];
		fill[3][2][2] = r4l3[2];
		fill[3][2][3] = r4l3[3];
		fill[3][3][0] = r4l4[0];
		fill[3][3][1] = r4l4[1];
		fill[3][3][2] = r4l4[2];
		fill[3][3][3] = r4l4[3];
	}
	Tetrimino() {}
};

MainPage::MainPage()
{
	InitializeComponent();
}

static SolidColorBrush^ colors[8];
static Tetrimino Tetriminos[7];

static Grid^ FieldG;
static int field[10][22];

static void DrawField()
{
	FieldG->Children->Clear();
	for (int i = 0; i < 10; i++)
	{
		for (int j = 4; j < 22; j++)
		{
			auto rect = ref new Windows::UI::Xaml::Shapes::Rectangle();
			rect->Fill = colors[rand() % 7];
			FieldG->Children->Append(rect);
			FieldG->SetColumn(rect, i);
			FieldG->SetRow(rect, j - 2);
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
		
		"0010",
		"0010",
		"0010",
		"0100",
		
		"0000",
		"0000",
		"1111",
		"0000",
		
		"0100",
		"0100",
		"0100",
		"0100");

	FieldG = this->Field;

	auto timer = ref new Windows::UI::Xaml::DispatcherTimer();
	TimeSpan ts;
	ts.Duration = 200000;
	timer->Interval = ts;
	timer->Start();
	auto registrationtoken = timer->Tick += ref new EventHandler<Object^>(this, &MainPage::OnTick);

	DrawField();
}

void MainPage::OnTick(Object^ sender, Object^ e) {
	DrawField();
}
