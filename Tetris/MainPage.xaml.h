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

		void startTimer();
		void refreshFunc(Platform::Object^ sender, Platform::Object^ e);
		

	private:
		void GamePage_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnTick(Object^ sender, Object^ e);
		static Windows::UI::Xaml::Shapes::Rectangle^** FieldGrid;
	};
}
