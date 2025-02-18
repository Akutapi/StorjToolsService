#pragma once

#include "Node.g.h"

namespace winrt::Storj_Tools::implementation
{
    struct Node : NodeT<Node>
    {
        Node() = default;
        std::wstring GetWName() const;
        void SetWName(std::wstring const& value);

        hstring Name() const;
        void Name(hstring const& value);

        hstring Path() const;
        void Path(hstring const& value);

        hstring Status() const;
        void Status(hstring const& value);

        hstring SVersion() const;
        void SVersion(hstring const& value);

        hstring StatusIcon() const;
        void StatusIcon(hstring const& value);

        UINT32 StatusColor() const;
        void StatusColor(UINT32 value);

    private:
        std::wstring wName;
        hstring name;
        hstring path;
        hstring status;
        hstring sVersion;
        hstring statusIcon;
        UINT32  statusColor;
    };
}

namespace winrt::Storj_Tools::factory_implementation
{
    struct Node : NodeT<Node, implementation::Node>
    {
    };
}
