#include "pch.h"
#include "NodesPage.xaml.h"
#if __has_include("NodesPage.g.cpp")
#include "NodesPage.g.cpp"
#endif

using namespace std;
using namespace std::chrono;
using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Dispatching;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Storj_Tools::implementation
{
	void NodesPage::StartNodeService(IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& args)
	{
		auto button = sender.as<Button>();
		if (!button) return;
		hstring nodeName = unbox_value<hstring>(button.CommandParameter());
		storjData.StartNodeService(nodeName);
	}

	void NodesPage::OnUpdateTimerTick([[maybe_unused]] IInspectable const& sender, [[maybe_unused]] IInspectable const& args)
	{
		storjData.UpdateNodes();
	}

	void NodesPage::OnNavigatedTo([[maybe_unused]] NavigationEventArgs const& args)
	{
		storjData = App::GetStorjData(); // Použití globální instance 
		storjData.UpdateNodes();
		if (updateTimer) return;
		updateTimer = DispatcherQueue::GetForCurrentThread().CreateTimer();
		updateTimer.Interval(seconds(static_cast<int>(round(storjData.UpdateNodeUIInterval()))));
		updateTimer.Tick({ this, &NodesPage::OnUpdateTimerTick });
		updateTimer.Start();
	}

	void NodesPage::OnNavigatedFrom([[maybe_unused]] winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args)
	{
		if (!updateTimer) return;
		updateTimer.Stop();
		updateTimer = nullptr;
	}

	Storj_Tools::StorjData NodesPage::Data()
	{
		return storjData;
	}
}
