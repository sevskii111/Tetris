//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include <algorithm>
#include "MainPage.xaml.h"
#include <random>


using namespace Tetris;

using namespace Platform;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Gaming::Input;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Core;
// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409


void lock();
void gameover();
void startTimer();
static bool tryInject(uint8** field, int x, int y, int tetroInd, uint8 rotation, uint8** result, int8 forced_color = -1);

struct Tetrimino
{
	bool fill[4][4];
	uint8 color, rotation_type;
	Tetrimino(char l1[4], char l2[4], char l3[4], char l4[4], uint8 c, uint8 r)
	{
		fill[0][0] = l1[0] == '1';
		fill[0][1] = l1[1] == '1';
		fill[0][2] = l1[2] == '1';
		fill[0][3] = l1[3] == '1';
		fill[1][0] = l2[0] == '1';
		fill[1][1] = l2[1] == '1';
		fill[1][2] = l2[2] == '1';
		fill[1][3] = l2[3] == '1';
		fill[2][0] = l3[0] == '1';
		fill[2][1] = l3[1] == '1';
		fill[2][2] = l3[2] == '1';
		fill[2][3] = l3[3] == '1';
		fill[3][0] = l4[0] == '1';
		fill[3][1] = l4[1] == '1';
		fill[3][2] = l4[2] == '1';
		fill[3][3] = l4[3] == '1';

		rotation_type = r;
		color = c + 2;
	}
	Tetrimino() {}
	bool** get(uint8 rotation) {
		bool** r = new bool*[4];
		for (int i = 0; i < 4; i++)
		{
			r[i] = new bool[4];
			for (int j = 0; j < 4; j++)
			{
				if (rotation_type == 0) {
					switch (rotation) {
					case 0:
						r[i][j] = fill[i][j];
						break;
					case 1:
						r[i][j] = fill[3 - j][3 - i];
						break;
					case 2:
						r[i][j] = fill[3 - i][j];
						break;
					case 3:
						r[i][j] = fill[j][i];
						break;
					}
				}
				else if (rotation_type == 1) {
					if (i > 2 || j > 2) {
						r[i][j] = fill[i][j];
					}
					else {
						switch (rotation) {
						case 0:
							r[i][j] = fill[i][j];
							break;
						case 1:
							r[i][j] = fill[2 - j][i];
							break;
						case 2:
							r[i][j] = fill[2 - i][2 - j];
							break;
						case 3:
							r[i][j] = fill[j][2 - i];
							break;
						}
					}
				}
				else if (rotation_type == 2) {
					r[i][j] = fill[i][j];
				}
			}
		}
		return r;
	}
};

static struct GameState {
	static uint8** field;
	static uint8** injField;
	static int8 curTetr, tetrX, tetrY, tetrRot, tetrInd, lines;
	static int level;
	static uint8* currBag;
	static uint8* nextBag;
	static long long score;
	static bool active, paused;
};

uint8** GameState::field;
uint8** GameState::injField;
int8 GameState::curTetr, GameState::tetrX, GameState::tetrY, GameState::tetrRot, GameState::tetrInd, GameState::lines;
int GameState::level;
uint8* GameState::currBag;
uint8* GameState::nextBag;
long long GameState::score;
bool GameState::active, GameState::paused;

MainPage::MainPage()
{
	InitializeComponent();
}

static SolidColorBrush^ colors[9];
static Tetrimino Tetriminos[7] = { Tetrimino(
		"....",
		"1111",
		"....",
		"....",
		0, 0) , Tetrimino(
		"1...",
		"111.",
		"....",
		"....",
		1, 1) , Tetrimino(
		"..1.",
		"111.",
		"....",
		"....",
		2, 1) , Tetrimino(
		".11.",
		".11.",
		"....",
		"....",
		3, 2) , Tetrimino(
		".11.",
		"11..",
		"....",
		"....",
		4, 1) , Tetrimino(
		".1..",
		"111.",
		"....",
		"....",
		5, 1) , Tetrimino(
		"1100",
		".11.",
		"0000",
		"0000",
		6, 1) };

static Grid^ FieldG;
static Grid^ NextG;
static TextBox^ ScoreG;
static TextBox^ LevelG;

Windows::UI::Xaml::Shapes::Rectangle^** FieldGrid;
Windows::UI::Xaml::Shapes::Rectangle^** NextGrid;

