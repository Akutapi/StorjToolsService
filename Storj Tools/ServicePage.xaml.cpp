#include "pch.h"
#include "ServicePage.xaml.h"
#if __has_include("ServicePage.g.cpp")
#include "ServicePage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::Storj_Tools::implementation
{
	void ServicePage::SaveSettings_Click([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Microsoft::UI::Xaml::RoutedEventArgs const& args)
	{
		throw hresult_not_implemented();
	}
}
