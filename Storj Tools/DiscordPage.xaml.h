#pragma once

#include "DiscordPage.g.h"
#include "StorjData.h"

namespace winrt::Storj_Tools::implementation
{
    struct DiscordPage : DiscordPageT<DiscordPage>
    {
		DiscordPage() = default;
        
        void OnNavigatedTo(winrt::Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& args);

        winrt::Windows::Foundation::IAsyncAction PasteUserId_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
        winrt::Windows::Foundation::IAsyncAction PasteBotToken_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args);
	    Storj_Tools::StorjData Data();

    private:
		Storj_Tools::StorjData storjData;
        winrt::Windows::Foundation::IAsyncOperation<winrt::hstring> GetClipboardText();
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct DiscordPage : DiscordPageT<DiscordPage, implementation::DiscordPage>
    {
		
    };
}