DispatcherTimer^ refreshTimer;


uint8* getBag() {
	uint8* bag = new uint8[7];
	for (int i = 0; i < 7; i++)
	{
		bag[i] = i;
	}
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(bag, bag + 7, g);
	return bag;
}

static bool freshStart = true;
static void initGame() {
	if (freshStart) {
		FieldGrid = new Windows::UI::Xaml::Shapes::Rectangle ^ *[10];
		NextGrid = new Windows::UI::Xaml::Shapes::Rectangle ^ *[4];
		GameState::field = new uint8 * [10];
		GameState::injField = new uint8 * [10];
		for (int i = 0; i < 4; i++)
		{
			NextGrid[i] = new Windows::UI::Xaml::Shapes::Rectangle ^ [4];
			for (int j = 0; j < 4; j++)
			{
				auto rect = ref new Windows::UI::Xaml::Shapes::Rectangle();
				rect->Fill = colors[0];
				rect->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Gray);
				rect->StrokeThickness = 1;
				NextG->Children->Append(rect);
				NextG->SetColumn(rect, i);
				NextG->SetRow(rect, j);
				NextGrid[i][j] = rect;
			}
		}

	}
	for (int i = 0; i < 10; i++)
	{
		if (freshStart) {
			GameState::field[i] = new uint8[22];
			GameState::injField[i] = new uint8[22];
			FieldGrid[i] = new Windows::UI::Xaml::Shapes::Rectangle ^ [20];
		}
		for (int j = 0; j < 2; j++)
		{
			GameState::field[i][j] = 0;
			GameState::injField[i][j] = 0;
		}
		for (int j = 2; j < 22; j++)
		{
			if (freshStart) {
				auto rect = ref new Windows::UI::Xaml::Shapes::Rectangle();
				rect->Fill = colors[0];
				rect->Stroke = ref new SolidColorBrush(Windows::UI::Colors::Gray);
				rect->StrokeThickness = 1;
				FieldG->Children->Append(rect);
				FieldG->SetColumn(rect, i);
				FieldG->SetRow(rect, j - 2);
				FieldGrid[i][j - 2] = rect;
			}
			GameState::field[i][j] = 0;
			GameState::injField[i][j] = 0;
		}
	}

	GameState::level = 0;
	GameState::score = 0;
	GameState::lines = 0;
	GameState::tetrY = 0;
	GameState::tetrX = 3;
	GameState::tetrRot = 0;
	GameState::paused = false;

	GameState::currBag = getBag();
	GameState::nextBag = getBag();

	GameState::curTetr = GameState::currBag[0];

	startTimer();
	lock();
}

static void DrawField(uint8** field)
{
	uint8** f = new uint8 * [10];
	int o = 4;
	while (tryInject(field, GameState::tetrX, GameState::tetrY + o, GameState::curTetr, GameState::tetrRot, f)) {
		for (int i = 0; i < 10; i++)
		{
			delete[] f[i];
		}
		o++;
	}
	if (o > 4) {
		tryInject(field, GameState::tetrX, GameState::tetrY + o - 1, GameState::curTetr, GameState::tetrRot, f, 1);
	}
	for (int i = 0; i < 10; i++)
	{
		for (int j = 2; j < 22; j++)
		{
			if (o > 4) {
				FieldGrid[i][j - 2]->Fill = colors[f[i][j]];
			}
			else {
				FieldGrid[i][j - 2]->Fill = colors[field[i][j]];
			}
		}
		if (o > 4) {
			delete[] f[i];
		}
	}
	delete[] f;
}

static bool tryInject(uint8** field, int x, int y, int tetroInd, uint8 rotation, uint8** result, int8 forced_color)
{
	bool** tetr = Tetriminos[tetroInd].get(rotation);
	int8 color;
	if (forced_color < 0) {
		color = Tetriminos[tetroInd].color;
	}
	else {
		color = forced_color;
	}
	uint8 checked = 0;
	for (int i = 0; i < 10; i++)
	{
		result[i] = new uint8[22];
		for (int j = 0; j < 22; j++)
		{
			result[i][j] = field[i][j];
			int xo = i - x, yo = j - y;
			if (xo >= 0 && xo <= 3 && yo >= 0 && yo <= 3 && tetr[yo][xo])
			{
				checked++;
				if (result[i][j] != 0) {
					return false;
				}
				result[i][j] = color;
			}
		}
	}
	if (checked < 4) {
		return false;
	}
	return true;
}

