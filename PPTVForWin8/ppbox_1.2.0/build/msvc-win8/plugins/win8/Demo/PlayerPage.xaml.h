//
// PlayerPage.xaml.h
// Declaration of the PlayerPage class
//

#pragma once

#include "PlayerPage.g.h"

namespace Demo
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class PlayerPage sealed
	{
	public:
		PlayerPage();

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
    private:
        void playButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void pauseButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void stopButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void backButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void video_Opened(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
        void slider_Changed(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e);
        void timer_Tick(Platform::Object^ sender);

        Windows::Media::MediaExtensionManager ^ _extensionManager;
        Windows::UI::Xaml::DispatcherTimer ^ _timer;
    };
}
