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
	Storj_Tools::StorjData NodesPage::StorjData()
	{
		return storjData;
	}
}
