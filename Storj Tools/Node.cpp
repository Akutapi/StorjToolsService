#include "pch.h"
#include "Node.h"
#if __has_include("Node.g.cpp")
#include "Node.g.cpp"
#endif

namespace winrt::Storj_Tools::implementation
{
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
	hstring Node::SVersion() const
	{
		return sVersion;
	}
	void Node::SVersion(hstring const& value)
	{
		sVersion = value;
	}
	DWORD Node::DStatus() const
	{
		return dStatus;
	}
	void Node::DStatus(DWORD const& value)
	{
		switch (value)
		{
		case SERVICE_STOPPED:
			hStatus = L"Service is stopped.";
			statusIcon = L"\uE8CD";
			statusColor = SolidColorBrush(Colors::Red());
			break;

		case SERVICE_START_PENDING:
			hStatus = L"Service is starting.";
			statusIcon = L"\uE8CE";
			statusColor = SolidColorBrush(Colors::Green());
			break;

		case SERVICE_STOP_PENDING:
			hStatus = L"Service is stopping.";
			statusIcon = L"\uE8CD";
			statusColor = SolidColorBrush(Colors::Red());
			break;

		case SERVICE_RUNNING:
			hStatus = L"Service is running.";
			statusIcon = L"\uE8CE";
			statusColor = SolidColorBrush(Colors::Green());
			break;

		case SERVICE_CONTINUE_PENDING:
			hStatus = L"Service is resuming.";
			statusIcon = L"\uE8CE";
			statusColor = SolidColorBrush(Colors::Green());
			break;

		case SERVICE_PAUSE_PENDING:
			hStatus = L"Service is pausing.";
			statusIcon = L"\uE8CD";
			statusColor = SolidColorBrush(Colors::Red());
			break;

		case SERVICE_PAUSED:
			hStatus = L"Service is paused.";
			statusIcon = L"\uE8CD";
			statusColor = SolidColorBrush(Colors::Red());
			break;
		default:
			hStatus = L"Unknown";
			statusIcon = L"\uE9CE";
			statusColor = SolidColorBrush(Colors::Yellow());
		}
	}
	hstring Node::HStatus() const
	{
		return hStatus;
	}
	hstring Node::StatusIcon() const
	{
		return statusIcon;
	}
	SolidColorBrush Node::StatusColor() const
	{
		return statusColor;
	}
	bool Node::IsStartButtonEnabled() const
	{
		if (hStatus == L"Service is running." || hStatus == L"Service is starting." || hStatus == L"Service is resuming.")
		{
			return false;
		}
		return true;
	}
	bool Node::operator==(Node const& other) const
	{
		 return name == other.name &&
				path == other.path &&
				hStatus == other.hStatus &&
				sVersion == other.sVersion;
	}
	bool Node::operator!=(Node const& other) const
	{
		return !(*this == other);
	}
	bool Node::operator<(Node const& other) const
	{
		return name < other.name; // Porovnání podle jména pro seřazení
	}
	bool Node::operator>(Node const& other) const
	{
		return name > other.name; // Porovnání podle jména pro seřazení
	}
}
