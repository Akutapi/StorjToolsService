#pragma once

#include "DiscordPage.g.h"
#include "StorjData.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::DataTransfer;

namespace winrt::Storj_Tools::implementation
{
    struct DiscordPage : DiscordPageT<DiscordPage>
    {
		DiscordPage() = default;
        
        void OnNavigatedTo(NavigationEventArgs const& args);

        IAsyncAction PasteUserId_Click(IInspectable const& sender, RoutedEventArgs const& args);
        IAsyncAction PasteBotToken_Click(IInspectable const& sender, RoutedEventArgs const& args);
	    Storj_Tools::StorjData Data();

    private:
		Storj_Tools::StorjData storjData;
        IAsyncOperation<hstring> GetClipboardText();
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct DiscordPage : DiscordPageT<DiscordPage, implementation::DiscordPage>
    {
		
    };
}
