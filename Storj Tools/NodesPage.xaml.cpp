#include "pch.h"
#include "NodesPage.xaml.h"
#if __has_include("NodesPage.g.cpp")
#include "NodesPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Storj_Tools::implementation
{
	void NodesPage::StartNodeService(Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
	{
		auto button = sender.as<Microsoft::UI::Xaml::Controls::Button>();
		if (button)
		{
			hstring nodeName = unbox_value<hstring>(button.CommandParameter());
			storjData.StartNodeService(nodeName);
		}
	}
	void NodesPage::OnUpdateTimerTick([[maybe_unused]] winrt::Windows::Foundation::IInspectable const& sender, [[maybe_unused]] winrt::Windows::Foundation::IInspectable const& args)
	{
		storjData.UpdateNodes();
	}
	void NodesPage::OnNavigatedTo([[maybe_unused]] winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args)
	{
		storjData = App::GetStorjData(); // Použití globální instance 
		storjData.UpdateNodes();
		if (!updateTimer)
		{
			//updateTimer = winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread().CreateTimer();
			//updateTimer.Interval(std::chrono::seconds(static_cast<int>(std::round(storjData.UpdateNodeUIInterval()))));
			//updateTimer.Tick({ this, &NodesPage::OnUpdateTimerTick });
			//updateTimer.Start();
		}
	}
	void NodesPage::OnNavigatedFrom([[maybe_unused]] winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args)
	{
		if (updateTimer)
		{
			updateTimer.Stop();
			updateTimer = nullptr;
		}
	}
	Storj_Tools::StorjData NodesPage::Data()
	{
		return storjData;
	}
}
