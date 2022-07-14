#pragma once

#include <wx/wx.h>
#include <wx/dcbuffer.h>


class wxLargeUnselectableListBox : public wxScrolledWindow {
public:
	wxLargeUnselectableListBox(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

	void AppendItem(wxString item);
	void InsertItem(wxString item, size_t index);

	// Replace all items in the list
	void SetItems(wxArrayString& items);

	// Insert a sequence of items at a given index
	void InsertItems(wxArrayString& items, size_t index);

	// Remove count items from the list, starting from index
	// If exceeds the array boundary, removes all items until the end
	void RemoveItems(size_t index, size_t count);

	void Clear();

	// You can only programmatically select an item to highlight
	void SelectItem(size_t index);

protected:
	void OnPaint(wxPaintEvent& evt);
	void OnSize(wxSizeEvent& evt);
	void OnScroll(wxScrollWinEvent& evt);
	void OnBackgroundErase(wxEraseEvent& evt) {}
	void UpdateScrollbars();

private:
	wxArrayString m_items;
	size_t m_selectedItem;

	int m_fontSize;
	int m_marginFromLeft;
	int m_marginBetweenLines;

	int m_scrollRate;

	wxDECLARE_EVENT_TABLE();
};