const int scores[] = { 0, 40, 100, 300, 1200 };
const int speeds[] = { 8000000,  7100000,  6300000,  5500000,  4700000,  3800000,  3000000,  2100000,  1300000,  1000000, 800000 };
void checkLines() {

	uint8 lines[4];
	uint8 linesC = 0;


	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 10; j++)
		{
			if (GameState::field[j][i]) {
				gameover();
				return;
			}
		}
	}
	for (int i = 2; i < 22; i++)
	{
		bool line = true;
		for (int j = 0; j < 10; j++)
		{
			if (!GameState::field[j][i]) {
				line = false;
				break;
			}
		}
		if (line) {
			lines[linesC] = i;
			linesC++;
		}
	}
	for (int i = linesC; i > 0; i--)
	{
		for (int j = lines[i - 1] + i; j > 2; j--)
		{
			for (int k = 0; k < 10; k++)
			{
				GameState::field[k][j] = GameState::field[k][j - 1];
			}
		}
	}
	GameState::lines += linesC;
	GameState::score += scores[linesC] * (1 + GameState::level);

	ScoreG->Text = "" + GameState::score;
	if (GameState::lines > (GameState::level * 10 + 10))
	{
		if (GameState::level <= 10) {
			GameState::level++;
			GameState::lines = 0;
		}
	}

	LevelG->Text = "LEVEL:" + GameState::level;
	
	DrawField(GameState::field);
}

void lock()
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 22; j++)
		{
			GameState::field[i][j] = GameState::injField[i][j];
		}
	}
	GameState::tetrX = 3;
	GameState::tetrY = 0;
	GameState::tetrRot = 0;
	GameState::curTetr = GameState::currBag[GameState::tetrInd];
	GameState::tetrInd++;
	Tetrimino nextTetr;
	if (GameState::tetrInd == 7) {
		delete[] GameState::currBag;
		GameState::currBag = GameState::nextBag;
		GameState::nextBag = getBag();
		GameState::tetrInd = 0;
		nextTetr = Tetriminos[GameState::currBag[GameState::tetrInd]];
	}
	else {
		nextTetr = Tetriminos[GameState::currBag[GameState::tetrInd]];
	}

	bool** nextTetrF = nextTetr.get(0);
	uint8 nextTetrC = nextTetr.color;
	for (int i = 1; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (nextTetrF[i - 1][j]) {
				NextGrid[j][i]->Fill = colors[nextTetrC];
			}
			else {
				NextGrid[j][i]->Fill = colors[0];
			}
		}
	}
	checkLines();
}



void Tetris::MainPage::GamePage_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	ApplicationView::PreferredLaunchViewSize = Size(640, 853);
	ApplicationView::PreferredLaunchWindowingMode = ApplicationViewWindowingMode::PreferredLaunchViewSize;
	Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler(this, &MainPage::fixSize);

	Window::Current->CoreWindow->Dispatcher->AcceleratorKeyActivated += ref new Windows::Foundation::TypedEventHandler<CoreDispatcher^, AcceleratorKeyEventArgs^>(this, &MainPage::keyPressed);


	colors[0] = ref new SolidColorBrush(Windows::UI::Colors::Black);
	colors[1] = ref new SolidColorBrush(Windows::UI::Colors::Gray);

	colors[2] = ref new SolidColorBrush(Windows::UI::Colors::Cyan);
	colors[3] = ref new SolidColorBrush(Windows::UI::Colors::Blue);
	colors[4] = ref new SolidColorBrush(Windows::UI::Colors::Orange);
	colors[5] = ref new SolidColorBrush(Windows::UI::Colors::Yellow);
	colors[6] = ref new SolidColorBrush(Windows::UI::Colors::Green);
	colors[7] = ref new SolidColorBrush(Windows::UI::Colors::Purple);
	colors[8] = ref new SolidColorBrush(Windows::UI::Colors::Red);
	

	FieldG = this->Field;
	NextG = this->Next;
	ScoreG = this->Score;
	LevelG = this->Level;

	refreshTimer = ref new DispatcherTimer;
	auto registrationtoken = refreshTimer->Tick += ref new Windows::Foundation::EventHandler<Platform::Object^>(this, &MainPage::gameLoop);
	initGame();
	
	freshStart = false;
	//DrawField();
}

