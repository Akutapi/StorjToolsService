#include "pch.h"
#include "ServicePage.xaml.h"
#if __has_include("ServicePage.g.cpp")
#include "ServicePage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml::Navigation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Storj_Tools::implementation
{
	Storj_Tools::StorjData ServicePage::Data()
	{
		return storjData;
	}
	void ServicePage::OnNavigatedTo([[maybe_unused]] NavigationEventArgs const& args)
	{
		storjData = App::GetStorjData(); // Použití globální instance
	}
}
