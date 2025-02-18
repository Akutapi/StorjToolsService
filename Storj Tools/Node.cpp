#include "pch.h"
#include "Node.h"
#if __has_include("Node.g.cpp")
#include "Node.g.cpp"
#endif

namespace winrt::Storj_Tools::implementation
{
	std::wstring Node::GetWName() const
	{
		return wName;
	}
	void Node::SetWName(std::wstring const& value)
	{
		wName = value;
	}
	hstring Node::Name() const
	{
		return name;
	}
	void Node::Name(hstring const& value)
	{
		name = value;
	}
	hstring Node::Path() const
	{
		return path;
	}
	void Node::Path(hstring const& value)
	{
		path = value;
	}
	hstring Node::Status() const
	{
		return status;
	}
	void Node::Status(hstring const& value)
	{
		status = value;
	}
	hstring Node::SVersion() const
	{
		return sVersion;
	}
	void Node::SVersion(hstring const& value)
	{
		sVersion = value;
	}
	hstring Node::StatusIcon() const
	{
		return statusIcon;
	}
	void Node::StatusIcon(hstring const& value)
	{
		statusIcon = value;
	}
	UINT32 Node::StatusColor() const
	{
		return statusColor;
	}
	void Node::StatusColor(UINT32 value)
	{
		statusColor = value;
	}
}