void startTimer()
{
	GameState::active = true;
	refreshTimer->Stop();
	
	TimeSpan t;
	t.Duration = speeds[GameState::level];
	refreshTimer->Interval = t;
	refreshTimer->Start();
}

void gameover()
{
	GameState::active = false;
	refreshTimer->Stop();
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 22; j++)
		{
			GameState::field[i][j] = 8;
		}
	}
	DrawField(GameState::field);
}

static int a = 0;
void MainPage::gameLoop(Platform::Object^ sender, Platform::Object^ e)
{
	uint8** f = new uint8 * [10];
	if (tryInject(GameState::field, GameState::tetrX, GameState::tetrY + 1, GameState::curTetr, GameState::tetrRot, f))
	{
		GameState::tetrY++;
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 22; j++)
			{
				GameState::injField[i][j] = f[i][j];
			}
		}
		DrawField(GameState::injField);
	}
	else if (GameState::tetrY < 2) {
		gameover();
	}
	else {
		lock();
	}

}

bool tryMove(int8 xo, int8 yo) {
	if (!GameState::paused) {
		uint8** f = new uint8 * [10];
		if (tryInject(GameState::field, GameState::tetrX + xo, GameState::tetrY, GameState::curTetr, GameState::tetrRot, f))
		{
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 22; j++)
				{
					GameState::injField[i][j] = f[i][j];
				}
			}
			GameState::tetrX += xo;
			GameState::tetrY += yo;
			DrawField(GameState::injField);
			return true;
		}
		return false;
	}
	else {
		return false;
	}
}

bool tryRotate(uint8 r) {
	if (!GameState::paused) {
		uint8** f = new uint8 * [10];
		uint8 rr = (GameState::tetrRot + r) % 4;
		int xo = -3;
		if (tryInject(GameState::field, GameState::tetrX, GameState::tetrY, GameState::curTetr, rr, f))
		{
			xo = 0;
		}
		else if (tryInject(GameState::field, GameState::tetrX + 1, GameState::tetrY, GameState::curTetr, rr, f))
		{
			xo = 1;
		}
		else if (tryInject(GameState::field, GameState::tetrX - 1, GameState::tetrY, GameState::curTetr, rr, f))
		{
			xo = -1;
		}
		else if (tryInject(GameState::field, GameState::tetrX + 2, GameState::tetrY, GameState::curTetr, rr, f))
		{
			xo = 2;
		}
		else if (tryInject(GameState::field, GameState::tetrX - 2, GameState::tetrY, GameState::curTetr, rr, f))
		{
			xo = -2;
		}
		if (xo > -3)
		{
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 22; j++)
				{
					GameState::injField[i][j] = f[i][j];
				}
			}
			GameState::tetrRot = rr;
			GameState::tetrX += xo;
			DrawField(GameState::injField);
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
	
}


void Tetris::MainPage::keyPressed(Windows::UI::Core::CoreDispatcher^ sender, Windows::UI::Core::AcceleratorKeyEventArgs^ args)
{
	CoreAcceleratorKeyEventType type = args->EventType;
	VirtualKey key = args->VirtualKey;
	if (GameState::active) {
		if (type == CoreAcceleratorKeyEventType::KeyDown)
		{
			switch (key) {
			case VirtualKey::Up:
			case VirtualKey::E:
				tryRotate(1);
				break;
			case VirtualKey::Right:
				tryMove(1, 0);
				break;
			case VirtualKey::Q:
				tryRotate(3);
				break;
			case VirtualKey::Down:
				while (tryMove(0, 1));
				lock();
				break;
			case VirtualKey::Left:
				tryMove(-1, 0);
				break;
			case VirtualKey::Space:
				if (GameState::paused) {
					refreshTimer->Start();
					GameState::paused = false;
				}
				else {
					refreshTimer->Stop();
					GameState::paused = true;
				}
			}
		}
	}
	if (key == VirtualKey::Escape && type == CoreAcceleratorKeyEventType::KeyDown) {
		initGame();
	}
}

void MainPage::fixSize(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e)
{
	ApplicationView::GetForCurrentView()->TryResizeView(Size(640, 853));
}