#pragma once
#include "Node.g.h"
#include <winsvc.h>

using namespace winrt::Microsoft::UI;
using namespace winrt::Microsoft::UI::Xaml::Media;

namespace winrt::Storj_Tools::implementation
{
    struct Node : NodeT<Node>
    {
        Node() = default;

        hstring Name() const;
        void Name(hstring const& value);

        hstring Path() const;
        void Path(hstring const& value);

        hstring SVersion() const;
        void SVersion(hstring const& value);

        DWORD DStatus() const;
        void DStatus(DWORD const& value);

		hstring HStatus() const;
        hstring StatusIcon() const;
        SolidColorBrush StatusColor() const;

		bool IsStartButtonEnabled() const;

        bool operator==(Node const& other) const;
        bool operator!=(Node const& other) const;
        bool operator<(Node const& other) const;
        bool operator>(Node const& other) const;

    private:
        hstring name = L"Unknown";
        hstring path = L"Unknown";
        hstring sVersion = L"Unknown";
		DWORD dStatus = SERVICE_STOPPED;
		hstring hStatus = L"Unknown";
        hstring statusIcon = L"\uE71A";
		SolidColorBrush statusColor = SolidColorBrush(Colors::Red());
    };

}

namespace winrt::Storj_Tools::factory_implementation
{
    struct Node : NodeT<Node, implementation::Node>
    {
    };
}
