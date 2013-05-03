//
// PlayerPage.xaml.cpp
// Implementation of the PlayerPage class
//

#include "pch.h"
#include "PlayerPage.xaml.h"
#include "MainPage.xaml.h"

using namespace Demo;

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
using namespace Windows::UI::Xaml::Interop;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

static Platform::String ^ duration_string(double v)
{
    int64 i = (int64)v;
    int64 h = i / 3600;
    int64 m = i % 3600;
    int64 s = m % 60;
    m /= 60;

    return h.ToString() + L":" + m + L":" + s;
}

ref class DurationFormatter sealed : Windows::UI::Xaml::Data::IValueConverter 
{
    // This converts the DateTime object to the Platform::String^ to display.
public:
    virtual Platform::Object^ Convert(
        Platform::Object^ value, 
        Windows::UI::Xaml::Interop::TypeName targetType, 
        Platform::Object^ parameter, 
        Platform::String^ language)
    {
        double v = safe_cast<double>(value);
        return duration_string(v);
    }

    // No need to implement converting back on a one-way binding 
    virtual Platform::Object^ ConvertBack(Platform::Object^ value, Windows::UI::Xaml::Interop::TypeName targetType, 
        Platform::Object^ parameter, Platform::String^ language)
    {
        throw ref new Platform::NotImplementedException();
    }
};

PlayerPage::PlayerPage()
{
	InitializeComponent();

    _extensionManager = ref new Windows::Media::MediaExtensionManager();
    _extensionManager->RegisterSchemeHandler("PpboxSource.PpboxSchemeHandler", "ppvod:");
    _extensionManager->RegisterSchemeHandler("PpboxSource.PpboxSchemeHandler", "ppvod2:");
    _extensionManager->RegisterSchemeHandler("PpboxSource.PpboxSchemeHandler", "pplive2:");
    _extensionManager->RegisterSchemeHandler("PpboxSource.PpboxSchemeHandler", "pplive3:");
    _extensionManager->RegisterSchemeHandler("PpboxSource.PpboxSchemeHandler", "identify:");

    _timer = ref new DispatcherTimer();
    TimeSpan interval;
    interval.Duration = 10000000; // 1s
    _timer->Interval = interval;
    _timer->Tick += ref new EventHandler<Object ^>([this](Object ^ sender, Object ^){
        timer_Tick(sender);
    });

    slider->ThumbToolTipValueConverter = ref new DurationFormatter();
}

/// <summary>
/// Invoked when this page is about to be displayed in a Frame.
/// </summary>
/// <param name="e">Event data that describes how this page was reached.  The Parameter
/// property is typically used to configure the page.</param>
void PlayerPage::OnNavigatedTo(NavigationEventArgs^ e)
{
	(void) e;	// Unused parameter
    String ^ url = dynamic_cast<Platform::String ^>(e->Parameter);
    if (url) {
        if (wcschr(url->Data(), L'?')) {
            url += L'&';
        } else {
            url += L'?';
        }
        url += "cdn.live.noshift=true&m3u8.noshift=true";
        if (url->Data()[0] != L'p') {
            String ^ url1 = ref new String(L"identify");
            wchar_t const * p = wcschr(url->Data(), L':');
            url1 += ref new String(p);
            url1 += L'#';
            url1 += ref new String(url->Data(), p - url->Data());
            url = url1;
        }
        video->Source = ref new Uri(url);
    } else {
        video->Source = ref new Uri(L"pplive2://host/e9301e073cf94732a380b765c8b9573d-5-400");
    }
    _timer->Start();
}


void PlayerPage::playButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    video->Play();
    _timer->Start();
}


void PlayerPage::pauseButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    video->Pause();
    _timer->Stop();
}


void PlayerPage::stopButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    video->Stop();
    _timer->Stop();
}


void PlayerPage::backButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    Frame->Navigate(TypeName(MainPage::typeid));
}

static int64 const TIME_SCALE = 10000000;

void PlayerPage::video_Opened(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
    slider->Maximum = (double)(video->NaturalDuration.TimeSpan.Duration / TIME_SCALE);
    text2->Text = duration_string(slider->Maximum);
}


void PlayerPage::slider_Changed(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
    TimeSpan time;
    time.Duration = (int64)slider->Value * TIME_SCALE;
    TimeSpan time2 = video->Position;
    if (time2.Duration + 5 * TIME_SCALE < time.Duration || time2.Duration > time.Duration + 5 * TIME_SCALE) {
        video->Position = time;
    }
}


void PlayerPage::timer_Tick(Platform::Object^ sender)
{
    TimeSpan time = video->Position;
    slider->Value = (double)(time.Duration / TIME_SCALE);
    text1->Text = duration_string(slider->Value);
}