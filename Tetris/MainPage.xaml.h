//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace Tetris
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

		void gameLoop(Platform::Object^ sender, Platform::Object^ e);

	private:
		void GamePage_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		static Windows::UI::Xaml::Shapes::Rectangle^** FieldGrid;
		void fixSize(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e);
		void keyPressed(Windows::UI::Core::CoreDispatcher^ sender, Windows::UI::Core::AcceleratorKeyEventArgs^ args);
		

	};
}
